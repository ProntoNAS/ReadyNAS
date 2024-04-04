/***************************************************************
 *
 * 	- 05-01-2008
 *	- Initial release by Bin Zhang
 *
 * 	- 06-03-2008
 * 	- Extend boot info with all the new field for NV6
 * 	- by Jason Qian
 *
 *	- 06-05-2008
 *	- Replace proc data_out with ioctl function
 *	- Only allow init (process 1) to write
 *	- Remove proc data_in
 *	- by Jason Qian
 *
***************************************************************/

#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "vpd.h"
#include <linux/dmi.h>
#include <linux/string.h>
#include <asm/processor.h>

#include "../../../libs/libreadynas/src/systypes.h"

MODULE_DESCRIPTION("ReadyNAS VPD Driver");
MODULE_AUTHOR("Bin Zhang");
MODULE_LICENSE("Proprietary");

int systype = UNKNOWN;
module_param(systype, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(systype, "ReadyNAS system type");

#define VPD_DEVICE_NAME		"vpd"
#define VPD_DEVICE_MAJOR	60
#define VPD_DEVICE_MINOR	0

// IOCTL CMD
#define IOCTL_VPD_CMD_FLAG	0x10
#define IOCTL_VPD_WRITE		1
#define IOCTL_BASIC_WRITE	3
#define IOCTL_LEGACY_COMPAT	5

#define DEV_BOOT	10
static char vpd_buffer[512];
static struct vpd_data_struct vpd_data;
static char boot_info_buf[2048];

static const char boot_info_format[] =
	"cause:\t%s\n"
	"vendor:\t%s\n"
	"model:\t%s\n"
	"model_valid:\t%u\n"
	"version:\t%u\n"
	"serial:\t%s\n"
	"loader:\t%s\n"
	"max_chan:\t%u\n"
	"xraid:\t%u\n"
	"mode:\t%u\n"
	"fginit:\t%u\n"
	"iscsi:\t%u\n"
	"stream:\t%u\n"
	"no-edition:\t%u\n"
	"ads:\t%u\n"
	"10gige:\t%u\n"
	"replication:\t%u\n"
	"teaming:\t%u\n"
	"vault_promo_1:\t%u\n"
	"vault_promo_2:\t%u\n"

	"pcbid:\t%u\n"
	"board_rev:\t%04x\n"
	"system_rev:\t%04x\n"
	"watchdog:\t%u\n"
	"jbod:\t%u\n"
	"auto:\t%u\n"

	"minrpm:\t%u\n"
	"minpwm:\t%u\n"

	"maxtemp0:\t%u\n"
	"mintemp0:\t%u\n"
	"maxtemp1:\t%u\n"
	"mintemp1:\t%u\n"
	"maxtemp2:\t%u\n"
	"mintemp2:\t%u\n"

	"cpu_target:\t%u\n"
	"cpu_tolerance:\t%u\n"

	"labelvol0:\t%s\n"
	"minvol0:\t%u.%02u\n"
	"maxvol0:\t%u.%02u\n"
	"labelvol1:\t%s\n"
	"minvol1:\t%u.%02u\n"
	"maxvol1:\t%u.%02u\n"
	"labelvol2:\t%s\n"
	"minvol2:\t%u.%02u\n"
	"maxvol2:\t%u.%02u\n"
	"labelvol3:\t%s\n"
	"minvol3:\t%u.%02u\n"
	"maxvol3:\t%u.%02u\n"
	"labelvol4:\t%s\n"
	"minvol4:\t%u.%02u\n"
	"maxvol4:\t%u.%02u\n"
	"labelvol5:\t%s\n"
	"minvol5:\t%u.%02u\n"
	"maxvol5:\t%u.%02u\n"
	"labelvol6:\t%s\n"
	"minvol6:\t%u.%02u\n"
	"maxvol6:\t%u.%02u\n"
	"labelvol7:\t%s\n"
	"minvol7:\t%u.%02u\n"
	"maxvol7:\t%u.%02u\n"
	"labelvol8:\t%s\n"
	"minvol8:\t%u.%02u\n"
	"maxvol8:\t%u.%02u\n"
	"labelvol9:\t%s\n"
	"minvol9:\t%u.%02u\n"
	"maxvol9:\t%u.%02u\n"

	"labelfan0:\t%s\n"
	"reqfan0:\t%u\n"
	"selffan0:\t%u\n"
	"altfan0:\t%u\n"
	"minfan0:\t%u\n"
	"typefan0:\t%u\n"

	"labelfan1:\t%s\n"
	"reqfan1:\t%u\n"
	"selffan1:\t%u\n"
	"altfan1:\t%u\n"
	"minfan1:\t%u\n"
	"typefan1:\t%u\n"

	"labelfan2:\t%s\n"
	"reqfan2:\t%u\n"
	"selffan2:\t%u\n"
	"altfan2:\t%u\n"
	"minfan2:\t%u\n"
	"typefan2:\t%u\n"

	"labelfan3:\t%s\n"
	"reqfan3:\t%u\n"
	"selffan3:\t%u\n"
	"altfan3:\t%u\n"
	"minfan3:\t%u\n"
	"typefan3:\t%u\n"

	"labelfan4:\t%s\n"
	"reqfan4:\t%u\n"
	"selffan4:\t%u\n"
	"altfan4:\t%u\n"
	"minfan4:\t%u\n"
	"typefan4:\t%u\n"

	"rcause:\t%x\n"
	"ssh:\t%lu";

#define V(m)	vpd_data.vpd_##m
#define V1(m)	vpd_data.vpd_hwm_vol_##m
#define V2(m)   vpd_data.vpd_hwm_fan_##m
#define V3(m)	vpd_data.vpd_fan_##m
#define V4(m)	vpd_data.vpd_ecc_##m
#define V5(m)	vpd_data.vpd_max##m
#define V6(m)	vpd_data.vpd_max_temp_##m
#define V7(m)	vpd_data.vpd_min_temp_##m
#define V8(m)	vpd_data.vpd_ledlum##m

#define VBYTE	vpd_buffer[pos++]
#define VPOINT	(vpd_buffer+pos)

static void boot_info_init(void)
{
#ifdef __arm__
	char model[32] = {'\0'};

	switch (systype) {
	case RN102:
		strcpy(model, "ReadyNAS 102");
		break;
	case RN104:
		strcpy(model, "ReadyNAS 104");
		break;
	case RN202:
		if (num_online_cpus() == 4)
			strcpy(model, "ReadyNAS 212");
		else
			strcpy(model, "ReadyNAS 202");
		break;
	case RN204:
		if (num_online_cpus() == 4)
			strcpy(model, "ReadyNAS 214");
		else
			strcpy(model, "ReadyNAS 204");
		break;
	case RN2120:
		strcpy(model, "ReadyNAS 2120");
		break;
	case RN25:
		strcpy(model, "ReadyNAS 25");
		break;
	default:
		strcpy(model, "UNKNOWN");
	}

	snprintf(boot_info_buf, sizeof(boot_info_buf),	"vendor:\tNETGEAR\n"
							"model:\t%s", model);
#else
	const char *p;
	const struct dmi_device *dmi;
	char model[32] = {'\0'};
	char serial[32] = {'\0'};
	char sku[12] = {'\0'};
	char board_rev[5] = {'\0'};
	char system_rev[5] = {'\0'};
	char reason[17] = "Normal";

	p = dmi_get_system_info(DMI_PRODUCT_NAME);
	if (p) {
		strncpy(model, p, sizeof(model)-1);
		model[31] = '\0';
	}
	
	p = dmi_get_system_info(DMI_PRODUCT_SERIAL);
	if (p) {
		strncpy(serial, p, sizeof(serial)-1);
		serial[31] = '\0';
		if (strncasecmp(serial, "To b", 4) == 0)
			serial[0] = '\0';
	}

	p = dmi_get_system_info(DMI_PRODUCT_SKU);
	if (p) {
		strncpy(sku, p, sizeof(sku)-1);
		sku[sizeof(sku)-1] = '\0';
		if (strncasecmp(sku, "To b", 4) == 0)
			sku[0] = '\0';
	}

	p = dmi_get_system_info(DMI_BOARD_VERSION);
	if (p) {
		strncpy(board_rev, p, sizeof(board_rev)-1);
		board_rev[4] = '\0';
		if (strncasecmp(board_rev, "To b", 4) == 0)
			board_rev[0] = '\0';
	}

	p = dmi_get_system_info(DMI_CHASSIS_VERSION);
	if (p) {
		strncpy(system_rev, p, sizeof(system_rev)-1);
		system_rev[4] = '\0';
		if (strncasecmp(system_rev, "To b", 4) == 0)
			system_rev[0] = '\0';
	}

	dmi = dmi_find_device(DMI_DEV_TYPE_OEM_STRING, NULL, NULL);
	if (dmi) {
		strncpy(reason, dmi->name, sizeof(reason)-1);
		reason[16] = '\0';
	}

	snprintf(boot_info_buf, sizeof(boot_info_buf),	"vendor:\t%s\n"
							"model:\t%s\n"
							"serial:\t%s\n"
							"sku:\t%s\n"
							"loader:\t%s\n"
							"board_rev:\t%s\n"
							"system_rev:\t%s\n"
							"reason:\t%s",
				dmi_get_system_info(DMI_SYS_VENDOR),
				strim(model),
				strim(serial),
				strim(sku),
				dmi_get_system_info(DMI_BIOS_VERSION),
				strim(board_rev),
				strim(system_rev),
				strim(reason)
		);
#endif
}

static void basic_boot_info_init(void)
{
	struct basic_vpd_data_struct *vpd;

	vpd = (void *)vpd_buffer;

	snprintf(boot_info_buf, sizeof(boot_info_buf),
			"vendor:\t%s\n"
			"model:\t%s\n"
			"serial:\t%s\n"
			"sku:\t%s\n"
			"uuid:\t%s",
			vpd->vendor,
			vpd->model,
			vpd->serial,
			vpd->sku,
			vpd->uuid
	);
}

static void vpd_boot_info_init(void)
{
	snprintf(boot_info_buf, sizeof(boot_info_buf), boot_info_format,
			"normal",
			V(vendor),
			V(model),
			(V(features) & (1<<6)) > 0 ? 1 : 0,
			V(ver),
			V(serial),
			V(version),
			V(sata_chn_max),
			(V(features) & (1<<0)) > 0 ? 1 : 0,
			(V(features) & (1<<1)) > 0 ? 1 : 0,
			(V(features) & (1<<2)) > 0 ? 1 : 0,
			(V(features) & (1<<27)) > 0 ? 1 : 0,
			(V(features) & (1<<28)) > 0 ? 0 : 1,
			(V(features1) & (1<<0)) > 0 ? 1 : 0,
			(V(features1) & (1<<1)) > 0 ? 1 : 0,
			(V(features1) & (1<<2)) > 0 ? 1 : 0,
			(V(features1) & (1<<3)) > 0 ? 1 : 0,
			(V(features1) & (1<<4)) > 0 ? 1 : 0,
			(V(features1) & (1<<5)) > 0 ? 1 : 0,
			(V(features1) & (1<<6)) > 0 ? 1 : 0,
			1,
			V(board_rev),
			V(system_rev),
			(V(features) & (1<<26)) > 0 ? 1 : 0,
			(V(features) & (1<<3)) > 0 ? 1 : 0,
			0,
			(V3(rpm_min)+1)*64,
			V3(pwm_min),
			V6(1),V7(1),V6(2),V7(2),V6(3),V7(3),
			V(cpu_target),V(cpu_tolerance),

			V1(label[0]), V1(min1[0]), V1(min0[0]), V1(max1[0]), V1(max0[0]),
			V1(label[1]), V1(min1[1]), V1(min0[1]), V1(max1[1]), V1(max0[1]),
			V1(label[2]), V1(min1[2]), V1(min0[2]), V1(max1[2]), V1(max0[2]),
			V1(label[3]), V1(min1[3]), V1(min0[3]), V1(max1[3]), V1(max0[3]),
			V1(label[4]), V1(min1[4]), V1(min0[4]), V1(max1[4]), V1(max0[4]),
			V1(label[5]), V1(min1[5]), V1(min0[5]), V1(max1[5]), V1(max0[5]),
			V1(label[6]), V1(min1[6]), V1(min0[6]), V1(max1[6]), V1(max0[6]),
			V1(label[7]), V1(min1[7]), V1(min0[7]), V1(max1[7]), V1(max0[7]),
			V1(label[8]), V1(min1[8]), V1(min0[8]), V1(max1[8]), V1(max0[8]),
			V1(label[9]), V1(min1[9]), V1(min0[9]), V1(max1[9]), V1(max0[9]),

			V2(label[0]), V2(req[0]), V2(self[0]), V2(alt[0]), V2(min[0]), V2(typ[0]),
			V2(label[1]), V2(req[1]), V2(self[1]), V2(alt[1]), V2(min[1]), V2(typ[1]),
			V2(label[2]), V2(req[2]), V2(self[2]), V2(alt[2]), V2(min[2]), V2(typ[2]),
			V2(label[3]), V2(req[3]), V2(self[3]), V2(alt[3]), V2(min[3]), V2(typ[3]),
			V2(label[4]), V2(req[4]), V2(self[4]), V2(alt[4]), V2(min[4]), V2(typ[4]),
			0,
			(V(ssh_timestamp) ? V(ssh_timestamp) + 1167638400 : 0)
		);	
}

static void vpd_info_init(void)
{
	int i,j,pos,tmp;

	/* init vpd_hwm_vol 0-119 byte */
	pos = 0;
	for(i=0; i<10; i++) {
		memset(V1(label)[i],0,9);
		for( j = 0; j < 8; j++)
			V1(label)[i][j] = VBYTE;
		V1(min0)[i] = VBYTE;
		V1(min1)[i] = VBYTE;
		V1(max0)[i] = VBYTE;
		V1(max1)[i] = VBYTE;
	}
	
	/* init vpd_hwm_fan 120-179 */    	
	pos = 120;		
	for(i=0; i<5; i++) {
		memset(V2(label)[i],0,8);
		for( j=0; j < 7; j++)
			V2(label)[i][j] = VBYTE;
		tmp = VBYTE;
		V2(req)[i] = tmp & 0x01;
		V2(self)[i] = (tmp>>1) & 0x01;
		V2(alt)[i] = (tmp>>2) & 0x3F;
		V2(min)[i] = *((unsigned short*)VPOINT);
		pos +=2;
		V2(typ)[i] = *((unsigned short*)VPOINT);
		pos +=2;
	}

	/* init vpd_cpu_* */
	pos = 180;
	V(cpu_target) = VBYTE;
	pos = 181;
	V(cpu_tolerance) = VBYTE;
	pos = 255;
	V(ver) = VBYTE;

	/* init vpd_mac0 256-261 vpd_mac1 456-461 */
	pos = 256;
	for( i =0; i<6; i++)
		V(mac0)[i] = VBYTE;
	pos = 456;
	for( i =0; i<6; i++)
		V(mac1)[i] = VBYTE;

	/* init vpd_vendor 264-295 model 296-327*/
	pos = 264;
	memcpy(V(vendor),VPOINT,32);
	pos = 296;
	memcpy(V(model),VPOINT,32);
	pos = 328;
	memcpy(V(serial),VPOINT,16);
	pos = 344;
	memcpy(V(version),VPOINT,24);

	/* init vpd_oc 374 */
	pos = 374;
	V(oc) = VBYTE;

	/* init vpd_sata_chn_max 375 */
	pos = 375;
	V(sata_chn_max) = VBYTE;

	/* init vpd_features 376-379 */
	pos = 376;
	V(features)=VBYTE;
	V(features)<<=8;
	V(features)+=VBYTE;
	V(features)<<=8;
	V(features)+=VBYTE;
	V(features)<<=8;
	V(features)+=VBYTE;
	
	/* init vpd_features1 380-383 */
	pos = 380;
	V(features1)=VBYTE;
	V(features1)<<=8;
	V(features1)+=VBYTE;
	V(features1)<<=8;
	V(features1)+=VBYTE;
	V(features1)<<=8;
	V(features1)+=VBYTE;

	/* init vpd_boardid 384 */
	pos = 384;
	V(boardid)=VBYTE;
	
	/* init vpd_fan_rpm_min, vpd_fan_pwm_min, 
	   vpd_pwm_cft 385,386,387 */
	pos=385;
	V3(rpm_min)=VBYTE;	
	V3(pwm_min)=VBYTE;
	V3(pwm_cft)=VBYTE;
		
	/* init vpd_board_rev,vpd_system_rev 388-391 */
	pos=388;
	V(board_rev) = ((unsigned char)VBYTE<<16) + VBYTE;
	V(system_rev) = ((unsigned char)VBYTE<<16) + VBYTE;

	/* init vpd_ecc_flag 392 */
	pos = 392;
	V4(flag)=VBYTE;

	/* init vpd_ecc_ignore 394-395 */
	pos = 394;
	V4(ignore)=*((unsigned short*)VPOINT);

	/* init vpd_ecc_count 396-399 */
	pos = 396;
	V4(count)=*((unsigned long*)VPOINT);

	/* init vpd_ecc_correct 400-401 */
	pos=400;
	V4(correct)=*((unsigned short*)VPOINT);
	
	/* init vpd_spd03t30 and vpd_spd41t43 403-430 441-443 */
	pos=403;
	for(i=0;i<28;i++)
		V(spd03t30)[i]=VBYTE;
	pos=441;
	for(i=0;i<3;i++)
		V(spd41t43)[i]=VBYTE;

	/* init vpd_ssh_timestamp 431-434 */
	pos=431;
	V(ssh_timestamp)=*((unsigned long*)VPOINT);

	/* init vpd_maxspeed,vpd_maxtemp,vpd_maxrpm 462-464 */
	pos=462;
	V5(speed)=VBYTE;
	V5(temp)=VBYTE;
	V5(rpm)=VBYTE;

	/* init vpd_spindly, vpd_ledlum 465-468 */
	pos=465;
	V(spindly)=VBYTE/16;
	V8(0_1)=VBYTE;
	V8(2_3)=VBYTE;
	V8(4_5)=VBYTE;

	/* init vpd_max_temp1-3, vpd_min_temp1-3 469-474 */
	pos=469;
	V6(1)=VBYTE;V7(1)=VBYTE;		
	V6(2)=VBYTE;V7(2)=VBYTE;		
	V6(3)=VBYTE;V7(3)=VBYTE;	
	vpd_boot_info_init();	
}

static struct ctl_table boot_info_table[] = {
	{
		.procname     = "info", 
		.data	      = boot_info_buf,
		.maxlen       = sizeof(boot_info_buf), 
		.mode	      =	0444,
		.proc_handler = &proc_dostring,
	},
	{}
};

static void add_legacy_compat(void)
{
	int len = strlen(boot_info_buf);

	if (len > (sizeof(boot_info_buf) - 24))
		return;
	strcat(boot_info_buf, "\nmode:\t0");
	strcat(boot_info_buf, "\nmodel_valid:\t1");
}
	


static struct ctl_table boot_table[] = {
	{
		.procname     = "boot",
		.mode	      = 0555,
		.child	      = boot_info_table, 
	},
	{}
};
	
static struct ctl_table boot_root_table[] = {
	{
		.procname      = "dev",
		.mode          = 0555,
		.child         = boot_table,
	},
	{}
};

static struct ctl_table_header *boot_sysctl_header;

static long vpd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;

	if(current->pid != 1)
		return err;

	switch (cmd & (IOCTL_VPD_CMD_FLAG-1)) {
	case IOCTL_VPD_WRITE:
		if (copy_from_user(&vpd_buffer, (unsigned long *)arg, 512))
			return -EFAULT;
		vpd_info_init();
		break;
	case IOCTL_BASIC_WRITE:
		if (copy_from_user(&vpd_buffer, (unsigned long *)arg, sizeof(struct basic_vpd_data_struct)))
			return -EFAULT;
		basic_boot_info_init();
		break;
	case IOCTL_LEGACY_COMPAT:
		add_legacy_compat();
		break;
	}

	return err;
}

static struct file_operations vpd_fops = {
	owner:		THIS_MODULE,
	unlocked_ioctl:	vpd_ioctl,
	compat_ioctl:	vpd_ioctl,
};

static int vpd_init(void) 
{
	pr_info("ReadyNAS VPD init\n");

	boot_sysctl_header = register_sysctl_table(boot_root_table);

	register_chrdev(VPD_DEVICE_MAJOR, VPD_DEVICE_NAME, &vpd_fops);

	boot_info_init();
	
	return 0;
}

static void vpd_exit(void)
{
	pr_info("ReadyNAS VPD exit\n");
	unregister_chrdev(VPD_DEVICE_MAJOR, VPD_DEVICE_NAME);
	if (boot_sysctl_header)
		unregister_sysctl_table(boot_sysctl_header);
}

module_init(vpd_init);
module_exit(vpd_exit);

