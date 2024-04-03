/* -*- linux-c -*- */
/*
 * USB at76c503/at76c505 driver
 *
 * Copyright (c) 2002 - 2003 Oliver Kurth
 * Copyright (c) 2004 Joerg Albert <joerg.albert@gmx.de>
 * Copyright (c) 2004 Nick Jones
 * Copyright (c) 2004 Balint Seeber <n0_5p4m_p13453@hotmail.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation; either version 2 of
 *	the License, or (at your option) any later version.
 *
 * This file is part of the Berlios driver for WLAN USB devices based on the
 * Atmel AT76C503A/505/505A. See at76c503.h for details.
 *
 * Some iw_handler code was taken from airo.c, (C) 1999 Benjamin Reed
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/usb.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <linux/rtnetlink.h>  /* for rtnl_lock() */
#include <linux/firmware.h>

#ifdef CONFIG_IPAQ_HANDHELD
#include <asm/mach-types.h>
#include <asm/arch/ipaq.h>
#include <asm/arch-pxa/h5400-asic.h>
#include <asm/arch-sa1100/h3600.h>
#endif

#include "at76c503.h"

#if WIRELESS_EXT < 17
#define IW_QUAL_QUAL_UPDATED    0x01
#define IW_QUAL_LEVEL_UPDATED   0x02
#define IW_QUAL_NOISE_UPDATED   0x04
#define IW_QUAL_QUAL_INVALID    0x10
#define IW_QUAL_LEVEL_INVALID   0x20
#define IW_QUAL_NOISE_INVALID   0x40
#endif

#if WIRELESS_EXT < 19
#define IW_QUAL_ALL_UPDATED     0x07
#define IW_QUAL_DBM		0x08
#define IW_QUAL_ALL_INVALID     0x70
#endif

/* timeout in seconds for the usb_control_msg in get_cmd_status
 * and set_card_command
 */
#ifndef USB_CTRL_GET_TIMEOUT
# define USB_CTRL_GET_TIMEOUT 5
#endif

/* number of endpoints of an interface */
#define NUM_EP(intf) (intf)->altsetting[0].desc.bNumEndpoints
#define EP(intf,nr) (intf)->altsetting[0].endpoint[(nr)].desc
#define GET_DEV(udev) usb_get_dev((udev))
#define PUT_DEV(udev) usb_put_dev((udev))
#define SET_NETDEV_OWNER(ndev,owner) /* not needed anymore ??? */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
#define gfp_t int
#endif

/* Backwards compatibility for usb_kill_urb() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 10)
#  define usb_kill_urb usb_unlink_urb
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 9)
# define eth_hdr(s) (s)->mac.ethernet
# define set_eth_hdr(s,p) (s)->mac.ethernet=(p)
#else
# define set_eth_hdr(s,p) (s)->mac.raw=(unsigned char *)(p)
#endif

/* wireless extension level this source currently supports */
#define WIRELESS_EXT_SUPPORTED	16

#ifndef USB_ASYNC_UNLINK
#ifdef URB_ASYNC_UNLINK
#define USB_ASYNC_UNLINK	URB_ASYNC_UNLINK
#else
#define USB_ASYNC_UNLINK	0
#endif
#endif

#ifndef FILL_BULK_URB
#define FILL_BULK_URB(a,b,c,d,e,f,g) usb_fill_bulk_urb(a,b,c,d,e,f,g)
#endif

int at76_debug = DBG_DEFAULTS;

/* uncond. debug output */
#define dbg_uc(format, arg...) \
  printk(KERN_DEBUG __FILE__ ": " format "\n" , ## arg)

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#define assert(x) \
  do {\
   if (!(x)) \
     err("%d: assertion " #x " failed", __LINE__);\
  } while (0)

/* how often do we re-try these packets ? */
#define AUTH_RETRIES  3
#define ASSOC_RETRIES 3
#define DISASSOC_RETRIES 3



static unsigned long spin_l_istate_flags;
#define LOCK_ISTATE()   spin_lock_irqsave(&dev->istate_spinlock,spin_l_istate_flags);
#define UNLOCK_ISTATE() spin_unlock_irqrestore(&dev->istate_spinlock,spin_l_istate_flags);


#define NEW_STATE(dev,newstate) \
  do {\
    scan_hook(newstate == SCANNING);		\
    LOCK_ISTATE()				\
    dbg(DBG_PROGRESS, "%s: state %d -> %d (" #newstate ")",\
        dev->netdev->name, dev->istate, newstate);\
    dev->istate = newstate;\
    UNLOCK_ISTATE()	  \
  } while (0)

/* the beacon timeout in infra mode when we are connected (in seconds) */
#define BEACON_TIMEOUT 10

/* the interval in ticks we poll if scan is completed */
#define SCAN_POLL_INTERVAL (HZ/4)


/* Version Information */
#define DRIVER_NAME "at76_usb"
#define DRIVER_AUTHOR \
"Oliver Kurth <oku@masqmail.cx>, Joerg Albert <joerg.albert@gmx.de>, " \
"Alex <alex@foogod.com>, Nick Jones, Balint Seeber <n0_5p4m_p13453@hotmail.com>"
#define DRIVER_DESC "Atmel at76c50x USB Wireless LAN Driver"


/* Module paramaters */
module_param_named(debug, at76_debug, int, 0600);
MODULE_PARM_DESC(debug, "Debugging level");

static int rx_copybreak = 200;
module_param(rx_copybreak, int, 0400);
MODULE_PARM_DESC(rx_copybreak, "rx packet copy threshold");

static int scan_min_time = 10;
module_param(scan_min_time, int, 0400);
MODULE_PARM_DESC(scan_min_time, "scan min channel time (default: 10)");

static int scan_max_time = 120;
module_param(scan_max_time, int, 0400);
MODULE_PARM_DESC(scan_max_time, "scan max channel time (default: 120)");

static int scan_mode = SCAN_TYPE_ACTIVE;
module_param(scan_mode, int, 0400);
MODULE_PARM_DESC(scan_mode, "scan mode: 0 active (with ProbeReq, default), 1 passive");

static int preamble_type = PREAMBLE_TYPE_LONG;
module_param(preamble_type, int, 0400);
MODULE_PARM_DESC(preamble_type, "preamble type: 0 long (default), 1 short");

static int auth_mode = 0;
module_param(auth_mode, int, 0400);
MODULE_PARM_DESC(auth_mode, "authentication mode: 0 open system (default), "
		 "1 shared secret");

static int pm_mode = PM_ACTIVE;
module_param(pm_mode, int, 0400);
MODULE_PARM_DESC(pm_mode, "power management mode: 1 active (def.), 2 powersave, 3 smart save");

static int pm_period = 0;
module_param(pm_period, int, 0400);
MODULE_PARM_DESC(pm_period, "period of waking up the device in usec");

static int international_roaming = IR_OFF;
module_param(international_roaming, int, 0400);
MODULE_PARM_DESC(international_roaming, "enable international roaming: 0 (no, default), 1 (yes)");

static int default_iw_mode = IW_MODE_INFRA;
module_param(default_iw_mode, int, 0400);
MODULE_PARM_DESC(default_iw_mode, "default IW mode for a new device: "
		 "1 (ad-hoc), 2 (infrastructure, def.), 6 (monitor mode)");

static int monitor_scan_min_time = 50;
module_param(monitor_scan_min_time, int, 0400);
MODULE_PARM_DESC(monitor_scan_min_time, "scan min channel time in MONITOR MODE (default: 50)");

static int monitor_scan_max_time = 600;
module_param(monitor_scan_max_time, int, 0400);
MODULE_PARM_DESC(monitor_scan_max_time, "scan max channel time in MONITOR MODE (default: 600)");

static char* netdev_name = "wlan%d";
module_param(netdev_name, charp, 0400);
MODULE_PARM_DESC(netdev_name, "network device name (default is wlan%d)");


#define DEF_RTS_THRESHOLD 1536
#define DEF_FRAG_THRESHOLD 1536
#define DEF_SHORT_RETRY_LIMIT 8
//#define DEF_LONG_RETRY_LIMIT 4
#define DEF_CHANNEL 10

#define MAX_RTS_THRESHOLD (MAX_FRAG_THRESHOLD + 1)

/* The frequency of each channel in MHz */
static const long channel_frequency[] = {
        2412, 2417, 2422, 2427, 2432, 2437, 2442,
        2447, 2452, 2457, 2462, 2467, 2472, 2484
};
#define NUM_CHANNELS ( sizeof(channel_frequency) / sizeof(channel_frequency[0]) )

/* the broadcast address */
static const u8 bc_addr[ETH_ALEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
static const u8 off_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* the supported rates of this hardware, bit7 marks a basic rate */
static const u8 hw_rates[4] = {0x82,0x84,0x0b,0x16};

/* the max padding size for tx in bytes (see calc_padding)*/
#define MAX_PADDING_SIZE 53

/* the size of the ieee802.11 header (excl. the at76c503 tx header) */
#define IEEE802_11_MGMT_HEADER_SIZE offsetof(struct ieee80211_hdr_3addr, payload)

#define BEACON_MAX_DATA_LENGTH 1500
/* beacon in ieee80211_hdr_3addr.payload */
struct ieee802_11_beacon_data {
	u8	timestamp[8];           // TSFTIMER
	__le16	beacon_interval;         // Kms between TBTTs (Target Beacon Transmission Times)
	__le16	capability_information;
	u8	data[BEACON_MAX_DATA_LENGTH]; /* contains: SSID (tag,length,value), 
						 Supported Rates (tlv), channel */
} __attribute__ ((packed));

/* disassoc frame in ieee80211_hdr_3addr.payload */
struct ieee802_11_disassoc_frame {
	__le16 reason;
} __attribute__ ((packed));
#define DISASSOC_FRAME_SIZE \
  (AT76C503_TX_HDRLEN + IEEE802_11_MGMT_HEADER_SIZE +\
   sizeof(struct ieee802_11_disassoc_frame))

/* assoc request in ieee80211_hdr_3addr.payload */
struct ieee802_11_assoc_req {
	__le16	capability;
	__le16	listen_interval;
	u8	data[1]; /* variable number of bytes for SSID 
			 and supported rates (tlv coded) */
};
/* the maximum size of an AssocReq packet */
#define ASSOCREQ_MAX_SIZE \
  (AT76C503_TX_HDRLEN + IEEE802_11_MGMT_HEADER_SIZE +\
   offsetof(struct ieee802_11_assoc_req,data) +\
   1+1+IW_ESSID_MAX_SIZE + 1+1+4)

/* reassoc request in ieee80211_hdr_3addr.payload */
struct ieee802_11_reassoc_req {
	__le16	capability;
	__le16	listen_interval;
	u8	curr_ap[ETH_ALEN]; /* the bssid of the AP we are
				   currently associated to */
	u8	data[1]; /* variable number of bytes for SSID 
			 and supported rates (tlv coded) */
} __attribute__ ((packed));

/* the maximum size of an AssocReq packet */
#define REASSOCREQ_MAX_SIZE \
  (AT76C503_TX_HDRLEN + IEEE802_11_MGMT_HEADER_SIZE +\
   offsetof(struct ieee802_11_reassoc_req,data) +\
   1+1+IW_ESSID_MAX_SIZE + 1+1+4)


/* assoc/reassoc response */
struct ieee802_11_assoc_resp {
	__le16	capability;
	__le16	status;
	__le16	assoc_id;
	u8	data[1]; /* variable number of bytes for 
			 supported rates (tlv coded) */
} __attribute__ ((packed));

/* auth. request/response in ieee80211_hdr_3addr.payload */
struct ieee802_11_auth_frame {
	__le16 algorithm;
	__le16 seq_nr;
	__le16 status;
	u8 challenge[0];
} __attribute__ ((packed));
/* for shared secret auth, add the challenge text size */
#define AUTH_FRAME_SIZE \
  (AT76C503_TX_HDRLEN + IEEE802_11_MGMT_HEADER_SIZE +\
   sizeof(struct ieee802_11_auth_frame))

/* deauth frame in ieee80211_hdr_3addr.payload */
struct ieee802_11_deauth_frame {
	__le16 reason;
} __attribute__ ((packed));
#define DEAUTH_FRAME_SIZE \
  (AT76C503_TX_HDRLEN + IEEE802_11_MGMT_HEADER_SIZE +\
   sizeof(struct ieee802_11_disauth_frame))


#define KEVENT_CTRL_HALT 1
#define KEVENT_NEW_BSS 2
#define KEVENT_SET_PROMISC 3
#define KEVENT_MGMT_TIMEOUT 4
#define KEVENT_SCAN 5 
#define KEVENT_JOIN 6
#define KEVENT_STARTIBSS 7
#define KEVENT_SUBMIT_RX 8
#define KEVENT_RESTART 9 /* restart the device */
#define KEVENT_ASSOC_DONE  10 /* execute the power save settings:
			     listen interval, pm mode, assoc id */
#define KEVENT_EXTERNAL_FW  11
#define KEVENT_INTERNAL_FW  12
#define KEVENT_RESET_DEVICE 13


static u8 snapsig[] = {0xaa, 0xaa, 0x03};
//#ifdef COLLAPSE_RFC1042
/* RFC 1042 encapsulates Ethernet frames in 802.2 SNAP (0xaa, 0xaa, 0x03) with
 * a SNAP OID of 0 (0x00, 0x00, 0x00) */
static u8 rfc1042sig[] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00};
//#endif /* COLLAPSE_RFC1042 */

/* local function prototypes */
static void iwspy_update(struct at76c503 *dev, struct at76c503_rx_buffer *buf);

static void at76c503_read_bulk_callback (struct urb *urb);
static void at76c503_write_bulk_callback(struct urb *urb);
static void defer_kevent (struct at76c503 *dev, int flag);
static struct bss_info *find_matching_bss(struct at76c503 *dev,
					  struct bss_info *curr);
static int auth_req(struct at76c503 *dev, struct bss_info *bss, int seq_nr,
		    u8 *challenge);
static int disassoc_req(struct at76c503 *dev, struct bss_info *bss);
static int assoc_req(struct at76c503 *dev, struct bss_info *bss);
static int reassoc_req(struct at76c503 *dev, struct bss_info *curr,
		       struct bss_info *new);
static void dump_bss_table(struct at76c503 *dev, int force_output);
static int submit_rx_urb(struct at76c503 *dev);
static int startup_device(struct at76c503 *dev);

static int set_iroaming(struct at76c503 *dev, int onoff);
static void set_monitor_mode(struct at76c503 *dev, int use_prism);

/* second step of initialization (after fw download) */
static int init_new_device(struct at76c503 *dev);

/* some abbrev. for wireless events */
static inline void iwevent_scan_complete(struct net_device *dev)
{
	union iwreq_data wrqu;
	wrqu.data.length = 0;
	wrqu.data.flags = 0;
	wireless_send_event(dev, SIOCGIWSCAN, &wrqu, NULL);
	dbg(DBG_WE_EVENTS, "%s: SIOCGIWSCAN sent", dev->name);
}
static inline void iwevent_bss_connect(struct net_device *dev, u8 *bssid)
{
	union iwreq_data wrqu;
	wrqu.data.length = 0;
	wrqu.data.flags = 0;
	memcpy(wrqu.ap_addr.sa_data, bssid, ETH_ALEN);
	wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	wireless_send_event(dev, SIOCGIWAP, &wrqu, NULL);
	dbg(DBG_WE_EVENTS, "%s: %s: SIOCGIWAP sent", dev->name, __FUNCTION__);
}

static inline void iwevent_bss_disconnect(struct net_device *dev)
{
	union iwreq_data wrqu;
	wrqu.data.length = 0;
	wrqu.data.flags = 0;
	memset(wrqu.ap_addr.sa_data, '\0', ETH_ALEN);
	wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	wireless_send_event(dev, SIOCGIWAP, &wrqu, NULL);
	dbg(DBG_WE_EVENTS, "%s: %s: SIOCGIWAP sent", dev->name, __FUNCTION__);
}


/* hexdump len many bytes from buf into obuf, separated by delim,
   add a trailing \0 into obuf */
static char *hex2str(char *obuf, u8 *buf, int len, char delim)
{
#define BIN2HEX(x) ((x) < 10 ? '0'+(x) : (x)+'A'-10)

  char *ret = obuf;
  while (len--) {
    *obuf++ = BIN2HEX(*buf>>4);
    *obuf++ = BIN2HEX(*buf&0xf);
    if (delim != '\0')
      *obuf++ = delim;
    buf++;
  }
  if (delim != '\0' && obuf > ret)
	  obuf--; // remove last inserted delimiter
  *obuf = '\0';

  return ret;
}

/* == PROC is_cloaked_ssid ==
   returns != 0, if the given SSID is a cloaked one:
   - length 0
   - length > 0, all bytes are \0
   - length == 1, SSID ' '
*/
static inline int is_cloaked_ssid(u8 *ssid, int length)
{
	static const u8 zeros[32];

	return (length == 0) || 
		(length == 1 && *ssid == ' ') ||
		(length > 0 && !memcmp(ssid,zeros,length));
}

static inline void free_bss_list(struct at76c503 *dev)
{
	struct list_head *next, *ptr;
	unsigned long flags;

	spin_lock_irqsave(&dev->bss_list_spinlock, flags);

	dev->curr_bss = dev->new_bss = NULL;

	list_for_each_safe(ptr, next, &dev->bss_list) {
		list_del(ptr);
		kfree(list_entry(ptr, struct bss_info, list));
	}

	spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);
}

static inline char *mac2str(u8 *mac)
{
	static char str [6*3];
  
	sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return str;
}

static void scan_hook(int blink)
{
#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400()) {
		if (blink)
			ipaq_led_blink (RED_LED, 1, 2);
		else
			ipaq_led_off (RED_LED);
	}
#endif
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8)

/* == PROC analyze_usb_config == 
   This procedure analyzes the configuration after the
   USB device got reset and find the start of the interface and the
   two endpoint descriptors.
   Returns < 0 if the descriptors seems to be wrong. */
static int analyze_usb_config(u8 *cfgd, int cfgd_len,
				int *intf_idx, int *ep0_idx, int *ep1_idx)
{
	u8 *cfgd_start = cfgd;
	u8 *cfgd_end = cfgd + cfgd_len; /* first byte after config descriptor */
	int nr_intf=0, nr_ep=0; /* number of interface, number of endpoint descr.
			       found */

	assert(cfgd_len >= 2);
	if (cfgd_len < 2)
		return -1;

	if (*(cfgd+1) != USB_DT_CONFIG) {
		err("not a config descriptor");
		return -2;
	}

	if (*cfgd != USB_DT_CONFIG_SIZE) {
		err("invalid length for config descriptor: %d", *cfgd);
		return -3;
	}

	/* scan the config descr */
	while ((cfgd+1) < cfgd_end) {

		switch (*(cfgd+1)) {

		case USB_DT_INTERFACE:
			nr_intf++;
			if (nr_intf == 1)
				*intf_idx = cfgd - cfgd_start;
			break;

		case USB_DT_ENDPOINT:
			nr_ep++;
			if (nr_ep == 1)
				*ep0_idx = cfgd - cfgd_start;
			else
				if (nr_ep == 2)
					*ep1_idx = cfgd - cfgd_start;
			break;
		default:
			;
		}
		cfgd += *cfgd;
	} /* while ((cfgd+1) < cfgd_end) */

	if (nr_ep != 2 || nr_intf != 1) {
		err("unexpected nr of intf (%d) or endpoints (%d)",
		    nr_intf, nr_ep);
		return -4;
	}

	return 0;
} /* end of analyze_usb_config */



/* == PROC update_usb_intf_descr ==
   currently (2.6.0-test2) usb_reset_device() does not recognize that
   the interface descr. are changed.
   This procedure reads the configuration and does a limited parsing of
   the interface and endpoint descriptors.
   This is IMHO needed until usb_reset_device() is changed inside the
   kernel's USB subsystem.
   Copied from usb/core/config.c:usb_get_configuration()

   THIS IS VERY UGLY CODE - DO NOT COPY IT ! */

#define AT76C503A_USB_CONFDESCR_LEN 0x20
/* the short configuration descriptor before reset */
//#define AT76C503A_USB_SHORT_CONFDESCR_LEN 0x19

static int update_usb_intf_descr(struct at76c503 *dev)
{
	int intf0; /* begin of intf descriptor in configuration */ 
	int ep0, ep1; /* begin of endpoint descriptors */

	struct usb_device *udev = dev->udev;
	struct usb_config_descriptor *cfg_desc;
	int result = 0, size;
	u8 *buffer;
	struct usb_host_interface *ifp;
	int i;

	dbg(DBG_DEVSTART, "%s: ENTER", __FUNCTION__);

	cfg_desc = (struct usb_config_descriptor *)
		kmalloc(AT76C503A_USB_CONFDESCR_LEN, GFP_KERNEL);
	if (!cfg_desc) {
		err("cannot kmalloc config desc");
		return -ENOMEM;
	}

	result = usb_get_descriptor(udev, USB_DT_CONFIG, 0,
				    cfg_desc, AT76C503A_USB_CONFDESCR_LEN);
	if (result < AT76C503A_USB_CONFDESCR_LEN) {
		if (result < 0)
			err("unable to get descriptor");
		else {
			err("config descriptor too short (expected >= %i, got %i)",
			    AT76C503A_USB_CONFDESCR_LEN, result);
			result = -EINVAL;
		}
		goto err;
	}

	/* now check the config descriptor */
	le16_to_cpus(&cfg_desc->wTotalLength);
	size = cfg_desc->wTotalLength;
	buffer = (u8 *)cfg_desc;
	
	if (cfg_desc->bNumInterfaces > 1) {
		err("found %d interfaces", cfg_desc->bNumInterfaces);
		result = - EINVAL;
		goto err;
	}

	if ((result=analyze_usb_config(buffer, size, &intf0, &ep0, &ep1))) {

		err("analyze_usb_config returned %d for config desc %s",
		    result,
		    hex2str(dev->obuf, (u8 *)cfg_desc, 
			    min((int)(sizeof(dev->obuf)-1)/2,size), '\0'));
		result=-EINVAL;
		goto err;
	}

	/* we got the correct config descriptor - update the interface's endpoints */
	ifp = &udev->actconfig->interface[0]->altsetting[0];

	if (ifp->endpoint)
		kfree(ifp->endpoint);

	memcpy(&ifp->desc, buffer+intf0, USB_DT_INTERFACE_SIZE);

	if (!(ifp->endpoint = kmalloc(2 * sizeof(struct usb_host_endpoint), 
				      GFP_KERNEL))) {
		result = -ENOMEM;
		goto err;
	}
	memset(ifp->endpoint, 0, 2 * sizeof(struct usb_host_endpoint));
	memcpy(&ifp->endpoint[0].desc, buffer+ep0, USB_DT_ENDPOINT_SIZE);
	le16_to_cpus(&ifp->endpoint[0].desc.wMaxPacketSize);
	memcpy(&ifp->endpoint[1].desc, buffer+ep1, USB_DT_ENDPOINT_SIZE);
	le16_to_cpus(&ifp->endpoint[1].desc.wMaxPacketSize);

	/* we must set the max packet for the new ep (see usb_set_maxpacket() ) */

#define usb_endpoint_out(ep_dir)	(!((ep_dir) & USB_DIR_IN))
	for(i=0; i < ifp->desc.bNumEndpoints; i++) {
		struct usb_endpoint_descriptor	*d = &ifp->endpoint[i].desc;		
		int b = d->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
		if (usb_endpoint_out(d->bEndpointAddress)) {
			if (d->wMaxPacketSize > udev->epmaxpacketout[b])
				udev->epmaxpacketout[b] = d->wMaxPacketSize;
		} else {
			if (d->wMaxPacketSize > udev->epmaxpacketin[b])
				udev->epmaxpacketin[b] = d->wMaxPacketSize;
		}
	}
			     
	dbg(DBG_DEVSTART, "%s: ifp %p num_altsetting %d "
	    "endpoint addr x%x, x%x", __FUNCTION__,
	    ifp, udev->actconfig->interface[0]->num_altsetting,
	    ifp->endpoint[0].desc.bEndpointAddress,
	    ifp->endpoint[1].desc.bEndpointAddress);
	result = 0;
err:
	kfree(cfg_desc);
	dbg(DBG_DEVSTART, "%s: EXIT with %d", __FUNCTION__, result);
	return result;
} /* update_usb_intf_descr */
		
#endif	/* #if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8) */


static int at76c503_remap(struct usb_device *udev)
{
	int ret;
	ret = usb_control_msg(udev, usb_sndctrlpipe(udev,0),
			      0x0a, INTERFACE_VENDOR_REQUEST_OUT,
			      0, 0,
			      NULL, 0, HZ * USB_CTRL_GET_TIMEOUT);
	if (ret < 0)
		return ret;

	return 0;
}


static int get_op_mode(struct usb_device *udev)
{
	int ret;
	u8 op_mode;

	ret = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			      0x33, INTERFACE_VENDOR_REQUEST_IN,
			      0x01, 0,
			      &op_mode, 1, HZ * USB_CTRL_GET_TIMEOUT);
	if(ret < 0)
		return ret;
	return op_mode;
}

/* this loads a block of the second part of the firmware */
static inline int load_ext_fw_block(struct usb_device *udev,
		      int i, unsigned char *buf, int bsize)
{
	return usb_control_msg(udev, usb_sndctrlpipe(udev,0),
			       0x0e, DEVICE_VENDOR_REQUEST_OUT,
			       0x0802, i,
			       buf, bsize, HZ * USB_CTRL_GET_TIMEOUT);
}

static inline int get_hw_cfg_rfmd(struct usb_device *udev,
	       unsigned char *buf, int buf_size)
{
	return usb_control_msg(udev, usb_rcvctrlpipe(udev,0),
			       0x33, INTERFACE_VENDOR_REQUEST_IN,
			       ((0x0a << 8) | 0x02), 0,
			       buf, buf_size, HZ * USB_CTRL_GET_TIMEOUT);
}

/* Intersil boards use a different "value" for GetHWConfig requests */
static inline
int get_hw_cfg_intersil(struct usb_device *udev,
	       unsigned char *buf, int buf_size)
{
	return usb_control_msg(udev, usb_rcvctrlpipe(udev,0),
			       0x33, INTERFACE_VENDOR_REQUEST_IN,
			       ((0x09 << 8) | 0x02), 0,
			       buf, buf_size, HZ * USB_CTRL_GET_TIMEOUT);
}

/* Get the hardware configuration for the adapter and place the appropriate
 * data in the appropriate fields of 'dev' (the GetHWConfig request and
 * interpretation of the result depends on the type of board we're dealing
 * with) */
static int get_hw_config(struct at76c503 *dev)
{
	int ret;
	union {
		struct hwcfg_intersil i;
		struct hwcfg_rfmd     r3;
		struct hwcfg_r505     r5;
	} *hwcfg = kmalloc(sizeof(*hwcfg), GFP_KERNEL);

	if (!hwcfg)
		return -ENOMEM;

	switch (dev->board_type) {

	  case BOARDTYPE_503_INTERSIL_3861:
	  case BOARDTYPE_503_INTERSIL_3863:
		ret = get_hw_cfg_intersil(dev->udev, (unsigned char *)&hwcfg->i, sizeof(hwcfg->i));
		if (ret < 0) break;
		memcpy(dev->mac_addr, hwcfg->i.mac_addr, ETH_ALEN);
		memcpy(dev->cr31_values, hwcfg->i.cr31_values, 14);
		memcpy(dev->cr58_values, hwcfg->i.cr58_values, 14);
		memcpy(dev->pidvid, hwcfg->i.pidvid, 4);
		dev->regulatory_domain = hwcfg->i.regulatory_domain;
		break;

	  case BOARDTYPE_503_RFMD:
	  case BOARDTYPE_503_RFMD_ACC:
		ret = get_hw_cfg_rfmd(dev->udev, (unsigned char *)&hwcfg->r3, sizeof(hwcfg->r3));
		if (ret < 0) break;
		memcpy(dev->cr20_values, hwcfg->r3.cr20_values, 14);
		memcpy(dev->cr21_values, hwcfg->r3.cr21_values, 14);
		memcpy(dev->bb_cr, hwcfg->r3.bb_cr, 14);
		memcpy(dev->pidvid, hwcfg->r3.pidvid, 4);
		memcpy(dev->mac_addr, hwcfg->r3.mac_addr, ETH_ALEN);
		dev->regulatory_domain = hwcfg->r3.regulatory_domain;
		memcpy(dev->low_power_values, hwcfg->r3.low_power_values, 14);
		memcpy(dev->normal_power_values, hwcfg->r3.normal_power_values, 14);
		break;

	  case BOARDTYPE_505_RFMD:
	  case BOARDTYPE_505_RFMD_2958:
	  case BOARDTYPE_505A_RFMD_2958:
		ret = get_hw_cfg_rfmd(dev->udev, (unsigned char *)&hwcfg->r5, sizeof(hwcfg->r5));
		if (ret < 0) break;
		memcpy(dev->cr39_values, hwcfg->r5.cr39_values, 14);
		memcpy(dev->bb_cr, hwcfg->r5.bb_cr, 14);
		memcpy(dev->pidvid, hwcfg->r5.pidvid, 4);
		memcpy(dev->mac_addr, hwcfg->r5.mac_addr, ETH_ALEN);
		dev->regulatory_domain = hwcfg->r5.regulatory_domain;
		memcpy(dev->cr15_values, hwcfg->r5.cr15_values, 14);
		break;

	  default:
		err("Bad board type set (%d).  Unable to get hardware config.", dev->board_type);
		ret = -EINVAL;
	}

	kfree(hwcfg);

	if (ret < 0) {
		err("Get HW Config failed (%d)", ret);
	}
	return ret;
}

/* == PROC getRegDomain == */
static struct reg_domain const *getRegDomain(u16 code)
{
	static struct reg_domain const fd_tab[] = {
		{0x10, "FCC (U.S)", 0x7ff}, /* ch 1-11 */
		{0x20, "IC (Canada)", 0x7ff}, /* ch 1-11 */
		{0x30, "ETSI (Europe - (Spain+France)", 0x1fff},  /* ch 1-13 */
		{0x31, "Spain", 0x600},    /* ch 10,11 */
		{0x32, "France", 0x1e00},  /* ch 10-13 */
		{0x40, "MKK (Japan)", 0x2000},  /* ch 14 */
		{0x41, "MKK1 (Japan)", 0x3fff},  /* ch 1-14 */
		{0x50, "Israel", 0x3fc},  /* ch 3-9 */
	};
	static int const tab_len = sizeof(fd_tab) / sizeof(struct reg_domain);

	/* use this if an unknown code comes in */
	static struct reg_domain const unknown = 
		{0, "<unknown>", 0xffffffff};
  
	int i;

	for(i=0; i < tab_len; i++)
		if (code == fd_tab[i].code)
			break;
  
	return (i >= tab_len) ? &unknown : &fd_tab[i];
} /* getFreqDomain */

static inline int get_mib(struct usb_device *udev,
	    u16 mib, u8 *buf, int buf_size)
{
	return usb_control_msg(udev, usb_rcvctrlpipe(udev,0),
			       0x33, INTERFACE_VENDOR_REQUEST_IN,
			       mib << 8, 0,
			       buf, buf_size, HZ * USB_CTRL_GET_TIMEOUT);
}

static inline int get_cmd_status(struct usb_device *udev,
		   u8 cmd, u8 *cmd_status)
{
	return usb_control_msg(udev, usb_rcvctrlpipe(udev,0),
			       0x22, INTERFACE_VENDOR_REQUEST_IN,
			       cmd, 0,
			       cmd_status, 40, HZ * USB_CTRL_GET_TIMEOUT);
}

