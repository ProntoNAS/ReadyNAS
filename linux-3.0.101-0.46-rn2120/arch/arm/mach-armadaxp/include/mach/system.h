/*
 * include/mach/system.h
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_SYSTEM_H
#define __MACH_SYSTEM_H

#include <asm/proc-fns.h>
#include <mach/hardware.h>
#include "boardEnv/mvBoardEnvLib.h"

#ifdef CONFIG_MV_LARGE_PAGE_SUPPORT
#define LSP_PG_SZ_VER  " (" MV_PAGE_SIZE_STR ")"
#else
#define LSP_PG_SZ_VER  ""
#endif

#define USI_VERSION    "\n  USI version: rn2120-1.4"
#define LSP_VERSION    "linux-3.0.39-axp-2012_Q4.1" LSP_PG_SZ_VER USI_VERSION

static inline void arch_idle(void)
{
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	printk("Reseting...\n");
	mvBoardReset();
	while (1);/* This should never be reached */
}

#endif
