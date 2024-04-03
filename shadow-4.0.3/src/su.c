/*
 * Copyright 1989 - 1994, Julianne Frances Haugh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Julianne F. Haugh nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JULIE HAUGH AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL JULIE HAUGH OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* Some parts substantially derived from an ancestor of: */
/* su for GNU.  Run a shell with substitute user and group IDs.
   Copyright (C) 1992-2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include <config.h>

#include "rcsid.h"
RCSID (PKG_VER "$Id: su.c 6 2005-03-20 15:34:28Z bubulle $")
#include <sys/types.h>
#include <stdio.h>
#ifdef USE_PAM
#include "pam_defs.h"
static const struct pam_conv conv = {
	misc_conv,
	NULL
};

static pam_handle_t *pamh = NULL;
#endif

#include "prototypes.h"
#include "defines.h"

#include <grp.h>
#include <signal.h>
#include <pwd.h>
#include <getopt.h>
#include "pwauth.h"
#include "getdef.h"

/*
 * Assorted #defines to control su's behavior
 */

/*
 * Global variables
 */

/* not needed by sulog.c anymore */
static char name[BUFSIZ];
static char oldname[BUFSIZ];

static char *Prog;

extern struct passwd pwent;

/*
 * External identifiers
 */

extern char **newenvp;
extern size_t newenvc;

extern char **environ;

/* local function prototypes */

/* If nonzero, change some environment vars to indicate the user su'd to.  */
static int change_environment;

static struct option const longopts[] =
{
  {"command", required_argument, 0, 'c'},
  {"preserve-environment", no_argument, 0, 'p'},
  {"shell", required_argument, 0, 's'},
  {"help", no_argument, 0, 'h'},
  {0, 0, 0, 0}
};

#ifndef USE_PAM

static RETSIGTYPE die (int);
static int iswheel (const char *);

/*
 * die - set or reset termio modes.
 *
 *	die() is called before processing begins. signal() is then called
 *	with die() as the signal handler. If signal later calls die() with a
 *	signal number, the terminal modes are then reset.
 */

static RETSIGTYPE die (int killed)
{
	static TERMIO sgtty;

	if (killed)
		STTY (0, &sgtty);
	else
		GTTY (0, &sgtty);

	if (killed) {
		closelog ();
		exit (killed);
	}
}

static int iswheel (const char *username)
{
	struct group *grp;

	grp = getgrgid (0);
	if (!grp || !grp->gr_mem)
		return 0;
	return is_on_list (grp->gr_mem, username);
}
#endif				/* !USE_PAM */

/* borrowed from GNU sh-utils' "su.c" */
static int
restricted_shell (const char *shell)
{
       char *line;

       setusershell ();
       while ((line = getusershell ()) != NULL) {
               if (*line != '#' && strcmp (line, shell) == 0) {
                       endusershell ();
                       return 0;
               }
       }
       endusershell ();
       return 1;
}

/* borrowed from GNU sh-utils' "su.c" */
static int
elements (char **arr)
{
  int n = 0;

  for (n = 0; *arr; ++arr)
    ++n;
  return n;
}

/* borrowed from GNU sh-utils' "su.c" */
static void
run_shell (char *shell, const char *command, char **additional_args, int login)
{
  const char **args;
  int argno = 1;
  char cmd[BUFSIZ];
  int cmd_len_left = sizeof(cmd) - 1;

  cmd[0] = '\0';

  if (additional_args)
    args = (const char **) xmalloc (sizeof (char *)
                                    * (10 + elements (additional_args)));
  else
    args = (const char **) xmalloc (sizeof (char *) * 10);

  if (login)
    {
      char *arg0;
      char *shell_basename;

      shell_basename = getdef_str("SU_NAME");
      if (!shell_basename)
       shell_basename = Basename(shell);

      arg0 = xmalloc (strlen (shell_basename) + 2);
      arg0[0] = '-';
      strcpy (arg0 + 1, shell_basename);
      args[0] = arg0;
    }
  else
    args[0] = Basename(shell);
  if (command || additional_args)
    args[argno++] = "-c";
  if (command) {
    if (strlen(command) > cmd_len_left) {
      fprintf(stderr, _("Command line args too long\n"));
      exit(1);
    }
    strcat(cmd, command);
    cmd_len_left -= strlen(command);
  }
  if (additional_args)
    for (; *additional_args; ++additional_args) {
      if ((strlen(*additional_args) + 1) > cmd_len_left) {
	fprintf(stderr, _("Command line args too long\n"));
	exit(1);
      }
      if (cmd[0]) {
	strcat(cmd, " ");
	cmd_len_left--;
      }
      strcat(cmd, *additional_args);
      cmd_len_left -= strlen(*additional_args);
    }
  if (cmd[0]) args[argno++] = cmd;
  args[argno] = NULL;
  execv (shell, (char **) args);
  fprintf (stderr, _("No shell\n"));
  SYSLOG((LOG_WARN, "Cannot execute %s\n", shell));
}