#define EXT_FW_BLOCK_SIZE 1024
static int download_external_fw(struct usb_device *udev, u8 *buf, int size)
{
	int i = 0, ret = 0;
	u8 *block;

	if (size < 0) return -EINVAL;
	if ((size > 0) && (buf == NULL)) return -EFAULT;

	block = kmalloc(EXT_FW_BLOCK_SIZE, GFP_KERNEL);
	if (block == NULL) return -ENOMEM;

	dbg(DBG_DEVSTART, "downloading external firmware");

	while(size > 0){
		int bsize = size > EXT_FW_BLOCK_SIZE ? EXT_FW_BLOCK_SIZE : size;

		memcpy(block, buf, bsize);
		dbg(DBG_DEVSTART,
		    "ext fw, size left = %5d, bsize = %4d, i = %2d", size, bsize, i);
		if((ret = load_ext_fw_block(udev, i, block, bsize)) < 0){
			err("load_ext_fw_block failed: %d, i = %d", ret, i);
			goto exit;
		}
		buf += bsize;
		size -= bsize;
		i++;
	}

	/* for fw >= 0.100, the device needs
	   an extra empty block: */
	if((ret = load_ext_fw_block(udev, i, block, 0)) < 0){
		err("load_ext_fw_block failed: %d, i = %d", ret, i);
		goto exit;
	}

 exit:
	kfree(block);
	return ret;
}

static int set_card_command(struct usb_device *udev, int cmd,
		    unsigned char *buf, int buf_size)
{
	int ret;
	struct at76c503_command *cmd_buf =
		(struct at76c503_command *)kmalloc(
			sizeof(struct at76c503_command) + buf_size,
			GFP_KERNEL);

	if(cmd_buf){
		cmd_buf->cmd = cmd;
		cmd_buf->reserved = 0;
		cmd_buf->size = cpu_to_le16(buf_size);
		if(buf_size > 0)
			memcpy(&(cmd_buf[1]), buf, buf_size);
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev,0),
				      0x0e, DEVICE_VENDOR_REQUEST_OUT,
				      0, 0,
				      cmd_buf,
				      sizeof(struct at76c503_command) + buf_size,
				      HZ * USB_CTRL_GET_TIMEOUT);
		kfree(cmd_buf);
		return ret;
	}

	return -ENOMEM;
}

#define MAKE_CMD_STATUS_CASE(c)	case (c): return #c

static const char* get_cmd_status_string(u8 cmd_status)
{
	switch (cmd_status)
	{
		MAKE_CMD_STATUS_CASE(CMD_STATUS_IDLE);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_COMPLETE);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_UNKNOWN);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_INVALID_PARAMETER);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_FUNCTION_NOT_SUPPORTED);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_TIME_OUT);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_IN_PROGRESS);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_HOST_FAILURE);
		MAKE_CMD_STATUS_CASE(CMD_STATUS_SCAN_FAILED);
	}

	return "UNKNOWN";
}

/* TODO: should timeout */
static int wait_completion(struct at76c503 *dev, int cmd)
{
	u8 *cmd_status = kmalloc(40, GFP_KERNEL);
	struct net_device *netdev = dev->netdev;
	int ret = 0;

	do{
		ret = get_cmd_status(dev->udev, cmd, cmd_status);
		if(ret < 0){
			err("%s: get_cmd_status failed: %d", netdev->name, ret);
			break;
		}

		dbg(DBG_WAIT_COMPLETE, "%s: Waiting on cmd %d, cmd_status[5] = %d (%s)",
		    dev->netdev->name, cmd, cmd_status[5], get_cmd_status_string(cmd_status[5]));

		if(cmd_status[5] == CMD_STATUS_IN_PROGRESS ||
		   cmd_status[5] == CMD_STATUS_IDLE){
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ/10); // 100 ms
		}else break;
	}while(1);

	if (ret >= 0)
		/* if get_cmd_status did not fail, return the status
		   retrieved */
		ret = cmd_status[5];
	kfree(cmd_status);
	return ret;
}

static int set_mib(struct at76c503 *dev, struct set_mib_buffer *buf)
{
	struct usb_device *udev = dev->udev;
	int ret;
	struct at76c503_command *cmd_buf =
		(struct at76c503_command *)kmalloc(
			sizeof(struct at76c503_command) + buf->size + 4,
			GFP_KERNEL);

	if(cmd_buf){
		cmd_buf->cmd = CMD_SET_MIB;
		cmd_buf->reserved = 0;
		cmd_buf->size = cpu_to_le16(buf->size + 4);
		memcpy(&(cmd_buf[1]), buf, buf->size + 4);
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev,0),
				      0x0e, DEVICE_VENDOR_REQUEST_OUT,
				      0, 0,
				      cmd_buf,
				      sizeof(struct at76c503_command) + buf->size + 4,
				      HZ * USB_CTRL_GET_TIMEOUT);
		if (ret >= 0)
			if ((ret=wait_completion(dev, CMD_SET_MIB)) != 
			    CMD_STATUS_COMPLETE) {
				info("%s: set_mib: wait_completion failed with %d",
				     dev->netdev->name, ret);
				ret = -156; /* ??? */
			}
		kfree(cmd_buf);
		return ret;
	}

	return -ENOMEM;
}

/* return < 0 on error, == 0 if no command sent, == 1 if cmd sent */
static int set_radio(struct at76c503 *dev, int on_off)
{
	int ret;

	if(dev->radio_on != on_off){
		ret = set_card_command(dev->udev, CMD_RADIO, NULL, 0);
		if(ret < 0){
			err("%s: set_card_command(CMD_RADIO) failed: %d", dev->netdev->name, ret);
		} else
			ret = 1;
		dev->radio_on = on_off;
	} else
		ret = 0;
	return ret;
}


/* == PROC set_pm_mode ==
   sets power save modi (PM_ACTIVE/PM_SAVE/PM_SMART_SAVE) */
static int set_pm_mode(struct at76c503 *dev, u8 mode)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC_MGMT;
	dev->mib_buf.size = 1;
	dev->mib_buf.index = POWER_MGMT_MODE_OFFSET;

	dev->mib_buf.data[0] = mode;

	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (pm_mode) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

/* == PROC set_associd ==
   sets the assoc id for power save mode */
static int set_associd(struct at76c503 *dev, u16 id)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC_MGMT;
	dev->mib_buf.size = 2;
	dev->mib_buf.index = STATION_ID_OFFSET;

	dev->mib_buf.data[0] = id & 0xff;
	dev->mib_buf.data[1] = id >> 8;

	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (associd) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

/* == PROC set_listen_interval ==
   sets the listen interval for power save mode.
   really needed, as we have a similar parameter in the assocreq ??? */
static int set_listen_interval(struct at76c503 *dev, u16 interval)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC;
	dev->mib_buf.size = 2;
	dev->mib_buf.index = STATION_ID_OFFSET;

	dev->mib_buf.data[0] = interval & 0xff;
	dev->mib_buf.data[1] = interval >> 8;

	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (listen_interval) failed: %d",
		    dev->netdev->name, ret);
	}
	return ret;
}

static int set_preamble(struct at76c503 *dev, u8 type)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_LOCAL;
	dev->mib_buf.size = 1;
	dev->mib_buf.index = PREAMBLE_TYPE_OFFSET;
	dev->mib_buf.data[0] = type;
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (preamble) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

static int set_frag(struct at76c503 *dev, u16 size)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC;
	dev->mib_buf.size = 2;
	dev->mib_buf.index = FRAGMENTATION_OFFSET;
	*(__le16*)dev->mib_buf.data = cpu_to_le16(size);
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (frag threshold) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

static int set_rts(struct at76c503 *dev, u16 size)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC;
	dev->mib_buf.size = 2;
	dev->mib_buf.index = RTS_OFFSET;
	*(__le16*)dev->mib_buf.data = cpu_to_le16(size);
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (rts) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

static int set_autorate_fallback(struct at76c503 *dev, int onoff)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_LOCAL;
	dev->mib_buf.size = 1;
	dev->mib_buf.index = TX_AUTORATE_FALLBACK_OFFSET;
	dev->mib_buf.data[0] = onoff;
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (autorate fallback) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

static int set_mac_address(struct at76c503 *dev, void *addr)
{
        int ret = 0;

        memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
        dev->mib_buf.type = MIB_MAC_ADD;
        dev->mib_buf.size = ETH_ALEN;
        dev->mib_buf.index = offsetof(struct mib_mac_addr, mac_addr);
        memcpy(dev->mib_buf.data, addr, ETH_ALEN);
        ret = set_mib(dev, &dev->mib_buf);
        if(ret < 0){
                err("%s: set_mib (MAC_ADDR, mac_addr) failed: %d",
                    dev->netdev->name, ret);
        }
        return ret;
}

#if 0
/* implemented to get promisc. mode working, but does not help.
   May still be useful for multicast eventually. */
static int set_group_address(struct at76c503 *dev, u8 *addr, int n)
{
        int ret = 0;

        memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
        dev->mib_buf.type = MIB_MAC_ADD;
        dev->mib_buf.size = ETH_ALEN;
        dev->mib_buf.index = offsetof(struct mib_mac_addr, group_addr) + n*ETH_ALEN;
        memcpy(dev->mib_buf.data, addr, ETH_ALEN);
        ret = set_mib(dev, &dev->mib_buf);
        if(ret < 0){
                err("%s: set_mib (MIB_MAC_ADD, group_addr) failed: %d",
                    dev->netdev->name, ret);
        }

#if 1
	/* I do not know anything about the group_addr_status field... (oku) */
        memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
        dev->mib_buf.type = MIB_MAC_ADD;
        dev->mib_buf.size = 1;
        dev->mib_buf.index = offsetof(struct mib_mac_addr, group_addr_status) + n;
        dev->mib_buf.data[0] = 1;
        ret = set_mib(dev, &dev->mib_buf);
        if(ret < 0){
                err("%s: set_mib (MIB_MAC_ADD, group_addr_status) failed: %d",
                    dev->netdev->name, ret);
        }
#endif
        return ret;
}
#endif

static int set_promisc(struct at76c503 *dev, int onoff)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_LOCAL;
	dev->mib_buf.size = 1;
	dev->mib_buf.index = offsetof(struct mib_local, promiscuous_mode);
	dev->mib_buf.data[0] = onoff ? 1 : 0;
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (promiscuous_mode) failed: %d", dev->netdev->name, ret);
	}
	return ret;
}

static int dump_mib_mac_addr(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_mac_addr *mac_addr =
		kmalloc(sizeof(struct mib_mac_addr), GFP_KERNEL);

	if(!mac_addr){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MAC_ADD,
		      (u8*)mac_addr, sizeof(struct mib_mac_addr));
	if(ret < 0){
		err("%s: get_mib (MAC_ADDR) failed: %d", dev->netdev->name, ret);
		goto err;
	}

	dbg_uc("%s: MIB MAC_ADDR: mac_addr %s res 0x%x 0x%x group_addr %s status %d %d %d %d", 
	       dev->netdev->name, mac2str(mac_addr->mac_addr),
		   mac_addr->res[0], mac_addr->res[1],
	       hex2str(dev->obuf, (u8 *)mac_addr->group_addr, 
		       min((int)(sizeof(dev->obuf)-1)/2, 4*ETH_ALEN), '\0'),
	       mac_addr->group_addr_status[0], mac_addr->group_addr_status[1],
	       mac_addr->group_addr_status[2], mac_addr->group_addr_status[3]);

 err:
	kfree(mac_addr);
 exit:
	return ret;
}

static int dump_mib_mac_wep(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_mac_wep *mac_wep =
		kmalloc(sizeof(struct mib_mac_wep), GFP_KERNEL);

	if(!mac_wep){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MAC_WEP,
		      (u8*)mac_wep, sizeof(struct mib_mac_wep));
	if(ret < 0){
		err("%s: get_mib (MAC_WEP) failed: %d", dev->netdev->name, ret);
		goto err;
	}

	dbg_uc("%s: MIB MAC_WEP: priv_invoked %u def_key_id %u key_len %u "
	    "excl_unencr %u wep_icv_err %u wep_excluded %u encr_level %u key %d: %s",
	    dev->netdev->name, mac_wep->privacy_invoked,
	    mac_wep->wep_default_key_id, mac_wep->wep_key_mapping_len,
	    mac_wep->exclude_unencrypted,le32_to_cpu( mac_wep->wep_icv_error_count),
	    le32_to_cpu(mac_wep->wep_excluded_count),
	    mac_wep->encryption_level, mac_wep->wep_default_key_id,
	    mac_wep->wep_default_key_id < 4 ?
	    hex2str(dev->obuf,
		    mac_wep->wep_default_keyvalue[mac_wep->wep_default_key_id],
		    min((int)(sizeof(dev->obuf)-1)/2,
			mac_wep->encryption_level == 2 ? 13 : 5), '\0') :
	       "<invalid key id>");

 err:
	kfree(mac_wep);
 exit:
	return ret;
}

static int dump_mib_mac_mgmt(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_mac_mgmt *mac_mgmt =
		kmalloc(sizeof(struct mib_mac_mgmt), GFP_KERNEL);
	char country_string[4];

	if(!mac_mgmt){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MAC_MGMT,
		      (u8*)mac_mgmt, sizeof(struct mib_mac_mgmt));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}

	memcpy(&country_string, mac_mgmt->country_string, 3);
	country_string[3] = '\0';

	dbg_uc("%s: MIB MAC_MGMT: beacon_period %d CFP_max_duration %d "
	       "medium_occupancy_limit %d station_id 0x%x ATIM_window %d "
	       "CFP_mode %d privacy_opt_impl %d DTIM_period %d CFP_period %d "
	       "current_bssid %s current_essid %s current_bss_type %d "
	       "pm_mode %d ibss_change %d res %d "
	       "multi_domain_capability_implemented %d "
	       "international_roaming %d country_string %s",
	       dev->netdev->name,
	       le16_to_cpu(mac_mgmt->beacon_period),
	       le16_to_cpu(mac_mgmt->CFP_max_duration),
	       le16_to_cpu(mac_mgmt->medium_occupancy_limit),
	       le16_to_cpu(mac_mgmt->station_id),
	       le16_to_cpu(mac_mgmt->ATIM_window),
	       mac_mgmt->CFP_mode,
	       mac_mgmt->privacy_option_implemented,
	       mac_mgmt->DTIM_period,
	       mac_mgmt->CFP_period,
	       mac2str(mac_mgmt->current_bssid),
	       hex2str(dev->obuf, (u8 *)mac_mgmt->current_essid, 
		       min((int)(sizeof(dev->obuf)-1)/2, 
			   IW_ESSID_MAX_SIZE), '\0'),
	       mac_mgmt->current_bss_type,
	       mac_mgmt->power_mgmt_mode,
	       mac_mgmt->ibss_change,
	       mac_mgmt->res,
	       mac_mgmt->multi_domain_capability_implemented,
	       mac_mgmt->multi_domain_capability_enabled,
	       country_string);
 err:
	kfree(mac_mgmt);
 exit:
	return ret;
}

static int dump_mib_mac(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_mac *mac =
		kmalloc(sizeof(struct mib_mac), GFP_KERNEL);

	if(!mac){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MAC,
		      (u8*)mac, sizeof(struct mib_mac));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}

	dbg_uc("%s: MIB MAC: max_tx_msdu_lifetime %d max_rx_lifetime %d "
	       "frag_threshold %d rts_threshold %d cwmin %d cwmax %d "
	       "short_retry_time %d long_retry_time %d scan_type %d "
	       "scan_channel %d probe_delay %u min_channel_time %d "
	       "max_channel_time %d listen_int %d desired_ssid %s "
	       "desired_bssid %s desired_bsstype %d",
	       dev->netdev->name,
	       le32_to_cpu(mac->max_tx_msdu_lifetime),
	       le32_to_cpu(mac->max_rx_lifetime),
	       le16_to_cpu(mac->frag_threshold),
	       le16_to_cpu(mac->rts_threshold),
	       le16_to_cpu(mac->cwmin),
	       le16_to_cpu(mac->cwmax),
	       mac->short_retry_time,
	       mac->long_retry_time,
	       mac->scan_type,
	       mac->scan_channel,
	       le16_to_cpu(mac->probe_delay),
	       le16_to_cpu(mac->min_channel_time),
	       le16_to_cpu(mac->max_channel_time),
	       le16_to_cpu(mac->listen_interval),
	       hex2str(dev->obuf, mac->desired_ssid, 
		       min((int)(sizeof(dev->obuf)-1)/2, 
			   IW_ESSID_MAX_SIZE), '\0'),
	       mac2str(mac->desired_bssid),
	       mac->desired_bsstype);
 err:
	kfree(mac);
 exit:
	return ret;
}

static int dump_mib_phy(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_phy *phy =
		kmalloc(sizeof(struct mib_phy), GFP_KERNEL);

	if(!phy){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_PHY,
		      (u8*)phy, sizeof(struct mib_phy));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}

	dbg_uc("%s: MIB PHY: ed_threshold %d slot_time %d sifs_time %d "
	       "preamble_length %d plcp_header_length %d mpdu_max_length %d "
	       "cca_mode_supported %d operation_rate_set "
	       "0x%x 0x%x 0x%x 0x%x channel_id %d current_cca_mode %d "
	       "phy_type %d current_reg_domain %d",
	       dev->netdev->name,
	       le32_to_cpu(phy->ed_threshold),
	       le16_to_cpu(phy->slot_time),
	       le16_to_cpu(phy->sifs_time),
	       le16_to_cpu(phy->preamble_length),
	       le16_to_cpu(phy->plcp_header_length),
	       le16_to_cpu(phy->mpdu_max_length),
	       le16_to_cpu(phy->cca_mode_supported),
	       phy->operation_rate_set[0], phy->operation_rate_set[1],
	       phy->operation_rate_set[2], phy->operation_rate_set[3],
	       phy->channel_id,
	       phy->current_cca_mode,
	       phy->phy_type,
	       phy->current_reg_domain);
 err:
	kfree(phy);
 exit:
	return ret;
}

static int dump_mib_local(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_local *local =
		kmalloc(sizeof(struct mib_phy), GFP_KERNEL);

	if(!local){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_LOCAL,
		      (u8*)local, sizeof(struct mib_local));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}

	dbg_uc("%s: MIB PHY: beacon_enable %d txautorate_fallback %d "
	       "ssid_size %d promiscuous_mode %d preamble_type %d",
	       dev->netdev->name,
	       local->beacon_enable,
	       local->txautorate_fallback,
	       local->ssid_size,
	       local->promiscuous_mode,
	       local->preamble_type);
 err:
	kfree(local);
 exit:
	return ret;
}


static int get_mib_mdomain(struct at76c503 *dev, struct mib_mdomain *val)
{
	int ret = 0;
	struct mib_mdomain *mdomain =
		kmalloc(sizeof(struct mib_mdomain), GFP_KERNEL);

	if(!mdomain){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MDOMAIN,
		      (u8*)mdomain, sizeof(struct mib_mdomain));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}

	memcpy(val, mdomain, sizeof(*val));

 err:
	kfree(mdomain);
 exit:
	return ret;
}

static void dump_mib_mdomain(struct at76c503 *dev)
{
	char obuf1[2*14+1], obuf2[2*14+1]; /* to hexdump tx_powerlevel, 
					      channel_list */
	int ret;
	struct mib_mdomain mdomain;

	if ((ret=get_mib_mdomain(dev, &mdomain)) < 0) {
		err("%s: get_mib_mdomain returned %d", __FUNCTION__, ret);
		return;
	}
	
	dbg(DBG_MIB, "%s: MIB MDOMAIN: channel_list %s tx_powerlevel %s",
	    dev->netdev->name,
	    hex2str(obuf1, mdomain.channel_list,
		    (sizeof(obuf1)-1)/2,'\0'),
	    hex2str(obuf2, mdomain.tx_powerlevel,
		    (sizeof(obuf2)-1)/2,'\0'));
}

static
int get_current_bssid(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_mac_mgmt *mac_mgmt =
		kmalloc(sizeof(struct mib_mac_mgmt), GFP_KERNEL);

	if(!mac_mgmt){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = get_mib(dev->udev, MIB_MAC_MGMT,
		      (u8*)mac_mgmt, sizeof(struct mib_mac_mgmt));
	if(ret < 0){
		err("%s: get_mib failed: %d", dev->netdev->name, ret);
		goto err;
	}
	memcpy(dev->bssid, mac_mgmt->current_bssid, ETH_ALEN);
	info("using BSSID %s", mac2str(dev->bssid));
 err:
	kfree(mac_mgmt);
 exit:
	return ret;
}

static int get_current_channel(struct at76c503 *dev)
{
	int ret = 0;
	struct mib_phy *phy =
		kmalloc(sizeof(struct mib_phy), GFP_KERNEL);

	if(!phy){
		ret = -ENOMEM;
		goto exit;
	}
	ret = get_mib(dev->udev, MIB_PHY, (u8*)phy,
		      sizeof(struct mib_phy));
	if(ret < 0){
		err("%s: get_mib(MIB_PHY) failed: %d", dev->netdev->name, ret);
		goto err;
	}
	dev->channel = phy->channel_id;
 err:
	kfree(phy);
 exit:
	return ret;
}

/* == PROC start_scan ==
  start a scan. use_essid is != 0 if any probe_delay (if scan mode is not 
  passive) should contain the ESSID configured. ir_step describes the
  international roaming step (0, 1) */
static int start_scan(struct at76c503 *dev, int use_essid, int ir_step)
{
	struct at76c503_start_scan scan;

	memset(&scan, 0, sizeof(struct at76c503_start_scan));
	memset(scan.bssid, 0xff, ETH_ALEN);

	if (use_essid) {
		memcpy(scan.essid, dev->essid, IW_ESSID_MAX_SIZE);
		scan.essid_size = dev->essid_size;
	} else
		scan.essid_size = 0;

	//jal: why should we start at a certain channel? we do scan the whole range
	//allowed by reg domain.
	scan.channel = dev->channel;

	/* atmelwlandriver differs between scan type 0 and 1 (active/passive)
	   For ad-hoc mode, it uses type 0 only.*/
	if ((dev->international_roaming == IR_ON && ir_step == 0) ||
		dev->iw_mode == IW_MODE_MONITOR)
		scan.scan_type = SCAN_TYPE_PASSIVE;
	else
		scan.scan_type = dev->scan_mode;

	/* INFO: For probe_delay, not multiplying by 1024 as this will be 
	   slightly less than min_channel_time
	   (per spec: probe delay < min. channel time) */
	LOCK_ISTATE()
	if (dev->istate == MONITORING) {
		scan.min_channel_time = cpu_to_le16(dev->monitor_scan_min_time);
		scan.max_channel_time = cpu_to_le16(dev->monitor_scan_max_time);
		scan.probe_delay = cpu_to_le16(dev->monitor_scan_min_time * 1000);
	} else {
		scan.min_channel_time = cpu_to_le16(dev->scan_min_time);
		scan.max_channel_time = cpu_to_le16(dev->scan_max_time);
		scan.probe_delay = cpu_to_le16(dev->scan_min_time * 1000);
	}
	UNLOCK_ISTATE()
	if (dev->international_roaming == IR_ON && ir_step == 1)
		scan.international_scan = 0;
	else
		scan.international_scan = dev->international_roaming;

	/* other values are set to 0 for type 0 */

	dbg(DBG_PROGRESS, "%s: start_scan (use_essid = %d, intl = %d, "
	    "channel = %d, probe_delay = %d, scan_min_time = %d, "
	    "scan_max_time = %d)",
	    dev->netdev->name, use_essid,
	    scan.international_scan, scan.channel,
	    le16_to_cpu(scan.probe_delay),
	    le16_to_cpu(scan.min_channel_time),
	    le16_to_cpu(scan.max_channel_time));

	return set_card_command(dev->udev, CMD_SCAN,
				(unsigned char*)&scan, sizeof(scan));
}

static int start_ibss(struct at76c503 *dev)
{
	struct at76c503_start_bss bss;

	memset(&bss, 0, sizeof(struct at76c503_start_bss));
	memset(bss.bssid, 0xff, ETH_ALEN);
	memcpy(bss.essid, dev->essid, IW_ESSID_MAX_SIZE);
	bss.essid_size = dev->essid_size;
	bss.bss_type = ADHOC_MODE;
	bss.channel = dev->channel;

	return set_card_command(dev->udev, CMD_START_IBSS,
				(unsigned char*)&bss, sizeof(struct at76c503_start_bss));
}

/* idx points into dev->bss */
static int join_bss(struct at76c503 *dev, struct bss_info *ptr)
{
	struct at76c503_join join;

	assert(ptr != NULL);

	memset(&join, 0, sizeof(struct at76c503_join));
	memcpy(join.bssid, ptr->bssid, ETH_ALEN);
	memcpy(join.essid, ptr->ssid, ptr->ssid_len);
	join.essid_size = ptr->ssid_len;
	join.bss_type = (dev->iw_mode == IW_MODE_ADHOC ? 1 : 2);
	join.channel = ptr->channel;
	join.timeout = cpu_to_le16(2000);

	dbg(DBG_PROGRESS, "%s join addr %s ssid %s type %d ch %d timeout %d",
	    dev->netdev->name, mac2str(join.bssid), 
	    join.essid, join.bss_type, join.channel, le16_to_cpu(join.timeout));
	return set_card_command(dev->udev, CMD_JOIN,
				(unsigned char*)&join,
				sizeof(struct at76c503_join));
} /* join_bss */

/* the firmware download timeout (after remap) */
static void fw_dl_timeout(unsigned long par)
{
	struct at76c503 *dev = (struct at76c503 *)par;
	defer_kevent(dev, KEVENT_RESET_DEVICE);
}


/* the restart timer timed out */
static void restart_timeout(unsigned long par)
{
	struct at76c503 *dev = (struct at76c503 *)par;
	defer_kevent(dev, KEVENT_RESTART);
}

/* we got to check the bss_list for old entries */
static void bss_list_timeout(unsigned long par)
{
	struct at76c503 *dev = (struct at76c503 *)par;
	unsigned long flags;
	struct list_head *lptr, *nptr;
	struct bss_info *ptr;

	spin_lock_irqsave(&dev->bss_list_spinlock, flags);

	list_for_each_safe(lptr, nptr, &dev->bss_list) {

		ptr = list_entry(lptr, struct bss_info, list);

		if (ptr != dev->curr_bss && ptr != dev->new_bss &&
		    time_after(jiffies, ptr->last_rx+BSS_LIST_TIMEOUT)) {
			dbg(DBG_BSS_TABLE_RM,
			    "%s: bss_list: removing old BSS %s ch %d",
			    dev->netdev->name, mac2str(ptr->bssid), ptr->channel);
			list_del(&ptr->list);
			kfree(ptr);
		}
	}
	spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);
	/* restart the timer */
	mod_timer(&dev->bss_list_timer, jiffies+BSS_LIST_TIMEOUT);
	
}

/* we got a timeout for a infrastructure mgmt packet */
static void mgmt_timeout(unsigned long par)
{
	struct at76c503 *dev = (struct at76c503 *)par;
	defer_kevent(dev, KEVENT_MGMT_TIMEOUT);
}

/* == PROC handle_mgmt_timeout_scan == */
/* called in istate SCANNING on expiry of the mgmt_timer, when a scan was run before
   (dev->scan_runs > 0) */
static void handle_mgmt_timeout_scan(struct at76c503 *dev)
{

	u8 *cmd_status;
	int ret;
	struct mib_mdomain mdomain;

	cmd_status = kmalloc(40, GFP_KERNEL);
	if (cmd_status == NULL) {
		err("%s: %s: cmd_status kmalloc returned NULL", 
		    dev->netdev->name, __FUNCTION__);
		return;
	}

	
	if ((ret=get_cmd_status(dev->udev, CMD_SCAN, cmd_status)) < 0) {
		err("%s: %s: get_cmd_status failed with %d",
		    dev->netdev->name, __FUNCTION__, ret);
		cmd_status[5] = CMD_STATUS_IN_PROGRESS;	
                /* INFO: Hope it was a one off error - if not, scanning 
		   further down the line and stop this cycle */
	}
	LOCK_ISTATE()
	dbg(DBG_PROGRESS, "%s %s:%d got cmd_status %d (istate %d, "
	    "scan_runs %d)",
	    dev->netdev->name, __FUNCTION__, __LINE__, cmd_status[5],
	    dev->istate, dev->scan_runs);
	UNLOCK_ISTATE()
	if (cmd_status[5] == CMD_STATUS_COMPLETE) {
		LOCK_ISTATE()
		if (dev->istate == SCANNING) {
			UNLOCK_ISTATE()
			dump_bss_table(dev,0);
			switch (dev->scan_runs) {

			case 1:
				assert(dev->international_roaming);
				if ((ret=get_mib_mdomain(dev, &mdomain)) < 0) {
					err("get_mib_mdomain returned %d", ret);
				} else {
					char obuf1[2*14+1], obuf2[2*14+1];
					
					dbg(DBG_MIB, "%s: MIB MDOMAIN: channel_list %s "
					    "tx_powerlevel %s",
					    dev->netdev->name,
					    hex2str(obuf1, mdomain.channel_list,
						    (sizeof(obuf1)-1)/2,'\0'),
					    hex2str(obuf2, mdomain.tx_powerlevel,
						    (sizeof(obuf2)-1)/2,'\0'));
				}
				if ((ret = start_scan(dev, 0, 1)) < 0) {
					err("%s: %s: start_scan (ANY) failed with %d", 
					    dev->netdev->name, __FUNCTION__, ret);
				}
				dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
				    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
				mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
				break;

			case 2:
				if ((ret = start_scan(dev, 1, 1)) < 0) {
					err("%s: %s: start_scan (SSID) failed with %d", 
					    dev->netdev->name, __FUNCTION__, ret);
				}
				dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
				    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
				mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
				break;

			case 3:
				dev->site_survey_state = SITE_SURVEY_COMPLETED;
				/* report the end of scan to user space */
				iwevent_scan_complete(dev->netdev);
				NEW_STATE(dev,JOINING);
				assert(dev->curr_bss == NULL); /* done in free_bss_list, 
								  find_bss will start with first bss */
				/* call join_bss immediately after
				   re-run of all other threads in kevent */
				defer_kevent(dev,KEVENT_JOIN);
				break;

			default:
				err("unexpected dev->scan_runs %d", dev->scan_runs);
			} /* switch (dev->scan_runs)*/
			dev->scan_runs++;
		} else {
			
			assert(dev->istate == MONITORING);
			UNLOCK_ISTATE()
			dbg(DBG_MONITOR_MODE, "%s: MONITOR MODE: restart scan",
			    dev->netdev->name);
			start_scan(dev, 0, 0);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
			    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
			mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
		}

	} else { 
		if ((cmd_status[5] != CMD_STATUS_IN_PROGRESS) &&
		    (cmd_status[5] != CMD_STATUS_IDLE))
			err("%s: %s: Bad scan status: %s", 
			    dev->netdev->name, __FUNCTION__, 
			    get_cmd_status_string(cmd_status[5]));

		/* the first cmd status after scan start is always a IDLE ->
		   start the timer to poll again until COMPLETED */
		dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
		    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
		mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
	}

	kfree(cmd_status);
}

