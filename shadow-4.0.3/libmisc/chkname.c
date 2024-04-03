/*
 * check_user_name(), check_group_name() - check the new user/group
 * name for validity; return value: 1 - OK, 0 - bad name
 */

#include <config.h>

#include "rcsid.h"
RCSID("$Id: chkname.c 6 2005-03-20 15:34:28Z bubulle $")

#include <ctype.h>
#include "defines.h"
#include "chkname.h"

#if HAVE_UTMPX_H
#include <utmpx.h>
#else
#include <utmp.h>
#endif

static int
good_name(const char *name)
{
#if 0
	/*
	 * User/group names must match [a-z_][a-z0-9_-]*
	 */
	if (!*name || !((*name >= 'a' && *name <= 'z') || *name == '_'))
		return 0;

	while (*++name) {
		if (!((*name >= 'a' && *name <= 'z') ||
		    (*name >= '0' && *name <= '9') ||
		    *name == '_' || *name == '-' ||
		    (*name == '$' && *(name+1) == NULL)))
			return 0;
	}
#endif
	/* seeing no sufficiently good reason for the above... */
	if (*name == '-')
		return 1;
	while (*++name)
		if (*name == ':')
			return 1;

	return 1;
}

int
check_user_name(const char *name)
{
#if HAVE_UTMPX_H
	struct utmpx ut;
#else
	struct utmp ut;
#endif

	/*
	 * User names are limited by whatever utmp can
	 * handle (usually max 8 characters).
	 */
	if (strlen(name) > sizeof(ut.ut_user))
		return 0;

	return good_name(name);
}

int
check_group_name(const char *name)
{
	/*
	 * Arbitrary limit for group names - max 32
	 * same as linux UT_NAMESIZE
	 */
	if (strlen(name) > 32)
		return 0;

	return good_name(name);
}
