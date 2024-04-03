#include "includes.h"

#include "auth.h"
#include "log.h"

#ifdef WITH_SELINUX

#include <selinux/selinux.h>
#include <selinux/flask.h>
#include <selinux/context.h>
#include <selinux/get_context_list.h>
#include <selinux/get_default_type.h>

extern Authctxt *the_authctxt;

static security_context_t
selinux_get_user_context(const char *name)
{
	security_context_t user_context = NULL;
	char *role = NULL;
	int ret = 0;

	if (the_authctxt)
		role = the_authctxt->role;
	if (role != NULL && role[0])
		ret = get_default_context_with_role(name, role, NULL,
		    &user_context);
	else
		ret = get_default_context(name, NULL, &user_context);
	if (ret < 0) {
		if (security_getenforce() > 0)
			fatal("Failed to get default security context for %s.",
			    name);
		else
			error("Failed to get default security context for %s. "
			    "Continuing in permissive mode",
			    name);
	}
	return user_context;
}

void
setup_selinux_pty(const char *name, const char *tty)
{
	security_context_t new_tty_context, user_context, old_tty_context;

	if (is_selinux_enabled() <= 0)
		return;

	new_tty_context = old_tty_context = NULL;
	user_context = selinux_get_user_context(name);

	if (getfilecon(tty, &old_tty_context) < 0) {
		error("getfilecon(%.100s) failed: %.100s",
		    tty, strerror(errno));
	} else {
		if (security_compute_relabel(user_context, old_tty_context,
		    SECCLASS_CHR_FILE, &new_tty_context) != 0) {
			error("security_compute_relabel(%.100s) failed: "
			    "%.100s", tty, strerror(errno));
		} else {
			if (setfilecon(tty, new_tty_context) != 0)
				error("setfilecon(%.100s, %s) failed: %.100s",
				    tty, new_tty_context, strerror(errno));
			freecon(new_tty_context);
		}
		freecon(old_tty_context);
	}
	if (user_context)
		freecon(user_context);
}

void
setup_selinux_exec_context(const char *name)
{
	security_context_t user_context;

	if (is_selinux_enabled() <= 0)
		return;

	user_context = selinux_get_user_context(name);

	if (setexeccon(user_context)) {
		if (security_getenforce() > 0)
			fatal("Failed to set exec security context %s for %s.",
			    user_context, name);
		else
			error("Failed to set exec security context %s for %s. "
			    "Continuing in permissive mode",
			    user_context, name);
	}
	if (user_context)
		freecon(user_context);
}

#else /* WITH_SELINUX */

void
setup_selinux_pty(const char *name, const char *tty)
{
	(void) name;
	(void) tty;
}

void
setup_selinux_exec_context(const char *name)
{
	(void) name;
}

#endif /* WITH_SELINUX */