/* the deferred procedure called from kevent() */
static void handle_mgmt_timeout(struct at76c503 *dev)
{
	LOCK_ISTATE()
	if ((dev->istate != SCANNING && dev->istate != MONITORING) || 
	     (at76_debug & DBG_MGMT_TIMER))
		/* this is normal behavior in states MONITORING, SCANNING ... */
		dbg(DBG_PROGRESS, "%s: timeout, state %d", dev->netdev->name,
		    dev->istate);

	switch(dev->istate) {

	case MONITORING:
	case SCANNING: 
		UNLOCK_ISTATE()
		handle_mgmt_timeout_scan(dev);
		break;

	case JOINING:
		UNLOCK_ISTATE()
		assert(0);
		break;

	case CONNECTED: /* we haven't received the beacon of this BSS for 
			   BEACON_TIMEOUT seconds */
		UNLOCK_ISTATE()
		info("%s: lost beacon bssid %s",
		     dev->netdev->name, mac2str(dev->curr_bss->bssid));
		/* jal: starting mgmt_timer in ad-hoc mode is questionable, 
		   but I'll leave it here to track down another lockup problem */
		if (dev->iw_mode != IW_MODE_ADHOC) {
			netif_carrier_off(dev->netdev);
			netif_stop_queue(dev->netdev);
			iwevent_bss_disconnect(dev->netdev);
			NEW_STATE(dev,SCANNING);
			defer_kevent(dev,KEVENT_SCAN);
		}
		break;

	case AUTHENTICATING:
		UNLOCK_ISTATE()
		if (dev->retries-- >= 0) {
			auth_req(dev, dev->curr_bss, 1, NULL);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
			    __FUNCTION__, __LINE__);
			mod_timer(&dev->mgmt_timer, jiffies+HZ);
		} else {
			/* try to get next matching BSS */
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
		}
		break;

	case ASSOCIATING:
		UNLOCK_ISTATE()
		if (dev->retries-- >= 0) {
			assoc_req(dev,dev->curr_bss);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
			    __FUNCTION__, __LINE__);
			mod_timer(&dev->mgmt_timer, jiffies+HZ);
		} else {
			/* jal: TODO: we may be authenticated to several
			   BSS and may try to associate to the next of them here
			   in the future ... */

			/* try to get next matching BSS */
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
		}
		break;

	case REASSOCIATING:
		UNLOCK_ISTATE()
		if (dev->retries-- >= 0)
			reassoc_req(dev, dev->curr_bss, dev->new_bss);
		else {
			/* we disassociate from the curr_bss and
			   scan again ... */
			NEW_STATE(dev,DISASSOCIATING);
			dev->retries = DISASSOC_RETRIES;
			disassoc_req(dev, dev->curr_bss);
		}
		dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
		    __FUNCTION__, __LINE__);
		mod_timer(&dev->mgmt_timer, jiffies+HZ);
		break;

	case DISASSOCIATING:
		UNLOCK_ISTATE()
		if (dev->retries-- >= 0) {
			disassoc_req(dev, dev->curr_bss);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
			    __FUNCTION__, __LINE__);
			mod_timer(&dev->mgmt_timer,jiffies+HZ);
		} else {
			/* we scan again ... */
			NEW_STATE(dev,SCANNING);
			defer_kevent(dev,KEVENT_SCAN);
		}
		break;

	case INIT:
		UNLOCK_ISTATE()
		break;

	default:
		UNLOCK_ISTATE()
		assert(0);
	} /* switch (dev->istate) */

}/* handle_mgmt_timeout */

/* calc. the padding from txbuf->wlength (which excludes the USB TX header) 
   guess this is needed to compensate a flaw in the AT76C503A USB part ... */
static inline int calc_padding(int wlen)
{
	/* add the USB TX header */
	wlen += AT76C503_TX_HDRLEN;

	wlen = wlen % 64;

	if (wlen < 50)
		return 50 - wlen;

	if (wlen >=61)
		return 64 + 50 - wlen;

	return 0;
}

/* send a management frame on bulk-out.
   txbuf->wlength must be set (in LE format !) */
static int send_mgmt_bulk(struct at76c503 *dev, struct at76c503_tx_buffer *txbuf)
{
	unsigned long flags;
	int ret = 0;
	int urb_status;
	void *oldbuf = NULL;

	netif_carrier_off(dev->netdev); /* disable running netdev watchdog */
	netif_stop_queue(dev->netdev); /* stop tx data packets */

	spin_lock_irqsave(&dev->mgmt_spinlock, flags);

	if ((urb_status=dev->write_urb->status) == -EINPROGRESS) {
		oldbuf=dev->next_mgmt_bulk; /* to kfree below */
		dev->next_mgmt_bulk = txbuf;
		txbuf = NULL;
	}
	spin_unlock_irqrestore(&dev->mgmt_spinlock, flags);

	if (oldbuf) {
		/* a data/mgmt tx is already pending in the URB -
		   if this is no error in some situations we must
		   implement a queue or silently modify the old msg */
		err("%s: %s removed pending mgmt buffer %s",
		    dev->netdev->name, __FUNCTION__,
		    hex2str(dev->obuf, (u8 *)dev->next_mgmt_bulk,
			    min((int)(sizeof(dev->obuf))/3, 64),' '));
		kfree(dev->next_mgmt_bulk);
	}

	if (txbuf) {

		txbuf->tx_rate = 0;
//		txbuf->padding = 0;
		txbuf->padding = calc_padding(le16_to_cpu(txbuf->wlength));

		if (dev->next_mgmt_bulk) {
			err("%s: %s URB status %d, but mgmt is pending",
			    dev->netdev->name, __FUNCTION__, urb_status);
		}

		dbg(DBG_TX_MGMT, "%s: tx mgmt: wlen %d tx_rate %d pad %d %s",
		    dev->netdev->name, le16_to_cpu(txbuf->wlength),
		    txbuf->tx_rate, txbuf->padding,
		    hex2str(dev->obuf, txbuf->packet,
			    min((sizeof(dev->obuf)-1)/2,
				(size_t)le16_to_cpu(txbuf->wlength)),'\0'));

		/* txbuf was not consumed above -> send mgmt msg immediately */
		memcpy(dev->bulk_out_buffer, txbuf,
		       le16_to_cpu(txbuf->wlength) + AT76C503_TX_HDRLEN);
		FILL_BULK_URB(dev->write_urb, dev->udev,
			      usb_sndbulkpipe(dev->udev, 
					      dev->bulk_out_endpointAddr),
			      dev->bulk_out_buffer,
			      le16_to_cpu(txbuf->wlength) + 
			      txbuf->padding +
			      AT76C503_TX_HDRLEN,
			      (usb_complete_t)at76c503_write_bulk_callback, dev);
		ret = usb_submit_urb(dev->write_urb, GFP_ATOMIC);
		if (ret) {
			err("%s: %s error in tx submit urb: %d",
			    dev->netdev->name, __FUNCTION__, ret);
		}
		kfree(txbuf);
	} /* if (txbuf) */

	return ret;

} /* send_mgmt_bulk */

static int disassoc_req(struct at76c503 *dev, struct bss_info *bss)
{
	struct at76c503_tx_buffer *tx_buffer;
	struct ieee80211_hdr_3addr *mgmt;
	struct ieee802_11_disassoc_frame *req;

	assert(bss != NULL);
	if (bss == NULL)
		return -EFAULT;

	tx_buffer = kmalloc(DISASSOC_FRAME_SIZE + MAX_PADDING_SIZE,
			    GFP_ATOMIC);
	if (!tx_buffer)
		return -ENOMEM;

	mgmt = (struct ieee80211_hdr_3addr *)&(tx_buffer->packet);
	req  = (struct ieee802_11_disassoc_frame *)&(mgmt->payload);

	/* make wireless header */
	mgmt->frame_ctl = cpu_to_le16(IEEE80211_FTYPE_MGMT|IEEE80211_STYPE_AUTH);
	mgmt->duration_id = cpu_to_le16(0x8000);
	memcpy(mgmt->addr1, bss->bssid, ETH_ALEN);
	memcpy(mgmt->addr2, dev->netdev->dev_addr, ETH_ALEN);
	memcpy(mgmt->addr3, bss->bssid, ETH_ALEN);
	mgmt->seq_ctl = cpu_to_le16(0);

	req->reason = 0;

	/* init. at76c503 tx header */
	tx_buffer->wlength = cpu_to_le16(DISASSOC_FRAME_SIZE -
		AT76C503_TX_HDRLEN);
	
	dbg(DBG_TX_MGMT, "%s: DisAssocReq bssid %s",
	    dev->netdev->name, mac2str(mgmt->addr3));

	/* either send immediately (if no data tx is pending
	   or put it in pending list */
	return send_mgmt_bulk(dev, tx_buffer); 

} /* disassoc_req */

/* challenge is the challenge string (in TLV format) 
   we got with seq_nr 2 for shared secret authentication only and
   send in seq_nr 3 WEP encrypted to prove we have the correct WEP key;
   otherwise it is NULL */
static int auth_req(struct at76c503 *dev, struct bss_info *bss, int seq_nr, u8 *challenge)
{
	struct at76c503_tx_buffer *tx_buffer;
	struct ieee80211_hdr_3addr *mgmt;
	struct ieee802_11_auth_frame *req;
	
	int buf_len = (seq_nr != 3 ? AUTH_FRAME_SIZE : 
		       AUTH_FRAME_SIZE + 1 + 1 + challenge[1]);

	assert(bss != NULL);
	assert(seq_nr != 3 || challenge != NULL);
	
	tx_buffer = kmalloc(buf_len + MAX_PADDING_SIZE, GFP_ATOMIC);
	if (!tx_buffer)
		return -ENOMEM;

	mgmt = (struct ieee80211_hdr_3addr *)&(tx_buffer->packet);
	req  = (struct ieee802_11_auth_frame *)&(mgmt->payload);

	/* make wireless header */
	/* first auth msg is not encrypted, only the second (seq_nr == 3) */
	mgmt->frame_ctl = cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH |
		(seq_nr == 3 ? IEEE80211_FCTL_PROTECTED : 0));

	mgmt->duration_id = cpu_to_le16(0x8000);
	memcpy(mgmt->addr1, bss->bssid, ETH_ALEN);
	memcpy(mgmt->addr2, dev->netdev->dev_addr, ETH_ALEN);
	memcpy(mgmt->addr3, bss->bssid, ETH_ALEN);
	mgmt->seq_ctl = cpu_to_le16(0);

	req->algorithm = cpu_to_le16(dev->auth_mode);
	req->seq_nr = cpu_to_le16(seq_nr);
	req->status = cpu_to_le16(0);

	if (seq_nr == 3)
		memcpy(req->challenge, challenge, 1+1+challenge[1]);

	/* init. at76c503 tx header */
	tx_buffer->wlength = cpu_to_le16(buf_len - AT76C503_TX_HDRLEN);
	
	dbg(DBG_TX_MGMT, "%s: AuthReq bssid %s alg %d seq_nr %d",
	    dev->netdev->name, mac2str(mgmt->addr3),
	    le16_to_cpu(req->algorithm), le16_to_cpu(req->seq_nr));
	if (seq_nr == 3) {
		dbg(DBG_TX_MGMT, "%s: AuthReq challenge: %s ...",
		    dev->netdev->name,
		    hex2str(dev->obuf, req->challenge, 
			    min((int)sizeof(dev->obuf)/3, 18),' '));
	}

	/* either send immediately (if no data tx is pending
	   or put it in pending list */
	return send_mgmt_bulk(dev, tx_buffer); 

} /* auth_req */

static int assoc_req(struct at76c503 *dev, struct bss_info *bss)
{
	struct at76c503_tx_buffer *tx_buffer;
	struct ieee80211_hdr_3addr *mgmt;
	struct ieee802_11_assoc_req *req;
	u8 *tlv;

	assert(bss != NULL);

	tx_buffer = kmalloc(ASSOCREQ_MAX_SIZE + MAX_PADDING_SIZE,
			    GFP_ATOMIC);
	if (!tx_buffer)
		return -ENOMEM;

	mgmt = (struct ieee80211_hdr_3addr *)&(tx_buffer->packet);
	req  = (struct ieee802_11_assoc_req *)&(mgmt->payload);
	tlv = req->data;

	/* make wireless header */
	mgmt->frame_ctl = cpu_to_le16(IEEE80211_FTYPE_MGMT|IEEE80211_STYPE_ASSOC_REQ);

	mgmt->duration_id = cpu_to_le16(0x8000);
	memcpy(mgmt->addr1, bss->bssid, ETH_ALEN);
	memcpy(mgmt->addr2, dev->netdev->dev_addr, ETH_ALEN);
	memcpy(mgmt->addr3, bss->bssid, ETH_ALEN);
	mgmt->seq_ctl = cpu_to_le16(0);

	/* we must set the Privacy bit in the capabilities to assure an
	   Agere-based AP with optional WEP transmits encrypted frames
	   to us.  AP only set the Privacy bit in their capabilities
	   if WEP is mandatory in the BSS! */
	req->capability = cpu_to_le16(bss->capa | 
				      (dev->wep_enabled ? WLAN_CAPABILITY_PRIVACY : 0) |
				      (dev->preamble_type == PREAMBLE_TYPE_SHORT ?
				       WLAN_CAPABILITY_SHORT_PREAMBLE : 0));

	req->listen_interval = cpu_to_le16(2 * bss->beacon_interval);
	
	/* write TLV data elements */

	*tlv++ = MFIE_TYPE_SSID;
	*tlv++ = bss->ssid_len;
	memcpy(tlv, bss->ssid, bss->ssid_len);
	tlv += bss->ssid_len;

	*tlv++ = MFIE_TYPE_RATES;
	*tlv++ = sizeof(hw_rates);
	memcpy(tlv, hw_rates, sizeof(hw_rates));
	tlv += sizeof(hw_rates); /* tlv points behind the supp_rates field */

	/* init. at76c503 tx header */
	tx_buffer->wlength = cpu_to_le16(tlv-(u8 *)mgmt);
	
	{
		/* output buffer for ssid and rates */
		char orates[4*2+1];
		int len;

		tlv = req->data;
		len = min(IW_ESSID_MAX_SIZE, (int)*(tlv+1));
		memcpy(dev->obuf, tlv+2, len);
		dev->obuf[len] = '\0';
		tlv += (1 + 1 + *(tlv+1)); /* points to IE of rates now */
		dbg(DBG_TX_MGMT, "%s: AssocReq bssid %s capa x%04x ssid %s rates %s",
		    dev->netdev->name, mac2str(mgmt->addr3),
		    le16_to_cpu(req->capability), dev->obuf,
		    hex2str(orates,tlv+2,min((sizeof(orates)-1)/2,(size_t)*(tlv+1)),
			    '\0'));
	}

	/* either send immediately (if no data tx is pending
	   or put it in pending list */
	return send_mgmt_bulk(dev, tx_buffer); 

} /* assoc_req */

/* we are currently associated to curr_bss and
   want to reassoc to new_bss */
static int reassoc_req(struct at76c503 *dev, struct bss_info *curr_bss,
		struct bss_info *new_bss)
{
	struct at76c503_tx_buffer *tx_buffer;
	struct ieee80211_hdr_3addr *mgmt;
	struct ieee802_11_reassoc_req *req;
	
	u8 *tlv;

	assert(curr_bss != NULL);
	assert(new_bss != NULL);
	if (curr_bss == NULL || new_bss == NULL)
		return -EFAULT;

	tx_buffer = kmalloc(REASSOCREQ_MAX_SIZE + MAX_PADDING_SIZE,
			    GFP_ATOMIC);
	if (!tx_buffer)
		return -ENOMEM;

	mgmt = (struct ieee80211_hdr_3addr *)&(tx_buffer->packet);
	req  = (struct ieee802_11_reassoc_req *)&(mgmt->payload);
	tlv = req->data;

	/* make wireless header */
	/* jal: encrypt this packet if wep_enabled is TRUE ??? */
	mgmt->frame_ctl = cpu_to_le16(IEEE80211_FTYPE_MGMT|IEEE80211_STYPE_REASSOC_REQ);
	mgmt->duration_id = cpu_to_le16(0x8000);
	memcpy(mgmt->addr1, new_bss->bssid, ETH_ALEN);
	memcpy(mgmt->addr2, dev->netdev->dev_addr, ETH_ALEN);
	memcpy(mgmt->addr3, new_bss->bssid, ETH_ALEN);
	mgmt->seq_ctl = cpu_to_le16(0);

	/* we must set the Privacy bit in the capabilities to assure an
	   Agere-based AP with optional WEP transmits encrypted frames
	   to us.  AP only set the Privacy bit in their capabilities
	   if WEP is mandatory in the BSS! */
	req->capability = cpu_to_le16(new_bss->capa | 
				      (dev->wep_enabled ? WLAN_CAPABILITY_PRIVACY : 0) |
				      (dev->preamble_type == PREAMBLE_TYPE_SHORT ?
				       WLAN_CAPABILITY_SHORT_PREAMBLE : 0));

	req->listen_interval = cpu_to_le16(2 * new_bss->beacon_interval);
	
	memcpy(req->curr_ap, curr_bss->bssid, ETH_ALEN);

	/* write TLV data elements */

	*tlv++ = MFIE_TYPE_SSID;
	*tlv++ = new_bss->ssid_len;
	memcpy(tlv,new_bss->ssid, new_bss->ssid_len);
	tlv += new_bss->ssid_len;

	*tlv++ = MFIE_TYPE_RATES;
	*tlv++ = sizeof(hw_rates);
	memcpy(tlv, hw_rates, sizeof(hw_rates));
	tlv += sizeof(hw_rates); /* tlv points behind the supp_rates field */

	/* init. at76c503 tx header */
	tx_buffer->wlength = cpu_to_le16(tlv-(u8 *)mgmt);
	
	{
		/* output buffer for rates and bssid */
		char orates[4*2+1];
		char ocurr[6*3+1];
		tlv = req->data;
		memcpy(dev->obuf, tlv+2, min(sizeof(dev->obuf),(size_t)*(tlv+1)));
		dev->obuf[IW_ESSID_MAX_SIZE] = '\0';
		tlv += (1 + 1 + *(tlv+1)); /* points to IE of rates now */
		dbg(DBG_TX_MGMT, "%s: ReAssocReq curr %s new %s capa x%04x ssid %s rates %s",
		    dev->netdev->name,
		    hex2str(ocurr, req->curr_ap, ETH_ALEN, ':'),
		    mac2str(mgmt->addr3), le16_to_cpu(req->capability), dev->obuf,
		    hex2str(orates,tlv+2,min((sizeof(orates)-1)/2,(size_t)*(tlv+1)),
			    '\0'));
	}

	/* either send immediately (if no data tx is pending
	   or put it in pending list */
	return send_mgmt_bulk(dev, tx_buffer); 

} /* reassoc_req */


/* shamelessly copied from usbnet.c (oku) */
static void defer_kevent (struct at76c503 *dev, int flag)
{
	set_bit (flag, &dev->kevent_flags);
	if (!schedule_work (&dev->kevent))
		dbg(DBG_KEVENT, "%s: kevent %d may have been dropped",
		     dev->netdev->name, flag);
	else
		dbg(DBG_KEVENT, "%s: kevent %d scheduled",
		    dev->netdev->name, flag);
}

static void kevent(void *data)
{
	struct at76c503 *dev = data;
	int ret;
	unsigned long flags;

	/* on errors, bits aren't cleared, but no reschedule
	   is done. So work will be done next time something
	   else has to be done. This is ugly. TODO! (oku) */

	dbg(DBG_KEVENT, "%s: kevent entry flags: 0x%lx", dev->netdev->name,
	    dev->kevent_flags);

	down(&dev->sem);

	if(test_bit(KEVENT_CTRL_HALT, &dev->kevent_flags)){
		/* this never worked... but it seems
		   that it's rarely necessary, if at all (oku) */
		ret = usb_clear_halt(dev->udev,
				     usb_sndctrlpipe (dev->udev, 0));
		if(ret < 0)
			err("usb_clear_halt() failed: %d", ret);
		else{
			clear_bit(KEVENT_CTRL_HALT, &dev->kevent_flags);
			info("usb_clear_halt() successful");
		}
	}
	if(test_bit(KEVENT_NEW_BSS, &dev->kevent_flags)){
		struct net_device *netdev = dev->netdev;
		struct mib_mac_mgmt *mac_mgmt = kmalloc(sizeof(struct mib_mac_mgmt), GFP_KERNEL);

		ret = get_mib(dev->udev, MIB_MAC_MGMT, (u8*)mac_mgmt,
			      sizeof(struct mib_mac_mgmt));
		if(ret < 0){
			err("%s: get_mib failed: %d", netdev->name, ret);
			goto new_bss_clean;
		}

		dbg(DBG_PROGRESS, "ibss_change = 0x%2x", mac_mgmt->ibss_change);
		memcpy(dev->bssid, mac_mgmt->current_bssid, ETH_ALEN);
		dbg(DBG_PROGRESS, "using BSSID %s", mac2str(dev->bssid));
    
		iwevent_bss_connect(dev->netdev, dev->bssid);

		memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
		dev->mib_buf.type = MIB_MAC_MGMT;
		dev->mib_buf.size = 1;
		dev->mib_buf.index = IBSS_CHANGE_OK_OFFSET;
		ret = set_mib(dev, &dev->mib_buf);
		if(ret < 0){
			err("%s: set_mib (ibss change ok) failed: %d", netdev->name, ret);
			goto new_bss_clean;
		}
		clear_bit(KEVENT_NEW_BSS, &dev->kevent_flags);
	new_bss_clean:
		kfree(mac_mgmt);
	}

	if(test_bit(KEVENT_SET_PROMISC, &dev->kevent_flags)){
		info("%s: KEVENT_SET_PROMISC", dev->netdev->name);

		set_promisc(dev, dev->promisc);
		clear_bit(KEVENT_SET_PROMISC, &dev->kevent_flags);
	}

	/* check this _before_ KEVENT_JOIN, 'cause _JOIN sets _STARTIBSS bit */
	if (test_bit(KEVENT_STARTIBSS, &dev->kevent_flags)) {
		clear_bit(KEVENT_STARTIBSS, &dev->kevent_flags);
		LOCK_ISTATE()
		assert(dev->istate == STARTIBSS);
		UNLOCK_ISTATE()
		ret = start_ibss(dev);
		if(ret < 0){
			err("%s: start_ibss failed: %d", dev->netdev->name, ret);
			goto end_startibss;
		}

		ret = wait_completion(dev, CMD_START_IBSS);
		if (ret != CMD_STATUS_COMPLETE) {
			err("%s start_ibss failed to complete,%d",
			    dev->netdev->name, ret);
			goto end_startibss;
		}

		ret = get_current_bssid(dev);
		if(ret < 0) goto end_startibss;

		ret = get_current_channel(dev);
		if(ret < 0) goto end_startibss;

		/* not sure what this is good for ??? */
		memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
		dev->mib_buf.type = MIB_MAC_MGMT;
		dev->mib_buf.size = 1;
		dev->mib_buf.index = IBSS_CHANGE_OK_OFFSET;
		ret = set_mib(dev, &dev->mib_buf);
		if(ret < 0){
			err("%s: set_mib (ibss change ok) failed: %d", dev->netdev->name, ret);
			goto end_startibss;
		}

		netif_carrier_on(dev->netdev);
		netif_start_queue(dev->netdev);
	}
end_startibss:

	/* check this _before_ KEVENT_SCAN, 'cause _SCAN sets _JOIN bit */
	if (test_bit(KEVENT_JOIN, &dev->kevent_flags)) {
		clear_bit(KEVENT_JOIN, &dev->kevent_flags);
		LOCK_ISTATE()
		if (dev->istate == INIT){
			UNLOCK_ISTATE()	
			goto end_join;
		}
		assert(dev->istate == JOINING);
		UNLOCK_ISTATE()
		/* dev->curr_bss == NULL signals a new round,
		   starting with list_entry(dev->bss_list.next, ...) */

		/* secure the access to dev->curr_bss ! */
		spin_lock_irqsave(&dev->bss_list_spinlock, flags);
		dev->curr_bss=find_matching_bss(dev, dev->curr_bss);
		spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);

		if (dev->curr_bss != NULL) {
			if ((ret=join_bss(dev,dev->curr_bss)) < 0) {
				err("%s: join_bss failed with %d",
				    dev->netdev->name, ret);
				goto end_join;
			}
			
			ret=wait_completion(dev,CMD_JOIN);
			if (ret != CMD_STATUS_COMPLETE) {
				if (ret != CMD_STATUS_TIME_OUT)
					err("%s join_bss completed with %d",
					    dev->netdev->name, ret);
				else
					info("%s join_bss ssid %s timed out",
						     dev->netdev->name,
					     mac2str(dev->curr_bss->bssid));

				/* retry next BSS immediately */
				defer_kevent(dev,KEVENT_JOIN);
				goto end_join;
			}

			/* here we have joined the (I)BSS */
			if (dev->iw_mode == IW_MODE_ADHOC) {
				struct bss_info *bptr = dev->curr_bss;
				NEW_STATE(dev,CONNECTED);
				/* get ESSID, BSSID and channel for dev->curr_bss */
				dev->essid_size = bptr->ssid_len;
				memcpy(dev->essid, bptr->ssid, bptr->ssid_len);
				memcpy(dev->bssid, bptr->bssid, ETH_ALEN);
				dev->channel = bptr->channel;
				iwevent_bss_connect(dev->netdev,bptr->bssid);
				netif_carrier_on(dev->netdev);
				netif_start_queue(dev->netdev);
				/* just to be sure */
				del_timer_sync(&dev->mgmt_timer);
			} else {
				/* send auth req */
				NEW_STATE(dev,AUTHENTICATING);
				auth_req(dev, dev->curr_bss, 1, NULL);
				dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
				    __FUNCTION__, __LINE__);
				mod_timer(&dev->mgmt_timer, jiffies+HZ);
			}
			goto end_join;
		} /* if (dev->curr_bss != NULL) */

		/* here we haven't found a matching (i)bss ... */
		if (dev->iw_mode == IW_MODE_ADHOC) {
			NEW_STATE(dev,STARTIBSS);
			defer_kevent(dev,KEVENT_STARTIBSS);
			goto end_join;
		}
		/* haven't found a matching BSS in infra mode - try again */
		NEW_STATE(dev,SCANNING);
		defer_kevent(dev, KEVENT_SCAN);
	} /* if (test_bit(KEVENT_JOIN, &dev->kevent_flags)) */
end_join:

	if(test_bit(KEVENT_MGMT_TIMEOUT, &dev->kevent_flags)){
		clear_bit(KEVENT_MGMT_TIMEOUT, &dev->kevent_flags);
		handle_mgmt_timeout(dev);
	}

	if (test_bit(KEVENT_SCAN, &dev->kevent_flags)) {
		clear_bit(KEVENT_SCAN, &dev->kevent_flags);
		LOCK_ISTATE()
		assert(dev->istate == SCANNING);

		/* only clear the bss list when a scan is actively initiated,
		 * otherwise simply rely on bss_list_timeout */
		if( dev->site_survey_state == SITE_SURVEY_IN_PROGRESS)
			free_bss_list(dev);
		UNLOCK_ISTATE()

		dev->scan_runs=3;
		if ((ret=start_scan(dev, 0, 1)) < 0) {
			err("%s: %s: start_scan failed with %d",
			    dev->netdev->name, __FUNCTION__, ret);
		} else {
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
			    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
			mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
		}

	} /* if (test_bit(KEVENT_SCAN, &dev->kevent_flags)) */


	if (test_bit(KEVENT_SUBMIT_RX, &dev->kevent_flags)) {
		clear_bit(KEVENT_SUBMIT_RX, &dev->kevent_flags);
		submit_rx_urb(dev);
	}


	if (test_bit(KEVENT_RESTART, &dev->kevent_flags)) {
		clear_bit(KEVENT_RESTART, &dev->kevent_flags);
		LOCK_ISTATE()
		assert(dev->istate == INIT);
		UNLOCK_ISTATE()
		startup_device(dev);

		/* call it here for default_iw_mode == IW_MODE_MONITOR and
		   no subsequent  "iwconfig wlanX mode monitor" or
		   "iwpriv wlanX monitor 1|2 C" to set dev->netdev->type 
		   correctly */
		set_monitor_mode(dev, dev->monitor_prism_header);


		netif_carrier_off(dev->netdev); /* disable running netdev watchdog */
		netif_stop_queue(dev->netdev); /* stop tx data packets */
		if (dev->iw_mode != IW_MODE_MONITOR) {
			NEW_STATE(dev,SCANNING);
			defer_kevent(dev,KEVENT_SCAN);
		} else {
			NEW_STATE(dev,MONITORING);
			start_scan(dev,0,0);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer for %d ticks",
			    __FUNCTION__, __LINE__, SCAN_POLL_INTERVAL);
			mod_timer(&dev->mgmt_timer, jiffies + SCAN_POLL_INTERVAL);
		}
	}

	if (test_bit(KEVENT_ASSOC_DONE, &dev->kevent_flags)) {
		clear_bit(KEVENT_ASSOC_DONE, &dev->kevent_flags);
		LOCK_ISTATE()
		assert(dev->istate == ASSOCIATING ||
		       dev->istate == REASSOCIATING);
		UNLOCK_ISTATE()
		if (dev->iw_mode == IW_MODE_INFRA) {
			assert(dev->curr_bss != NULL);
			if (dev->curr_bss != NULL && 
			    dev->pm_mode != PM_ACTIVE) {
				/* calc the listen interval in units of
				   beacon intervals of the curr_bss */
			       dev->pm_period_beacon = (dev->pm_period_us >> 10) / 
					dev->curr_bss->beacon_interval;

#ifdef DEBUG /* only to check if we need to set the listen interval here
             or could do it in the (re)assoc_req parameter */
				dump_mib_mac(dev);
#endif

				if (dev->pm_period_beacon < 2)
					dev->pm_period_beacon = 2;
				else
					if ( dev->pm_period_beacon > 0xffff)
						dev->pm_period_beacon = 0xffff;

				dbg(DBG_PM, "%s: pm_mode %d assoc id x%x listen int %d",
				    dev->netdev->name, dev->pm_mode,
				    dev->curr_bss->assoc_id, dev->pm_period_beacon);

				set_associd(dev, dev->curr_bss->assoc_id);
				set_listen_interval(dev, (u16)dev->pm_period_beacon);
				set_pm_mode(dev, dev->pm_mode);
#ifdef DEBUG
				dump_mib_mac(dev);
				dump_mib_mac_mgmt(dev);
#endif
			}
		}

		netif_carrier_on(dev->netdev);
		netif_wake_queue(dev->netdev); /* _start_queue ??? */
		NEW_STATE(dev,CONNECTED);
		iwevent_bss_connect(dev->netdev,dev->curr_bss->bssid);
		dbg(DBG_PROGRESS, "%s: connected to BSSID %s",
		    dev->netdev->name, mac2str(dev->curr_bss->bssid));
	}


	if (test_bit(KEVENT_RESET_DEVICE, &dev->kevent_flags)) {

		clear_bit(KEVENT_RESET_DEVICE, &dev->kevent_flags);

		dbg(DBG_DEVSTART, "resetting the device");

		usb_reset_device(dev->udev);

/* with 2.6.8 the reset above will cause a disconnect as the USB subsys
   recognizes the change in the config descriptors. Subsequently the device
   will be registered again. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8)

		//jal: patch the state (patch by Dmitri)
		dev->udev->state = USB_STATE_CONFIGURED;

		/* jal: currently (2.6.0-test2 and 2.4.23) 
		   usb_reset_device() does not recognize that
		   the interface descr. are changed.
		   This procedure reads the configuration and does a limited parsing of
		   the interface and endpoint descriptors */
		update_usb_intf_descr(dev);

		/* continue immediately with external fw download */
		set_bit (KEVENT_EXTERNAL_FW, &dev->kevent_flags);
#else
		/* kernel >= 2.6.8 */
		NEW_STATE(dev, WAIT_FOR_DISCONNECT);
#endif
	}

	if (test_bit(KEVENT_EXTERNAL_FW, &dev->kevent_flags)) {
		u8 op_mode;

		clear_bit(KEVENT_EXTERNAL_FW, &dev->kevent_flags);

		op_mode = get_op_mode(dev->udev);
		dbg(DBG_DEVSTART, "opmode %d", op_mode);
	
		if (op_mode != OPMODE_NORMAL_NIC_WITHOUT_FLASH) {
			err("unexpected opmode %d", op_mode);
			goto end_external_fw;
		}

		if (dev->extfw && dev->extfw_size) {
			ret = download_external_fw(dev->udev, dev->extfw,
						   dev->extfw_size);
			if (ret < 0) {
				err("Downloading external firmware failed: %d", ret);
				goto end_external_fw;
			}
			if (dev->board_type == BOARDTYPE_505A_RFMD_2958) {
				info("200 ms delay for board type 7");
				/* jal: can I do this in kevent ??? */
				set_current_state(TASK_INTERRUPTIBLE);
				schedule_timeout(HZ/5+1);
			}
		}
		NEW_STATE(dev,INIT);
		init_new_device(dev);
	}
