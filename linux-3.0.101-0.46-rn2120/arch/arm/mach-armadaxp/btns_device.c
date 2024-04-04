/********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
*******************************************************************************/
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include "btns_dev.h"
#include "../plat-armada/mv_drivers_lsp/mv_btns/btns_driver.h"
#include "ctrlEnv/mvCtrlEnvLib.h"

/*
 * common debug for all
 */
#undef DEBUG
#define DEBUG

#ifdef DEBUG
#define dprintk   printk
#else
#define dprintk(a...)
#endif

static struct btns_platform_data 	btns_data;

static struct btn_data			btn_data_rn2120[] = { 
	[0] = {
		.gpp_id		= 41,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_CHANGE,
		.btn_name	= "reset",
	},
	[1] = {
		.gpp_id		= 27,
		.default_gpp_val= 0x8000000,
		.btn_op		= BTN_CHANGE,
		.btn_name	= "power",
	}

};

static struct btn_data			btn_data_rd_6192[] = { 
	[0] = {
		.gpp_id		= UP_GPP,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_PUSH,
		.btn_name	= "UP",
	},
	[1] = {
		.gpp_id		= DOWN_GPP,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_PUSH,
		.btn_name	= "DOWN",
	},
	[2] = {
		.gpp_id		= LEFT_GPP,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_PUSH,
		.btn_name	= "LEFT",
	},
/*	[3] = {
		.gpp_id		= PRESSED_GPP,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_PUSH,
		.btn_name	= "PRESSED",
	},*/
	[3] = {
		.gpp_id		= RIGHT_GPP,
		.default_gpp_val= 0x0,
		.btn_op		= BTN_PUSH,
		.btn_name	= "RIGHT",
	},

};

static struct platform_device btns_device = {
	.name           = MV_BTNS_NAME,
        .id             = 0,
        .num_resources  = 0,
	.dev = {
		.platform_data  = &btns_data,
	},
};

static int btns_init_data(struct platform_device *pdev)
{
	//	btns_data.btns_data_arr = NULL;
	btns_data.btns_data_arr = btn_data_rn2120;
	btns_data.btns_num = (btns_data.btns_data_arr == NULL) ? 
		0 : ARRAY_SIZE(btn_data_rn2120);


	if(btns_data.btns_num)	
		dprintk("%s - Number of configured buttons: %d\n", __FUNCTION__ ,btns_data.btns_num);
	
	return 0;
}

static int __init   mv_btns_init(void)
{
	int                       status;
	
	printk(KERN_NOTICE "MV Buttons Device Load\n");

	/* Initialize btns related structures and data*/
	status = btns_init_data(&btns_device);	
	if (status) {
            printk("Can't initialize Marvell Buttons Data, status=%d\n", status);
            return status;
        }
	
	/* register device */
	status = platform_device_register(&btns_device);
        if (status) {
            printk("Can't register Marvell Buttons Device, status=%d\n", status);
            return status;
        }

    	return 0;
}

subsys_initcall(mv_btns_init);

