#ifndef	__ASM_SH_KEYBOARD_H
#define	__ASM_SH_KEYBOARD_H
/*
 *	$Id: keyboard.h,v 1.1.1.1 2006-08-28 17:51:51 bzhang Exp $
 */

#include <linux/kd.h>
#include <linux/config.h>
#include <asm/machvec.h>

#ifdef CONFIG_SH_MPC1211
#include <asm/mpc1211/keyboard-mpc1211.h>
#endif
#endif