end_external_fw:

	if (test_bit(KEVENT_INTERNAL_FW, &dev->kevent_flags)) {
		clear_bit(KEVENT_INTERNAL_FW, &dev->kevent_flags);

		dbg(DBG_DEVSTART, "downloading internal firmware");

		ret=usbdfu_download(dev->udev, dev->intfw,
				    dev->intfw_size,
				    dev->board_type == BOARDTYPE_505A_RFMD_2958 ? 2000: 0);

		if (ret < 0) {
			err("downloading internal fw failed with %d",ret);
			goto end_internal_fw;
		}
 
		dbg(DBG_DEVSTART, "sending REMAP");

		/* no REMAP for 505A (see SF driver) */
		if (dev->board_type != BOARDTYPE_505A_RFMD_2958)
			if ((ret=at76c503_remap(dev->udev)) < 0) {
				err("sending REMAP failed with %d",ret);
				goto end_internal_fw;
			}

		dbg(DBG_DEVSTART, "sleeping for 2 seconds");
		NEW_STATE(dev,EXTFW_DOWNLOAD);
		mod_timer(&dev->fw_dl_timer, jiffies+2*HZ+1);
	}
end_internal_fw:

	up(&dev->sem);

	dbg(DBG_KEVENT, "%s: kevent exit flags: 0x%lx", dev->netdev->name,
	    dev->kevent_flags);

	return;
}

static int essid_matched(struct at76c503 *dev, struct bss_info *ptr)
{
	/* common criteria for both modi */

	int retval = (dev->essid_size == 0  /* ANY ssid */ ||
		      (dev->essid_size == ptr->ssid_len &&
		       !memcmp(dev->essid, ptr->ssid, ptr->ssid_len)));
	if (!retval)
		dbg(DBG_BSS_MATCH, "%s bss table entry %p: essid didn't match",
		    dev->netdev->name, ptr);
	return retval;
}

static inline int mode_matched(struct at76c503 *dev, struct bss_info *ptr)
{
	int retval;

	if (dev->iw_mode == IW_MODE_ADHOC)
		retval =  ptr->capa & WLAN_CAPABILITY_IBSS;
	else
		retval =  ptr->capa & WLAN_CAPABILITY_ESS;
	if (!retval)
		dbg(DBG_BSS_MATCH, "%s bss table entry %p: mode didn't match",
		    dev->netdev->name, ptr);
	return retval;
}

static int rates_matched(struct at76c503 *dev, struct bss_info *ptr)
{
	int i;
	u8 *rate;

	for(i=0,rate=ptr->rates; i < ptr->rates_len; i++,rate++)
		if (*rate & 0x80) {
			/* this is a basic rate we have to support
			   (see IEEE802.11, ch. 7.3.2.2) */
			if (*rate != (0x80|hw_rates[0]) && *rate != (0x80|hw_rates[1]) &&
			    *rate != (0x80|hw_rates[2]) && *rate != (0x80|hw_rates[3])) {
				dbg(DBG_BSS_MATCH,
				    "%s: bss table entry %p: basic rate %02x not supported",
				     dev->netdev->name, ptr, *rate);
				return 0;
			}
		}
	/* if we use short preamble, the bss must support it */
	if (dev->preamble_type == PREAMBLE_TYPE_SHORT &&
	    !(ptr->capa & WLAN_CAPABILITY_SHORT_PREAMBLE)) {
		dbg(DBG_BSS_MATCH, "%s: %p does not support short preamble",
		    dev->netdev->name, ptr);
		return 0;
	} else
		return 1;
}

static inline int wep_matched(struct at76c503 *dev, struct bss_info *ptr)
{
	if (!dev->wep_enabled && 
	    ptr->capa & WLAN_CAPABILITY_PRIVACY) {
		/* we have disabled WEP, but the BSS signals privacy */
		dbg(DBG_BSS_MATCH, "%s: bss table entry %p: requires encryption",
		    dev->netdev->name, ptr);
		return 0;
	}
	/* otherwise if the BSS does not signal privacy it may well
	   accept encrypted packets from us ... */
	return 1;
}

static inline int bssid_matched(struct at76c503 *dev, struct bss_info *ptr)
{
	if (!dev->wanted_bssid_valid ||
		!memcmp(ptr->bssid, dev->wanted_bssid, ETH_ALEN)) {
		return 1;
	} else {
		if (at76_debug & DBG_BSS_MATCH) {
			dbg_uc("%s: requested bssid - %s does not match", 
				dev->netdev->name, mac2str(dev->wanted_bssid));
			dbg_uc("       AP bssid - %s of bss table entry %p", 
				mac2str(ptr->bssid), ptr);
		}
		return 0;
	}
}

static void dump_bss_table(struct at76c503 *dev, int force_output)
{
	struct bss_info *ptr;
	/* hex dump output buffer for debug */
	unsigned long flags;
	struct list_head *lptr;

	if ((at76_debug & DBG_BSS_TABLE) || (force_output)) {
		spin_lock_irqsave(&dev->bss_list_spinlock, flags);

		dbg_uc("%s BSS table (curr=%p, new=%p):", dev->netdev->name,
		       dev->curr_bss, dev->new_bss);

		list_for_each(lptr, &dev->bss_list) {
			ptr = list_entry(lptr, struct bss_info, list);
			dbg_uc("0x%p: bssid %s channel %d ssid %s (%s)"
			    " capa x%04x rates %s rssi %d link %d noise %d",
			    ptr, mac2str(ptr->bssid),
			    ptr->channel,
			    ptr->ssid,
			    hex2str(dev->obuf, ptr->ssid,
				    min((sizeof(dev->obuf)-1)/2,
					(size_t)ptr->ssid_len),'\0'),
			    ptr->capa,
			    hex2str(dev->obuf_s, ptr->rates, 
				    min(sizeof(dev->obuf_s)/3,
					(size_t)ptr->rates_len), ' '),
			       ptr->rssi, ptr->link_qual, ptr->noise_level);
		}

		spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);
	}
}

/* try to find a matching bss in dev->bss, starting at position start.
   returns the ptr to a matching bss in the list or
   NULL if none found */
/* last is the last bss tried, last == NULL signals a new round,
   starting with list_entry(dev->bss_list.next, ...) */
/* this proc must be called inside an acquired dev->bss_list_spinlock
   otherwise the timeout on bss may remove the newly chosen entry ! */
static struct bss_info *find_matching_bss(struct at76c503 *dev,
					  struct bss_info *last)
{
	struct bss_info *ptr = NULL;
	struct list_head *curr;

	curr  = last != NULL ? last->list.next : dev->bss_list.next;
	while (curr != &dev->bss_list) {
		ptr = list_entry(curr, struct bss_info, list);
		if (essid_matched(dev,ptr) &&
		    mode_matched(dev,ptr)  &&
		    wep_matched(dev,ptr)   &&
		    rates_matched(dev,ptr) &&
		    bssid_matched(dev,ptr))
			break;
		curr = curr->next;
	}

	if (curr == &dev->bss_list)
		ptr = NULL;
	/* otherwise ptr points to the struct bss_info we have chosen */

	dbg(DBG_BSS_TABLE, "%s %s: returned %p", dev->netdev->name,
	    __FUNCTION__, ptr);
	return ptr;
} /* find_matching_bss */


/* we got an association response */
static void rx_mgmt_assoc(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	struct ieee802_11_assoc_resp *resp = 
		(struct ieee802_11_assoc_resp *)mgmt->payload;
	u16 assoc_id = le16_to_cpu(resp->assoc_id);
	u16 status = le16_to_cpu(resp->status);
	u16 capa = le16_to_cpu(resp->capability); 
	dbg(DBG_RX_MGMT, "%s: rx AssocResp bssid %s capa x%04x status x%04x "
	    "assoc_id x%04x rates %s",
	    dev->netdev->name, mac2str(mgmt->addr3), capa, status, assoc_id,
	    hex2str(dev->obuf, resp->data+2,
		    min((size_t)*(resp->data+1),(sizeof(dev->obuf)-1)/2), '\0'));
	LOCK_ISTATE()
	if (dev->istate == ASSOCIATING) {
		UNLOCK_ISTATE()
		assert(dev->curr_bss != NULL);
		if (dev->curr_bss == NULL)
			return;

		if (status == WLAN_STATUS_SUCCESS) {
			struct bss_info *ptr = dev->curr_bss;
			ptr->assoc_id = assoc_id & 0x3fff;
			/* update iwconfig params */
			memcpy(dev->bssid, ptr->bssid, ETH_ALEN);
			memcpy(dev->essid, ptr->ssid, ptr->ssid_len);
			dev->essid_size = ptr->ssid_len;
			dev->channel = ptr->channel;
			defer_kevent(dev,KEVENT_ASSOC_DONE);
		} else {
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
		}
		del_timer_sync(&dev->mgmt_timer);
	} else {
		UNLOCK_ISTATE()
		info("%s: AssocResp in state %d ignored",
		     dev->netdev->name, dev->istate);
	}
} /* rx_mgmt_assoc */

static void rx_mgmt_reassoc(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	struct ieee802_11_assoc_resp *resp = 
		(struct ieee802_11_assoc_resp *)mgmt->payload;
	unsigned long flags;
	u16 capa = le16_to_cpu(resp->capability);
	u16 status = le16_to_cpu(resp->status);
	u16 assoc_id = le16_to_cpu(resp->assoc_id);

	dbg(DBG_RX_MGMT, "%s: rx ReAssocResp bssid %s capa x%04x status x%04x "
	    "assoc_id x%04x rates %s",
	    dev->netdev->name, mac2str(mgmt->addr3), capa, status, assoc_id,
	    hex2str(dev->obuf, resp->data+2,
		    min((size_t)*(resp->data+1),(sizeof(dev->obuf)-1)/2), '\0'));
	LOCK_ISTATE()
	if (dev->istate == REASSOCIATING) {
		UNLOCK_ISTATE()
		assert(dev->new_bss != NULL);
		if (dev->new_bss == NULL)
			return;

		if (status == WLAN_STATUS_SUCCESS) {
			struct bss_info *bptr = dev->new_bss;
			bptr->assoc_id = assoc_id;
			NEW_STATE(dev,CONNECTED);

			iwevent_bss_connect(dev->netdev,bptr->bssid);

			spin_lock_irqsave(&dev->bss_list_spinlock, flags);
			dev->curr_bss = dev->new_bss;
			dev->new_bss = NULL;
			spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);

			/* get ESSID, BSSID and channel for dev->curr_bss */
			dev->essid_size = bptr->ssid_len;
			memcpy(dev->essid, bptr->ssid, bptr->ssid_len);
			memcpy(dev->bssid, bptr->bssid, ETH_ALEN);
			dev->channel = bptr->channel;
			dbg(DBG_PROGRESS, "%s: reassociated to BSSID %s",
			    dev->netdev->name, mac2str(dev->bssid));
			defer_kevent(dev, KEVENT_ASSOC_DONE);
		} else {
			del_timer_sync(&dev->mgmt_timer);
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
		}
	} else {
		info("%s: ReAssocResp in state %d ignored",
		     dev->netdev->name, dev->istate);
		UNLOCK_ISTATE()
	}
} /* rx_mgmt_reassoc */

static void rx_mgmt_disassoc(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	struct ieee802_11_disassoc_frame *resp = 
		(struct ieee802_11_disassoc_frame *)mgmt->payload;

	dbg(DBG_RX_MGMT, "%s: rx DisAssoc bssid %s reason x%04x destination %s",
	    dev->netdev->name, mac2str(mgmt->addr3),
	    le16_to_cpu(resp->reason),
	    hex2str(dev->obuf, mgmt->addr1, 
		    min((int)sizeof(dev->obuf)/3, ETH_ALEN), ':'));
	LOCK_ISTATE()
	if (dev->istate == SCANNING || dev->istate == INIT) {
		UNLOCK_ISTATE()
		return;
	}
	UNLOCK_ISTATE()

	assert(dev->curr_bss != NULL);
	if (dev->curr_bss == NULL)
		return;
	LOCK_ISTATE()
	if (dev->istate == REASSOCIATING) {
		UNLOCK_ISTATE()
		assert(dev->new_bss != NULL);
		if (dev->new_bss == NULL)
			return;
	} else
		UNLOCK_ISTATE()
	if (!memcmp(mgmt->addr3, dev->curr_bss->bssid, ETH_ALEN) &&
		(!memcmp(dev->netdev->dev_addr, mgmt->addr1, ETH_ALEN) ||
			!memcmp(bc_addr, mgmt->addr1, ETH_ALEN))) {
		/* this is a DisAssoc from the BSS we are connected or
		   trying to connect to, directed to us or broadcasted */
		/* jal: TODO: can the DisAssoc also come from the BSS
		   we've sent a ReAssocReq to (i.e. from dev->new_bss) ? */
		LOCK_ISTATE()
		if (dev->istate == DISASSOCIATING ||
		    dev->istate == ASSOCIATING  ||
		    dev->istate == REASSOCIATING  ||
		    dev->istate == CONNECTED  ||
		    dev->istate == JOINING)
		{
			if (dev->istate == CONNECTED) {
				UNLOCK_ISTATE()
				netif_carrier_off(dev->netdev);
				netif_stop_queue(dev->netdev);
				iwevent_bss_disconnect(dev->netdev);
			} else UNLOCK_ISTATE()
			del_timer_sync(&dev->mgmt_timer);
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
		} else {
			
		/* ignore DisAssoc in states AUTH, ASSOC */
			info("%s: DisAssoc in state %d ignored",
			     dev->netdev->name, dev->istate);
			UNLOCK_ISTATE()
		}
	}
	/* ignore DisAssoc to other STA or from other BSSID */
} /* rx_mgmt_disassoc */

static void rx_mgmt_auth(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	struct ieee802_11_auth_frame *resp = 
		(struct ieee802_11_auth_frame *)mgmt->payload;
	int seq_nr = le16_to_cpu(resp->seq_nr);
	int alg = le16_to_cpu(resp->algorithm);
	int status = le16_to_cpu(resp->status);

	dbg(DBG_RX_MGMT, "%s: rx AuthFrame bssid %s alg %d seq_nr %d status %d " 
	    "destination %s",
	    dev->netdev->name, mac2str(mgmt->addr3),
	    alg, seq_nr, status,
	    hex2str(dev->obuf, mgmt->addr1,
		    min((int)sizeof(dev->obuf)/3, ETH_ALEN), ':'));

	if (alg == WLAN_AUTH_SHARED_KEY &&
	    seq_nr == 2) {
		dbg(DBG_RX_MGMT, "%s: AuthFrame challenge %s ...",
		    dev->netdev->name,
		    hex2str(dev->obuf, resp->challenge,
			    min((int)sizeof(dev->obuf)/3,18), ' '));
	}
	LOCK_ISTATE()
	if (dev->istate != AUTHENTICATING) {
		info("%s: ignored AuthFrame in state %d",
		     dev->netdev->name, dev->istate);
		UNLOCK_ISTATE()
		return;
	}
	UNLOCK_ISTATE()
	if (dev->auth_mode != alg) {
		info("%s: ignored AuthFrame for alg %d",
		     dev->netdev->name, alg);
		return;
	}

	assert(dev->curr_bss != NULL);
	if (dev->curr_bss == NULL)
		return;

	if (!memcmp(mgmt->addr3, dev->curr_bss->bssid, ETH_ALEN) &&
	    !memcmp(dev->netdev->dev_addr, mgmt->addr1, ETH_ALEN)) {
		/* this is a AuthFrame from the BSS we are connected or
		   trying to connect to, directed to us */
		if (status != WLAN_STATUS_SUCCESS) {
			del_timer_sync(&dev->mgmt_timer);
			/* try to join next bss */
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
			return;
		}

		if (dev->auth_mode == WLAN_AUTH_OPEN ||
			seq_nr == 4) {
			dev->retries = ASSOC_RETRIES;
			NEW_STATE(dev,ASSOCIATING);
			assoc_req(dev, dev->curr_bss);
			dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
			    __FUNCTION__, __LINE__);
			mod_timer(&dev->mgmt_timer,jiffies+HZ);
			return;
		}

		assert(seq_nr == 2);
		auth_req(dev, dev->curr_bss, seq_nr+1, resp->challenge);
		dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer + HZ",
		    __FUNCTION__, __LINE__);
		mod_timer(&dev->mgmt_timer,jiffies+HZ);
	}
	/* else: ignore AuthFrames to other recipients */
} /* rx_mgmt_auth */

static void rx_mgmt_deauth(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	struct ieee802_11_deauth_frame *resp = 
		(struct ieee802_11_deauth_frame *)mgmt->payload;

	dbg(DBG_RX_MGMT|DBG_PROGRESS,
	    "%s: rx DeAuth bssid %s reason x%04x destination %s",
	    dev->netdev->name, mac2str(mgmt->addr3),
	    le16_to_cpu(resp->reason),
	    hex2str(dev->obuf, mgmt->addr1,
		    min((int)sizeof(dev->obuf)/3,ETH_ALEN), ':'));
	LOCK_ISTATE()
	if (dev->istate == DISASSOCIATING ||
	    dev->istate == AUTHENTICATING ||
	    dev->istate == ASSOCIATING ||
	    dev->istate == REASSOCIATING  ||
	    dev->istate == CONNECTED) {
		UNLOCK_ISTATE()
		assert(dev->curr_bss != NULL);
		if (dev->curr_bss == NULL)
			return;

		if (!memcmp(mgmt->addr3, dev->curr_bss->bssid, ETH_ALEN) &&
		(!memcmp(dev->netdev->dev_addr, mgmt->addr1, ETH_ALEN) ||
		 !memcmp(bc_addr, mgmt->addr1, ETH_ALEN))) {
			/* this is a DeAuth from the BSS we are connected or
			   trying to connect to, directed to us or broadcasted */
			LOCK_ISTATE()
			if (dev->istate == CONNECTED) {
				UNLOCK_ISTATE()
				iwevent_bss_disconnect(dev->netdev);
			} else UNLOCK_ISTATE()
			NEW_STATE(dev,JOINING);
			defer_kevent(dev,KEVENT_JOIN);
			del_timer_sync(&dev->mgmt_timer);
		}
		/* ignore DeAuth to other STA or from other BSSID */
	} else {
		/* ignore DeAuth in states SCANNING */
		info("%s: DeAuth in state %d ignored",
		     dev->netdev->name, dev->istate);
		UNLOCK_ISTATE()
	}
} /* rx_mgmt_deauth */

static void rx_mgmt_beacon(struct at76c503 *dev,
			   struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;

	/* beacon content */
	struct ieee802_11_beacon_data *bdata = 
		(struct ieee802_11_beacon_data *)mgmt->payload;

	/* length of var length beacon parameters */
	int varpar_len = min(le16_to_cpu(buf->wlength) -
			     (int)(IEEE802_11_MGMT_HEADER_SIZE +
			      offsetof(struct ieee802_11_beacon_data, data)),
			     BEACON_MAX_DATA_LENGTH);

	struct list_head *lptr;
	struct bss_info *match; /* entry matching addr3 with its bssid */
	int new_entry = 0;
	int len;
	struct data_element {
		u8 type;
		u8 length;
		u8 data_head;
	} *element;
	int have_ssid = 0;
	int have_rates = 0;
	int have_channel = 0;
	int keep_going = 1;
	unsigned long flags;
	
	spin_lock_irqsave(&dev->bss_list_spinlock, flags);
	LOCK_ISTATE()	
	if (dev->istate == CONNECTED) {
		UNLOCK_ISTATE()
		/* in state CONNECTED we use the mgmt_timer to control
		   the beacon of the BSS */
		assert(dev->curr_bss != NULL);
		if (dev->curr_bss == NULL)
			goto rx_mgmt_beacon_end;
		if (!memcmp(dev->curr_bss->bssid, mgmt->addr3, ETH_ALEN)) {
			//dbg(DBG_MGMT_TIMER, "%s:%d: starting mgmt_timer "
			//    "+BEACON_TIMEOUT*HZ", __FUNCTION__, __LINE__);
			mod_timer(&dev->mgmt_timer, jiffies+BEACON_TIMEOUT*HZ);
			dev->curr_bss->rssi = buf->rssi;
			dev->beacons_received++;
			goto rx_mgmt_beacon_end;
		}
	} else 
	    UNLOCK_ISTATE()

	/* look if we have this BSS already in the list */
	match = NULL;

	if (!list_empty(&dev->bss_list)) {
		list_for_each(lptr, &dev->bss_list) {
			struct bss_info *bss_ptr = 
				list_entry(lptr, struct bss_info, list);
			if (!memcmp(bss_ptr->bssid, mgmt->addr3, ETH_ALEN)) {
				match = bss_ptr;
				break;
			}
		}
	}

	if (match == NULL) {
		/* haven't found the bss in the list */
		if ((match=kmalloc(sizeof(struct bss_info), GFP_ATOMIC)) == NULL) {
			dbg(DBG_BSS_TABLE, "%s: cannot kmalloc new bss info (%zd byte)",
			    dev->netdev->name, sizeof(struct bss_info));
			goto rx_mgmt_beacon_end;
		}
		memset(match,0,sizeof(*match));
		new_entry = 1;
		/* append new struct into list */
		list_add_tail(&match->list, &dev->bss_list);
	}
	
	/* we either overwrite an existing entry or append a new one
	   match points to the entry in both cases */
	
	match->capa = le16_to_cpu(bdata->capability_information);
	
	/* while beacon_interval is not (!) */
	match->beacon_interval = le16_to_cpu(bdata->beacon_interval);
	
	match->rssi = buf->rssi;
	match->link_qual = buf->link_quality;
	match->noise_level = buf->noise_level;
	
	memcpy(match->mac,mgmt->addr2,ETH_ALEN); //just for info
	memcpy(match->bssid,mgmt->addr3,ETH_ALEN);
	dbg(DBG_RX_BEACON, "%s: bssid %s", dev->netdev->name, 
			mac2str(match->bssid));
	
	element = (struct data_element*)bdata->data;
	
#define data_end(element) (&(element->data_head) + element->length)
	
	// This routine steps through the bdata->data array to try and get 
	// some useful information about the access point.
	// Currently, this implementation supports receipt of: SSID, 
	// supported transfer rates and channel, in any order, with some 
	// tolerance for intermittent unknown codes (although this 
	// functionality may not be necessary as the useful information will 
	// usually arrive in consecutively, but there have been some 
	// reports of some of the useful information fields arriving in a 
	// different order).
	// It does not support any more IE types although MFIE_TYPE_TIM may 
	// be supported (on my AP at least).  
	// The bdata->data array is about 1500 bytes long but only ~36 of those 
	// bytes are useful, hence the have_ssid etc optimizations.

	while (keep_going &&
	       ((int)(data_end(element) - bdata->data) <= varpar_len)) {

		switch (element->type) {
		
		case MFIE_TYPE_SSID:
			len = min(IW_ESSID_MAX_SIZE, (int)element->length);
			if (!have_ssid && ((new_entry) || 
                                           !is_cloaked_ssid(&(element->data_head), len))) {
			/* we copy only if this is a new entry,
			   or the incoming SSID is not a cloaked SSID. This 
			   will protect us from overwriting a real SSID read 
			   in a ProbeResponse with a cloaked one from a 
			   following beacon. */
			
				match->ssid_len = len;
				memcpy(match->ssid, &(element->data_head), len);
				match->ssid[len] = '\0'; /* terminate the 
							    string for 
							    printing */
				dbg(DBG_RX_BEACON, "%s: SSID - %s", 
					dev->netdev->name, match->ssid);
			}
			have_ssid = 1;
			break;
		
		case MFIE_TYPE_RATES:
			if (!have_rates) {
				match->rates_len = 
					min((int)sizeof(match->rates), 
						(int)element->length);
				memcpy(match->rates, &(element->data_head), 
					match->rates_len);
				have_rates = 1;
				dbg(DBG_RX_BEACON, 
				    "%s: SUPPORTED RATES %s", 
				    dev->netdev->name,
				    hex2str(dev->obuf, &(element->data_head),
					    min((sizeof(dev->obuf)-1)/2,
						(size_t)element->length), '\0'));
			}
			break;
		
		case MFIE_TYPE_DS_SET:
			if (!have_channel) {
				match->channel = element->data_head;
				have_channel = 1;
				dbg(DBG_RX_BEACON, "%s: CHANNEL - %d", 
					dev->netdev->name, match->channel);
			}
			break;
		
		case MFIE_TYPE_CF_SET:
		case MFIE_TYPE_TIM:
		case MFIE_TYPE_IBSS_SET:
		default:
		{
			dbg(DBG_RX_BEACON, "%s: beacon IE id %d len %d %s",
			    dev->netdev->name, element->type, element->length,
			    hex2str(dev->obuf,&(element->data_head),
				    min((sizeof(dev->obuf)-1)/2,
					(size_t)element->length),'\0'));
			break;
		}

		} // switch (element->type)
		
		// advance to the next 'element' of data
		element = (struct data_element*)data_end(element);

		// Optimization: after all, the bdata->data array is  
		// varpar_len bytes long, whereas we get all of the useful 
		// information after only ~36 bytes, this saves us a lot of 
		// time (and trouble as the remaining portion of the array 
		// could be full of junk)
		// Comment this out if you want to see what other information
		// comes from the AP - although little of it may be useful

		//if (have_ssid && have_rates && have_channel)
		//	keep_going = 0;
	}
	
	dbg(DBG_RX_BEACON, "%s: Finished processing beacon data", 
		dev->netdev->name);
	
	match->last_rx = jiffies; /* record last rx of beacon */
	
rx_mgmt_beacon_end:
	spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);
} /* rx_mgmt_beacon */


/* calc the link level from a given rx_buffer */
static void calc_level(struct at76c503 *dev, struct at76c503_rx_buffer *buf, struct iw_quality* qual)
{	int max_rssi = 42; /* just a gues for now, might be different for other chips */

    	qual->level = (buf->rssi * 100 / max_rssi);
	if (qual->level > 100)
		qual->level = 100;
	qual->updated |= IW_QUAL_LEVEL_UPDATED; 
}


/* calc the link quality from a given rx_buffer */
static void calc_qual(struct at76c503 *dev, struct at76c503_rx_buffer *buf, struct iw_quality* qual)
{
	if((dev->board_type == BOARDTYPE_503_INTERSIL_3861) ||
	   (dev->board_type == BOARDTYPE_503_INTERSIL_3863)) {
	    qual->qual=buf->link_quality;
	} else {
	    qual->qual = qual->level * dev->beacons_received *
	    		        dev->beacon_period /
	                        (jiffies_to_msecs(jiffies) - dev->beacons_last_qual);
		
	    dev->beacons_last_qual = jiffies_to_msecs(jiffies);
	    dev->beacons_received = 0;
	}
	qual->qual = (qual->qual > 100) ? 100 : qual->qual;
	qual->updated |= IW_QUAL_QUAL_UPDATED;
}


/* calc the noise quality from a given rx_buffer */
static void calc_noise(struct at76c503 *dev, struct at76c503_rx_buffer *buf, struct iw_quality* qual)
{
	qual->noise = 0;
	qual->updated |= IW_QUAL_NOISE_INVALID;
}


static void update_wstats(struct at76c503 *dev, struct at76c503_rx_buffer *buf)
{
	struct iw_quality *qual = &dev->wstats.qual;

	if (buf->rssi && dev->istate == CONNECTED) {
		qual->updated = 0;
		calc_level(dev, buf, qual);
		calc_qual(dev, buf, qual);
		calc_noise(dev, buf, qual);
	} else {
		qual->qual = 0;
		qual->level = 0;
		qual->noise = 0;
		qual->updated = IW_QUAL_ALL_INVALID;
	}
}

static void rx_mgmt(struct at76c503 *dev, struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *mgmt =
		(struct ieee80211_hdr_3addr *)buf->packet;
	u16 subtype = le16_to_cpu(mgmt->frame_ctl) & IEEE80211_FCTL_STYPE;

	/* update wstats */
	LOCK_ISTATE()
	if (dev->istate != INIT && dev->istate != SCANNING) {
		UNLOCK_ISTATE()
		/* jal: this is a dirty hack needed by Tim in ad-hoc mode */
		if (dev->iw_mode == IW_MODE_ADHOC ||
		    (dev->curr_bss != NULL &&
		     !memcmp(mgmt->addr3, dev->curr_bss->bssid, ETH_ALEN))) {
			/* Data packets always seem to have a 0 link level, so we
			   only read link quality info from management packets.
			   Atmel driver actually averages the present, and previous
			   values, we just present the raw value at the moment - TJS */
			
			update_wstats(dev, buf);
		}
	} else UNLOCK_ISTATE()

	if (at76_debug & DBG_RX_MGMT_CONTENT) {
		dbg_uc("%s rx mgmt subtype x%x %s",
		       dev->netdev->name, subtype,
		       hex2str(dev->obuf, (u8 *)mgmt, 
			       min((sizeof(dev->obuf)-1)/2,
				   (size_t)le16_to_cpu(buf->wlength)), '\0'));
	}

	switch (subtype) {
	case IEEE80211_STYPE_BEACON:
	case IEEE80211_STYPE_PROBE_RESP:
		rx_mgmt_beacon(dev,buf);
		break;

	case IEEE80211_STYPE_ASSOC_RESP:
		rx_mgmt_assoc(dev,buf);
		break;

	case IEEE80211_STYPE_REASSOC_RESP:
		rx_mgmt_reassoc(dev,buf);
		break;

	case IEEE80211_STYPE_DISASSOC:
		rx_mgmt_disassoc(dev,buf);
		break;

	case IEEE80211_STYPE_AUTH:
		rx_mgmt_auth(dev,buf);
		break;

	case IEEE80211_STYPE_DEAUTH:
		rx_mgmt_deauth(dev,buf);
		break;

	default:
		info("%s: mgmt, but not beacon, subtype = %x",
		     dev->netdev->name, subtype);
	}

	return;
}

static void dbg_dumpbuf(const char *tag, const u8 *buf, int size)
{
	int i;

	if (!at76_debug) return;

	for (i=0; i<size; i++) {
		if ((i % 8) == 0) {
			if (i) printk("\n");
			printk(KERN_DEBUG __FILE__ ": %s: ", tag);
		}
		printk("%02x ", buf[i]);
	}
	printk("\n");
}

/* A short overview on Ethernet-II, 802.2, 802.3 and SNAP
   (taken from http://www.geocities.com/billalexander/ethernet.html):

Ethernet Frame Formats:

Ethernet (a.k.a. Ethernet II)

        +---------+---------+---------+----------
        |   Dst   |   Src   |  Type   |  Data... 
        +---------+---------+---------+----------

         <-- 6 --> <-- 6 --> <-- 2 --> <-46-1500->

         Type 0x80 0x00 = TCP/IP
         Type 0x06 0x00 = XNS
         Type 0x81 0x37 = Novell NetWare
         

802.3

        +---------+---------+---------+----------
        |   Dst   |   Src   | Length  | Data...  
        +---------+---------+---------+----------

         <-- 6 --> <-- 6 --> <-- 2 --> <-46-1500->

802.2 (802.3 with 802.2 header)

        +---------+---------+---------+-------+-------+-------+----------
        |   Dst   |   Src   | Length  | DSAP  | SSAP  |Control| Data...  
        +---------+---------+---------+-------+-------+-------+----------

                                       <- 1 -> <- 1 -> <- 1 -> <-43-1497->

SNAP (802.3 with 802.2 and SNAP headers) 

        +---------+---------+---------+-------+-------+-------+-----------+---------+-----------
        |   Dst   |   Src   | Length  | 0xAA  | 0xAA  | 0x03  |  Org Code |   Type  | Data...   
        +---------+---------+---------+-------+-------+-------+-----------+---------+-----------

                                                               <--  3  --> <-- 2 --> <-38-1492->

*/

