/*
 *  linux/drivers/mfd/lpc_ich.h
 *
 *  Copyright (c) 2012 Extreme Engineering Solution, Inc.
 *  Author: Aaron Sierra <asierra@xes-inc.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 2 as published
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef LPC_ICH_H
#define LPC_ICH_H

/* Watchdog resources */
#define ICH_RES_IO_TCO		0
#define ICH_RES_IO_SMI		1
#define ICH_RES_MEM_OFF		2
#define ICH_RES_MEM_GCS_PMC	0

/* GPIO resources */
#define ICH_RES_GPIO	0
#define ICH_RES_GPE0	1

/* GPIO compatibility */
enum {
	ICH_I3100_GPIO,
	ICH_V5_GPIO,
	ICH_V6_GPIO,
	ICH_V7_GPIO,
	ICH_V9_GPIO,
	ICH_V10CORP_GPIO,
	ICH_V10CONS_GPIO,
	AVOTON_GPIO,
	LPT_GPIO,
};

struct lpc_ich_info {
	char name[32];
	unsigned int iTCO_version;
	unsigned int gpio_version;
	u8 use_gpio;
};

/*
 * ichx-lpt-gpio deivce-specific "control" command definitions.
 *
 * Author: <hiro.sugawara@netgear.com>
 */

#define	ICHX_LPT_POL_GPIO_IRQ_POS	0x1000000
#define	ICHX_LPT_POL_GPIO_IRQ_NEG	0x2000000
#define	ICHX_LPT_POL_GPIO_IRQ_INV	0x3000000
#define	IS_ICHX_LPT_POL_GPIO_IRQ(cmd)	(	\
		((cmd) >> 24) == 1	||	\
		((cmd) >> 24) == 2	||	\
		((cmd) >> 24) == 3)

#define MK_ICHX_LPT_POL_GPIO_IRQ(typ, gpi)	\
		(ICHX_LPT_POL_GPIO_IRQ_##typ | (gpi))
#define	GPI_ICHX_LPT_POL_GPIO_IRQ(cmd)	((cmd) & 0xff)
#define	ICHX_LPT_POL_GPIO_IRQ(cmd)	((cmd) & ~0xffffff)

#define	ICHX_LPT_CMD_GPIO_IRQ_REGISTER	0x8000000

#define	IS_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd)	\
		(((cmd) & ~0xffffff) == ICHX_LPT_CMD_GPIO_IRQ_REGISTER)
#define	GPI_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd)	((cmd) & 0xff)
#define	IRQ_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd)	(((cmd) >> 8) & 0xff)
#define	TYP_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd)	(((cmd) >> 16) & 0xff)
#define	MK_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(gpi, irq, typ)	\
			(((gpi) & 0xff) |	\
			(((irq) & 0xff) << 8) |	\
			(((typ) & 0xff) << 16) |	\
			ICHX_LPT_CMD_GPIO_IRQ_REGISTER)

#define	ICHX_LPT_TYP_GPIO_IRQ_NONE	0
#define	ICHX_LPT_TYP_GPIO_IRQ_SMI	1
#define	ICHX_LPT_TYP_GPIO_IRQ_SCI	2
#define	ICHX_LPT_TYP_GPIO_IRQ_NMI	3
#define	ICHX_LPT_TYP_GPIO_IRQ_GOOD(n)	(0 <= (n) && (n) <= 3)
#endif
