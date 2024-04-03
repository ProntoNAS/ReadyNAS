/*
 * $Id: cap_sys.c,v 1.1.1.1 1999/04/17 22:16:31 morgan Exp $
 *
 * Copyright (c) 1997-8 Andrew G. Morgan   <morgan@linux.kernel.org>
 *
 * This file contains the system calls for getting and setting
 * capabilities
 */

#include "libcap.h"
#define __LIBRARY__
#include <linux/unistd.h>
/* glic >= 2.1 knows capset/capget. no need to define it here */ 
/* 
_syscall2(int, capget,
	  cap_user_header_t, header,
	  cap_user_data_t, data)

_syscall2(int, capset,
	  cap_user_header_t, header,
	  const cap_user_data_t, data)
*/
/*
 * $Log: cap_sys.c,v $
 * Revision 1.1.1.1  1999/04/17 22:16:31  morgan
 * release 1.0 of libcap
 *
 * Revision 1.4  1998/06/08 00:14:01  morgan
 * change to accommodate alpha (glibc?)
 *
 * Revision 1.3  1998/05/24 22:54:09  morgan
 * updated for 2.1.104
 *
 * Revision 1.2  1997/04/28 00:57:11  morgan
 * fixes and zefram's patches
 *
 * Revision 1.1  1997/04/21 04:32:52  morgan
 * Initial revision
 *
 */