/* Convert the 802.11 header on a packet into an ethernet-style header
 * (basically, pretend we're an ethernet card receiving ethernet packets)
 *
 * This routine returns with the skbuff pointing to the actual data (just past
 * the end of the newly-created ethernet header).
 */
static void ieee80211_to_eth(struct sk_buff *skb, int iw_mode)
{
	struct ieee80211_hdr_3addr *i802_11_hdr;
	struct ethhdr *eth_hdr_p;
	u8 *src_addr;
	u8 *dest_addr;
	__be16 proto = 0;
	int build_ethhdr = 1;

	i802_11_hdr = (struct ieee80211_hdr_3addr *)skb->data;

#ifdef DEBUG
	{
		dbg_uc("%s: ENTRY skb len %d data %s", __FUNCTION__,
		       skb->len, hex2str(dev->obuf, skb->data,
					 min((int)sizeof(dev->obuf)/3,64), ' '));
	}
#endif

	skb_pull(skb, sizeof(struct ieee80211_hdr_3addr));
//	skb_trim(skb, skb->len - 4); /* Trim CRC */

	src_addr = iw_mode == IW_MODE_ADHOC ? i802_11_hdr->addr2
	       				    : i802_11_hdr->addr3;
	dest_addr = i802_11_hdr->addr1;

	eth_hdr_p = (struct ethhdr *)skb->data;
	if (!memcmp(eth_hdr_p->h_source, src_addr, ETH_ALEN) &&
	    !memcmp(eth_hdr_p->h_dest, dest_addr, ETH_ALEN)) {
		/* An ethernet frame is encapsulated within the data portion.
		 * Just use its header instead. */
		skb_pull(skb, sizeof(struct ethhdr));
		build_ethhdr = 0;
	} else if (!memcmp(skb->data, snapsig, sizeof(snapsig))) {
		/* SNAP frame - collapse it */
		skb_pull(skb, sizeof(rfc1042sig)+2);
		proto = *(__be16 *)(skb->data - 2);
	} else {
#ifdef IEEE_STANDARD
		/* According to all standards, we should assume the data
		 * portion contains 802.2 LLC information, so we should give it
		 * an 802.3 header (which has the same implications) */
		proto = htons(skb->len);
#else /* IEEE_STANDARD */
		/* Unfortunately, it appears no actual 802.11 implementations
		 * follow any standards specs.  They all appear to put a
		 * 16-bit ethertype after the 802.11 header instead, so we take
		 * that value and make it into an Ethernet-II packet. */
		/* Note that this means we can never support non-SNAP 802.2
		 * frames (because we can't tell when we get one) */

		/* jal: This isn't true. My WRT54G happily sends SNAP.
		   Difficult to speak for all APs, so I don't dare to define
		   IEEE_STANDARD ... */
		proto = *(__be16 *)(skb->data);
		skb_pull(skb, 2);
#endif /* IEEE_STANDARD */
	}

	eth_hdr_p = (struct ethhdr *)(skb->data-sizeof(struct ethhdr));
	set_eth_hdr(skb, eth_hdr_p);
	if (build_ethhdr) {
		/* This needs to be done in this order (eth_hdr_p->h_dest may
		 * overlap src_addr) */
		memcpy(eth_hdr_p->h_source, src_addr, ETH_ALEN);
		memcpy(eth_hdr_p->h_dest, dest_addr, ETH_ALEN);
		/* make an 802.3 header (proto = length) */
		eth_hdr_p->h_proto = proto;
	}

	if (ntohs(eth_hdr_p->h_proto) > 1518) {
		skb->protocol = eth_hdr_p->h_proto;
	} else if (*(unsigned short *)skb->data == 0xFFFF) {
		/* Magic hack for Novell IPX-in-802.3 packets */
		skb->protocol = htons(ETH_P_802_3);
	} else {
		/* Assume it's an 802.2 packet (it should be, and we have no
		 * good way to tell if it isn't) */
		skb->protocol = htons(ETH_P_802_2);
	}

#ifdef DEBUG
	{
		char da[3*ETH_ALEN], sa[3*ETH_ALEN];
		dbg_uc("%s: EXIT skb da %s sa %s proto x%04x len %d data %s", __FUNCTION__,
		       hex2str(da, eth_hdr(skb)->h_dest, ETH_ALEN, ':'),
		       hex2str(sa, eth_hdr(skb)->h_source, ETH_ALEN, ':'),
		       ntohs(skb->protocol), skb->len,
		       hex2str(dev->obuf, skb->data, 
			       min((int)sizeof(dev->obuf)/3,64), ' '));
	}
#endif

}

/* Adjust the skb to trim the hardware header and CRC, and set up skb->mac,
 * skb->protocol, etc.
 */ 
static void ieee80211_fixup(struct sk_buff *skb, int iw_mode)
{
	struct ieee80211_hdr_3addr *i802_11_hdr;
	struct ethhdr *eth_hdr_p;
	u8 *src_addr;
	u8 *dest_addr;
	__be16 proto = 0;

	i802_11_hdr = (struct ieee80211_hdr_3addr *)skb->data;
	skb_pull(skb, sizeof(struct ieee80211_hdr_3addr));
//	skb_trim(skb, skb->len - 4); /* Trim CRC */

	src_addr = iw_mode == IW_MODE_ADHOC ? i802_11_hdr->addr2
	       				    : i802_11_hdr->addr3;
	dest_addr = i802_11_hdr->addr1;

	skb->mac.raw = (unsigned char *)i802_11_hdr;

	eth_hdr_p = (struct ethhdr *)skb->data;
	if (!memcmp(eth_hdr_p->h_source, src_addr, ETH_ALEN) &&
	    !memcmp(eth_hdr_p->h_dest, dest_addr, ETH_ALEN)) {
		/* There's an ethernet header encapsulated within the data
		 * portion, count it as part of the hardware header */
		skb_pull(skb, sizeof(struct ethhdr));
		proto = eth_hdr_p->h_proto;
	} else if (!memcmp(skb->data, snapsig, sizeof(snapsig))) {
		/* SNAP frame - collapse it */
		/* RFC1042/802.1h encapsulated packet.  Treat the SNAP header
		 * as part of the HW header and note the protocol. */
		/* NOTE: prism2 doesn't collapse Appletalk frames (why?). */
		skb_pull(skb, sizeof(rfc1042sig) + 2);
		proto = *(__be16 *)(skb->data - 2);
	}

	if (ntohs(proto) > 1518) {
		skb->protocol = proto;
	} else {
#ifdef IEEE_STANDARD
		/* According to all standards, we should assume the data
		 * portion contains 802.2 LLC information */
		skb->protocol = htons(ETH_P_802_2);
#else /* IEEE_STANDARD */
		/* Unfortunately, it appears no actual 802.11 implementations
		 * follow any standards specs.  They all appear to put a
		 * 16-bit ethertype after the 802.11 header instead, so we'll
		 * use that (and consider it part of the hardware header). */
		/* Note that this means we can never support non-SNAP 802.2
		 * frames (because we can't tell when we get one) */
		skb->protocol = *(__be16 *)skb->data;
		skb_pull(skb, 2);
#endif /* IEEE_STANDARD */
	}
}

/* check for fragmented data in dev->rx_skb. If the packet was no fragment
   or it was the last of a fragment set a skb containing the whole packet
   is returned for further processing. Otherwise we get NULL and are
   done and the packet is either stored inside the fragment buffer
   or thrown away. The check for rx_copybreak is moved here.
   Every returned skb starts with the ieee802_11 header and contains
   _no_ FCS at the end */
static struct sk_buff *check_for_rx_frags(struct at76c503 *dev)
{	
	struct sk_buff *skb = (struct sk_buff *)dev->rx_skb;
	struct at76c503_rx_buffer *buf = (struct at76c503_rx_buffer *)skb->data;
	struct ieee80211_hdr_3addr *i802_11_hdr =
		(struct ieee80211_hdr_3addr *)buf->packet;
	/* seq_ctrl, fragment_number, sequence number of new packet */
	u16 sctl = le16_to_cpu(i802_11_hdr->seq_ctl);
	u16 fragnr = sctl & 0xf;
	u16 seqnr = sctl>>4;
	u16 frame_ctl = le16_to_cpu(i802_11_hdr->frame_ctl);

	/* length including the IEEE802.11 header, excl. the trailing FCS,
	   excl. the struct at76c503_rx_buffer */
	int length = le16_to_cpu(buf->wlength) - dev->rx_data_fcs_len;
	
	/* where does the data payload start in skb->data ? */
	u8 *data = (u8 *)i802_11_hdr + sizeof(struct ieee80211_hdr_3addr);

	/* length of payload, excl. the trailing FCS */
	int data_len = length - (data - (u8 *)i802_11_hdr);

	int i;
	struct rx_data_buf *bptr, *optr;
	unsigned long oldest = ~0UL;

	dbg(DBG_RX_FRAGS, "%s: rx data frame_ctl %04x addr2 %s seq/frag %d/%d "
	    "length %d data %d: %s ...",
	    dev->netdev->name, frame_ctl,
	    mac2str(i802_11_hdr->addr2),
	    seqnr, fragnr, length, data_len,
	    hex2str(dev->obuf, data,
		    min((int)(sizeof(dev->obuf)-1)/2,32), '\0'));

	dbg(DBG_RX_FRAGS_SKB, "%s: incoming skb: head %p data %p "
	    "tail %p end %p len %d",
	    dev->netdev->name, skb->head, skb->data, skb->tail,
	    skb->end, skb->len);

	if (data_len <= 0) {
		/* buffers contains no data */
		info("%s: rx skb without data", dev->netdev->name);
		return NULL;
	}

	if (fragnr == 0 && !(frame_ctl & IEEE80211_FCTL_MOREFRAGS)) {
		/* unfragmented packet received */
		if (length < rx_copybreak && (skb = dev_alloc_skb(length)) != NULL) {
			memcpy(skb_put(skb, length),
			       dev->rx_skb->data + AT76C503_RX_HDRLEN, length);
		} else {
			skb_pull(skb, AT76C503_RX_HDRLEN);
			skb_trim(skb, length);
			/* Use a new skb for the next receive */
			dev->rx_skb = NULL;
		}

		dbg(DBG_RX_FRAGS, "%s: unfragmented", dev->netdev->name);

		return skb;
	}

	/* remove the at76c503_rx_buffer header - we don't need it anymore */
	/* we need the IEEE802.11 header (for the addresses) if this packet
	   is the first of a chain */

	assert(length  > AT76C503_RX_HDRLEN);
	skb_pull(skb, AT76C503_RX_HDRLEN);
	/* remove FCS at end */
	skb_trim(skb, length);

	dbg(DBG_RX_FRAGS_SKB, "%s: trimmed skb: head %p data %p tail %p "
	    "end %p len %d data %p data_len %d",
	    dev->netdev->name, skb->head, skb->data, skb->tail,
	    skb->end, skb->len, data, data_len);

	/* look if we've got a chain for the sender address.
	   afterwards optr points to first free or the oldest entry,
	   or, if i < NR_RX_DATA_BUF, bptr points to the entry for the
	   sender address */
	/* determining the oldest entry doesn't cope with jiffies wrapping
	   but I don't care to delete a young entry at these rare moments ... */

	for(i=0,bptr=dev->rx_data,optr=NULL; i < NR_RX_DATA_BUF; i++,bptr++) {
		if (bptr->skb != NULL) {
			if (!memcmp(i802_11_hdr->addr2, bptr->sender,ETH_ALEN))
				break;
			else
				if (optr == NULL) {
					optr = bptr;
					oldest = bptr->last_rx;
				} else {
					if (bptr->last_rx < oldest)
						optr = bptr;
				}
		} else {
			optr = bptr;
			oldest = 0UL;
		}
	}
	
	if (i < NR_RX_DATA_BUF) {

		dbg(DBG_RX_FRAGS, "%s: %d. cacheentry (seq/frag=%d/%d) "
		    "matched sender addr",
		    dev->netdev->name, i, bptr->seqnr, bptr->fragnr);

		/* bptr points to an entry for the sender address */
		if (bptr->seqnr == seqnr) {
			int left;
			/* the fragment has the current sequence number */
			if (((bptr->fragnr+1)&0xf) == fragnr) {
				bptr->last_rx = jiffies;
				/* the next following fragment number ->
				    add the data at the end */
				/* is & 0xf necessary above ??? */

				// for test only ???
				if ((left=skb_tailroom(bptr->skb)) < data_len) {
					info("%s: only %d byte free (need %d)",
					    dev->netdev->name, left, data_len);
				} else 
					memcpy(skb_put(bptr->skb, data_len),
					       data, data_len);
				bptr->fragnr = fragnr;
				if (!(frame_ctl &
				      IEEE80211_FCTL_MOREFRAGS)) {
					/* this was the last fragment - send it */
					skb = bptr->skb;
					bptr->skb = NULL; /* free the entry */
					dbg(DBG_RX_FRAGS, "%s: last frag of seq %d",
					    dev->netdev->name, seqnr);
					return skb;
				} else
					return NULL;
			} else {
				/* wrong fragment number -> ignore it */
				dbg(DBG_RX_FRAGS, "%s: frag nr does not match: %d+1 != %d",
				    dev->netdev->name, bptr->fragnr, fragnr);
				return NULL;
			}
		} else {
			/* got another sequence number */
			if (fragnr == 0) {
				/* it's the start of a new chain - replace the
				   old one by this */
				/* bptr->sender has the correct value already */
				dbg(DBG_RX_FRAGS, "%s: start of new seq %d, "
				    "removing old seq %d", dev->netdev->name,
				    seqnr, bptr->seqnr);
				bptr->seqnr = seqnr;
				bptr->fragnr = 0;
				bptr->last_rx = jiffies;
				/* swap bptr->skb and dev->rx_skb */
				skb = bptr->skb;
				bptr->skb = dev->rx_skb;
				dev->rx_skb = skb;
			} else {
				/* it from the middle of a new chain ->
				   delete the old entry and skip the new one */
				dbg(DBG_RX_FRAGS, "%s: middle of new seq %d (%d) "
				    "removing old seq %d", dev->netdev->name,
				    seqnr, fragnr, bptr->seqnr);
				dev_kfree_skb(bptr->skb);
				bptr->skb = NULL;
			}
			return NULL;
		}
	} else {
		/* if we didn't find a chain for the sender address optr
		   points either to the first free or the oldest entry */

		if (fragnr != 0) {
			/* this is not the begin of a fragment chain ... */
			dbg(DBG_RX_FRAGS, "%s: no chain for non-first fragment (%d)",
			    dev->netdev->name, fragnr);
			return NULL;
		}
		assert(optr != NULL);
		if (optr == NULL)
			return NULL;

		if (optr->skb != NULL) {
			/* swap the skb's */
			skb = optr->skb;
			optr->skb = dev->rx_skb;
			dev->rx_skb = skb;

			dbg(DBG_RX_FRAGS, "%s: free old contents: sender %s seq/frag %d/%d",
			    dev->netdev->name, mac2str(optr->sender),
			    optr->seqnr, optr->fragnr); 

		} else {
			/* take the skb from dev->rx_skb */
			optr->skb = dev->rx_skb;
			dev->rx_skb = NULL; /* let submit_rx_urb() allocate a new skb */

			dbg(DBG_RX_FRAGS, "%s: use a free entry", dev->netdev->name);
		}
		memcpy(optr->sender, i802_11_hdr->addr2, ETH_ALEN);
		optr->seqnr = seqnr;
		optr->fragnr = 0;
		optr->last_rx = jiffies;
		
		return NULL;
	}
} /* check_for_rx_frags */

/* rx interrupt: we expect the complete data buffer in dev->rx_skb */
static void rx_data(struct at76c503 *dev)
{
	struct net_device *netdev = (struct net_device *)dev->netdev;
	struct net_device_stats *stats = &dev->stats;
	struct sk_buff *skb = dev->rx_skb;
	struct at76c503_rx_buffer *buf = (struct at76c503_rx_buffer *)skb->data;
	struct ieee80211_hdr_3addr *i802_11_hdr;
	int length = le16_to_cpu(buf->wlength);

	if (at76_debug & DBG_RX_DATA) {
		dbg_uc("%s received data packet:", netdev->name);
		dbg_dumpbuf(" rxhdr", skb->data, AT76C503_RX_HDRLEN);
	}
	if (at76_debug & DBG_RX_DATA_CONTENT)
		dbg_dumpbuf("packet", skb->data + AT76C503_RX_HDRLEN,
			    length);

	if ((skb=check_for_rx_frags(dev)) == NULL)
		return;

	/* if an skb is returned, the at76c503a_rx_header and the FCS is already removed */
	i802_11_hdr = (struct ieee80211_hdr_3addr *)skb->data;

	skb->dev = netdev;
	skb->ip_summed = CHECKSUM_NONE; /* TODO: should check CRC */

	if (i802_11_hdr->addr1[0] & 1) {
		if (!memcmp(i802_11_hdr->addr1, netdev->broadcast, ETH_ALEN))
			skb->pkt_type = PACKET_BROADCAST;
		else
			skb->pkt_type = PACKET_MULTICAST;
	} else if (memcmp(i802_11_hdr->addr1, netdev->dev_addr, ETH_ALEN)) {
		skb->pkt_type=PACKET_OTHERHOST;
	}

	if (netdev->type == ARPHRD_ETHER) {
		ieee80211_to_eth(skb, dev->iw_mode);
	} else {
		ieee80211_fixup(skb, dev->iw_mode);
	}

	netdev->last_rx = jiffies;
	netif_rx(skb);
	stats->rx_packets++;
	stats->rx_bytes += length;

	return;
}

static int submit_rx_urb(struct at76c503 *dev)
{
	int ret, size;
	struct sk_buff *skb = dev->rx_skb;
	
	if (dev->read_urb == NULL) {
		err("%s: dev->read_urb is NULL", __FUNCTION__);
		return -EFAULT;
	}

	if (skb == NULL) {
		skb = dev_alloc_skb(sizeof(struct at76c503_rx_buffer));
		if (skb == NULL) {
			err("%s: unable to allocate rx skbuff.", dev->netdev->name);
			ret = -ENOMEM;
			goto exit;
		}
		dev->rx_skb = skb;
	} else {
		skb_push(skb, skb_headroom(skb));
		skb_trim(skb, 0);
	}

	size = skb_tailroom(skb);
	FILL_BULK_URB(dev->read_urb, dev->udev,
		usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
		skb_put(skb, size), size,
		(usb_complete_t)at76c503_read_bulk_callback, dev);
	ret = usb_submit_urb(dev->read_urb, GFP_ATOMIC);
	if (ret < 0) { 
		if (ret == -ENODEV) 
			dbg(DBG_DEVSTART, "usb_submit_urb returned -ENODEV");
		else
			err("%s: rx, usb_submit_urb failed: %d", dev->netdev->name, ret);
	}

exit:
	if (ret < 0) {
		if (ret != -ENODEV) {
			/* If we can't submit the URB, the adapter becomes completely
		 	* useless, so try again later */
			if (--dev->nr_submit_rx_tries > 0)
				defer_kevent(dev, KEVENT_SUBMIT_RX);
			else {
				err("%s: giving up to submit rx urb after %d failures -"
			    	    " please unload the driver and/or power cycle the device",
			    	    dev->netdev->name, NR_SUBMIT_RX_TRIES);
			}
		}
	} else
		/* reset counter to initial value */
		dev->nr_submit_rx_tries = NR_SUBMIT_RX_TRIES;
	return ret;
}


/* we are doing a lot of things here in an interrupt. Need
   a bh handler (Watching TV with a TV card is probably
   a good test: if you see flickers, we are doing too much.
   Currently I do see flickers... even with our tasklet :-( )
   Maybe because the bttv driver and usb-uhci use the same interrupt
*/
/* Or maybe because our BH handler is preempting bttv's BH handler.. BHs don't
 * solve everything.. (alex) */
static void at76c503_read_bulk_callback (struct urb *urb)
{
	struct at76c503 *dev = (struct at76c503 *)urb->context;

	dev->rx_urb = urb;
	tasklet_schedule(&dev->tasklet);

	return;
}

static void rx_monitor_mode(struct at76c503 *dev)
{
	struct net_device *netdev = (struct net_device *)dev->netdev;
	struct at76c503_rx_buffer *buf = 
		(struct at76c503_rx_buffer *)dev->rx_skb->data;
	/* length including the IEEE802.11 header, excl. the trailing FCS,
		excl. the struct at76c503_rx_buffer */
	int length = le16_to_cpu(buf->wlength) - dev->rx_data_fcs_len;
	struct sk_buff *skb = dev->rx_skb;
	struct net_device_stats *stats = &dev->stats;
	struct iw_statistics *wstats = &dev->wstats;

	update_wstats(dev, buf);

	if (length < 0) {
		/* buffer contains no data */
		dbg(DBG_MONITOR_MODE, "%s: MONITOR MODE: rx skb without data",
		    dev->netdev->name);
		return;
	}

	if (netdev->type == ARPHRD_IEEE80211_PRISM) {
		int skblen = sizeof(p80211msg_lnxind_wlansniffrm_t) + length;
		p80211msg_lnxind_wlansniffrm_t *prism;
		u8* payload;

		if ((skb = dev_alloc_skb(skblen)) == NULL) {
			err("%s: MONITOR MODE: dev_alloc_skb for Prism header "
			    "returned NULL", dev->netdev->name);
			return;
		}

		skb_put(skb, skblen);

		prism = (p80211msg_lnxind_wlansniffrm_t*)skb->data;
		payload = skb->data + sizeof(p80211msg_lnxind_wlansniffrm_t);

		prism->msgcode = DIDmsg_lnxind_wlansniffrm;
		prism->msglen = sizeof(p80211msg_lnxind_wlansniffrm_t);
		strcpy(prism->devname, netdev->name);

		prism->hosttime.did = DIDmsg_lnxind_wlansniffrm_hosttime;
		prism->hosttime.status = 0;
		prism->hosttime.len = 4;
		prism->hosttime.data = jiffies;

		prism->mactime.did = DIDmsg_lnxind_wlansniffrm_mactime;
		prism->mactime.status = 0;
		prism->mactime.len = 4;
		memcpy(&prism->mactime.data, buf->rx_time, 4);

		prism->channel.did = DIDmsg_lnxind_wlansniffrm_channel;
		prism->channel.status = P80211ENUM_msgitem_status_no_value;
		prism->channel.len = 4;
		/* INFO: The channel is encoded in the beacon info. 
		   (Kismet can figure it out, so less processing here) */
		prism->channel.data = 0;

		prism->rssi.did = DIDmsg_lnxind_wlansniffrm_rssi;
		prism->rssi.status = 0;
		prism->rssi.len = 4;
		prism->rssi.data = buf->rssi;

		prism->sq.did = DIDmsg_lnxind_wlansniffrm_sq;
		prism->sq.status = 0;
		prism->sq.len = 4;
		prism->sq.data = wstats->qual.qual;

		prism->signal.did = DIDmsg_lnxind_wlansniffrm_signal;
		prism->signal.status = 0;
		prism->signal.len = 4;
		prism->signal.data = wstats->qual.level;

		prism->noise.did = DIDmsg_lnxind_wlansniffrm_noise;
		prism->noise.status = 0;
		prism->noise.len = 4;
		prism->noise.data = wstats->qual.noise;

		prism->rate.did = DIDmsg_lnxind_wlansniffrm_rate;
		prism->rate.status = 0;
		prism->rate.len = 4;
		prism->rate.data = hw_rates[buf->rx_rate] & (~0x80);

		prism->istx.did = DIDmsg_lnxind_wlansniffrm_istx;
		prism->istx.status = 0;
		prism->istx.len = 4;
		prism->istx.data = P80211ENUM_truth_false;

		prism->frmlen.did = DIDmsg_lnxind_wlansniffrm_frmlen;
		prism->frmlen.status = 0;
		prism->frmlen.len = 4;
		prism->frmlen.data = length;

		memcpy(payload, buf->packet, length);
	} else {
		/* netdev->type != ARPHRD_IEEE80211_PRISM */
		skb_pull(skb, AT76C503_RX_HDRLEN);
		skb_trim(skb, length);
		dev->rx_skb = NULL;
	}

	skb->dev = netdev;
	skb->ip_summed = CHECKSUM_NONE;
	skb->mac.raw = skb->data;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_80211_RAW);

	netdev->last_rx = jiffies;
	netif_rx(skb);
	stats->rx_packets++;
	stats->rx_bytes += length;
} /* end of rx_monitor_mode */


static void rx_tasklet(unsigned long param)
{
	struct at76c503 *dev = (struct at76c503 *)param;
	struct urb *urb;
	struct net_device *netdev;
	struct at76c503_rx_buffer *buf;
	struct ieee80211_hdr_3addr *i802_11_hdr;
	u16 frame_ctl;

	if (!dev) return;
	urb = dev->rx_urb;
	netdev = (struct net_device *)dev->netdev;

	if (dev->device_unplugged) {
		dbg(DBG_DEVSTART, "device unplugged");
		if (urb)
			dbg(DBG_DEVSTART, "urb status %d", urb->status);
		return;
	}


	if(!urb || !dev->rx_skb || !netdev || !dev->rx_skb->data) return;

	buf = (struct at76c503_rx_buffer *)dev->rx_skb->data;

	if (!buf) return;

	i802_11_hdr = (struct ieee80211_hdr_3addr *)buf->packet;
	if (!i802_11_hdr) return;

	frame_ctl = le16_to_cpu(i802_11_hdr->frame_ctl);

	if(urb->status != 0){
		if ((urb->status != -ENOENT) && 
		    (urb->status != -ECONNRESET)) {
			dbg(DBG_URB,"%s %s: - nonzero read bulk status received: %d",
			    __FUNCTION__, netdev->name, urb->status);
			goto no_more_urb;
		}
		return;
	}

	if (at76_debug & DBG_RX_ATMEL_HDR) {
		dbg_uc("%s: rx frame: rate %d rssi %d noise %d link %d %s",
		    dev->netdev->name,
		    buf->rx_rate, buf->rssi, buf->noise_level,
		    buf->link_quality,
		    hex2str(dev->obuf,(u8 *)i802_11_hdr,
			    min((int)(sizeof(dev->obuf)-1)/2,48),'\0'));
	}
	LOCK_ISTATE()
	if (dev->istate == MONITORING) {
		UNLOCK_ISTATE()
		rx_monitor_mode(dev);
		goto finish;
	}
	UNLOCK_ISTATE()

	/* there is a new bssid around, accept it: */
	if(buf->newbss && dev->iw_mode == IW_MODE_ADHOC) {
		dbg(DBG_PROGRESS, "%s: rx newbss", netdev->name);
		defer_kevent(dev, KEVENT_NEW_BSS);
	}

	switch (frame_ctl & IEEE80211_FCTL_FTYPE) {
	case IEEE80211_FTYPE_DATA:
		rx_data(dev);
		break;

	case IEEE80211_FTYPE_MGMT:
		/* jal: TODO: find out if we can update iwspy also on
		   other frames than management (might depend on the
		   radio chip / firmware version !) */

		iwspy_update(dev, buf);

		rx_mgmt(dev, buf);
		break;

	case IEEE80211_FTYPE_CTL:
		dbg(DBG_RX_CTRL, "%s: ignored ctrl frame: %04x", dev->netdev->name,
		    frame_ctl);
		break;

	default:
		info("%s: it's a frame from mars: %2x", dev->netdev->name,
		     frame_ctl);
	} /* switch (frame_ctl & IEEE80211_FCTL_FTYPE) */
finish:
	submit_rx_urb(dev);
 no_more_urb:
	return;
}

static void at76c503_write_bulk_callback (struct urb *urb)
{
	struct at76c503 *dev = (struct at76c503 *)urb->context;
	struct net_device_stats *stats = &dev->stats;
	unsigned long flags;
	struct at76c503_tx_buffer *mgmt_buf;
	int ret;

	if(urb->status != 0){
		if((urb->status != -ENOENT) && 
		   (urb->status != -ECONNRESET)) {
			dbg(DBG_URB, "%s - nonzero write bulk status received: %d",
			    __FUNCTION__, urb->status);
		}else
			return; /* urb has been unlinked */
		stats->tx_errors++;
	}else
		stats->tx_packets++;

	spin_lock_irqsave(&dev->mgmt_spinlock, flags);
	mgmt_buf = dev->next_mgmt_bulk;
	dev->next_mgmt_bulk = NULL;
	spin_unlock_irqrestore(&dev->mgmt_spinlock, flags);

	if (mgmt_buf) {
		/* we don't copy the padding bytes, but add them
		   to the length */
		memcpy(dev->bulk_out_buffer, mgmt_buf,
		       le16_to_cpu(mgmt_buf->wlength) +
		       offsetof(struct at76c503_tx_buffer,packet));
		FILL_BULK_URB(dev->write_urb, dev->udev,
			      usb_sndbulkpipe(dev->udev, 
					      dev->bulk_out_endpointAddr),
			      dev->bulk_out_buffer,
			      le16_to_cpu(mgmt_buf->wlength) + 
			      mgmt_buf->padding + AT76C503_TX_HDRLEN,
			      (usb_complete_t)at76c503_write_bulk_callback, dev);
		ret = usb_submit_urb(dev->write_urb, GFP_ATOMIC);
		if (ret) {
			err("%s: %s error in tx submit urb: %d",
			    dev->netdev->name, __FUNCTION__, ret);
		}
		kfree(mgmt_buf);
	} else
		netif_wake_queue(dev->netdev);

}

#ifdef CONFIG_IPAQ_HANDHELD

static struct timer_list led_timer;

static void
ipaq_clear_led (unsigned long time)
{
	ipaq_led_off (RED_LED_2);
}

static void
ipaq_blink_led (void)
{
	ipaq_led_on (RED_LED_2);

	mod_timer (&led_timer, jiffies + (HZ / 25));
}

static void
ipaq_init_led (void)
{
	led_timer.function = ipaq_clear_led;

	init_timer (&led_timer);
}

#endif

static int at76c503_tx(struct sk_buff *skb, struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)(netdev->priv);
	struct net_device_stats *stats = &dev->stats;
	int ret = 0;
	int wlen;
	int submit_len;
	struct at76c503_tx_buffer *tx_buffer =
		(struct at76c503_tx_buffer *)dev->bulk_out_buffer;
	struct ieee80211_hdr_3addr *i802_11_hdr =
		(struct ieee80211_hdr_3addr *)&(tx_buffer->packet);
	u8 *payload = tx_buffer->packet + sizeof(struct ieee80211_hdr_3addr);

	if (netif_queue_stopped(netdev)) {
		err("%s: %s called while netdev is stopped", netdev->name,
		    __FUNCTION__);
		//skip this packet
		dev_kfree_skb(skb);
		return 0;
	}

	if (dev->write_urb->status == -EINPROGRESS) {
		err("%s: %s called while dev->write_urb is pending for tx",
		    netdev->name, __FUNCTION__);
		//skip this packet
		dev_kfree_skb(skb);
		return 0;
	}

	if (skb->len < 2*ETH_ALEN) {
		err("%s: %s: skb too short (%d)", dev->netdev->name,
		    __FUNCTION__, skb->len);
			dev_kfree_skb(skb);
			return 0;
	}

#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400 ())
		ipaq_blink_led ();