static void su_failure (const char *tty)
{
	sulog (tty, 0, oldname, name);	/* log failed attempt */
#ifdef USE_SYSLOG
	if (getdef_bool ("SYSLOG_SU_ENAB"))
		SYSLOG ((pwent.pw_uid ? LOG_INFO : LOG_NOTICE,
			 "- %s %s:%s", tty,
			 oldname[0] ? oldname : "???",
			 name[0] ? name : "???"));
	closelog ();
#endif
	puts (_("Sorry."));
	exit (1);
}


/*
 * su - switch user id
 *
 *	su changes the user's ids to the values for the specified user.  if
 *	no new user name is specified, "root" is used by default.
 *
 *	The only valid option is a "-" character, which is interpreted as
 *	requiring a new login session to be simulated.
 *
 *	Any additional arguments are passed to the user's shell. In
 *	particular, the argument "-c" will cause the next argument to be
 *	interpreted as a command by the common shell programs.
 */

int main (int argc, char **argv)
{
	char *cp;
	const char *tty = 0;	/* Name of tty SU is run from        */
	int fakelogin = 0;
	int amroot = 0;
	uid_t my_uid;
	struct passwd *pw = 0;
	char **envp = environ;
	char *command = 0, *shell = 0, **additional_args = 0;
	int optc;
	char *tmp_name;
#ifdef USE_PAM
	int ret;
#else				/* !USE_PAM */
	RETSIGTYPE (*oldsig) ();
	int is_console = 0;

#ifdef	SHADOWPWD
	struct spwd *spwd = 0;
#endif
#ifdef SU_ACCESS
	char *oldpass;
#endif
#endif				/* !USE_PAM */

	/* sanitize_env (); */

	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);

	change_environment = 1;

	/*
	 * Get the program name. The program name is used as a prefix to
	 * most error messages.
	 */

	Prog = Basename (argv[0]);

	OPENLOG ("su");

	initenv ();

	my_uid = getuid ();
	amroot = (my_uid == 0);

	/*
	 * Get the tty name. Entries will be logged indicating that the user
	 * tried to change to the named new user from the current terminal.
	 */

	if (isatty (0) && (cp = ttyname (0))) {
		if (strncmp (cp, "/dev/", 5) == 0)
			tty = cp + 5;
		else
			tty = cp;
#ifndef USE_PAM
		is_console = console (tty);
#endif
	} else {
		/*
		 * Be more paranoid, like su from SimplePAMApps.  --marekm
		 */
		if (!amroot) {
			fprintf (stderr,
				 _("%s: must be run from a terminal\n"),
				 Prog);
			exit (1);
		}
		tty = "???";
	}

	/*
	 * Process the command line arguments. 
	 */

	while ((optc = getopt_long (argc, argv, "c:mps:h", longopts, NULL)) != -1) {
		switch (optc) {
		    case 0:
			break;
		    case 'c':
			command = optarg;
			break;
		    case 'm':
		    case 'p':
			change_environment = 0;
			break;
		    case 's':
			shell = optarg;
			break;
		    default:
			fprintf(stderr, _("\
Usage: su [OPTS] [-] [username [ARGS]]\n\
	-	make this a login shell\n\
	-c, --command=<command>\n\
		pass command to the invoked shell using its -c\n\
		option\n\
       -m, -p, --preserve-environment\n\
		do not reset environment variables, and keep the\n\
		same shell\n\
       -s, --shell=<shell>\n\
		use shell instead of the default in /etc/passwd\n"));
			exit(1);
		}
	}
	
        if (optind < argc && !strcmp (argv[optind], "-")) {
		fakelogin = 1;
		++optind;
	}

	if (optind < argc)
		strncpy(name, argv[optind++], sizeof(name) - 1);
	else {
	        struct passwd *root_pw = getpwuid(0);
		if (root_pw == NULL) {
		  SYSLOG((LOG_CRIT, "There is no UID 0 user."));
		  su_failure(tty);
		}
                strcpy(name, root_pw->pw_name);
	}

	if (optind < argc)
		additional_args = argv + optind;

	/*
	 * Get the user's real name.  The current UID is used to determine
	 * who has executed su.  That user ID must exist.
	 */

	pw = get_my_pwent();
	if (!pw) {
		SYSLOG((LOG_CRIT, "Unknown UID: %d\n", (int) my_uid));
		su_failure(tty);
	}
	STRFCPY(oldname, pw->pw_name);

	/*
	 * If a new login is being set up, the old environment will be
	 * ignored and a new one created later on.
	 */

	if (fakelogin) {
		if ((cp = getdef_str ("ENV_TZ")))
			addenv (*cp == '/' ? tz (cp) : cp, NULL);
		/*
		 * The clock frequency will be reset to the login value if required
		 */
		if ((cp = getdef_str ("ENV_HZ")))
			addenv (cp, NULL);	/* set the default $HZ, if one */
		/*
		 * The terminal type will be left alone if it is present in
		 * the environment already.
		 */
		if ((cp = getenv ("TERM")))
			addenv ("TERM", cp);
	} else {
		while (*envp)
			addenv (*envp++, NULL);
	}

