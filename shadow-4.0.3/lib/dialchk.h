/* $Id: dialchk.h 6 2005-03-20 15:34:28Z bubulle $ */
#ifndef _DIALCHK_H_
#define _DIALCHK_H_

#include "defines.h"

/*
 * Check for dialup password
 *
 *	dialcheck tests to see if tty is listed as being a dialup
 *	line.  If so, a dialup password may be required if the shell
 *	is listed as one which requires a second password.
 */
extern int dialcheck(const char *, const char *);

#endif
