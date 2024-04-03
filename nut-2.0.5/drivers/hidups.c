/* hidups.c - prototype HID UPS driver for Network UPS Tools
 
   Copyright (C) 2001  Russell Kroll <rkroll@exploits.org>
 
   Based on evtest.c v1.10 - Copyright (c) 1999-2000 Vojtech Pavlik
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or 
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "hidups.h"
#include "main.h"

	int	fd;

#ifdef HIDIOCGPHYS
	/* standards here are in flux for now */
	char *hiddev_prefix = "/dev/usb/hid/hiddev";  /* in devfs */
	int  hiddev_limit = 16;             /* no dynamic minors */
#endif

struct ups_state {
	short int ol;
	short int runtime;
	short int charge;
	short int lb;
	float voltage;
};

int setvalue(int type, int value);
int offdelay = DEFAULT_OFFDELAY;
int ondelay = DEFAULT_ONDELAY;
int lowbatt_pct = 10;
unsigned short vendor = 0;
unsigned short product = 0;
unsigned short ups_flags = 0;
unsigned short ups_poll = 0;
unsigned short poll_interval = 2;
static time_t on_batt = 0;

static void open_usb(const char *port, int flags);

void upsdrv_shutdown(void)
{
	/* XXX: replace with a proper shutdown function
	fatalx("shutdown not supported"); */
	
	/* 1) set DelayBeforeStartup */
	if (getval ("ondelay"))
		ondelay = atoi (getval ("ondelay"));
	
	setvalue(UPS_WAKEDELAY, ondelay);

	/* 2) set DelayBeforeShutdown */
	if (getval ("offdelay"))
		offdelay = atoi (getval ("offdelay"));

	setvalue(UPS_GRACEDELAY, offdelay);
}

void instcmd(int auxcmd, int dlen, char *data) {}

void upsdrv_help(void) {}

void upsdrv_makevartable(void)
{
    char temp [128];
	
    /* add command line/conf variables */
	sprintf(temp, "Set shutdown delay, in seconds (default=%d).", DEFAULT_OFFDELAY);
	addvar (VAR_VALUE, "offdelay", temp);

	sprintf(temp, "Set startup delay, in ten seconds units for MGE (default=%d).", DEFAULT_ONDELAY);
	addvar (VAR_VALUE, "ondelay", temp);
	
	sprintf(temp, "Define what battery %% to shut down at (default=0).");
	addvar (VAR_VALUE, "lowbatt_pct", temp);
	
#ifdef HIDIOCGPHYS

	/* add command line/conf variables */
	snprintf(temp, sizeof(temp), "/dev/... prefix (default %s)",
		hiddev_prefix);

	addvar(VAR_VALUE, "HidDevPrefix", temp);

	snprintf(temp, sizeof(temp), 
		"how many devices to check (default %d)       ",
		hiddev_limit);

	addvar(VAR_VALUE, "HidDevLimit", temp);
#endif
}

void upsdrv_banner(void)
{
	printf("Network UPS Tools: HID UPS driver 0.13 (%s)\n\n", UPS_VERSION);

	experimental_driver = 0;
}

static inline int find_application(int fd, int usage) 
{
	int	i = 0, ret;

	while ((ret = ioctl(fd, HIDIOCAPPLICATION, i)) > 0 &&
	       ret != usage) 
		i++;

	return (ret == usage);
}