#ifndef USE_PAM
#ifdef SU_ACCESS
	/*
	 * Sort out the password of user calling su, in case needed later
	 * -- chris
	 */
#ifdef SHADOWPWD
	if ((spwd = getspnam (oldname)))
		pw->pw_passwd = spwd->sp_pwdp;
#endif
	oldpass = xstrdup (pw->pw_passwd);
#endif				/* SU_ACCESS */

#else				/* USE_PAM */
	ret = pam_start ("su", name, &conv, &pamh);
	if (ret != PAM_SUCCESS) {
		SYSLOG ((LOG_ERR, "pam_start: error %d", ret);
			fprintf (stderr, _("%s: pam_start: error %d\n"),
				 Prog, ret));
		exit (1);
	}

	ret = pam_set_item (pamh, PAM_TTY, (const void *) tty);
	if (ret == PAM_SUCCESS)
		ret =
		    pam_set_item (pamh, PAM_RUSER, (const void *) oldname);
	if (ret != PAM_SUCCESS) {
		SYSLOG ((LOG_ERR, "pam_set_item: %s",
			 pam_strerror (pamh, ret)));
		fprintf (stderr, "%s: %s\n", Prog,
			 pam_strerror (pamh, ret));
		pam_end (pamh, ret);
		exit (1);
	}
#endif				/* USE_PAM */

      top:
	/*
	 * This is the common point for validating a user whose name is
	 * known. It will be reached either by normal processing, or if the
	 * user is to be logged into a subsystem root.
	 *
	 * The password file entries for the user is gotten and the account
	 * validated.
	 */

	if (!(pw = getpwnam (name))) {
		(void) fprintf (stderr, _("Unknown id: %s\n"), name);
		closelog ();
		exit (1);
	}
#ifndef USE_PAM
#ifdef SHADOWPWD
	spwd = NULL;
	if (strcmp (pw->pw_passwd, SHADOW_PASSWD_STRING) == 0
	    && (spwd = getspnam (name)))
		pw->pw_passwd = spwd->sp_pwdp;
#endif
#endif				/* !USE_PAM */
	pwent = *pw;

#ifndef USE_PAM
	/*
	 * BSD systems only allow "wheel" to SU to root. USG systems don't,
	 * so we make this a configurable option.
	 */

	/* The original Shadow 3.3.2 did this differently. Do it like BSD:
	 *
	 * - check for uid 0 instead of name "root" - there are systems with
	 *   several root accounts under different names,
	 *
	 * - check the contents of /etc/group instead of the current group
	 *   set (you must be listed as a member, GID 0 is not sufficient).
	 *
	 * In addition to this traditional feature, we now have complete su
	 * access control (allow, deny, no password, own password).  Thanks
	 * to Chris Evans <lady0110@sable.ox.ac.uk>.
	 */

	if (!amroot) {
		if (pwent.pw_uid == 0 && getdef_bool ("SU_WHEEL_ONLY")
		    && !iswheel (oldname)) {
			fprintf (stderr,
				 _("You are not authorized to su %s\n"),
				 name);
			exit (1);
		}
#ifdef SU_ACCESS
		switch (check_su_auth (oldname, name)) {
		case 0:	/* normal su, require target user's password */
			break;
		case 1:	/* require no password */
			pwent.pw_passwd = "";	/* XXX warning: const */
			break;
		case 2:	/* require own password */
			puts (_("(Enter your own password.)"));
			pwent.pw_passwd = oldpass;
			break;
		default:	/* access denied (-1) or unexpected value */
			fprintf (stderr,
				 _("You are not authorized to su %s\n"),
				 name);
			exit (1);
		}
#endif				/* SU_ACCESS */
	}
