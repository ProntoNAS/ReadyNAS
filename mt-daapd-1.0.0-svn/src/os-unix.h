/*
 * $Id: os-unix.h,v 1.1.1.1 2010-09-22 17:14:54 jmaggard Exp $
 * Copyright (C) 2006 Ron Pedde (rpedde@users.sourceforge.net)
 *
 */

#ifndef _OS_UNIX_H_
#define _OS_UNIX_H_

#define PATHSEP '/'
#define PATHSEP_STR "/"
#define OS_SOCKETTYPE unsigned int

/* unix-specific functions */
extern int os_drop_privs(char *user);
void os_set_pidfile(char *file);
#endif