#endif

	/* we can get rid of memcpy, if we set netdev->hard_header_len
	   to 8 + sizeof(struct ieee80211_hdr_3addr), because then we have
	   enough space */
	//  dbg(DBG_TX, "skb->data - skb->head = %d", skb->data - skb->head);

	if (ntohs(*(__be16 *)(skb->data + 2*ETH_ALEN)) <= 1518) {
		/* this is a 802.3 packet */
		if (skb->data[2*ETH_ALEN+2] == rfc1042sig[0] &&
		    skb->data[2*ETH_ALEN+2+1] == rfc1042sig[1]) {
			/* higher layer delivered SNAP header - keep it */
			memcpy(payload, skb->data + 2*ETH_ALEN+2, skb->len - 2*ETH_ALEN -2);
			wlen = sizeof(struct ieee80211_hdr_3addr) + skb->len - 2*ETH_ALEN -2;
		} else {
			err("%s: %s: no support for non-SNAP 802.2 packets "
			    "(DSAP x%02x SSAP x%02x cntrl x%02x)",
			    dev->netdev->name, __FUNCTION__,
			    skb->data[2*ETH_ALEN+2], skb->data[2*ETH_ALEN+2+1],
			    skb->data[2*ETH_ALEN+2+2]);
			dev_kfree_skb(skb);
			return 0;
		}
	} else {
		/* add RFC 1042 header in front */
		memcpy(payload, rfc1042sig, sizeof(rfc1042sig));
		memcpy(payload + sizeof(rfc1042sig),
		       skb->data + 2*ETH_ALEN, skb->len - 2*ETH_ALEN);
		wlen = sizeof(struct ieee80211_hdr_3addr) + sizeof(rfc1042sig) + 
			skb->len - 2*ETH_ALEN;
	}

	/* make wireless header */
	i802_11_hdr->frame_ctl =
		cpu_to_le16(IEEE80211_FTYPE_DATA |
			    (dev->wep_enabled ? IEEE80211_FCTL_PROTECTED : 0) |
			    (dev->iw_mode == IW_MODE_INFRA ? IEEE80211_FCTL_TODS : 0));

	if(dev->iw_mode == IW_MODE_ADHOC){
		memcpy(i802_11_hdr->addr1, skb->data, ETH_ALEN); /* destination */
		memcpy(i802_11_hdr->addr2, skb->data+ETH_ALEN, ETH_ALEN); /* source */
		memcpy(i802_11_hdr->addr3, dev->bssid, ETH_ALEN);
	}else if(dev->iw_mode == IW_MODE_INFRA){
		memcpy(i802_11_hdr->addr1, dev->bssid, ETH_ALEN);
		memcpy(i802_11_hdr->addr2, skb->data+ETH_ALEN, ETH_ALEN); /* source */
		memcpy(i802_11_hdr->addr3, skb->data, ETH_ALEN); /* destination */
	}

	i802_11_hdr->duration_id = cpu_to_le16(0);
	i802_11_hdr->seq_ctl = cpu_to_le16(0);

	/* setup 'Atmel' header */
	tx_buffer->wlength = cpu_to_le16(wlen);
	tx_buffer->tx_rate = dev->txrate; 
        /* for broadcast destination addresses, the firmware 0.100.x 
	   seems to choose the highest rate set with CMD_STARTUP in
	   basic_rate_set replacing this value */
	
	memset(tx_buffer->reserved, 0, 4);

	tx_buffer->padding = calc_padding(wlen);
	submit_len = wlen + AT76C503_TX_HDRLEN + tx_buffer->padding;

	{
		dbg(DBG_TX_DATA_CONTENT, "%s skb->data %s", dev->netdev->name,
		    hex2str(dev->obuf, skb->data, 
			    min((int)(sizeof(dev->obuf)-1)/2,32),'\0'));
		dbg(DBG_TX_DATA, "%s tx  wlen x%x pad x%x rate %d hdr %s",
		    dev->netdev->name,
		    le16_to_cpu(tx_buffer->wlength),
		    tx_buffer->padding, tx_buffer->tx_rate, 
		    hex2str(dev->obuf, (u8 *)i802_11_hdr, 
			    min((sizeof(dev->obuf)-1)/2, 
				sizeof(struct ieee80211_hdr_3addr)),'\0'));
		dbg(DBG_TX_DATA_CONTENT, "%s payload %s", dev->netdev->name,
		    hex2str(dev->obuf, payload, 
			    min((int)(sizeof(dev->obuf)-1)/2,48),'\0'));
	}

	/* send stuff */
	netif_stop_queue(netdev);
	netdev->trans_start = jiffies;

	FILL_BULK_URB(dev->write_urb, dev->udev,
		      usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
		      tx_buffer, submit_len,
		      (usb_complete_t)at76c503_write_bulk_callback, dev);
	ret = usb_submit_urb(dev->write_urb, GFP_ATOMIC);
	if(ret){
		stats->tx_errors++;
		err("%s: error in tx submit urb: %d", netdev->name, ret);
		if (ret == -EINVAL)
			err("-EINVAL: urb %p urb->hcpriv %p urb->complete %p",
			    dev->write_urb,
			    dev->write_urb ? dev->write_urb->hcpriv : (void *)-1,
			    dev->write_urb ? dev->write_urb->complete : (void *)-1);
		goto err;
	}

	stats->tx_bytes += skb->len;

	dev_kfree_skb(skb);
	return 0;

 err:
	return ret;
}


static void at76c503_tx_timeout(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)(netdev->priv);

	if (!dev)
		return;
	warn("%s: tx timeout.", netdev->name);
#if WIRELESS_EXT < 17
	dev->write_urb->transfer_flags |= USB_ASYNC_UNLINK;
#endif
	usb_unlink_urb(dev->write_urb);
	dev->stats.tx_errors++;
}

static int startup_device(struct at76c503 *dev)
{
	struct at76c503_card_config *ccfg = &dev->card_config;
	int ret;

	if (at76_debug & DBG_PARAMS) {
		char ossid[IW_ESSID_MAX_SIZE+1];

		/* make dev->essid printable */
		assert(dev->essid_size <= IW_ESSID_MAX_SIZE);
		memcpy(ossid, dev->essid, dev->essid_size);
		ossid[dev->essid_size] = '\0';

		dbg_uc("%s param: ssid %s (%s) mode %s ch %d wep %s key %d keylen %d",
		       dev->netdev->name, ossid, 
		       hex2str(dev->obuf, dev->essid,
			       min((int)(sizeof(dev->obuf)-1)/2,
				   IW_ESSID_MAX_SIZE), '\0'),
		       dev->iw_mode == IW_MODE_ADHOC ? "adhoc" : "infra",
		       dev->channel,
		       dev->wep_enabled ? "enabled" : "disabled",
		       dev->wep_key_id, dev->wep_keys_len[dev->wep_key_id]);
		dbg_uc("%s param: preamble %s rts %d retry %d frag %d "
		       "txrate %s auth_mode %d",
		       dev->netdev->name,		       
		       dev->preamble_type == PREAMBLE_TYPE_SHORT ? "short" : "long",
		       dev->rts_threshold, dev->short_retry_limit, 
		       dev->frag_threshold,
		       dev->txrate == TX_RATE_1MBIT ? "1MBit" :
		       dev->txrate == TX_RATE_2MBIT ? "2MBit" :
		       dev->txrate == TX_RATE_5_5MBIT ? "5.5MBit" :
		       dev->txrate == TX_RATE_11MBIT ? "11MBit" :
		       dev->txrate == TX_RATE_AUTO ? "auto" : "<invalid>",
		       dev->auth_mode);
		dbg_uc("%s param: pm_mode %d pm_period %d auth_mode %s "
		       "scan_times %d %d scan_mode %s international_roaming %d",
		       dev->netdev->name,		       
		       dev->pm_mode, dev->pm_period_us,
		       dev->auth_mode == WLAN_AUTH_OPEN ?
		       "open" : "shared_secret",
		       dev->scan_min_time, dev->scan_max_time,
		       dev->scan_mode == SCAN_TYPE_ACTIVE ? "active" : "passive",
		       dev->international_roaming);
	}

	memset(ccfg, 0, sizeof(struct at76c503_card_config));
	ccfg->promiscuous_mode = 0;
	ccfg->short_retry_limit = dev->short_retry_limit;
	//ccfg->long_retry_limit = dev->long_retry_limit;

	if (dev->wep_enabled) {
	    if (dev->wep_keys_len[dev->wep_key_id] > WEP_SMALL_KEY_LEN)
			ccfg->encryption_type = 2;
		else
			ccfg->encryption_type = 1;

		/* jal: always exclude unencrypted if WEP is active */
		ccfg->exclude_unencrypted = 1;
	} else {
		ccfg->exclude_unencrypted = 0;
		ccfg->encryption_type = 0;
	}

	ccfg->rts_threshold = cpu_to_le16(dev->rts_threshold);
	ccfg->fragmentation_threshold = cpu_to_le16(dev->frag_threshold);

	memcpy(ccfg->basic_rate_set, hw_rates, 4);
	/* jal: really needed, we do a set_mib for autorate later ??? */
	ccfg->auto_rate_fallback = (dev->txrate == TX_RATE_AUTO ? 1 : 0);
	ccfg->channel = dev->channel;
	ccfg->privacy_invoked = dev->wep_enabled;
	memcpy(ccfg->current_ssid, dev->essid, IW_ESSID_MAX_SIZE);
	ccfg->ssid_len = dev->essid_size;

	ccfg->wep_default_key_id = dev->wep_key_id;
	memcpy(ccfg->wep_default_key_value, dev->wep_keys, 4 * WEP_KEY_LEN);

	ccfg->short_preamble = dev->preamble_type;
	ccfg->beacon_period = cpu_to_le16(dev->beacon_period);

	ret = set_card_command(dev->udev, CMD_STARTUP, (unsigned char *)&dev->card_config,
			       sizeof(struct at76c503_card_config));
	if(ret < 0){
		err("%s: set_card_command failed: %d", dev->netdev->name, ret);
		return ret;
	}

	wait_completion(dev, CMD_STARTUP);

	/* remove BSSID from previous run */
	memset(dev->bssid, 0, ETH_ALEN);
	
	if (set_radio(dev, 1) == 1)
		wait_completion(dev, CMD_RADIO);

	if ((ret=set_preamble(dev, dev->preamble_type)) < 0)
		return ret;
	if ((ret=set_frag(dev, dev->frag_threshold)) < 0)
		return ret;

	if ((ret=set_rts(dev, dev->rts_threshold)) < 0)
		return ret;
	
	if ((ret=set_autorate_fallback(dev, dev->txrate == TX_RATE_AUTO ? 1 : 0)) < 0)
		return ret;

	if ((ret=set_pm_mode(dev, dev->pm_mode)) < 0)
		return ret;
	
	if ((ret=set_iroaming(dev, dev->international_roaming)) < 0)
		return ret;

	if (at76_debug & DBG_MIB)
	{
		dump_mib_mac(dev);
		dump_mib_mac_addr(dev);
		dump_mib_mac_mgmt(dev);
		dump_mib_mac_wep(dev);
		dump_mib_mdomain(dev);
		dump_mib_phy(dev);
		dump_mib_local(dev);
	}

	return 0;
}

static int at76c503_open(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)(netdev->priv);
	int ret = 0;

	dbg(DBG_PROC_ENTRY, "at76c503_open entry");

	if(down_interruptible(&dev->sem))
	   return -EINTR;

	/* if netdev->dev_addr != dev->mac_addr we must
	   set the mac address in the device ! */
	if (memcmp(netdev->dev_addr, dev->mac_addr, ETH_ALEN)) {
		if (set_mac_address(dev,netdev->dev_addr) >= 0)
			dbg(DBG_PROGRESS, "%s: set new MAC addr %s",
			    netdev->name, mac2str(netdev->dev_addr));
	}

#ifdef DEBUG //test only !!!
	dump_mib_mac_addr(dev);
#endif

	dev->site_survey_state=SITE_SURVEY_IDLE;
	dev->last_survey = jiffies;
	dev->nr_submit_rx_tries = NR_SUBMIT_RX_TRIES; /* init counter */

	if((ret=submit_rx_urb(dev)) < 0){
		err("%s: open: submit_rx_urb failed: %d", netdev->name, ret);
		goto err;
	}

	dev->open_count++;

	defer_kevent(dev, KEVENT_RESTART);

	dbg(DBG_PROC_ENTRY, "at76c503_open end");
 err:
	up(&dev->sem);
	return ret < 0 ? ret : 0;
}

static int at76c503_stop(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)(netdev->priv);
	unsigned long flags;

	dbg(DBG_DEVSTART, "%s: ENTER", __FUNCTION__);

	if (down_interruptible(&dev->sem))
		return -EINTR;

	netif_stop_queue(netdev);

  	NEW_STATE(dev,INIT);

	if (!(dev->device_unplugged)) {
		/* we are called by "ifconfig wlanX down", not because the
		   device isn't avail. anymore */	
		set_radio(dev, 0);

		/* we unlink the read urb, because the _open()
		   submits it again. _delete_device() takes care of the
		   read_urb otherwise. */
		usb_kill_urb(dev->read_urb);
	}

	del_timer_sync(&dev->mgmt_timer);

	spin_lock_irqsave(&dev->mgmt_spinlock,flags);
	if (dev->next_mgmt_bulk) {
		kfree(dev->next_mgmt_bulk);
		dev->next_mgmt_bulk = NULL;
	}
	spin_unlock_irqrestore(&dev->mgmt_spinlock,flags);

	/* free the bss_list */
	free_bss_list(dev);

	assert(dev->open_count > 0);
	dev->open_count--;

	up(&dev->sem);

	dbg(DBG_DEVSTART, "%s: EXIT", __FUNCTION__);

	return 0;
}

static struct net_device_stats *at76c503_get_stats(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)netdev->priv;

	return &dev->stats;
}

static struct iw_statistics *at76c503_get_wireless_stats(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)netdev->priv;

	dbg(DBG_IOCTL, "RETURN qual %d level %d noise %d updated %d",
	    dev->wstats.qual.qual, dev->wstats.qual.level,
	    dev->wstats.qual.noise, dev->wstats.qual.updated);
 
	return &dev->wstats;
}

static void at76c503_set_multicast(struct net_device *netdev)
{
	struct at76c503 *dev = (struct at76c503 *)netdev->priv;
	int promisc;

	promisc = ((netdev->flags & IFF_PROMISC) != 0);
	if(promisc != dev->promisc){
		/* grmbl. This gets called in interrupt. */
		dev->promisc = promisc;
		defer_kevent(dev, KEVENT_SET_PROMISC);
	}
}

/* we only store the new mac address in netdev struct,
   it got set when the netdev gets opened. */
static int at76c503_set_mac_address(struct net_device *netdev, void *addr)
{
	struct sockaddr *mac = addr;
	memcpy(netdev->dev_addr, mac->sa_data, ETH_ALEN);
	return 1;
}

/* == PROC iwspy_update == 
  check if we spy on the sender address of buf and update statistics */
static void iwspy_update(struct at76c503 *dev, struct at76c503_rx_buffer *buf)
{
	struct ieee80211_hdr_3addr *hdr = (struct ieee80211_hdr_3addr *)buf->packet;
	struct iw_quality qual;

	/* We can only set the level here */
	qual.updated = IW_QUAL_QUAL_INVALID | IW_QUAL_NOISE_INVALID;
        qual.level = 0;
        qual.noise = 0;
	calc_level(dev, buf, &qual);

	spin_lock_bh(&(dev->spy_spinlock));
	
	if (dev->spy_data.spy_number > 0) {
		wireless_spy_update(dev->netdev, hdr->addr2, &qual);
	}
	spin_unlock_bh(&(dev->spy_spinlock));
} /* iwspy_update */


/*******************************************************************************
 * structure that describes the private ioctls/iw handlers of this driver
 */
static const struct iw_priv_args at76c503_priv_args[] = {
	{ PRIV_IOCTL_SET_SHORT_PREAMBLE,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0,
	  "short_preamble" }, // 0 - long, 1 -short
	
	{ PRIV_IOCTL_SET_DEBUG, 
	  // we must pass the new debug mask as a string,
	  // 'cause iwpriv cannot parse hex numbers
	  // starting with 0x :-( 
	  IW_PRIV_TYPE_CHAR | 10, 0,
	  "set_debug"}, // set debug value
	
	{ PRIV_IOCTL_SET_POWERSAVE_MODE, 
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0,
	  "powersave_mode"}, // 1 -  active, 2 - power save,
			     // 3 - smart power save
	{ PRIV_IOCTL_SET_SCAN_TIMES, 
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0,
	  "scan_times"}, // min_channel_time,
			 // max_channel_time
	{ PRIV_IOCTL_SET_SCAN_MODE, 
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0,
	  "scan_mode"}, // 0 - active, 1 - passive scan

	{ PRIV_IOCTL_SET_INTL_ROAMING,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0,
	  "intl_roaming"},

	/* needed for Kismet, orinoco mode */
	{ PRIV_IOCTL_SET_MONITOR_MODE,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0,
	  "monitor"}, /* param1: monitor mode: 0 (off), 1 (on,Prism header), 
			                       2 (on, no Prism header)
			 param2: channel (to start scan at) */
};


/*******************************************************************************
 * at76c503 implementations of iw_handler functions:
 */
static int at76c503_iw_handler_commit(struct net_device *netdev,
			       struct iw_request_info *info,
			       void *null,
			       char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	unsigned long flags;
	
	dbg(DBG_IOCTL, "%s %s: restarting the device", netdev->name,
		__FUNCTION__);
	
	// stop any pending tx bulk urb
	//TODO
	
	// jal: TODO: protect access to dev->istate by a spinlock
	// (ISR's on other processors may read/write it)
	LOCK_ISTATE()
	if (dev->istate != INIT) {
		UNLOCK_ISTATE()
		NEW_STATE(dev,INIT);
		// stop pending management stuff
		del_timer_sync(&dev->mgmt_timer);

		spin_lock_irqsave(&dev->mgmt_spinlock,flags);
		if (dev->next_mgmt_bulk) {
			kfree(dev->next_mgmt_bulk);
			dev->next_mgmt_bulk = NULL;
		}
		spin_unlock_irqrestore(&dev->mgmt_spinlock,flags);

		netif_carrier_off(dev->netdev);
		netif_stop_queue(dev->netdev);
	} else UNLOCK_ISTATE()

	// do the restart after two seconds to catch
	// following ioctl's (from more params of iwconfig)
	// in _one_ restart
	mod_timer(&dev->restart_timer, jiffies+2*HZ);
	
	return 0;
}

static int at76c503_iw_handler_get_name(struct net_device *netdev,
				 struct iw_request_info *info,
				 char *name,
				 char *extra)
{
	strcpy(name, "IEEE 802.11b");
	
	dbg(DBG_IOCTL, "%s: SIOCGIWNAME - name %s", netdev->name, name);
	
	return 0;
}

static int at76c503_iw_handler_set_freq(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_freq *freq,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int chan = -1;
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWFREQ - freq.m %d freq.e %d", netdev->name, 
		freq->m, freq->e);
	
	// modelled on orinoco.c
	if ((freq->e == 0) && (freq->m <= 1000))
	{
		// Setting by channel number
		chan = freq->m;
	}
	else
	{
		// Setting by frequency - search the table
		int mult = 1;
		int i;
		
		for (i = 0; i < (6 - freq->e); i++) {
			mult *= 10;
		}
		
		for (i = 0; i < NUM_CHANNELS; i++) {
			if (freq->m == (channel_frequency[i] * mult))
				chan = i + 1;
		}
	}
	
	if (chan < 1 || !dev->domain ) {
		// non-positive channels are invalid
		// we need a domain info to set the channel
		// either that or an invalid frequency was 
		// provided by the user
		ret =  -EINVAL;
	} else if (!dev->international_roaming) {
		if (!(dev->domain->channel_map & (1 << (chan-1)))) {
			info("%s: channel %d not allowed for domain %s "
			     "(and international_roaming is OFF)",
			     dev->netdev->name, chan, dev->domain->name);
			ret = -EINVAL;
		}
	}
	
	if (ret == -EIWCOMMIT) {
		dev->channel = chan;
		dbg(DBG_IOCTL, "%s: SIOCSIWFREQ - ch %d", netdev->name, chan);
	}
	
	return ret;
}

static int at76c503_iw_handler_get_freq(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_freq *freq,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	freq->m = dev->channel;
	freq->e = 0;
	
	if (dev->channel)
	{
		dbg(DBG_IOCTL, "%s: SIOCGIWFREQ - freq %ld x 10e%d", netdev->name, 
			channel_frequency[dev->channel - 1], 6);
	}
	dbg(DBG_IOCTL, "%s: SIOCGIWFREQ - ch %d", netdev->name, dev->channel);
	
	return 0;
}

static int at76c503_iw_handler_set_mode(struct net_device *netdev,
				 struct iw_request_info *info,
				 __u32 *mode,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWMODE - %d", netdev->name, *mode);
	
	if ((*mode != IW_MODE_ADHOC) && (*mode != IW_MODE_INFRA) &&
	    (*mode != IW_MODE_MONITOR)) {
		ret = -EINVAL;
	} else {
		dev->iw_mode = *mode;
	}
	return ret;
}

static int at76c503_iw_handler_get_mode(struct net_device *netdev,
				 struct iw_request_info *info,
				 __u32 *mode,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	*mode = dev->iw_mode;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWMODE - %d", netdev->name, *mode);
	
	return 0;
}

static int at76c503_iw_handler_get_range(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_point *data,
				  char *extra)
{
	// inspired by atmel.c
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	struct iw_range *range = (struct iw_range*)extra;
	int i;
	
	data->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(struct iw_range));
	
	
	//TODO: range->throughput = xxxxxx;
	
	range->min_nwid = 0x0000;
	range->max_nwid = 0x0000;
	
	// this driver doesn't maintain sensitivity information
	range->sensitivity = 0;
	
	range->max_qual.qual = 100;
	range->max_qual.level = 100;
	range->max_qual.noise = 0;
	range->max_qual.updated = IW_QUAL_NOISE_INVALID;
	
	range->avg_qual.qual = 50;
	range->avg_qual.level = 50;
	range->avg_qual.noise = 0;
	range->avg_qual.updated = IW_QUAL_NOISE_INVALID;
	
	range->bitrate[0] = 1000000;
	range->bitrate[1] = 2000000;
	range->bitrate[2] = 5500000;
	range->bitrate[3] = 11000000;
	range->num_bitrates = 4;
	
	
	range->min_rts = 0;
	range->max_rts = MAX_RTS_THRESHOLD;
	
	range->min_frag = MIN_FRAG_THRESHOLD;
	range->max_frag = MAX_FRAG_THRESHOLD;
	
	
	//range->min_pmp = 0;
	//range->max_pmp = 5000000;
	//range->min_pmt = 0;
	//range->max_pnt = 0;
	//range->pmp_flags = IW_POWER_PERIOD;
	//range->pmt_flags = 0;
	//range->pm_capa = IW_POWER_PERIOD;
	//TODO: find out what values we can use to describe PM capabilities
	range->pmp_flags = IW_POWER_ON;
	range->pmt_flags = IW_POWER_ON;
	range->pm_capa = 0;
	
	
	range->encoding_size[0] = WEP_SMALL_KEY_LEN;
	range->encoding_size[1] = WEP_LARGE_KEY_LEN;
	range->num_encoding_sizes = 2;
	range->max_encoding_tokens = WEP_KEYS;
	//TODO: do we need this?  what is a valid value if we don't support?
	//range->encoding_login_index = -1;
	
	/* both WL-240U and Linksys WUSB11 v2.6 specify 15 dBm as output power
	   - take this for all (ignore antenna gains) */
	range->txpower[0] = 15;
	range->num_txpower = 1;
	range->txpower_capa = IW_TXPOW_DBM;

	// at time of writing (22/Feb/2004), version we intend to support 
	// is v16, 
	range->we_version_source = WIRELESS_EXT_SUPPORTED;
	range->we_version_compiled = WIRELESS_EXT;
	
	// same as the values used in atmel.c
	range->retry_capa = IW_RETRY_LIMIT ;
	range->retry_flags = IW_RETRY_LIMIT;
	range->r_time_flags = 0;
	range->min_retry = 1;
	range->max_retry = 255;
	
	
	range->num_channels = NUM_CHANNELS;
	range->num_frequency = 0;
	
	for (i = 0; 
		i < 32; //number of bits in reg_domain.channel_map 
		i++)
	{
		// test if channel map bit is raised
		if (dev->domain->channel_map & (0x1 << i))
		{
			range->num_frequency += 1;
			
			range->freq[i].i = i + 1;
			range->freq[i].m = channel_frequency[i] * 100000;
			range->freq[i].e = 1; // channel frequency*100000 * 10^1
		}
	}
	
	dbg(DBG_IOCTL, "%s: SIOCGIWRANGE", netdev->name);
	
	return 0;
}

static int at76c503_iw_handler_set_spy(struct net_device *netdev,
				struct iw_request_info *info,
				struct iw_point *data,
				char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = 0;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWSPY - number of addresses %d",
		netdev->name, data->length);
	
	spin_lock_bh(&(dev->spy_spinlock));
	ret = iw_handler_set_spy(dev->netdev, info, (union iwreq_data *)data, 
		extra);
	spin_unlock_bh(&(dev->spy_spinlock));
	
	return ret;
}

static int at76c503_iw_handler_get_spy(struct net_device *netdev,
				struct iw_request_info *info,
				struct iw_point *data,
				char *extra)
{

	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = 0;
	
	spin_lock_bh(&(dev->spy_spinlock));
	ret = iw_handler_get_spy(dev->netdev, info, 
		(union iwreq_data *)data, extra);
	spin_unlock_bh(&(dev->spy_spinlock));
	
	dbg(DBG_IOCTL, "%s: SIOCGIWSPY - number of addresses %d",
		netdev->name, data->length);
	
	return ret;
}

static int at76c503_iw_handler_set_thrspy(struct net_device *netdev,
				   struct iw_request_info *info,
				   struct iw_point *data,
				   char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWTHRSPY - number of addresses %d)",
		netdev->name, data->length);
	
	spin_lock_bh(&(dev->spy_spinlock));
	ret = iw_handler_set_thrspy(netdev, info, (union iwreq_data *)data, 
		extra);
	spin_unlock_bh(&(dev->spy_spinlock));
	
	return ret;
}

static int at76c503_iw_handler_get_thrspy(struct net_device *netdev,
				   struct iw_request_info *info,
				   struct iw_point *data,
				   char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret;
	
	spin_lock_bh(&(dev->spy_spinlock));
	ret = iw_handler_get_thrspy(netdev, info, (union iwreq_data *)data, 
		extra);
	spin_unlock_bh(&(dev->spy_spinlock));
	
	dbg(DBG_IOCTL, "%s: SIOCGIWTHRSPY - number of addresses %d)",
		netdev->name, data->length);
	
	return ret;
}

static int at76c503_iw_handler_set_wap(struct net_device *netdev,
				struct iw_request_info *info,
				struct sockaddr *ap_addr,
				char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWAP - wap/bssid %s", netdev->name, 
		mac2str(ap_addr->sa_data));
	
	// if the incoming address == ff:ff:ff:ff:ff:ff, the user has 
	// chosen any or auto AP preference
	if (!memcmp(ap_addr->sa_data, bc_addr, ETH_ALEN)
		|| !memcmp(ap_addr->sa_data, off_addr, ETH_ALEN)) {
		dev->wanted_bssid_valid = 0;
	} else {
		// user wants to set a preferred AP address
		dev->wanted_bssid_valid = 1;
		memcpy(dev->wanted_bssid, ap_addr->sa_data, ETH_ALEN);
	}
	
	return -EIWCOMMIT;
}

static int at76c503_iw_handler_get_wap(struct net_device *netdev,
				struct iw_request_info *info,
				struct sockaddr *ap_addr,
				char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	ap_addr->sa_family = ARPHRD_ETHER;
	memcpy(ap_addr->sa_data, dev->bssid, ETH_ALEN);
	
	dbg(DBG_IOCTL, "%s: SIOCGIWAP - wap/bssid %s", netdev->name, 
		mac2str(ap_addr->sa_data));
	
	return 0;
}


static int at76c503_iw_handler_set_scan(struct net_device *netdev,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	unsigned long flags;
	int ret = 0;
#if WIRELESS_EXT > 19
	struct iw_scan_req *req = NULL;
#endif
	
	dbg(DBG_IOCTL, "%s: SIOCSIWSCAN", netdev->name);

	if (!netif_running(netdev))
		return -ENETDOWN;

	/* jal: we don't allow "iwlist wlanX scan" while we are
	   in monitor mode */
	if (dev->iw_mode == IW_MODE_MONITOR)
		return -EBUSY;

	/* Timeout old surveys. */
	if ((jiffies - dev->last_survey) > (20 * HZ))
		dev->site_survey_state = SITE_SURVEY_IDLE;
	dev->last_survey = jiffies;

	/* Initiate a scan command */
	if (dev->site_survey_state == SITE_SURVEY_IN_PROGRESS)
		return -EBUSY;

	dev->site_survey_state = SITE_SURVEY_IN_PROGRESS;

	// stop pending management stuff
	del_timer_sync(&(dev->mgmt_timer));
	
	spin_lock_irqsave(&(dev->mgmt_spinlock), flags);
	if (dev->next_mgmt_bulk) {
		kfree(dev->next_mgmt_bulk);
		dev->next_mgmt_bulk = NULL;
	}
	spin_unlock_irqrestore(&(dev->mgmt_spinlock), flags);
	
	if (netif_running(dev->netdev)) {
		// pause network activity
		netif_carrier_off(dev->netdev);
		netif_stop_queue(dev->netdev);
	}
// Try to do passive or active scan if WE asks as.
#if WIRELESS_EXT > 19
	if (wrqu->data.length
	    && wrqu->data.length == sizeof(struct iw_scan_req)) {
		req = (struct iw_scan_req *)extra;

		if (req->scan_type == IW_SCAN_TYPE_PASSIVE)
			dev->scan_mode = SCAN_TYPE_PASSIVE;
		else if (req->scan_type == IW_SCAN_TYPE_ACTIVE)
			dev->scan_mode = SCAN_TYPE_ACTIVE;

		/* Sanity check values? */
		LOCK_ISTATE()
		if (req->min_channel_time > 0) {
			if (dev->istate == MONITORING)
				dev->monitor_scan_min_time = req->min_channel_time;
			else
				dev->scan_min_time = req->min_channel_time;
		}
		if (req->max_channel_time > 0) {
			if (dev->istate == MONITORING)
				dev->monitor_scan_max_time = req->max_channel_time;
			else
				dev->scan_max_time = req->max_channel_time;
		}
		UNLOCK_ISTATE()
	} 
#endif
	
	// change to scanning state
	NEW_STATE(dev, SCANNING);
	defer_kevent(dev, KEVENT_SCAN);
	
	return ret;
}