static void parse_event(struct hiddev_event *ev)
{
	int	sc = 0;
	int	ol = -1, ob = -1, lb = -1, rb = -1;

	/* XXX: deal with bogosity by throwing it out */
	if (ev->value > 3000000) {
		if (ev->hid == BATT_REMAINING_TIME_LIMIT) //JM: Looks like APCs can have very high values for this one.
			return;
		upslogx(LOG_INFO, "Ignoring bogus event 0x%x (%d)",
			ev->hid, ev->value);
		return;
	}

	/* x86 page == ups-specific, ignore these for now */
	if ((ev->hid & 0x860000) == 0x860000) {
/*		upslogx(LOG_INFO, "Ignoring x86 page event 0x%x (%d)",
			ev->hid, ev->value); */
		return;
	}

	upsdebugx(2, "event(%x, %d)",ev->hid,ev->value);
	switch (ev->hid) {
		case UPS_BATTVOLT:
			dstate_setinfo("battery.voltage", "%2.1f", 
				ev->value / 100.0);
			break;
		case BATT_RUNTIME_TO_EMPTY:
			dstate_setinfo("battery.runtime", "%d", ev->value);
			break;
		case BATT_REMAINING_CAPACITY:
			if (ev->value > 100)
				ev->value = (ev->value / 10);
			dstate_setinfo("battery.charge", "%d", ev->value);
			if (on_batt == -1 && getval("lowbatt_pct")) {
				lowbatt_pct = atoi(getval("lowbatt_pct"));
				if( ev->value < lowbatt_pct ) {
					upslogx(LOG_WARNING,
						"Battery is below the minumum level. (%d/%d)(cur/min)",
						ev->value, lowbatt_pct);
					lb = 1;
					sc = 1;
				}
			}
			break;

		/* OB/OL/LB: update temp storage and flag changed */
		case BATT_DISCHARGING_TL:
			if (!(vendor == TRIPPLITE_VENDOR_ID && product == TRIPPLITE_PRODUCT_ID))
				break;
			// Pass through
		case BATT_DISCHARGING:
			if ((ev->value == 0) || (ev->value == 1)) {
				ob = ev->value;
				sc = 1;
			}
			else
				upslogx(LOG_WARNING,
					"Got bogus value for BATT_DISCHARGING: %d",
					ev->value);
			break;

		case BATT_AC_PRESENT_TL:
			if( !(vendor == TRIPPLITE_VENDOR_ID && product == TRIPPLITE_PRODUCT_ID) )
				break;
		case BATT_AC_PRESENT:
			if ((ev->value == 0) || (ev->value == 1)) {
				ol = ev->value;
				sc = 1;
			}
			else
				upslogx(LOG_WARNING, 
					"Got bogus value for BATT_AC_PRESENT: %d",
					ev->value);
			break;

		case UPS_SHUTDOWN_IMMINENT:
		case UPS_SHUTDOWN_REQUESTED:
		case BATT_BELOW_RCL:
		case BATT_BELOW_RTL:
			if (ev->value == 0)
				return;
			if (ev->value == 1) {
				lb = ev->value;
				sc = 1;
			}
			else
				upslogx(LOG_WARNING,
					"Got bogus value for UPS_SHUTDOWN_IMMINENT: %d", 
					ev->value);
			break;

		case BATT_NEED_REPLACEMENT_TL:
			if( !(vendor == TRIPPLITE_VENDOR_ID && product == TRIPPLITE_PRODUCT_ID) )
				break;
		case BATT_NEED_REPLACEMENT:
			if ((ev->value == 0) || (ev->value == 1)) {
				rb = ev->value;
			}
			else
				upslogx(LOG_WARNING,
					"Got bogus value for BATT_NEED_REPLACEMENT: %d", 
					ev->value);
			break;

		case UPS_OVERLOAD:
			if (ev->value == 1)
				upslogx(LOG_WARNING, "UPS is overloaded!");
			break;

		case UPS_COMM_LOST:
			if (ev->value == 1)
				upslogx(LOG_WARNING, "UPS noticed lost communication!");
			break;

		case UPS_INTERNAL_FAILURE:
			if (ev->value == 1)
				upslogx(LOG_WARNING, "UPS had an internal failure!");
			break;

		/* things that we don't care about */
		case BATT_CHARGING:
		case BATT_CHARGING_TL:
		case UPS_TEMPERATURE:
		case UPS_TEST:
		case UPS_FREQ:
		case UPS_AUDIBLE_ALARM:
		case BATT_MFRDATE:
		case UPS_LOADPCT:
		case BATT_FULLY_CHARGED:
		case BATT_VOLT_UNREGULATED:
		case BATT_PRESENT:
		case APC_GARBAGE_1:
		case APC_GARBAGE_2:
		case UPS_CONFIG_VOLTAGE:
		case UPS_LOW_VOLTAGE_TRANSFER:
		case UPS_HIGH_VOLTAGE_TRANSFER:
		case BATT_REMAINING_TIME_LIMIT:
		case UPS_STATE_GOOD:
		case BATT_UNDEFINED:
		case UPS_BOOST:
			break;

		default:
			upslogx(LOG_INFO, "Unhandled event: 0x%x (%d)",
				ev->hid, ev->value);
	}

	if (rb == 1) {
		status_set("RB");
		status_commit();
		return;
	}

	if (ob == 1 || ol == 0) {
		if(!on_batt)
			time(&on_batt);
		return;
	}

	/* deal with any status changes */
	if ((sc == 0) || (ol+ob+lb == -3))
		return;

	status_init();

	if (ol == 1 || ob == 0) {
		on_batt = 0;
		status_set("OL");
	}
	else if (lb == 1 && on_batt == -1)
		status_set("LB");
	else
		status_set("OL");

	status_commit();
}

