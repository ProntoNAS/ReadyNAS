#ifndef _ASM_M32R_CURRENT_H
#define _ASM_M32R_CURRENT_H

/* $Id: current.h,v 1.1.1.1 2006-08-28 17:51:50 bzhang Exp $ */

#include <linux/thread_info.h>

struct task_struct;

static __inline__ struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

#define current	(get_current())

#endif	/* _ASM_M32R_CURRENT_H */