static int at76c503_iw_handler_get_scan(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_point *data,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	unsigned long flags;
	struct list_head *lptr, *nptr;
	struct bss_info *curr_bss;
	struct iw_event *iwe = kmalloc(sizeof(struct iw_event), GFP_KERNEL);
	char *curr_val, *curr_pos = extra;
	int i;

	dbg(DBG_IOCTL, "%s: SIOCGIWSCAN", netdev->name);

	if (!iwe)
		return -ENOMEM;

	if (dev->site_survey_state != SITE_SURVEY_COMPLETED)
		/* scan not yet finished */
		return -EAGAIN;

	spin_lock_irqsave(&(dev->bss_list_spinlock), flags);
	
	list_for_each_safe(lptr, nptr, &(dev->bss_list)) {
		curr_bss = list_entry(lptr, struct bss_info, list);
		
		iwe->cmd = SIOCGIWAP;
		iwe->u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(iwe->u.ap_addr.sa_data, curr_bss->bssid, 6);
		curr_pos = iwe_stream_add_event(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, IW_EV_ADDR_LEN);
		
		iwe->u.data.length = curr_bss->ssid_len;
		iwe->cmd = SIOCGIWESSID;
		iwe->u.data.flags = 1;
		
		curr_pos = iwe_stream_add_point(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, curr_bss->ssid);
		
		iwe->cmd = SIOCGIWMODE;
		iwe->u.mode = (curr_bss->capa & WLAN_CAPABILITY_IBSS) ? 
			IW_MODE_ADHOC :
			(curr_bss->capa & WLAN_CAPABILITY_ESS) ? 
			IW_MODE_MASTER :
			IW_MODE_AUTO;
			// IW_MODE_AUTO = 0 which I thought is 
			// the most logical value to return in this case
		curr_pos = iwe_stream_add_event(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, IW_EV_UINT_LEN);
		
		iwe->cmd = SIOCGIWFREQ;
		iwe->u.freq.m = curr_bss->channel;
		iwe->u.freq.e = 0;
		curr_pos = iwe_stream_add_event(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, IW_EV_FREQ_LEN);
		
		iwe->cmd = SIOCGIWENCODE;
		if (curr_bss->capa & WLAN_CAPABILITY_PRIVACY) {
			iwe->u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		} else {
			iwe->u.data.flags = IW_ENCODE_DISABLED;
		}
		iwe->u.data.length = 0;
		curr_pos = iwe_stream_add_point(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, NULL);

		/* Add quality statistics */
		iwe->cmd = IWEVQUAL;
		iwe->u.qual.noise=0;
		iwe->u.qual.updated=IW_QUAL_NOISE_INVALID | IW_QUAL_LEVEL_UPDATED;
    		iwe->u.qual.level = (curr_bss->rssi * 100 / 42);
		if (iwe->u.qual.level > 100)
			iwe->u.qual.level = 100;
		if((dev->board_type == BOARDTYPE_503_INTERSIL_3861) ||
	   		(dev->board_type == BOARDTYPE_503_INTERSIL_3863)) {
	    		iwe->u.qual.qual=curr_bss->link_qual;
		} else {
	    		iwe->u.qual.qual=0;
		        iwe->u.qual.updated |= IW_QUAL_QUAL_INVALID;
		}
		/* Add new value to event */
		curr_pos = iwe_stream_add_event(curr_pos, 
			extra + IW_SCAN_MAX_DATA, iwe, IW_EV_QUAL_LEN);

		/* Rate : stuffing multiple values in a single event require a bit
		 * more of magic - Jean II */
		curr_val = curr_pos + IW_EV_LCP_LEN;

		iwe->cmd = SIOCGIWRATE;
		/* Those two flags are ignored... */
		iwe->u.bitrate.fixed = iwe->u.bitrate.disabled = 0;
		/* Max 8 values */
		for(i=0; i < curr_bss->rates_len; i++) {
			/* Bit rate given in 500 kb/s units (+ 0x80) */
			iwe->u.bitrate.value = 
				((curr_bss->rates[i] & 0x7f) * 500000);
			/* Add new value to event */
			curr_val = iwe_stream_add_value(curr_pos, curr_val, 
							extra + IW_SCAN_MAX_DATA,
							iwe, IW_EV_PARAM_LEN);
		}

		/* Check if we added any event */
		if ((curr_val - curr_pos) > IW_EV_LCP_LEN)
			curr_pos = curr_val;


		// more information may be sent back using IWECUSTOM

	}
	
	spin_unlock_irqrestore(&(dev->bss_list_spinlock), flags);
	
	data->length = (curr_pos - extra);
	data->flags = 0;
	
	kfree(iwe);
	return 0;
}


static int at76c503_iw_handler_set_essid(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_point *data,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWESSID - %s", netdev->name, extra);
	
	if (data->flags)
	{
		memcpy(dev->essid, extra, data->length);
		dev->essid_size = data->length;
#if WIRELESS_EXT < 21
		/* For historic reasons, the SSID length used to include one
		 * extra character, C string nul termination, even though SSID is
		 * really an octet string that should not be presented as a C
		 * string. WE-21 changes this to explicitly require the length
		 * _not_ to include nul termination, but for WE < 21, decrement
		 * the length count here to remove the nul termination. */
		dev->essid_size = max(dev->essid_size - 1, 0);
#endif
	}
	else
	{
		/* Use any SSID */
		dev->essid_size = 0;
	}
	
	return -EIWCOMMIT;
}

static int at76c503_iw_handler_get_essid(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_point *data,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	if (dev->essid_size) {
		// not the ANY ssid in dev->essid
		data->flags = 1;
		data->length = dev->essid_size;
		memcpy(extra, dev->essid, data->length);
		extra[data->length] = '\0';
		data->length += 1;
	} else {
		// the ANY ssid was specified
		LOCK_ISTATE()
		if (dev->istate == CONNECTED &&
		    dev->curr_bss != NULL) {
			UNLOCK_ISTATE()
			// report the SSID we have found
			data->flags = 1;
			data->length = dev->curr_bss->ssid_len;
			memcpy(extra, dev->curr_bss->ssid, data->length);
			extra[dev->curr_bss->ssid_len] = '\0';
			data->length += 1;
		} else {
			UNLOCK_ISTATE()
			// report ANY back
			data->flags=0;
			data->length=0;
		}
	}
	
	dbg(DBG_IOCTL, "%s: SIOCGIWESSID - %s", netdev->name, extra);
	
	return 0;
}

static int at76c503_iw_handler_set_nickname(struct net_device *netdev,
				     struct iw_request_info *info,
				     struct iw_point *data,
				     char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWNICKN - %s", netdev->name, extra);
	
	// iwconfig gives length including 0 byte like in the case of essid
	memcpy(dev->nickn, extra, data->length);
	
	return 0;
}

static int at76c503_iw_handler_get_nickname(struct net_device *netdev,
				     struct iw_request_info *info,
				     struct iw_point *data,
				     char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	data->length = strlen(dev->nickn);
	memcpy(extra, dev->nickn, data->length);
	extra[data->length] = '\0';
	data->length += 1;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWNICKN - %s", netdev->name, extra);
	
	return 0;
}

static int at76c503_iw_handler_set_rate(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_param *bitrate,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWRATE - %d", netdev->name, 
		bitrate->value);
	
	switch (bitrate->value)
	{
		case -1: 	dev->txrate = TX_RATE_AUTO; break; // auto rate 
		case 1000000: 	dev->txrate = TX_RATE_1MBIT; break;
		case 2000000: 	dev->txrate = TX_RATE_2MBIT; break;
		case 5500000: 	dev->txrate = TX_RATE_5_5MBIT; break;
		case 11000000: 	dev->txrate = TX_RATE_11MBIT; break;
		default:	ret = -EINVAL;
	}
	
	return ret;
}

static int at76c503_iw_handler_get_rate(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_param *bitrate,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = 0;
	
	switch (dev->txrate)
	{
		// return max rate if RATE_AUTO
		case TX_RATE_AUTO: 	bitrate->value = 11000000; break; 
		case TX_RATE_1MBIT: 	bitrate->value = 1000000; break;
		case TX_RATE_2MBIT: 	bitrate->value = 2000000; break;
		case TX_RATE_5_5MBIT: 	bitrate->value = 5500000; break;
		case TX_RATE_11MBIT: 	bitrate->value = 11000000; break;
		default:		ret = -EINVAL;
	}
	
	bitrate->fixed = (dev->txrate != TX_RATE_AUTO);
	bitrate->disabled = 0;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWRATE - %d", netdev->name, 
		bitrate->value);
	
	return ret;
}

static int at76c503_iw_handler_set_rts(struct net_device *netdev,
				struct iw_request_info *info,
				struct iw_param *rts,
				char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = -EIWCOMMIT;
	int rthr = rts->value;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWRTS - value %d disabled %s",
		netdev->name, rts->value, 
		(rts->disabled) ? "true" : "false");
	
	if (rts->disabled)
		rthr = MAX_RTS_THRESHOLD;
	
	if ((rthr < 0) || (rthr > MAX_RTS_THRESHOLD)) {
		ret = -EINVAL;
	} else {
		dev->rts_threshold = rthr;
	}
	
	return ret;
}

static int at76c503_iw_handler_get_rts(struct net_device *netdev,
				struct iw_request_info *info,
				struct iw_param *rts,
				char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	rts->value = dev->rts_threshold;
	rts->disabled = (rts->value >= MAX_RTS_THRESHOLD);
	rts->fixed = 1;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWRTS - value %d disabled %s",
		netdev->name, rts->value, 
		(rts->disabled) ? "true" : "false");
	
	return 0;
}

static int at76c503_iw_handler_set_frag(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_param *frag,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = -EIWCOMMIT;
	int fthr = frag->value;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWFRAG - value %d, disabled %s",
		netdev->name, frag->value, 
		(frag->disabled) ? "true" : "false");
	
	if(frag->disabled)
		fthr = MAX_FRAG_THRESHOLD;
	
	if ((fthr < MIN_FRAG_THRESHOLD) || (fthr > MAX_FRAG_THRESHOLD)) {
		ret = -EINVAL;
	} else {
		dev->frag_threshold = fthr & ~0x1; // get an even value
	}
	
	return ret;
}

static int at76c503_iw_handler_get_frag(struct net_device *netdev,
				 struct iw_request_info *info,
				 struct iw_param *frag,
				 char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	frag->value = dev->frag_threshold;
	frag->disabled = (frag->value >= MAX_FRAG_THRESHOLD);
	frag->fixed = 1;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWFRAG - value %d, disabled %s",
		netdev->name, frag->value, 
		(frag->disabled) ? "true" : "false");
	
	return 0;
}

static int at76c503_iw_handler_get_txpow(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_param *power,
				  char *extra)
{
	power->value = 15;
	power->fixed = 1;	/* No power control */
	power->disabled = 0;
	power->flags = IW_TXPOW_DBM;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWTXPOW - txpow %d dBm", netdev->name, 
		power->value);
	
	return 0;
}

/* jal: short retry is handled by the firmware (at least 0.90.x),
   while long retry is not (?) */
static int at76c503_iw_handler_set_retry(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_param *retry,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWRETRY disabled %d  flags x%x val %d",
	    netdev->name, retry->disabled, retry->flags, retry->value);
	
	if(!retry->disabled && (retry->flags & IW_RETRY_LIMIT)) {
		if ((retry->flags & IW_RETRY_MIN) || 
		    !(retry->flags & IW_RETRY_MAX)) {
			dev->short_retry_limit = retry->value;
		} else
			ret = -EINVAL;
	} else {
		ret = -EINVAL;
	}
	
	return ret;
}

// adapted (ripped) from atmel.c
static int at76c503_iw_handler_get_retry(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_param *retry,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWRETRY", netdev->name);
	
	retry->disabled = 0;      // Can't be disabled
	
	// Note : by default, display the min retry number
	//if((retry->flags & IW_RETRY_MAX)) {
	//	retry->flags = IW_RETRY_LIMIT | IW_RETRY_MAX;
	//	retry->value = dev->long_retry_limit;
	//} else {
		retry->flags = IW_RETRY_LIMIT;
		retry->value = dev->short_retry_limit;
		
		//if(dev->long_retry_limit != dev->short_retry_limit) {
		//	dev->retry.flags |= IW_RETRY_MIN;
		//}
	//}

	return 0;
}

static int at76c503_iw_handler_set_encode(struct net_device *netdev,
				   struct iw_request_info *info,
				   struct iw_point *encoding,
				   char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int index = (encoding->flags & IW_ENCODE_INDEX) - 1;
	int len = encoding->length;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWENCODE - enc.flags %08x "
		"pointer %p len %d", netdev->name, encoding->flags, 
		encoding->pointer, encoding->length);
	dbg(DBG_IOCTL, "%s: SIOCSIWENCODE - old wepstate: enabled %s key_id %d "
	       "auth_mode %s",
	       netdev->name, (dev->wep_enabled) ? "true" : "false", 
	       dev->wep_key_id, 
	       (dev->auth_mode == WLAN_AUTH_SHARED_KEY) ? 
			"restricted" : "open");
	
	// take the old default key if index is invalid
	if ((index < 0) || (index >= WEP_KEYS))
		index = dev->wep_key_id;
	
	if (len > 0)
	{
		if (len > WEP_LARGE_KEY_LEN)
			len = WEP_LARGE_KEY_LEN;
		
		memset(dev->wep_keys[index], 0, WEP_KEY_LEN);
		memcpy(dev->wep_keys[index], extra, len);
		dev->wep_keys_len[index] = (len <= WEP_SMALL_KEY_LEN) ?
			WEP_SMALL_KEY_LEN : WEP_LARGE_KEY_LEN;
		dev->wep_enabled = 1;
	}
	
	dev->wep_key_id = index;
	dev->wep_enabled = ((encoding->flags & IW_ENCODE_DISABLED) == 0);
	
	if (encoding->flags & IW_ENCODE_RESTRICTED)
		dev->auth_mode = WLAN_AUTH_SHARED_KEY;
	if (encoding->flags & IW_ENCODE_OPEN)
		dev->auth_mode = WLAN_AUTH_OPEN;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWENCODE - new wepstate: enabled %s key_id %d "
		"key_len %d auth_mode %s",
		netdev->name, (dev->wep_enabled) ? "true" : "false", 
		dev->wep_key_id + 1, dev->wep_keys_len[dev->wep_key_id],
		(dev->auth_mode == WLAN_AUTH_SHARED_KEY) ? 
			"restricted" : "open");
	
	return -EIWCOMMIT;
}

static int at76c503_iw_handler_get_encode(struct net_device *netdev,
				   struct iw_request_info *info,
				   struct iw_point *encoding,
				   char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int index = (encoding->flags & IW_ENCODE_INDEX) - 1;
	
	if ((index < 0) || (index >= WEP_KEYS))
		index = dev->wep_key_id;
	
	encoding->flags = 
		(dev->auth_mode == WLAN_AUTH_SHARED_KEY) ? 
		  IW_ENCODE_RESTRICTED : IW_ENCODE_OPEN;
	
	if (!dev->wep_enabled)
		encoding->flags |= IW_ENCODE_DISABLED;
	
	if (encoding->pointer)
	{
		encoding->length = dev->wep_keys_len[index];
		
		memcpy(extra, dev->wep_keys[index], dev->wep_keys_len[index]);
		
		encoding->flags |= (index + 1);
	}
	
	dbg(DBG_IOCTL, "%s: SIOCGIWENCODE - enc.flags %08x "
		"pointer %p len %d", netdev->name, encoding->flags, 
		encoding->pointer, encoding->length);
	dbg(DBG_IOCTL, "%s: SIOCGIWENCODE - wepstate: enabled %s key_id %d "
		"key_len %d auth_mode %s",
		netdev->name, (dev->wep_enabled) ? "true" : "false", 
		dev->wep_key_id + 1, dev->wep_keys_len[dev->wep_key_id],
		(dev->auth_mode == WLAN_AUTH_SHARED_KEY) ? 
			"restricted" : "open");
	
	return 0;
}

static int at76c503_iw_handler_set_power(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_param *power,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	dbg(DBG_IOCTL, "%s: SIOCSIWPOWER - disabled %s flags x%x value x%x", 
		netdev->name, (power->disabled) ? "true" : "false", 
		power->flags, power->value);
	
	if (power->disabled)
	{
		dev->pm_mode = PM_ACTIVE;
	}
	else
	{
		// we set the listen_interval based on the period given
		// no idea how to handle the timeout of iwconfig ???
		if (power->flags & IW_POWER_PERIOD)
		{
			dev->pm_period_us = power->value;
		}
		
		dev->pm_mode = PM_SAVE; // use iw_priv to select SMART_SAVE
	}
	
	return -EIWCOMMIT;
}

static int at76c503_iw_handler_get_power(struct net_device *netdev,
				  struct iw_request_info *info,
				  struct iw_param *power,
				  char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	
	power->disabled = dev->pm_mode == PM_ACTIVE;
	
	if ((power->flags & IW_POWER_TYPE) == IW_POWER_TIMEOUT)
	{
		power->flags = IW_POWER_TIMEOUT;
		power->value = 0;
	}
	else
	{
		unsigned long flags;
		u16 beacon_int; // of the current bss
		
		power->flags = IW_POWER_PERIOD;

		spin_lock_irqsave(&dev->bss_list_spinlock, flags);
		beacon_int = dev->curr_bss != NULL ?
			dev->curr_bss->beacon_interval : 0;
		spin_unlock_irqrestore(&dev->bss_list_spinlock, flags);
		
		if (beacon_int != 0)
		{
			power->value =
				(beacon_int * dev->pm_period_beacon) << 10;
		}
		else
		{
			power->value = dev->pm_period_us;
		}
	}
	
	power->flags |= IW_POWER_ALL_R;
	
	dbg(DBG_IOCTL, "%s: SIOCGIWPOWER - disabled %s flags x%x value x%x", 
		netdev->name, (power->disabled) ? "true" : "false", 
		power->flags, power->value);
	
	return 0;
}


/*******************************************************************************
 * Private IOCTLS
 */
static int at76c503_iw_handler_PRIV_IOCTL_SET_SHORT_PREAMBLE
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int val = *((int *)name);
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_SHORT_PREAMBLE, %d",
		netdev->name, val);
	
	if (val < 0 || val > 2) {
		//allow value of 2 - in the win98 driver it stands
		//for "auto preamble" ...?
		ret = -EINVAL;
	} else {
		dev->preamble_type = val;
	}
	
	return ret;
}

static int at76c503_iw_handler_PRIV_IOCTL_SET_DEBUG
	(struct net_device *netdev, struct iw_request_info *info, 
	 struct iw_point *data, char *extra)
{
	char *ptr;
	u32 val;
	
	if (data->length > 0) {
		val = simple_strtol(extra, &ptr, 0);
		
		if (ptr == extra) {
			val = DBG_DEFAULTS;
		}
		
		dbg_uc("%s: PRIV_IOCTL_SET_DEBUG input %d: %s -> x%x",
		       netdev->name, data->length, extra, val);
	} else {
		val = DBG_DEFAULTS;
	}
	
	dbg_uc("%s: PRIV_IOCTL_SET_DEBUG, old 0x%x  new 0x%x",
			netdev->name, at76_debug, val);
	
	/* jal: some more output to pin down lockups */
	dbg_uc("%s: netif running %d queue_stopped %d carrier_ok %d",
			netdev->name, 
			netif_running(netdev),
			netif_queue_stopped(netdev),
			netif_carrier_ok(netdev));
	
	at76_debug = val;
	
	return 0;
}

static int at76c503_iw_handler_PRIV_IOCTL_SET_POWERSAVE_MODE
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int val = *((int *)name);
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_POWERSAVE_MODE, %d (%s)",
		netdev->name, val,
		val == PM_ACTIVE ? "active" : val == PM_SAVE ? "save" :
		val == PM_SMART_SAVE ? "smart save" : "<invalid>");
	if (val < PM_ACTIVE || val > PM_SMART_SAVE) {
		ret = -EINVAL;
	} else {
		dev->pm_mode = val;
	}
	
	return ret;
}

static int at76c503_iw_handler_PRIV_IOCTL_SET_SCAN_TIMES
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int mint = *((int *)name);
	int maxt = *((int *)name + 1);
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_SCAN_TIMES - min %d max %d",
		netdev->name, mint, maxt);
	if (mint <= 0 || maxt <= 0 || mint > maxt) {
		ret = -EINVAL;
	} else {
		LOCK_ISTATE()
		if (dev->istate == MONITORING) {
			dev->monitor_scan_min_time = mint;
			dev->monitor_scan_max_time = maxt;
			ret = 0;
		} else {
			dev->scan_min_time = mint;
			dev->scan_max_time = maxt;
		}
		UNLOCK_ISTATE()
	}
	
	return ret;
}

static int at76c503_iw_handler_PRIV_IOCTL_SET_SCAN_MODE
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int val = *((int *)name);
	int ret = -EIWCOMMIT;
	
	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_SCAN_MODE - mode %s",
		netdev->name, (val = SCAN_TYPE_ACTIVE) ? "active" : 
		(val = SCAN_TYPE_PASSIVE) ? "passive" : "<invalid>");
	
	if (val != SCAN_TYPE_ACTIVE && val != SCAN_TYPE_PASSIVE) {
		ret = -EINVAL;
	} else {
		dev->scan_mode = val;
	}
	
	return ret;
}

static int set_iroaming(struct at76c503 *dev, int onoff)
{
	int ret = 0;

	memset(&dev->mib_buf, 0, sizeof(struct set_mib_buffer));
	dev->mib_buf.type = MIB_MAC_MGMT;
	dev->mib_buf.size = 1;
	dev->mib_buf.index = IROAMING_OFFSET;
	dev->mib_buf.data[0] = (dev->international_roaming ? 1 : 0);
	ret = set_mib(dev, &dev->mib_buf);
	if(ret < 0){
		err("%s: set_mib (intl_roaming_enable) failed: %d", dev->netdev->name, ret);
	}

	return ret;
}

static int at76c503_iw_handler_PRIV_IOCTL_SET_INTL_ROAMING
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int val = *((int *)name);
	int ret = -EIWCOMMIT;

	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_INTL_ROAMING - mode %s",
		netdev->name, (val == IR_OFF) ? "off" : 
		(val == IR_ON) ? "on" : "<invalid>");

	if (val != IR_OFF && val != IR_ON) {
		ret = -EINVAL;
	} else {
		if (dev->international_roaming != val) {
			dev->international_roaming = val;
			set_iroaming(dev, val);
		}
	}

	return ret;
}

/* == PROC set_monitor_mode == 
   sets dev->netdev->type */
static void set_monitor_mode(struct at76c503 *dev, int use_prism)
{
	if (dev->iw_mode == IW_MODE_MONITOR) {
		if (use_prism) {
			dbg(DBG_MONITOR_MODE, "%s: MONITOR MODE ON: "
			    "Prism headers ARE used", dev->netdev->name);
			dev->netdev->type = ARPHRD_IEEE80211_PRISM;
		} else {
			dbg(DBG_MONITOR_MODE, "%s: MONITOR MODE ON: "
			    "Prism headers NOT used", dev->netdev->name);
			dev->netdev->type = ARPHRD_IEEE80211;
		}
	} else {
		dbg(DBG_MONITOR_MODE, "%s: MONITOR MODE OFF", 
		    dev->netdev->name);
		dev->netdev->type = ARPHRD_ETHER;
	}
} /* set_monitor_mode */

static int at76c503_iw_handler_PRIV_IOCTL_SET_MONITOR_MODE
	(struct net_device *netdev, struct iw_request_info *info, 
	 char *name, char *extra)
{
	struct at76c503 *dev = (struct at76c503*)netdev->priv;
	int *params = ((int *)name);
	int mode = params[0];
	int channel = params[1];
	int ret = 0;

	dbg(DBG_IOCTL, "%s: PRIV_IOCTL_SET_MONITOR_MODE - mode %d ch %d", 
	    netdev->name, mode, channel);

	if (mode != MM_OFF && mode != MM_ON && mode != MM_ON_NO_PRISM)
		ret = -EINVAL;
	else {
		if (mode != MM_OFF) {
			if ((channel >= 1) && 
			    (channel <= (sizeof(channel_frequency) / 
				 sizeof(channel_frequency[0]))))
			// INFO: This doesn't actually affect the scan
			dev->channel = channel;

			dev->monitor_prism_header = (mode == MM_ON);

			if (dev->iw_mode != IW_MODE_MONITOR) {
				ret = -EIWCOMMIT;
				dev->iw_mode = IW_MODE_MONITOR;
			}
		} else {
			/* mode == MM_OFF */
			if (dev->iw_mode == IW_MODE_MONITOR) {
				ret = -EIWCOMMIT;
				dev->iw_mode = IW_MODE_INFRA;
			}
		}
		set_monitor_mode(dev, dev->monitor_prism_header);
	}

	return ret;
}

/*******************************************************************************
 * structure that advertises the iw handlers of this driver
 */