static int getvalue(int type)
{
	struct	hiddev_usage_ref uref;

	/* TODO: build a report table so we don't need HID_REPORT_ID_UNKNOWN */

	memset(&uref, 0, sizeof(uref));
	uref.report_type = HID_REPORT_TYPE_FEATURE;
	uref.report_id = HID_REPORT_ID_UNKNOWN;
	uref.field_index = 0;
	uref.usage_index = 0;
	uref.usage_code = type;
	uref.value = 0;

	if (ioctl(fd, HIDIOCGUSAGE, &uref) >= 0)
		return uref.value;
	else
		return -2; /* -1 is valid for Delay values (== not set) */
}

static char *getstring(int type)
{
	struct	hiddev_usage_ref uref;
	struct	hiddev_string_descriptor sdesc;
	static	char	str[256];

	/* TODO: build a report table so we don't need HID_REPORT_ID_UNKNOWN */

	memset(&uref, 0, sizeof(uref));
	memset(&sdesc, 0, sizeof(sdesc));
	uref.report_type = HID_REPORT_TYPE_FEATURE;
	uref.report_id = HID_REPORT_ID_UNKNOWN;
	uref.usage_code = type;

	snprintf(str, sizeof(str), "Unknown");
	if (ioctl(fd, HIDIOCGUSAGE, &uref) == 0) {
		if ( 0 != (sdesc.index = uref.value)) {
		     if (ioctl(fd, HIDIOCGSTRING, &sdesc) > 0)
			  snprintf(str, sizeof(str), "%s", sdesc.value);
		     else
			  upslog_with_errno(LOG_ERR, "ioctl HIDIOCGSTRING");
		}
	}

	return str;
}

	/* results of querying some x86 page values on my APC UPS */

	/* note to APC: i can decode your "secret protocol" just by        *
	 * looking at it.  you're not helping anyone by keeping it closed! */

	/* 0x860060 == "441HMLL" - looks like a 'capability' string	*/
	/*	    == locale 4, 4 choices, 1 byte each			*/
	/*          == line sensitivity (high, medium, low, low)	*/

	/* 0x860013 == 44200155090 - capability again			*/
	/*          == locale 4, 4 choices, 2 bytes, 00, 15, 50, 90	*/
	/*          == minimum charge to return online			*/

	/* 0x860062 == D43133136127130					*/
	/*          == locale D, 4 choices, 3 bytes, 133, 136, 127, 130	*/
	/*          == high transfer voltage				*/

	/* 0x860064 == D43103100097106					*/
	/*          == locale D, 4 choices, 3 bytes, 103, 100, 097, 106	*/
	/*          == low transfer voltage				*/

	/* 0x860066 == 441HMLL (see 860060)				*/	

	/* 0x860074 == 4410TLN						*/
	/*          == locale 4, 4 choices, 1 byte, 0, T, L, N		*/
	/*          == alarm setting (5s, 30s, low battery, none)	*/

	/* 0x860077 == 443060180300600					*/
	/*          == locale 4, 4 choices, 3 bytes, 060,180,300,600	*/
	/*          == wake-up delay (after power returns)		*/