#endif				/* !USE_PAM */

	/*
	 * Set the default shell.
	 */

	if (pwent.pw_shell == NULL || pwent.pw_shell[0] == '\0')
		pwent.pw_shell = (char *) "/bin/sh";

	if (shell == 0 && change_environment == 0)
		shell = getenv ("SHELL");
	if (shell != 0 && getuid () && restricted_shell (pwent.pw_shell))
		shell = 0;
	if (shell == 0)
	shell = (char *) strdup (pwent.pw_shell);

	signal(SIGINT, SIG_IGN);
#ifdef USE_PAM
	ret = pam_authenticate (pamh, 0);
	if (ret != PAM_SUCCESS) {
		SYSLOG ((LOG_ERR, "pam_authenticate: %s",
			 pam_strerror (pamh, ret)));
		fprintf (stderr, "%s: %s\n", Prog,
			 pam_strerror (pamh, ret));
		pam_end (pamh, ret);
		su_failure (tty);
	}

	ret = pam_acct_mgmt (pamh, 0);
	if (ret != PAM_SUCCESS) {
		if (amroot) {
			fprintf (stderr, _("%s: %s\n(Ignored)\n"), Prog,
				 pam_strerror (pamh, ret));
		} else {
			SYSLOG ((LOG_ERR, "pam_acct_mgmt: %s",
				 pam_strerror (pamh, ret)));
			fprintf (stderr, "%s: %s\n", Prog,
				 pam_strerror (pamh, ret));
			pam_end (pamh, ret);
			su_failure (tty);
		}
	}
	ret = pam_get_item(pamh, PAM_USER, (const void **) &tmp_name);
	if (ret != PAM_SUCCESS) {
		SYSLOG((LOG_ERR, "pam_get_item: internal PAM error\n"));
		fprintf(stderr, "%s: Internal PAM error retrieving username\n", Prog);
		pam_end(pamh, ret);
		su_failure(tty);
	}
	strncpy(name, tmp_name, sizeof(name) - 1);
#else				/* !USE_PAM */
	/*
	 * Set up a signal handler in case the user types QUIT.
	 */

	die (0);
	oldsig = signal (SIGQUIT, die);

	/*
	 * See if the system defined authentication method is being used. 
	 * The first character of an administrator defined method is an '@'
	 * character.
	 */

	if (!amroot && pw_auth (pwent.pw_passwd, name, PW_SU, (char *) 0)) {
		SYSLOG ((pwent.pw_uid ? LOG_NOTICE : LOG_WARN,
			 "Authentication failed for %s", name));
		su_failure (tty);
	}
	signal (SIGQUIT, oldsig);

	/*
	 * Check to see if the account is expired. root gets to ignore any
	 * expired accounts, but normal users can't become a user with an
	 * expired password.
	 */

	if (!amroot) {
#ifdef	SHADOWPWD
		if (!spwd)
			spwd = pwd_to_spwd (&pwent);

		if (isexpired (&pwent, spwd)) {
			SYSLOG ((pwent.pw_uid ? LOG_WARN : LOG_CRIT,
				 "Expired account %s", name));
			su_failure (tty);
		}
#else
#if defined(ATT_AGE)
		if (pwent.pw_age[0] && isexpired (&pwent)) {
			SYSLOG ((pwent.pw_uid ? LOG_WARN : LOG_CRIT,
				 "Expired account %s", name));
			su_failure (tty);
		}
#endif				/* ATT_AGE */
#endif
	}

	/*
	 * Check to see if the account permits "su". root gets to ignore any
	 * restricted accounts, but normal users can't become a user if
	 * there is a "SU" entry in the /etc/porttime file denying access to
	 * the account.
	 */

	if (!amroot) {
		if (!isttytime (pwent.pw_name, "SU", time ((time_t *) 0))) {
			SYSLOG ((pwent.pw_uid ? LOG_WARN : LOG_CRIT,
				 "SU by %s to restricted account %s",
				 oldname, name));
			su_failure (tty);
		}
	}
#endif				/* !USE_PAM */

	signal (SIGINT, SIG_DFL);
	cp = getdef_str ((pwent.pw_uid == 0) ? "ENV_SUPATH" : "ENV_PATH");
#if 0
	addenv (cp ? cp : "PATH=/bin:/usr/bin", NULL);
#else
	/* XXX very similar code duplicated in libmisc/setupenv.c */
	if (!cp) {
		addenv ("PATH=/bin:/usr/bin", NULL);
	} else if (strchr (cp, '=')) {
		addenv (cp, NULL);
	} else {
		addenv ("PATH", cp);
	}