static const iw_handler	at76c503_handlers[] =
{
        [SIOCSIWCOMMIT-SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_commit,
        [SIOCGIWNAME  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_name,
        [SIOCSIWFREQ  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_freq,
        [SIOCGIWFREQ  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_freq,
        [SIOCSIWMODE  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_mode,
        [SIOCGIWMODE  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_mode,
        [SIOCGIWRANGE -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_range,
        [SIOCSIWSPY   -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_spy,
        [SIOCGIWSPY   -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_spy,
        [SIOCSIWTHRSPY-SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_thrspy,
        [SIOCGIWTHRSPY-SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_thrspy,
        [SIOCSIWAP    -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_wap,
        [SIOCGIWAP    -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_wap,
        [SIOCSIWSCAN  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_scan,
        [SIOCGIWSCAN  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_scan,
        [SIOCSIWESSID -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_essid,
        [SIOCGIWESSID -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_essid,
        [SIOCSIWNICKN -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_nickname,
        [SIOCGIWNICKN -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_nickname,
        [SIOCSIWRATE  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_rate,
        [SIOCGIWRATE  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_rate,
        [SIOCSIWRTS   -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_rts,
        [SIOCGIWRTS   -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_rts,
        [SIOCSIWFRAG  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_frag,
        [SIOCGIWFRAG  -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_frag,
        [SIOCGIWTXPOW -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_txpow,
        [SIOCGIWRETRY -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_retry,
        [SIOCGIWRETRY -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_retry,
        [SIOCSIWENCODE-SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_encode,
        [SIOCGIWENCODE-SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_encode,
        [SIOCSIWPOWER -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_set_power,
        [SIOCGIWPOWER -SIOCIWFIRST] = (iw_handler) at76c503_iw_handler_get_power,
};

/*******************************************************************************
 * structure that advertises the private iw handlers of this driver
 */
static const iw_handler	at76c503_priv_handlers[] =
{
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_SHORT_PREAMBLE,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_DEBUG,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_POWERSAVE_MODE,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_SCAN_TIMES,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_SCAN_MODE,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_INTL_ROAMING,
	(iw_handler) NULL,
	(iw_handler) at76c503_iw_handler_PRIV_IOCTL_SET_MONITOR_MODE,
	(iw_handler) NULL,
};

static const struct iw_handler_def at76c503_handler_def =
{
	.num_standard	= sizeof(at76c503_handlers)/sizeof(iw_handler),
	.num_private	= sizeof(at76c503_priv_handlers)/sizeof(iw_handler),
	.num_private_args = sizeof(at76c503_priv_args)/
				sizeof(struct iw_priv_args),
	.standard	= (iw_handler *) at76c503_handlers,
	.private	= (iw_handler *) at76c503_priv_handlers,
	.private_args	= (struct iw_priv_args *) at76c503_priv_args,
#if WIRELESS_EXT > 16
 	.get_wireless_stats = at76c503_get_wireless_stats,
#endif
#if WIRELESS_EXT == 15 || WIRELESS_EXT == 16
	.spy_offset	= offsetof(struct at76c503, spy_data),
#endif
};


static void at76c503_get_drvinfo(struct net_device *netdev,
				 struct ethtool_drvinfo *info)
{
	struct at76c503 *dev = (struct at76c503 *)netdev->priv;

	strncpy(info->driver, DRIVER_NAME, sizeof(info->driver) - 1);

	strncpy(info->version, DRIVER_VERSION, sizeof(info->version));
	info->version[sizeof(info->version)-1] = '\0';

	snprintf(info->bus_info, sizeof(info->bus_info) - 1, "usb%d:%d",
		 dev->udev->bus->busnum, dev->udev->devnum);

	snprintf(info->fw_version, sizeof(info->fw_version) - 1,
		 "%d.%d.%d-%d",
		 dev->fw_version.major, dev->fw_version.minor,
		 dev->fw_version.patch, dev->fw_version.build);
}

static struct ethtool_ops at76c503_ethtool_ops = {
	.get_drvinfo = at76c503_get_drvinfo,
};


static void at76c503_delete_device(struct at76c503 *dev)
{
	int i;

	if (!dev) 
		return;

	/* signal to _stop() that the device is gone */
	dev->device_unplugged = 1;

	dbg(DBG_PROC_ENTRY, "%s: ENTER",__FUNCTION__);

	if (dev->netdev_registered) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 17)
	    	if (down_trylock(&rtnl_sem) != 0) {
#else
		if (rtnl_trylock() == 0) {
#endif
			info("%s: rtnl_sem already down'ed", __FUNCTION__);
		} else {
			/* synchronously calls at76c503_stop() */
			unregister_netdevice(dev->netdev);
			rtnl_unlock();
		}
	}

	PUT_DEV(dev->udev);

	// assuming we used keventd, it must quiesce too
	flush_scheduled_work ();

	if(dev->bulk_out_buffer != NULL)
		kfree(dev->bulk_out_buffer);

	kfree(dev->ctrl_buffer);

	if(dev->write_urb != NULL) {
		usb_kill_urb(dev->write_urb);
		usb_free_urb(dev->write_urb);
	}
	if(dev->read_urb != NULL) {
		usb_kill_urb(dev->read_urb);
		usb_free_urb(dev->read_urb);
	}
	if(dev->ctrl_buffer != NULL) {
		usb_kill_urb(dev->ctrl_urb);
		usb_free_urb(dev->ctrl_urb);
	}

	dbg(DBG_PROC_ENTRY,"%s: unlinked urbs",__FUNCTION__);

	if(dev->rx_skb != NULL)
		kfree_skb(dev->rx_skb);

	free_bss_list(dev);
	del_timer_sync(&dev->bss_list_timer);

	LOCK_ISTATE()
	if (dev->istate == CONNECTED) {
		UNLOCK_ISTATE()
		iwevent_bss_disconnect(dev->netdev); 
	} else UNLOCK_ISTATE()

	for(i=0; i < NR_RX_DATA_BUF; i++)
		if (dev->rx_data[i].skb != NULL) {
			dev_kfree_skb(dev->rx_data[i].skb);
			dev->rx_data[i].skb = NULL;
		}
	dbg(DBG_PROC_ENTRY, "%s: before freeing dev/netdev", __FUNCTION__);
	free_netdev(dev->netdev); /* dev is in net_dev */ 
#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400()) {
		ipaq_led_off (RED_LED);
		ipaq_led_off (RED_LED_2);
	}
#endif
	dbg(DBG_PROC_ENTRY, "%s: EXIT", __FUNCTION__);
}

static int at76c503_alloc_urbs(struct at76c503 *dev)
{
	struct usb_interface *interface = dev->interface;
//	struct usb_host_interface *iface_desc = &interface->altsetting[0];
	struct usb_endpoint_descriptor *endpoint;
	struct usb_device *udev = dev->udev;
	int i, buffer_size;

	dbg(DBG_PROC_ENTRY, "%s: ENTER", __FUNCTION__);

	dbg(DBG_URB, "%s: NumEndpoints %d ", __FUNCTION__, NUM_EP(interface));

	for(i = 0; i < NUM_EP(interface); i++) {
		endpoint = &EP(interface,i);

		dbg(DBG_URB, "%s: %d. endpoint: addr x%x attr x%x",
		    __FUNCTION__,
		    i,
		    endpoint->bEndpointAddress,
		    endpoint->bmAttributes);

		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk in endpoint */

			dev->read_urb = usb_alloc_urb(0, GFP_KERNEL);
			if (!dev->read_urb) {
				err("No free urbs available");
				return -1;
			}
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
		}
		
		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk out endpoint */
			dev->write_urb = usb_alloc_urb(0, GFP_KERNEL);
			if (!dev->write_urb) {
				err("no free urbs available");
				return -1;
			}
			buffer_size = sizeof(struct at76c503_tx_buffer) + 
			  MAX_PADDING_SIZE;
			dev->bulk_out_size = buffer_size;
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
			if (!dev->bulk_out_buffer) {
				err("couldn't allocate bulk_out_buffer");
				return -1;
			}
			FILL_BULK_URB(dev->write_urb, udev, 
				      usb_sndbulkpipe(udev, 
						      endpoint->bEndpointAddress),
				      dev->bulk_out_buffer, buffer_size,
				      (usb_complete_t)at76c503_write_bulk_callback, dev);
		}
	}

	dev->ctrl_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->ctrl_urb) {
		err("no free urbs available");
		return -1;
	}
	dev->ctrl_buffer = kmalloc(1024, GFP_KERNEL);
	if (!dev->ctrl_buffer) {
		err("couldn't allocate ctrl_buffer");
		return -1;
	}

	dbg(DBG_PROC_ENTRY, "%s: EXIT", __FUNCTION__);

	return 0;
}

static struct at76c503 *alloc_new_device(struct usb_device *udev,
					 int board_type,
					 const char *netdev_name)
{
	struct net_device *netdev;
	struct at76c503 *dev = NULL;
	int i;

	/* allocate memory for our device state and initialize it */
	netdev = alloc_etherdev(sizeof(struct at76c503));
	if (netdev == NULL) {
		err("out of memory");
		return NULL;
	}

	dev = (struct at76c503 *)netdev->priv;
	memset(dev, 0, sizeof(*dev));

	dev->udev = udev;
	dev->netdev = netdev;

	init_MUTEX (&dev->sem);
	INIT_WORK (&dev->kevent, kevent, dev);

	dev->open_count = 0;

	init_timer(&dev->restart_timer);
	dev->restart_timer.data = (unsigned long)dev;
	dev->restart_timer.function = restart_timeout;

	init_timer(&dev->mgmt_timer);
	dev->mgmt_timer.data = (unsigned long)dev;
	dev->mgmt_timer.function = mgmt_timeout;

	init_timer(&dev->fw_dl_timer);
	dev->fw_dl_timer.data = (unsigned long)dev;
	dev->fw_dl_timer.function = fw_dl_timeout;


	spin_lock_init(&dev->mgmt_spinlock);
	spin_lock_init(&dev->istate_spinlock);
	dev->next_mgmt_bulk = NULL;
	dev->istate = INTFW_DOWNLOAD;

	/* initialize empty BSS list */
	dev->curr_bss = dev->new_bss = NULL;
	INIT_LIST_HEAD(&dev->bss_list);
	spin_lock_init(&dev->bss_list_spinlock);

	init_timer(&dev->bss_list_timer);
	dev->bss_list_timer.data = (unsigned long)dev;
	dev->bss_list_timer.function = bss_list_timeout;

	spin_lock_init(&dev->spy_spinlock);

	/* mark all rx data entries as unused */
	for(i=0; i < NR_RX_DATA_BUF; i++)
		dev->rx_data[i].skb = NULL;

	dev->tasklet.func = rx_tasklet;
	dev->tasklet.data = (unsigned long)dev;

	dev->board_type = board_type;

	dev->pm_mode = pm_mode;
	dev->pm_period_us = pm_period;

	dev_alloc_name(netdev, netdev_name);

	return dev;
} /* alloc_new_device */

/* == PROC init_new_device == 
   firmware got downloaded, we can continue with init */
/* We may have to move the register_netdev into alloc_new_device,
   because hotplug may try to configure the netdev _before_
   (or parallel to) the download of firmware */
static int init_new_device(struct at76c503 *dev)
{
	struct net_device *netdev = dev->netdev;
	int ret;

	/* set up the endpoint information */
	/* check out the endpoints */

	dev->interface = dev->udev->actconfig->interface[0];

	dbg(DBG_DEVSTART, "USB interface: %d endpoints",
	    NUM_EP(dev->interface));

	/* we let this timer run the whole time this driver instance lives */
	mod_timer(&dev->bss_list_timer, jiffies+BSS_LIST_TIMEOUT);

#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400 ())
		ipaq_init_led ();
#endif

	if(at76c503_alloc_urbs(dev) < 0)
		goto error;

	/* get firmware version */
	ret = get_mib(dev->udev, MIB_FW_VERSION, (u8*)&dev->fw_version, sizeof(dev->fw_version));
	if((ret < 0) || ((dev->fw_version.major == 0) && 
			 (dev->fw_version.minor == 0) && 
			 (dev->fw_version.patch == 0) && 
			 (dev->fw_version.build == 0))){
		err("getting firmware failed with %d, or version is 0", ret);
		err("this probably means that the ext. fw was not loaded correctly");
		goto error;
	}

	/* fw 0.84 doesn't send FCS with rx data */
	if (dev->fw_version.major == 0 && dev->fw_version.minor <= 84)
		dev->rx_data_fcs_len = 0;
	else
		dev->rx_data_fcs_len = 4;

	info("firmware version %d.%d.%d #%d (fcs_len %d)",
	     dev->fw_version.major, dev->fw_version.minor,
	     dev->fw_version.patch, dev->fw_version.build,
	     dev->rx_data_fcs_len);

	/* MAC address */
	ret = get_hw_config(dev);
	if(ret < 0){
		err("could not get MAC address");
		goto error;
	}

        dev->domain = getRegDomain(dev->regulatory_domain);
	/* init. netdev->dev_addr */
	memcpy(netdev->dev_addr, dev->mac_addr, ETH_ALEN);
	info("device's MAC %s, regulatory domain %s (id %d)",
	     mac2str(dev->mac_addr), dev->domain->name,
	     dev->regulatory_domain);

	/* initializing */
	dev->international_roaming = international_roaming;
	dev->channel = DEF_CHANNEL;
	dev->iw_mode = default_iw_mode;
	dev->monitor_prism_header = 1;
	memset(dev->essid, 0, IW_ESSID_MAX_SIZE);
	memset(dev->nickn, 0, sizeof(dev->nickn));
	dev->rts_threshold = DEF_RTS_THRESHOLD;
	dev->frag_threshold = DEF_FRAG_THRESHOLD;
	dev->short_retry_limit = DEF_SHORT_RETRY_LIMIT;
	//dev->long_retr_limit = DEF_LONG_RETRY_LIMIT;
	dev->txrate = TX_RATE_AUTO;
	dev->preamble_type = preamble_type;
	dev->beacon_period = 100;
	dev->beacons_last_qual=jiffies_to_msecs(jiffies);
	dev->auth_mode = auth_mode ? WLAN_AUTH_SHARED_KEY :
	  WLAN_AUTH_OPEN;
	dev->scan_min_time = scan_min_time;
	dev->scan_max_time = scan_max_time;
	dev->scan_mode = scan_mode;
	dev->monitor_scan_min_time = monitor_scan_min_time;
	dev->monitor_scan_max_time = monitor_scan_max_time;

	netdev->flags &= ~IFF_MULTICAST; /* not yet or never */
	netdev->open = at76c503_open;
	netdev->stop = at76c503_stop;
	netdev->get_stats = at76c503_get_stats;
	netdev->ethtool_ops = &at76c503_ethtool_ops;

#if WIRELESS_EXT > 16
	/* Add pointers to enable iwspy support. */
	dev->wireless_data.spy_data = &dev->spy_data;
	netdev->wireless_data = &dev->wireless_data;
#else  /* WIRELESS_EXT > 16 */
	netdev->get_wireless_stats = at76c503_get_wireless_stats;
#endif /* WIRELESS_EXT > 16 */

	netdev->hard_start_xmit = at76c503_tx;
	netdev->tx_timeout = at76c503_tx_timeout;
	netdev->watchdog_timeo = 2 * HZ;
	netdev->wireless_handlers = 
		(struct iw_handler_def*)&at76c503_handler_def;
	netdev->set_multicast_list = at76c503_set_multicast;
	netdev->set_mac_address = at76c503_set_mac_address;
	//  netdev->hard_header_len = 8 + sizeof(struct ieee80211_hdr_3addr);
	/*
//    netdev->hard_header = at76c503_header;
*/

	/* putting this inside rtnl_lock() - rtnl_unlock() hangs modprobe ...? */
	ret = register_netdev(dev->netdev);
	if (ret) {
		err("unable to register netdevice %s (status %d)!",
		    dev->netdev->name, ret);
		return -1;
	}
	info("registered %s", dev->netdev->name);
	dev->netdev_registered = 1;

	return 0;

 error:
	at76c503_delete_device(dev);
	return -1;

} /* init_new_device */


/* == PROC at76c503_get_fw_info ==
   disassembles the firmware image into version, str,
   internal and external fw part. returns 0 on success, < 0 on error */
static int at76c503_get_fw_info(u8 *fw_data, int fw_size,
				u32 *board, u32 *version, char **str,
				u8 **int_fw, int *int_fw_size,
				u8 **ext_fw, int *ext_fw_size)
{
/* fw structure (all numbers are little_endian)
   offset  length  description
   0       4       crc 32 (seed ~0, no post, all gaps are zeros, header included)
   4       4       board type (see at76c503.h)
   8       4       version (major<<24|middle<<16|minor<<8|build)
   c       4       offset of printable string (id) area from begin of image
                   (must be \0 terminated !)
  10       4       offset of internal fw part area
  14       4       length of internal fw part
  18       4       offset of external fw part area (may be first byte _behind_
                   image in case we have no external part)
  1c       4       length of external fw part
*/

	__le32 val;
	
	if (fw_size < 0x21) {
		err("fw too short (x%x)",fw_size);
		return -EFAULT;
	}

	/* crc currently not checked */

	memcpy(&val,fw_data+4,4);
	*board = le32_to_cpu(val);

	memcpy(&val,fw_data+8,4);
	*version = le32_to_cpu(val);

	memcpy(&val,fw_data+0xc,4);
	*str = fw_data + le32_to_cpu(val);

	memcpy(&val,fw_data+0x10,4);
	*int_fw = fw_data + le32_to_cpu(val);
	memcpy(&val,fw_data+0x14,4);
	*int_fw_size = le32_to_cpu(val);

	memcpy(&val,fw_data+0x18,4);
	*ext_fw = fw_data + le32_to_cpu(val);
	memcpy(&val,fw_data+0x1c,4);
	*ext_fw_size = le32_to_cpu(val);

	return 0;
}

/* == PROC at76c503_do_probe == */
static int at76c503_do_probe(struct module *mod, struct usb_device *udev,
			     u8 *fw_data, int fw_size, u32 board_type,
			     const char *netdev_name)
{
	struct usb_interface *intf = udev->actconfig->interface[0];
	int ret;
	struct at76c503 *dev = NULL;
	int op_mode;
	char *id_str;
	u32 version;

	GET_DEV(udev);

	if ((dev=alloc_new_device(udev, (u8)board_type, netdev_name)) == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	op_mode = get_op_mode(udev);

	usb_set_intfdata(intf, dev);
	dev->interface = intf;

	dbg(DBG_DEVSTART, "opmode %d", op_mode);

	/* we get OPMODE_NONE with 2.4.23, SMC2662W-AR ???
	   we get 204 with 2.4.23, Fiberline FL-WL240u (505A+RFMD2958) ??? */

	if (op_mode == OPMODE_HW_CONFIG_MODE) {
	  err("cannot handle a device in HW_CONFIG_MODE (opmode %d)", op_mode);
	  ret = -ENODEV;
	  goto error;
	}

	if (op_mode != OPMODE_NORMAL_NIC_WITH_FLASH &&
	    op_mode != OPMODE_NORMAL_NIC_WITHOUT_FLASH) {

		dbg(DBG_DEVSTART, "need to download firmware");

		/* disassem. the firmware */
		if ((ret=at76c503_get_fw_info(fw_data, fw_size, &dev->board_type,
					      &version, &id_str,
					      &dev->intfw, &dev->intfw_size,
					      &dev->extfw, &dev->extfw_size))) {
			goto error;
		}

		dbg(DBG_DEVSTART, "firmware board %u version %u.%u.%u#%u "
		    "(int %x:%tx, ext %x:%tx)",
		    dev->board_type, version>>24,(version>>16)&0xff,
		    (version>>8)&0xff, version&0xff,
		    dev->intfw_size, dev->intfw-fw_data,
		    dev->extfw_size, dev->extfw-fw_data);
		if (*id_str)
			dbg(DBG_DEVSTART, "firmware id %s",id_str);

		if (dev->board_type != board_type) {
			err("inconsistent board types %u != %u",
			    board_type, dev->board_type);
			at76c503_delete_device(dev);
			goto error;
		}

		/* download internal firmware part */
		dbg(DBG_DEVSTART, "downloading internal firmware");
		NEW_STATE(dev,INTFW_DOWNLOAD);
		defer_kevent(dev,KEVENT_INTERNAL_FW);

	} else {
		/* internal firmware already inside the device */
		/* get firmware version to test if external firmware is loaded */
		/* This works only for newer firmware, e.g. the Intersil 0.90.x
		   says "control timeout on ep0in" and subsequent get_op_mode() fail
		   too :-( */
		int force_fw_dwl = 0;

		/* disassem. the firmware */
		if ((ret=at76c503_get_fw_info(fw_data, fw_size, &dev->board_type,
					      &version, &id_str,
					      &dev->intfw, &dev->intfw_size,
					      &dev->extfw, &dev->extfw_size))) {
			goto error;
		}
		
		/* if version >= 0.100.x.y or device with built-in flash we can query the device
		 * for the fw version */
		if (version >= ((0<<24)|(100<<16)) || (op_mode == OPMODE_NORMAL_NIC_WITH_FLASH)) {
			ret = get_mib(udev, MIB_FW_VERSION, (u8*)&dev->fw_version, 
				      sizeof(dev->fw_version));
		} else {
			/* force fw download only if the device has no flash inside */
			force_fw_dwl = 1;
		}

		if ((force_fw_dwl) || (ret < 0) || ((dev->fw_version.major == 0) && 
						       (dev->fw_version.minor == 0) && 
						       (dev->fw_version.patch == 0) && 
						       (dev->fw_version.build == 0))) {
			if (force_fw_dwl)
				dbg(DBG_DEVSTART, "forced download of external firmware part");
			else
				dbg(DBG_DEVSTART, "cannot get firmware (ret %d) or all zeros "
				    "- download external firmware", ret);
			dbg(DBG_DEVSTART, "firmware board %u version %u.%u.%u#%u "
			    "(int %x:%tx, ext %x:%tx)",
			    dev->board_type, version>>24,(version>>16)&0xff,
			    (version>>8)&0xff, version&0xff,
			    dev->intfw_size, dev->intfw-fw_data,
			    dev->extfw_size, dev->extfw-fw_data);
			if (*id_str)
				dbg(DBG_DEVSTART, "firmware id %s",id_str);

			if (dev->board_type != board_type) {
				err("inconsistent board types %u != %u",
				    board_type, dev->board_type);
				at76c503_delete_device(dev);
				goto error;
			}

			NEW_STATE(dev,EXTFW_DOWNLOAD);
			defer_kevent(dev,KEVENT_EXTERNAL_FW);
		} else {
			NEW_STATE(dev,INIT);
			if (init_new_device(dev) < 0) {
				ret = -ENODEV;
				goto error;
			}
		}
	}

	SET_NETDEV_DEV(dev->netdev, &intf->dev);
	return 0;

error:
	PUT_DEV(udev);
	return ret;
}


/* Firmware names - this must be in sync with boardtype definitions */
static struct fwentry {
	const char *const fwname;
	const struct firmware *fw;
} firmwares[] = {
	{ "" },
	{ "atmel_at76c503-i3861.bin" },
	{ "atmel_at76c503-i3863.bin" },
	{ "atmel_at76c503-rfmd.bin" },
	{ "atmel_at76c503-rfmd-acc.bin" },
	{ "atmel_at76c505-rfmd.bin" },
	{ "atmel_at76c505-rfmd2958.bin" },
	{ "atmel_at76c505a-rfmd2958.bin" },
	{ "atmel_at76c505amx-rfmd.bin" }
};

/* USB Device IDs supported by this driver */

/* at76c503-i3861 */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_503I         0x7603 /* Generic AT76C503/3861 device */

#define VENDOR_ID_LINKSYS_OLD         0x066b
#define PRODUCT_ID_LINKSYS_WUSB11_V21 0x2211 /* Linksys WUSB11 v2.1/v2.6 */

#define VENDOR_ID_NETGEAR             0x0864
#define PRODUCT_ID_NETGEAR_MA101A     0x4100 /* Netgear MA 101 Rev. A */

#define VENDOR_ID_TEKRAM              0x0b3b
#define PRODUCT_ID_TEKRAM_U300C       0x1612 /* Tekram U-300C / Allnet ALL0193 */

#define VENDOR_ID_HP                  0x03f0
#define PRODUCT_ID_HP_HN210W          0x011c /* HP HN210W PKW-J7801A */

#define VENDOR_ID_M4Y750              0x0cde /* Unknown Vendor ID! */
#define PRODUCT_ID_M4Y750             0x0001 /* Sitecom/Z-Com/Zyxel M4Y-750 */

#define VENDOR_ID_DYNALINK            0x069a
#define PRODUCT_ID_DYNALINK_WLL013_I  0x0320 /* Dynalink/Askey WLL013 (intersil) */

#define VENDOR_ID_SMC_OLD             0x0d5c
#define PRODUCT_ID_SMC2662W_V1        0xa001 /* EZ connect 11Mpbs
Wireless USB Adapter SMC2662W (v1) */

#define VENDOR_ID_BENQ                0x04a5 /* BenQ (Acer) */
#define PRODUCT_ID_BENQ_AWL_300       0x9000 /* AWL-300 */

/* this adapter contains flash */
#define VENDOR_ID_ADDTRON             0x05dd  /* Addtron */
#define PRODUCT_ID_ADDTRON_AWU120     0xff31 /* AWU-120 */
/* also Compex WLU11 */

#define VENDOR_ID_INTEL               0x8086 /* Intel */
#define PRODUCT_ID_INTEL_AP310        0x0200 /* AP310 AnyPoint II USB */

#define VENDOR_ID_CONCEPTRONIC        0x0d8e
#define PRODUCT_ID_CONCEPTRONIC_C11U  0x7100 /* also Dynalink L11U */

#define VENDOR_ID_ARESCOM		0xd8e
#define PRODUCT_ID_WL_210		0x7110 /* Arescom WL-210, 
						  FCC id 07J-GL2411USB */
#define VENDOR_ID_IO_DATA		0x04bb
#define PRODUCT_ID_IO_DATA_WN_B11_USB   0x0919 /* IO-DATA WN-B11/USB */

#define VENDOR_ID_BT            0x069a
#define PRODUCT_ID_BT_VOYAGER_1010  0x0821 /* BT Voyager 1010 */


/* at76c503-i3863 */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_503_I3863    0x7604 /* Generic AT76C503/3863 device */

#define VENDOR_ID_SAMSUNG             0x055d
#define PRODUCT_ID_SAMSUNG_SWL2100U   0xa000 /* Samsung SWL-2100U */


/* at76c503-rfmd */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_503R         0x7605 /* Generic AT76C503/RFMD device */
#define PRODUCT_ID_W_BUDDIE_WN210     0x4102 /* AirVast W-Buddie WN210 */

#define VENDOR_ID_DYNALINK            0x069a
#define PRODUCT_ID_DYNALINK_WLL013_R  0x0321 /* Dynalink/Askey WLL013 (rfmd) */

#define VENDOR_ID_LINKSYS             0x077b
#define PRODUCT_ID_LINKSYS_WUSB11_V26 0x2219 /* Linksys WUSB11 v2.6 */
#define PRODUCT_ID_NE_NWU11B          0x2227 /* Network Everywhere NWU11B */

#define VENDOR_ID_NETGEAR             0x0864
#define PRODUCT_ID_NETGEAR_MA101B     0x4102 /* Netgear MA 101 Rev. B */

#define VENDOR_ID_ACTIONTEC           0x1668
#define PRODUCT_ID_ACTIONTEC_802UAT1  0x7605 /* Actiontec 802UAT1, HWU01150-01UK */

#define VENDOR_ID_DLINK               0x2001 /* D-Link */
#define PRODUCT_ID_DLINK_DWL120       0x3200 /* DWL-120 rev. E */

#define VENDOR_ID_DICK_SMITH_ELECTR   0x1371 /* Dick Smith Electronics */
#define PRODUCT_ID_DSE_XH1153         0x5743 /* XH1153 802.11b USB adapter */
                                             /* also: CNet CNUSB611 (D) */
#define PRODUCT_ID_WL_200U            0x0002 /* WL-200U */

#define VENDOR_ID_BENQ                0x04a5 /* BenQ (Acer) */
#define PRODUCT_ID_BENQ_AWL_400       0x9001 /* BenQ AWL-400 USB stick */

#define VENDOR_ID_3COM                0x506
#define PRODUCT_ID_3COM_3CRSHEW696    0xa01 /* 3COM 3CRSHEW696 */

#define VENDOR_ID_SIEMENS             0x681
#define PRODUCT_ID_SIEMENS_SANTIS_WLL013 0x1b /* Siemens Santis ADSL WLAN 
						 USB adapter WLL 013 */

#define VENDOR_ID_BELKIN_2		0x50d
#define PRODUCT_ID_BELKIN_F5D6050_V2	0x50	/* Belkin F5D6050, version 2 */

#define VENDOR_ID_BLITZ                 0x07b8  
#define PRODUCT_ID_BLITZ_NETWAVE_BWU613 0xb000 /* iBlitzz, BWU613 (not *B or *SB !) */

#define VENDOR_ID_GIGABYTE              0x1044  
#define PRODUCT_ID_GIGABYTE_GN_WLBM101  0x8003 /* Gigabyte GN-WLBM101 */

#define VENDOR_ID_PLANEX                0x2019
#define PRODUCT_ID_PLANEX_GW_US11S      0x3220 /* Planex GW-US11S */

#define VENDOR_ID_COMPAQ                0x049f
#define PRODUCT_ID_IPAQ_INT_WLAN        0x0032 /* internal WLAN adapter in h5[4,5]xx series iPAQs */


/* at76c503-rfmd-acc */
#define VENDOR_ID_BELKIN              0x0d5c
#define PRODUCT_ID_BELKIN_F5D6050     0xa002 /* Belkin F5D6050 / SMC 2662W v2 / SMC 2662W-AR */

#define VENDOR_ID_SMC                 0x083a
#define PRODUCT_ID_SMC_2664W          0x3501


/* at76c505-rfmd */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_505R         0x7606 /* Generic AT76C505/RFMD device */


/* at76c505-rfmd2958 */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_505R2958     0x7613 /* Generic AT76C505/RFMD device 
					       also OvisLink WL-1130USB */

#define VENDOR_ID_CNET                0x1371
#define PRODUCT_ID_CNET_CNUSB611G     0x0013 /* CNet CNUSB 611G */
#define PRODUCT_ID_FL_WL240U          0x0014 /* Fiberline WL-240U with the
                                                 CNet vendor id */

#define VENDOR_ID_LINKSYS_1915        0x1915 
#define PRODUCT_ID_LINKSYS_WUSB11V28  0x2233 /* Linksys WUSB11 v2.8 */

#define VENDOR_ID_XTERASYS            0x12fd
#define PRODUCT_ID_XTERASYS_XN_2122B  0x1001 /* Xterasys XN-2122B, also
					        IBlitzz BWU613B / BWU613SB */

#define VENDOR_ID_COREGA               0x07aa
#define PRODUCT_ID_COREGA_USB_STICK_11_KK 0x7613 /* Corega WLAN USB Stick 11 (K.K.) */

#define VENDOR_ID_MSI                 0x0db0
#define PRODUCT_ID_MSI_MS6978_WLAN_BOX_PC2PC 0x1020


/* at76c505a-rfmd2958 */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_505A         0x7614 /* Generic AT76C505A device */
#define PRODUCT_ID_ATMEL_505AS        0x7617 /* Generic AT76C505AS device */

#define VENDOR_ID_GIGASET             0x1690
#define PRODUCT_ID_GIGASET_11         0x0701


/* at76c505amx-rfmd */
#define VENDOR_ID_ATMEL               0x03eb
#define PRODUCT_ID_ATMEL_505AMX       0x7615 /* Generic AT76C505AMX device */


static struct usb_device_id dev_table[] = {
	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_503I        ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_LINKSYS_OLD, PRODUCT_ID_LINKSYS_WUSB11_V21),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_NETGEAR,  PRODUCT_ID_NETGEAR_MA101A    ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_TEKRAM,   PRODUCT_ID_TEKRAM_U300C      ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_HP,       PRODUCT_ID_HP_HN210W         ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_M4Y750,   PRODUCT_ID_M4Y750            ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_DYNALINK, PRODUCT_ID_DYNALINK_WLL013_I ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_SMC_OLD,  PRODUCT_ID_SMC2662W_V1       ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_BENQ,     PRODUCT_ID_BENQ_AWL_300      ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_ADDTRON,  PRODUCT_ID_ADDTRON_AWU120    ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_INTEL,    PRODUCT_ID_INTEL_AP310       ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_CONCEPTRONIC,PRODUCT_ID_CONCEPTRONIC_C11U),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_ARESCOM, PRODUCT_ID_WL_210),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_IO_DATA, PRODUCT_ID_IO_DATA_WN_B11_USB),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },
	{ USB_DEVICE(VENDOR_ID_BT,       PRODUCT_ID_BT_VOYAGER_1010   ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3861 },

	{ USB_DEVICE(VENDOR_ID_ATMEL,   PRODUCT_ID_ATMEL_503_I3863 ),
	  .driver_info = BOARDTYPE_503_INTERSIL_3863 },
	{ USB_DEVICE(VENDOR_ID_SAMSUNG, PRODUCT_ID_SAMSUNG_SWL2100U),
	  .driver_info = BOARDTYPE_503_INTERSIL_3863 },

	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_503R        ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_DYNALINK, PRODUCT_ID_DYNALINK_WLL013_R ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_LINKSYS,  PRODUCT_ID_LINKSYS_WUSB11_V26),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_LINKSYS,  PRODUCT_ID_NE_NWU11B         ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_NETGEAR,  PRODUCT_ID_NETGEAR_MA101B    ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_DLINK,    PRODUCT_ID_DLINK_DWL120      ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_ACTIONTEC,PRODUCT_ID_ACTIONTEC_802UAT1 ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_W_BUDDIE_WN210    ),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_DICK_SMITH_ELECTR, PRODUCT_ID_DSE_XH1153),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_DICK_SMITH_ELECTR, PRODUCT_ID_WL_200U),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_BENQ,     PRODUCT_ID_BENQ_AWL_400),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_3COM, PRODUCT_ID_3COM_3CRSHEW696),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_SIEMENS,  PRODUCT_ID_SIEMENS_SANTIS_WLL013),
	  .driver_info = BOARDTYPE_503_RFMD },
	{ USB_DEVICE(VENDOR_ID_BELKIN_2, PRODUCT_ID_BELKIN_F5D6050_V2 ),
	  .driver_info = BOARDTYPE_503_RFMD },
        { USB_DEVICE(VENDOR_ID_BLITZ,    PRODUCT_ID_BLITZ_NETWAVE_BWU613 ),
	  .driver_info = BOARDTYPE_503_RFMD },
        { USB_DEVICE(VENDOR_ID_GIGABYTE, PRODUCT_ID_GIGABYTE_GN_WLBM101 ),
	  .driver_info = BOARDTYPE_503_RFMD },
        { USB_DEVICE(VENDOR_ID_PLANEX,   PRODUCT_ID_PLANEX_GW_US11S ),
	  .driver_info = BOARDTYPE_503_RFMD },
        { USB_DEVICE(VENDOR_ID_COMPAQ,   PRODUCT_ID_IPAQ_INT_WLAN),
	  .driver_info = BOARDTYPE_503_RFMD },

	{ USB_DEVICE(VENDOR_ID_SMC, PRODUCT_ID_SMC_2664W),
	  .driver_info = BOARDTYPE_503_RFMD_ACC },
	{ USB_DEVICE(VENDOR_ID_BELKIN,   PRODUCT_ID_BELKIN_F5D6050    ),
	  .driver_info = BOARDTYPE_503_RFMD_ACC },

	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_505R       ),
	  .driver_info = BOARDTYPE_505_RFMD },

	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_505R2958   ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_CNET,     PRODUCT_ID_FL_WL240U         ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_CNET,     PRODUCT_ID_CNET_CNUSB611G    ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_LINKSYS_1915, PRODUCT_ID_LINKSYS_WUSB11V28 ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_XTERASYS, PRODUCT_ID_XTERASYS_XN_2122B ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_COREGA,   PRODUCT_ID_COREGA_USB_STICK_11_KK ),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },
        { USB_DEVICE(VENDOR_ID_MSI,      PRODUCT_ID_MSI_MS6978_WLAN_BOX_PC2PC),
	  .driver_info = BOARDTYPE_505_RFMD_2958 },

	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_505A       ),
	  .driver_info = BOARDTYPE_505A_RFMD_2958 },
	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_505AS      ),
	  .driver_info = BOARDTYPE_505A_RFMD_2958 },
        { USB_DEVICE(VENDOR_ID_GIGASET,  PRODUCT_ID_GIGASET_11       ),
	  .driver_info = BOARDTYPE_505A_RFMD_2958 },

	{ USB_DEVICE(VENDOR_ID_ATMEL,    PRODUCT_ID_ATMEL_505AMX    ),
	  .driver_info = BOARDTYPE_505AMX_RFMD },

	{ }
};

MODULE_DEVICE_TABLE (usb, dev_table);


static int at76c50x_probe(struct usb_interface *interface,
			  const struct usb_device_id *id)
{
	int retval;

	struct usb_device *udev;
	int boardtype = (int)id->driver_info;
	const char *const fw_name = firmwares[boardtype].fwname;
	const struct firmware *fw = firmwares[boardtype].fw;
	udev = interface_to_usbdev(interface);

	if (fw == NULL) {
		dbg(DBG_FW, "downloading firmware %s", fw_name);
		retval = request_firmware(&fw, fw_name, &udev->dev);
		if (retval == 0) {
			dbg(DBG_FW, "got it.");
		} else {
			err("firmware %s not found.", fw_name);
			err("You may need to download the firmware from "
			    "https://developer.berlios.de/projects/at76c503a/");
			return retval;
		}
	} else
		dbg(DBG_FW, "re-using previously loaded fw");

	retval = at76c503_do_probe(THIS_MODULE, udev,
				   fw->data, fw->size,
				   boardtype, netdev_name);
	return retval;
}

static void at76c50x_disconnect(struct usb_interface *interface)
{
	struct at76c503 *ptr;

	ptr = usb_get_intfdata (interface);
	usb_set_intfdata(interface, NULL);

	info("%s disconnecting", ((struct at76c503 *)ptr)->netdev->name);
	at76c503_delete_device(ptr);
	info(DRIVER_NAME " disconnected");
}

/* structure for registering this driver with the USB subsystem */
static struct usb_driver module_usb = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	.owner = THIS_MODULE,
#endif
	.name = DRIVER_NAME,
	.probe = at76c50x_probe,
	.disconnect = at76c50x_disconnect,
	.id_table = dev_table,
};


static int __init mod_init(void)
{
	int result;

	info(DRIVER_DESC " " DRIVER_VERSION " loading");

#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400()) {
		/* turn WLAN power on */
		/* both needed? */
		SET_H5400_ASIC_GPIO (GPB, RF_POWER_ON, 1);
		SET_H5400_ASIC_GPIO (GPB, WLAN_POWER_ON, 1);
	}
#endif

	/* register this driver with the USB subsystem */
	result = usb_register(&module_usb);
	if (result < 0) {
		err("usb_register failed (status %d)", result);
		return -1;
	}

	return 0;
}

static void __exit mod_exit(void)
{
	int i;

	info(DRIVER_DESC " " DRIVER_VERSION " unloading");
	usb_deregister(&module_usb);
	for (i = 0; i < ARRAY_SIZE(firmwares); i++) {
		if (firmwares[i].fw)
			release_firmware(firmwares[i].fw);
	}

#ifdef CONFIG_IPAQ_HANDHELD
	if (machine_is_h5400()) {
		/* turn WLAN power off */
		SET_H5400_ASIC_GPIO (GPB, RF_POWER_ON, 0);
		SET_H5400_ASIC_GPIO (GPB, WLAN_POWER_ON, 0);
	}
#endif
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