void upsdrv_updateinfo(void) 
{
	fd_set	fdset;
	struct	timeval	tv;
	int	rd;
	unsigned int	i;
	struct	hiddev_event ev[64];
	time_t	now = 0;

	/* Only poll every third time, to reduce USB traffic. */
	if (ups_poll == poll_interval) {
		ups_poll = 0;
	}
	else {
		ups_poll++;
		dstate_dataok();
		/* Monitor more closely when on batter power, but not quite
		 * as closely if it's an MGE UPS */
		if (!on_batt || (vendor == MGE_VENDOR_ID && (ups_poll%2 == 0)))
			return;
	}

try_again:
	if (on_batt > 0) {
		time(&now);
		if(difftime(now,on_batt) > 5) {
			upslogx(LOG_INFO, "UPS has been on battery power >5 seconds.\n");
			on_batt = -1;
			status_init();
			status_set("OB");
			status_commit();
		}
	}

	/* For some reason, APC XS BX1500 does not report changes
	 * in LOADPCT without HIDIOCINITREPORT. */
	//ioctl(fd, HIDIOCINITREPORT, 0);
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	rd = select(fd+1, &fdset, NULL, NULL, &tv);

	/* XXX: alarm around this read */
	if (rd > 0) {
		rd = read(fd, ev, sizeof(ev));

		if (rd < (int) sizeof(ev[0])) {
			if (rd < 0) {
				if( errno == EIO ) { // MGE on ReadyNAS Pro?
					upslog_with_errno(LOG_INFO, "read error");
					close(fd);
					upslogx(LOG_WARNING, "Waiting for UPS to be re-recognized...");
					for (i = 0; i < 20; i++) {
						rd = open(device_path, O_RDONLY);
						if (rd >= 0)
						{
							close(rd);
							break;
						}
						sleep(1);
					}
					open_usb(device_path, O_RDONLY);
					upslogx(LOG_WARNING, "UPS communicating again after %d seconds.", i+1);
					if (getvalue(BATT_AC_PRESENT) == 1)
					{
						dstate_setinfo("ups.status", "OL");
						on_batt = 0;
					}
					else
					{
						dstate_setinfo("ups.status", "OB");
						on_batt = -1;
						status_init();
						status_set("OB");
						status_commit();
					}
					goto try_again;
				}
				fatal_with_errno("read");
			}
			else {
				upslog_with_errno(LOG_INFO, "short read from device");
				dstate_datastale();
			}
		}
	    
		for (i = 0; i < rd / sizeof(ev[0]); i++)
			parse_event(&ev[i]);
	}	/* if rd > 0 */
	else if (ups_flags & FLAG_MANUAL_POLL)
	{
		static struct ups_state last_ups_state = { 1, 0, 0, 0, 0 };
		struct ups_state new_ups_state;
		memset(&new_ups_state, 0, sizeof(new_ups_state));

		ioctl(fd, HIDIOCINITREPORT, 0);

		new_ups_state.ol = getvalue(BATT_AC_PRESENT);
		new_ups_state.charge = getvalue(BATT_REMAINING_CAPACITY);
		new_ups_state.runtime = getvalue(BATT_RUNTIME_TO_EMPTY);
		new_ups_state.voltage = (float)getvalue(UPS_BATTVOLT) / 100.0;

		if (new_ups_state.charge > 100)
				new_ups_state.charge /= 10;

		if (new_ups_state.charge != last_ups_state.charge)
			dstate_setinfo("battery.charge", "%d", new_ups_state.charge);
		if (new_ups_state.runtime != last_ups_state.runtime)
			dstate_setinfo("battery.runtime", "%d", new_ups_state.runtime);
		if (new_ups_state.voltage != last_ups_state.voltage)
			dstate_setinfo("battery.voltage", "%2.1f", new_ups_state.voltage);

		if (new_ups_state.ol != last_ups_state.ol)
		{
			/* State change */
			if (new_ups_state.ol == 1)
			{
				status_init();
				status_set("OL");
				status_commit();

				upslogx(LOG_WARNING, "UPS switched to line power.");

				if (on_batt)
					on_batt = 0;
			}
			else
			{
				if(on_batt)
				{
					if (on_batt == -1 && getval("lowbatt_pct")) {
						lowbatt_pct = atoi(getval("lowbatt_pct"));
						if (new_ups_state.charge < lowbatt_pct) {
							upslogx(LOG_WARNING,
								"Battery is below the minumum level. (%d/%d)(cur/min)",
								new_ups_state.charge, lowbatt_pct);
							status_init();
							status_set("LB");
							status_commit();
						}
					}
				}
				else
				{
					upslogx(LOG_WARNING, "UPS switched to battery power.");
					time(&on_batt);
				}
			}
		}

		if (new_ups_state.ol)
		{
			new_ups_state.lb = 0;
			if (getvalue(UPS_COMM_LOST) == 1)
				upslogx(LOG_WARNING, "UPS noticed lost communication!");
			if (getvalue(BATT_NEED_REPLACEMENT) == 1)
			{
				upslogx(LOG_WARNING, "UPS battery needs to be replaced!");
				status_init();
				status_set("OL");
				status_set("RB");
				status_commit();
			}
		}
		else if (!last_ups_state.lb)
		{
			if (getvalue(UPS_SHUTDOWN_REQUESTED) == 1)
			{
				upslogx(LOG_WARNING, "UPS shutdown requested.");
				new_ups_state.lb = 1;
			}
			else if (getvalue(UPS_SHUTDOWN_IMMINENT) == 1)
			{
				upslogx(LOG_WARNING, "UPS shutdown imminent!");
				new_ups_state.lb = 1;
			}
			else if (getvalue(BATT_BELOW_RCL) == 1)
			{
				upslogx(LOG_WARNING, "Battery below required charge limit!");
				new_ups_state.lb = 1;
			}
			else if (getvalue(BATT_BELOW_RTL) == 1)
			{
				upslogx(LOG_WARNING, "Battery below required time limit!");
				new_ups_state.lb = 1;
			}
			else if (on_batt == -1 && getval("lowbatt_pct")) {
				lowbatt_pct = atoi(getval("lowbatt_pct"));
				if( new_ups_state.charge < lowbatt_pct ) {
					upslogx(LOG_WARNING,
						"Battery is below the minumum level. (%d/%d)(cur/min)",
						new_ups_state.charge, lowbatt_pct);
					new_ups_state.lb = 1;
				}
			}
			if (new_ups_state.lb)
			{
				status_init();
				status_set("LB");
				status_commit();
			}
		}
		/*upslogx(LOG_WARNING, "BEFORE: %hd, %hd, %hd, %hd, %f", last_ups_state.ol, last_ups_state.runtime, last_ups_state.charge, last_ups_state.lb, last_ups_state.voltage);
		upslogx(LOG_WARNING, "AFTER:  %hd, %hd, %hd, %hd, %f", new_ups_state.ol, new_ups_state.runtime, new_ups_state.charge, new_ups_state.lb, new_ups_state.voltage);*/
		memcpy(&last_ups_state, &new_ups_state, sizeof(struct ups_state));
	}
	dstate_dataok();
}

