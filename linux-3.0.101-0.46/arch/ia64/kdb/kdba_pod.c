/*
 * Kernel Debugger Architecture Dependent POD functions.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2006 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/types.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/module.h>
#include <asm/sal.h>
#include <asm/sn/sn_sal.h>

MODULE_AUTHOR("Jesse Barnes");
MODULE_DESCRIPTION("Enter POD through KDB");
MODULE_LICENSE("GPL");

/**
 * kdba_pod - enter POD mode from kdb
 * @argc: arg count
 * @argv: arg values
 *
 * Enter POD mode from kdb using SGI SN specific SAL function call.
 */
static int
kdba_pod(int argc, const char **argv)
{
	lkdb_printf("WARNING: pod commands are dangerous unless you know exactly\n"
		   "what you are doing.  If in doubt, type exit immediately.\n");
	return ia64_sn_pod_mode();
}

/**
 * kdba_pod_init - register 'pod' command with kdb
 *
 * Register the 'pod' command with kdb at load time.
 */
static int __init
kdba_pod_init(void)
{
	if (ia64_platform_is("sn2"))
		lkdb_register("pod", kdba_pod, NULL, "Enter POD", 0);

	return 0;
}

/**
 * kdba_pod_exit - unregister the 'pod' command
 *
 * Tell kdb that the 'pod' command is no longer available.
 */
static void __exit
kdba_pod_exit(void)
{
	if (ia64_platform_is("sn2"))
		lkdb_unregister("pod");
}

module_init(kdba_pod_init)
module_exit(kdba_pod_exit)