#endif

	if (change_environment || restricted_shell(pwent.pw_shell)) {
		environ = newenvp;			/* make new environment active */
		if (getenv ("IFS"))			/* don't export user IFS ... */
			addenv("IFS= \t\n", NULL);	/* ... instead, set a safe IFS */
	} else {
		if (getenv ("IFS"))
			putenv("IFS= \t\n");
	}

	if (pwent.pw_shell[0] == '*') {	/* subsystem root required */
		pwent.pw_shell++;	/* skip the '*' */
		subsystem (&pwent);	/* figure out what to execute */
		endpwent ();
#ifdef SHADOWPWD
		endspent ();
#endif
		goto top;
	}

	sulog (tty, 1, oldname, name);	/* save SU information */
	endpwent ();
#ifdef SHADOWPWD
	endspent ();
#endif
#ifdef USE_SYSLOG
	if (getdef_bool ("SYSLOG_SU_ENAB"))
		SYSLOG ((LOG_INFO, "+ %s %s:%s", tty,
			 oldname[0] ? oldname : "???",
			 name[0] ? name : "???"));
#endif

#ifdef USE_PAM
	/* set primary group id and supplementary groups */
	if (setup_groups (&pwent)) {
		pam_end (pamh, PAM_ABORT);
		exit (1);
	}

	/*
	 * pam_setcred() may do things like resource limits, console groups,
	 * and much more, depending on the configured modules
	 */
	ret = pam_setcred (pamh, PAM_ESTABLISH_CRED);
	if (ret != PAM_SUCCESS) {
		SYSLOG ((LOG_ERR, "pam_setcred: %s",
			 pam_strerror (pamh, ret)));
		fprintf (stderr, "%s: %s\n", Prog,
			 pam_strerror (pamh, ret));
		pam_end (pamh, ret);
		exit (1);
	}
	ret = pam_open_session(pamh, 0);
	if (ret != PAM_SUCCESS) {
		SYSLOG((LOG_ERR, "pam_open_session: %s\n", pam_strerror(pamh, ret)));
		fprintf(stderr, "%s: %s\n", Prog, pam_strerror(pamh, ret));
		pam_setcred(pamh, PAM_DELETE_CRED);
		pam_end(pamh, ret);
		exit(1);
	}
	/* We must fork before setuid() because we need to call
	 * pam_close_session() as root.
	 */

	/* We let the admin configure whether they need to keep login
	   around to close sessions */
	if (getdef_bool("CLOSE_SESSIONS")) {
		pid_t pid;
		int status;

		signal(SIGINT, SIG_IGN);
		pid = fork();

		switch(pid) {
		case -1:
			SYSLOG((LOG_ERR, "su: fork failure: %m"));
			perror("su: fork failure");
			pam_setcred(pamh, PAM_DELETE_CRED);
			pam_close_session(pamh, 0);
			pam_end(pamh, PAM_ABORT);
			exit(1);
		case 0: /* child */
			signal(SIGINT, SIG_DFL);
			break;
		default: /* parent */
			waitpid(pid, &status, 0);
			/* now we are done using PAM */
			pam_setcred(pamh, PAM_DELETE_CRED);
			ret = pam_close_session(pamh, 0);
			pam_end(pamh, ret);
			exit(WEXITSTATUS(status));
		}
	}

	/* become the new user */
	if (change_uid (&pwent)) {
		pam_close_session(pamh, 0);
		pam_setcred (pamh, PAM_DELETE_CRED);
		pam_end (pamh, PAM_ABORT);
		exit (1);
	}

#else				/* !USE_PAM */
	if (!amroot)		/* no limits if su from root */
		setup_limits (&pwent);

	if (setup_uid_gid (&pwent, is_console))
		exit (1);
#endif				/* !USE_PAM */

	if (fakelogin && (change_environment || restricted_shell(pwent.pw_shell)))
		setup_env(&pwent);
#if 1				/* Suggested by Joey Hess. XXX - is this right?  */
	else if (change_environment || restricted_shell(pwent.pw_shell)) {
		addenv("HOME", pwent.pw_dir);
		addenv("USER", pwent.pw_name);
		addenv("SHELL", shell);
	}
#endif

	/*
	 * This is a workaround for Linux libc bug/feature (?) - the
	 * /dev/log file descriptor is open without the close-on-exec flag
	 * and used to be passed to the new shell. There is "fcntl(LogFile,
	 * F_SETFD, 1)" in libc/misc/syslog.c, but it is commented out (at
	 * least in 5.4.33). Why?  --marekm
	 */
	closelog ();

	run_shell (shell, command, additional_args, fakelogin);
	 /*NOTREACHED*/ exit (1);
}