static void addhidvalue(int query, const char *varname)
{
	int	val;

	val = getvalue(query);

	if (val < -1)
		return;

	/* XXX: deal with "3 million" kernel bogosity for now */
	if (val > 3000000) {
		int	i;

		for (i = 0; i < 5; i++) {
			val = getvalue(query);
			if (val < 3000000)
				break;
		}

		if (val > 3000000) {
			upslogx(LOG_WARNING, "Unable to add 0x%x: got bogus value %d",
				query, val);
			return;
		}
	}

	dstate_setinfo(varname, "%d", val);
	upsdebugx(2, "addhidvalue(%x, %s): obtained %d", 
	  query, varname, val);
}

int setvalue(int type, int value)
{
	struct	hiddev_usage_ref uref;
	struct hiddev_report_info rinfo;

	/* 1) issue a get to retrieve all fields values */
	memset(&uref, 0, sizeof(uref));
	uref.report_type = HID_REPORT_TYPE_FEATURE;
	uref.report_id = HID_REPORT_ID_UNKNOWN;
	uref.field_index = 0;
	uref.usage_index = 0;
	uref.usage_code = type;
	uref.value = 0;

	if (ioctl(fd, HIDIOCGUSAGE, &uref) >= 0) {
		
		upsdebugx(2, "setvalue(%x, %d): obtained %d (report %x))", 
			type, value, uref.value, uref.report_id);
		
		/* get report info */
		rinfo.report_type = HID_REPORT_TYPE_FEATURE;
		rinfo.report_id = uref.report_id;
		
		if (ioctl(fd, HIDIOCGREPORT, &rinfo) >= 0) {
		
			/* set usage in report */
			uref.report_type = HID_REPORT_TYPE_OUTPUT;
			uref.value = value;
			if (ioctl(fd, HIDIOCSUSAGE, &uref) >= 0) {
	
				/* write report to device */
				rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
				rinfo.report_id = uref.report_id;
				if (ioctl(fd, HIDIOCSREPORT, &rinfo) >= 0) {
				
					upslogx(1, "setvalue(): report sent successfully");
					return 1;
				}
			} /* for MGE (maybe others !), use HID_REPORT_TYPE_FEATURE */
			else {
				uref.report_type = HID_REPORT_TYPE_FEATURE;
				if (ioctl(fd, HIDIOCSUSAGE, &uref) >= 0) {
	
					/* write report to device */
					rinfo.report_type = HID_REPORT_TYPE_FEATURE;
					if (ioctl(fd, HIDIOCSREPORT, &rinfo) >= 0) {
					
						upslogx(1, "setvalue(using type = feature): report sent successfully");
						return 1;
					}
				}
			}
		}
		return -1;
	}
	else
		return -1;
}

/* handler for settable variables in UPS*/
int setvar(const char *varname, const char *val)
{
	/* int setvalue(int type, int value) */

	return STAT_SET_UNKNOWN;
}

void upsdrv_initinfo(void) 
{
	int	val;
	char	*str, *ptr;

	dstate_setinfo("ups.mfr", "Generic");
	
	str = getstring(UPS_IMFR);

	if (str)
		dstate_setinfo("ups.mfr", "%s", str);

	str = getstring(UPS_IPRODUCT);

	if (str) {

		/* try to trim this back to something reasonable */
		if ((!strncmp(str, "BackUPS Pro", 11)) ||
			(!strncmp(str, "Back-UPS ", 9)) ||
			(!strncmp(str, "Smart-UPS", 9))) {
			ptr = strstr(str, "FW");
			if (ptr)
				*(ptr - 1) = '\0';

			/* we can be pretty sure of this now */
			dstate_setinfo("ups.mfr", "%s", "APC");
		}

		/* non-Pro models seem to have USB ports too now */
		if ((!strncmp(str, "BackUPS ", 8)) && (isdigit(str[8]))) {
			ptr = strstr(&str[8], " ");
			if (ptr)
				*(ptr - 1) = '\0';

			/* we can be pretty sure of this now */
			dstate_setinfo("ups.mfr", "%s", "APC");
		}

		dstate_setinfo("ups.model", "%s", str);
	}
	else
		dstate_setinfo("ups.model", "Generic USB UPS");

	str = getstring(UPS_ISERIAL);

	if (str)
		dstate_setinfo("ups.serial", "%s", str);

	str = getstring(BATT_ICHEMISTRY);
	if (str)
		dstate_setinfo("battery.chemistry", "%s", str);

	/* seed the status register */

	val = getvalue(BATT_AC_PRESENT);

	if (val == 1) {
		dstate_setinfo("ups.status", "OL");
	}
	else {
		dstate_setinfo("ups.status", "OB");
		on_batt = -1;
	}

	val = getvalue(UPS_BATTVOLT);

	if (val > 0)
		dstate_setinfo("battery.voltage", "%2.1f", val / 100.0);

	/* XXX: set capacitymode to percent */
	addhidvalue(BATT_REMAINING_CAPACITY, "battery.charge");
	addhidvalue(BATT_RUNTIME_TO_EMPTY, "battery.runtime");
	//addhidvalue(UPS_LOADPCT, "ups.load");	

	//addhidvalue(UPS_WAKEDELAY, "ups.delay.start");
	//addhidvalue(UPS_GRACEDELAY, "ups.delay.shutdown");

	/* install handlers */
	upsh.setvar = setvar;

	dstate_dataok();
}

static void open_usb(const char *port, int flags)
{
#ifdef HIDIOCGPHYS
	int  i;
#endif

    /* device paths, like /dev/usb/hiddevN, are unstable in any
     * hotplug-oriented bus (like usb) except in exotic cases
     * where device enumeration order is predictable.
     */
    if (strncmp (port, "usb-", 4) != 0) {
         if ((fd = open (port, flags)) < 0)
              fatal_with_errno("hiddev path open %s", port);
         return;
    }

#ifdef HIDIOCGPHYS
    /* better: stable ids, which don't change unless usb (or pci)
     * topology morphs.  like usb-00:02.3-3.4:0, where
     *  - '00:02.3' is a usb host controller id, in this case a
     *     pci slot name which is stable enough for most purposes.
     *  - '3.4' walks the usb tree:  start at the third root hub
     *     port, which has another hub.  then go to the ups on the
     *     fourth port of that hub.
     *  - ':0' says interface zero on that ups
     *
     * to use those we need to scan all the hid device paths, but
     * when we're done we know that a usb mouse or keyboard config
     * change won't have broken our ups config.
     */
    if (getval ("HidDevPrefix"))
         hiddev_prefix = getval ("HidDevPrefix");
    if (getval ("HidDevLimit"))
         hiddev_limit = atoi (getval ("HidDevLimit"));
    for (i = 0; i < hiddev_limit; i++) {
         char tmp [128];

         snprintf (tmp, sizeof tmp, "%s%d", hiddev_prefix, i);
         if ((fd = open (tmp, flags)) < 0)
              continue;

         if (ioctl (fd, HIDIOCGPHYS(sizeof tmp), &tmp) > 0) {
              if (strcmp (port, tmp) == 0)
                   return;
         }
         close (fd);
    }
    fatalx("can't match %s, tried up to  %s%d",
              port, hiddev_prefix, hiddev_limit);
#else
#warning "kernel doesn't return HIDDEV physical port paths"
    fatalx("can't understand %s, no kernel support", port);
#endif /* HIDIOCGPHYS */
}

void upsdrv_initups(void)
{
	char	*name;
	struct	hiddev_devinfo device_info;

	open_usb(device_path, O_RDONLY);

	ioctl(fd, HIDIOCGDEVINFO, &device_info);
	vendor = device_info.vendor;
	product = device_info.product;

	if ((!((vendor == MGE_VENDOR_ID) && ((product == 0x0001) || (product == 0xFFFF)))) &&
	    (!find_application(fd, UPS_USAGE)) &&
	    (!find_application(fd, POWER_USAGE)) &&
	    (!find_application(fd, -6291455)))
		fatalx("%s is not a UPS\n", device_path);

	if ((vendor == APC_VENDOR_ID) && (product == 0x0003))
		ups_flags |= FLAG_MANUAL_POLL;

	if (((vendor == MGE_VENDOR_ID) && ((product == 0x0001) || (product == 0xFFFF))) ||
	    (ups_flags & FLAG_MANUAL_POLL))
		poll_interval = 7;

	name = getstring(UPS_IPRODUCT);
	printf("Detected %s on port %s\n", name, device_path);

	ioctl(fd, HIDIOCINITREPORT, 0);
}

void upsdrv_cleanup(void)
{
}
