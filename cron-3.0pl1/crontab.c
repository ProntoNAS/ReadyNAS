/* Copyright 1988,1990,1993,1994 by Paul Vixie
 * All rights reserved
 *
 * Distribute freely, except: don't remove my name from the source or
 * documentation (don't take credit for my work), mark your changes (don't
 * get me blamed for your possible bugs), don't alter or remove this
 * notice.  May be sold if buildable source is provided to buyer.  No
 * warrantee of any kind, express or implied, is included with this
 * software; use at your own risk, responsibility for damages (if any) to
 * anyone resulting from the use of this software rests entirely with the
 * user.
 *
 * Send bug reports, bug fixes, enhancements, requests, flames, etc., and
 * I'll try to keep a version up to date.  I can be reached as follows:
 * Paul Vixie          <paul@vix.com>          uunet!decwrl!vixie!paul
 */

#if !defined(lint) && !defined(LINT)
static char rcsid[] = "$Id: crontab.c,v 2.13 1994/01/17 03:20:37 vixie Exp $";
#endif

/* crontab - install and manage per-user crontab files
 * vix 02may87 [RCS has the rest of the log]
 * vix 26jan87 [original]
 */


#define	MAIN_PROGRAM


#include "cron.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef USE_UTIMES
# include <sys/time.h>
#else
# include <time.h>
# include <utime.h>
#endif
#if defined(POSIX)
# include <locale.h>
#endif


#define NHEADER_LINES 3


enum opt_t	{ opt_unknown, opt_list, opt_delete, opt_edit, opt_replace };

#if DEBUGGING
static char	*Options[] = { "???", "list", "delete", "edit", "replace" };
#endif


static	PID_T		Pid;
static	char		*User, *RealUser;
static	char		Filename[MAX_FNAME];
static	FILE		*NewCrontab = NULL;
static	int		CheckErrorCount;
static	enum opt_t	Option;
static	struct passwd	*pw;
static	void		list_cmd __P((void)),
			delete_cmd __P((void)),
			edit_cmd __P((void)),
			poke_daemon __P((void)),
			check_error __P((char *)),
			parse_args __P((int c, char *v[]));
static	int		replace_cmd __P((void));


static void
usage(msg)
	char *msg;
{
	fprintf(stderr, "%s: usage error: %s\n", ProgramName, msg);
	fprintf(stderr, "usage:\t%s [-u user] file\n", ProgramName);
	fprintf(stderr, "\t%s [-u user] { -e | -l | -r }\n", ProgramName);
	fprintf(stderr, "\t\t(default operation is replace, per 1003.2)\n");
	fprintf(stderr, "\t-e\t(edit user's crontab)\n");
	fprintf(stderr, "\t-l\t(list user's crontab)\n");
	fprintf(stderr, "\t-r\t(delete user's crontab)\n");
	exit(ERROR_EXIT);
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	exitstatus;

	Pid = getpid();
	ProgramName = argv[0];

#if defined(POSIX)
	setlocale(LC_ALL, "");
#endif

#if defined(BSD)
	setlinebuf(stderr);
#endif
	parse_args(argc, argv);		/* sets many globals, opens a file */
	set_cron_uid();
	set_cron_cwd();
	if (!allowed(User)) {
		fprintf(stderr,
			"You (%s) are not allowed to use this program (%s)\n",
			User, ProgramName);
		fprintf(stderr, "See crontab(1) for more information\n");
		log_it(RealUser, Pid, "AUTH", "crontab command not allowed");
		exit(ERROR_EXIT);
	}
	exitstatus = OK_EXIT;
	switch (Option) {
	case opt_list:		list_cmd();
				break;
	case opt_delete:	delete_cmd();
				break;
	case opt_edit:		edit_cmd();
				break;
	case opt_replace:	if (replace_cmd() < 0)
					exitstatus = ERROR_EXIT;
				break;
				/* The following was added to shut
				 -Wall up, but it will never be hit,
				 because the option parser will catch
				 it */
	case opt_unknown: usage("unknown option specified");
	                  break;
	}
	exit(0);
	/*NOTREACHED*/
}
	
#if DEBUGGING
char *getoptarg = "u:lerx:";
#else
char *getoptarg = "u:ler";
#endif


static void
parse_args(argc, argv)
	int	argc;
	char	*argv[];
{
	int		argch;
	struct stat	statbuf;

	if (!(pw = getpwuid(getuid()))) {
		fprintf(stderr, "%s: your UID isn't in the passwd file.\n",
			ProgramName);
		fprintf(stderr, "bailing out.\n");
		exit(ERROR_EXIT);
	}
	if (((User=strdup(pw->pw_name)) == NULL) ||
	    ((RealUser=strdup(pw->pw_name)) == NULL)) {
	        fprintf(stderr, "Memory allocation error\n");
		exit(ERROR_EXIT);
	}
	Filename[0] = '\0';
	Option = opt_unknown;

	while (EOF != (argch = getopt(argc, argv, getoptarg))) {
		switch (argch) {
#if DEBUGGING
		case 'x':
			if (!set_debug_flags(optarg))
				usage("bad debug option");
			usage("unrecognized option");
			break;
#endif
		case 'u':
			if (!(pw = getpwnam(optarg)))
			{
				fprintf(stderr, "%s:  user `%s' unknown\n",
					ProgramName, optarg);
				exit(ERROR_EXIT);
			}
			if ((getuid() != ROOT_UID) &&
			    (getuid() != pw->pw_uid))
			{
				fprintf(stderr,
					"must be privileged to use -u\n");
				exit(ERROR_EXIT);
			}
			free(User);
			if ((User=strdup(pw->pw_name)) == NULL) {
			        fprintf(stderr, "Memory allocation error\n");
				exit(ERROR_EXIT);
			}
			break;
		case 'l':
			if (Option != opt_unknown)
				usage("only one operation permitted");
			Option = opt_list;
			break;
		case 'r':
			if (Option != opt_unknown)
				usage("only one operation permitted");
			Option = opt_delete;
			break;
		case 'e':
			if (Option != opt_unknown)
				usage("only one operation permitted");
			Option = opt_edit;
			break;
		default:
			usage("unrecognized option");
		}
	}

	endpwent();

	if (Option != opt_unknown) {
		if (argv[optind] != NULL) {
			usage("no arguments permitted after this option");
		}
	} else {
		if (argv[optind] != NULL) {
			Option = opt_replace;
			(void) strncpy (Filename, argv[optind], (sizeof Filename)-1);
			Filename[(sizeof Filename)-1] = '\0';

		} else {
			usage("file name must be specified for replace");
		}
	}

	if (Option == opt_replace) {
		/* we have to open the file here because we're going to
		 * chdir(2) into /var/cron before we get around to
		 * reading the file.
		 */
		if (!strcmp(Filename, "-")) {
			NewCrontab = stdin;
		} else {
			/* relinquish the setuid status of the binary during
			 * the open, lest nonroot users read files they should
			 * not be able to read.  we can't use access() here
			 * since there's a race condition.  thanks go out to
			 * Arnt Gulbrandsen <agulbra@pvv.unit.no> for spotting
			 * the race.
			 */

			if (swap_uids() < OK) {
				perror("swapping uids");
				exit(ERROR_EXIT);
			}
			if (!(NewCrontab = fopen(Filename, "r"))) {
				perror(Filename);
				exit(ERROR_EXIT);
			}
			/* Make sure we opened a normal file. */
			if (fstat(fileno(NewCrontab), &statbuf) < 0) {
				perror("fstat");
				exit(ERROR_EXIT);
			}
			if (!S_ISREG(statbuf.st_mode)) {
				fprintf(stderr, "%s: Not a regular file.\n", Filename);
				exit(ERROR_EXIT);
			}
			if (swap_uids_back() < OK) {
				perror("swapping uids back");
				exit(ERROR_EXIT);
			}
		}
	}

	Debug(DMISC, ("user=%s, file=%s, option=%s\n",
		      User, Filename, Options[(int)Option]))
}


static void
list_cmd() {
	char	n[MAX_FNAME];
	FILE	*f;
	int	ch;
#ifdef DEBIAN
	int     x;
	char    *ctnh;
#endif

	log_it(RealUser, Pid, "LIST", User);
	(void) snprintf(n, MAX_FNAME, CRON_TAB(User));
	if (!(f = fopen(n, "r"))) {
		if (errno == ENOENT)
			fprintf(stderr, "no crontab for %s\n", User);
		else
			perror(n);
		exit(ERROR_EXIT);
	}

	/* file is open. copy to stdout, close.
	 */
	Set_LineNum(1)
#ifdef DEBIAN
	  /* DEBIAN: Don't list header lines unless CRONTAB_NOHEADER is
	     'N'. */
	  /* ignore the top few comments since we probably put them there.
	   */
	  if (!(ctnh = getenv("CRONTAB_NOHEADER")) ||
	      toupper(*ctnh) != 'N') 
	    {
	    for (x = 0;  x < NHEADER_LINES;  x++) {
	      ch = get_char(f);
	      if (EOF == ch)
		break;
	      if ('#' != ch) {
		putchar(ch);
		break;
	      }
	      while (EOF != (ch = get_char(f)))
		if (ch == '\n')
		  break;
	      if (EOF == ch)
		break;
	    }
	  }
#endif
	while (EOF != (ch = get_char(f)))
		putchar(ch);
	fclose(f);
}


static void
delete_cmd() {
	char	n[MAX_FNAME];

	log_it(RealUser, Pid, "DELETE", User);
	(void) snprintf(n, MAX_FNAME, CRON_TAB(User));
	if (unlink(n)) {
		if (errno == ENOENT)
			fprintf(stderr, "no crontab for %s\n", User);
		else
			perror(n);
		exit(ERROR_EXIT);
	}
	poke_daemon();
}


static void
check_error(msg)
	char	*msg;
{
	CheckErrorCount++;
	fprintf(stderr, "\"%s\":%d: %s\n", Filename, LineNumber-1, msg);
}


static void
edit_cmd() {
	char		n[MAX_FNAME], q[MAX_TEMPSTR], *editor;
	FILE		*f;
	int		ch, t, x;
	struct stat	statbuf, fsbuf;
	time_t		mtime;
	WAIT_T		waiter;
	PID_T		pid, xpid;
	mode_t		um;

	log_it(RealUser, Pid, "BEGIN EDIT", User);
	(void) snprintf(n, MAX_FNAME, CRON_TAB(User));
	if (!(f = fopen(n, "r"))) {
		if (errno != ENOENT) {
			perror(n);
			exit(ERROR_EXIT);
		}
		fprintf(stderr, "no crontab for %s - using an empty one\n",
			User);
		if (!(f = fopen("/dev/null", "r"))) {
			perror("/dev/null");
			exit(ERROR_EXIT);
		}
	}

	um = umask(077);
#if 0
	/* The support for TMPDIR is temporarily removed, because of
	   interactions with emacs */
	if (getenv("TMPDIR")) {
	  strcpy(Filename, getenv("TMPDIR"));
	} else {
	  strcpy(Filename,"/tmp");
	}
#else
	  strcpy(Filename,"/tmp");
#endif
	 
	(void) sprintf(Filename+strlen(Filename), "/crontab.XXXXXXXXXX");
	if ((t = mkstemp(Filename)) == -1) {
		perror(Filename);
		(void) umask(um);
		goto fatal;
	}
	(void) umask(um);
#ifdef HAS_FCHOWN
	if (fchown(t, getuid(), getgid()) < 0) {
#else
	if (chown(Filename, getuid(), getgid()) < 0) {
#endif
		perror("fchown");
		goto fatal;
	}
	if (!(NewCrontab = fdopen(t, "r+"))) {
		perror("fdopen");
		goto fatal;
	}

	Set_LineNum(1)

	/* ignore the top few comments since we probably put them there.
	 */
	for (x = 0;  x < NHEADER_LINES;  x++) {
		ch = get_char(f);
		if (EOF == ch)
			break;
		if ('#' != ch) {
			putc(ch, NewCrontab);
			break;
		}
		while (EOF != (ch = get_char(f)))
			if (ch == '\n')
				break;
		if (EOF == ch)
			break;
	}

	/* copy the rest of the crontab (if any) to the temp file.
	 */
	if (EOF != ch)
		while (EOF != (ch = get_char(f)))
			putc(ch, NewCrontab);
	fclose(f);
	if (fflush(NewCrontab) < OK) {
		perror(Filename);
		exit(ERROR_EXIT);
	}
	if (fstat(t, &fsbuf) < 0) {
		perror("unable to fstat temp file");
		goto fatal;
	}
 again:
	rewind(NewCrontab);
	if (ferror(NewCrontab)) {
		fprintf(stderr, "%s: error while writing new crontab to %s\n",
			ProgramName, Filename);
 fatal:		unlink(Filename);
		exit(ERROR_EXIT);
	}
	if (fstat(t, &statbuf) < 0) {
		perror("fstat");
		goto fatal;
	}
	if (statbuf.st_dev != fsbuf.st_dev || statbuf.st_ino != fsbuf.st_ino) {
		fprintf(stderr, "temp file must be edited in place\n");
		exit(ERROR_EXIT);
	}

	mtime = statbuf.st_mtime;

	if ((!(editor = getenv("VISUAL")))
	 && (!(editor = getenv("EDITOR")))
	    ) {
		editor = EDITOR;
	}

	/* we still have the file open.  editors will generally rewrite the
	 * original file rather than renaming/unlinking it and starting a
	 * new one; even backup files are supposed to be made by copying
	 * rather than by renaming.  if some editor does not support this,
	 * then don't use it.  the security problems are more severe if we
	 * close and reopen the file around the edit.
	 */

	/* Turn off signals. */
	(void)signal(SIGHUP, SIG_IGN);
	(void)signal(SIGINT, SIG_IGN);
	(void)signal(SIGQUIT, SIG_IGN);
	switch (pid = fork()) {
	case -1:
		perror("fork");
		goto fatal;
	case 0:
		/* child */
		if (setuid(getuid()) < 0) {
			perror("setuid(getuid())");
			exit(ERROR_EXIT);
		}
		if (chdir("/tmp") < 0) {
			perror("chdir(/tmp)");
			exit(ERROR_EXIT);
		}
		if (strlen(editor) + strlen(Filename) + 2 >= MAX_TEMPSTR) {
			fprintf(stderr, "%s: editor or filename too long\n",
				ProgramName);
			exit(ERROR_EXIT);
		}
		snprintf(q, MAX_TEMPSTR, "%s %s", editor, Filename);
		execlp(_PATH_BSHELL, _PATH_BSHELL, "-c", q, NULL);
		perror(editor);
		exit(ERROR_EXIT);
		/*NOTREACHED*/
	default:
		/* parent */
		break;
	}

	/* parent */
	while (1) {
		xpid = waitpid(pid, &waiter, WUNTRACED);
		if (xpid == -1) {
			fprintf(stderr, "%s: waitpid() failed waiting for PID %d from \"%s\": %s\n",
				ProgramName, pid, editor, strerror(errno));
		} else if (xpid != pid) {
			fprintf(stderr, "%s: wrong PID (%d != %d) from \"%s\"\n",
				ProgramName, xpid, pid, editor);
			goto fatal;
		} else if (WIFSTOPPED(waiter)) {
		        /* raise(WSTOPSIG(waiter)); Not needed and breaks in job control shell*/
		} else if (WIFEXITED(waiter) && WEXITSTATUS(waiter)) {
			fprintf(stderr, "%s: \"%s\" exited with status %d\n",
				ProgramName, editor, WEXITSTATUS(waiter));
			goto fatal;
		} else if (WIFSIGNALED(waiter)) {
			fprintf(stderr,
				"%s: \"%s\" killed; signal %d (%score dumped)\n",
				ProgramName, editor, WTERMSIG(waiter),
				WCOREDUMP(waiter) ?"" :"no ");
			goto fatal;
		} else
			break;
	}
	(void)signal(SIGHUP, SIG_DFL);
	(void)signal(SIGINT, SIG_DFL);
	(void)signal(SIGQUIT, SIG_DFL);
	(void)signal(SIGTSTP, SIG_DFL);
	if (statbuf.st_dev != fsbuf.st_dev || statbuf.st_ino != fsbuf.st_ino) {
		fprintf(stderr, "temp file must be edited in place\n");
		exit(ERROR_EXIT);
	}
	if (fstat(t, &statbuf) < 0) {
		perror("fstat");
		goto fatal;
	}
	if (mtime == statbuf.st_mtime) {
		fprintf(stderr, "%s: no changes made to crontab\n",
			ProgramName);
		goto remove;
	}
	fprintf(stderr, "%s: installing new crontab\n", ProgramName);
	switch (replace_cmd()) {
	case 0:
		break;
	case -1:
		for (;;) {
			printf("Do you want to retry the same edit? ");
			fflush(stdout);
			q[0] = '\0';
			(void) fgets(q, sizeof q, stdin);
			switch (islower(q[0]) ? q[0] : tolower(q[0])) {
			case 'y':
				goto again;
			case 'n':
				goto abandon;
			default:
				fprintf(stderr, "Enter Y or N\n");
			}
		}
		/*NOTREACHED*/
	case -2:
	abandon:
		fprintf(stderr, "%s: edits left in %s\n",
			ProgramName, Filename);
		goto done;
	default:
		fprintf(stderr, "%s: panic: bad switch() in replace_cmd()\n",
		    ProgramName);
		goto fatal;
	}
 remove:
	unlink(Filename);
 done:
	log_it(RealUser, Pid, "END EDIT", User);
}

static char tn[MAX_FNAME];

static void sig_handler(int x)
{
	unlink(tn);
	exit(1);
}	

/* returns	0	on success
 *		-1	on syntax error
 *		-2	on install error
 */
static int
replace_cmd() {
	char	n[MAX_FNAME], envstr[MAX_ENVSTR];
	FILE	*tmp;
	int	ch, eof, fd;
	entry	*e;
	time_t	now = time(NULL);
	char	**envp = env_init();
	mode_t	um;
	int	saved_uid;

	if (envp == NULL) {
		fprintf(stderr, "%s: Cannot allocate memory.\n", ProgramName);
		return (-2);
	}

	/* Assume privilege.  This way we can only receive signals on our
	   input - the ones listed below (or from root - root's problem, not
	   ours). */
	saved_uid = getuid();
	if (setuid(geteuid()) < 0) {
		perror("setuid");
		return -2;
	}

	/* Assumes Linux-style signal handlers (takes int, returns void) */
	/* Signal handlers, to ensure we do not leave temp files in the
	   spool dir.  We don't remove these on exiting this function;
	   but that's OK, we exit immediately afterwards anyway. */
	signal(SIGHUP, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGTSTP, SIG_IGN);

	(void) snprintf(tn, MAX_FNAME, CRON_TAB("tmp.XXXXXX"));
	um = umask(077);
	fd = mkstemp(tn);
	if (!fd) {
		perror(tn);
		return(-2);
	}
	tmp = fdopen(fd, "w+");
	if (!tmp) {
		perror(tn);
		return (-2);
	}
	(void) umask(um);

	/* write a signature at the top of the file.
	 *
	 * VERY IMPORTANT: make sure NHEADER_LINES agrees with this code.
	 */
	fprintf(tmp, "# DO NOT EDIT THIS FILE - edit the master and reinstall.\n");
	fprintf(tmp, "# (%s installed on %-24.24s)\n", Filename, ctime(&now));
	fprintf(tmp, "# (Cron version -- %s)\n", rcsid);

	/* copy the crontab to the tmp
	 */
	rewind(NewCrontab);
	Set_LineNum(1)
	while (EOF != (ch = get_char(NewCrontab)))
		putc(ch, tmp);

	if (ferror(tmp) || fflush(tmp) || fsync(fd)) {
		fprintf(stderr, "%s: error while writing new crontab to %s\n",
			ProgramName, tn);
		perror("Error");
		fclose(tmp);  unlink(tn);
		return (-2);
	}

	/* check the syntax of the file being installed.
	 */
	rewind(tmp);
	/* BUG: was reporting errors after the EOF if there were any errors
	 * in the file proper -- kludged it by stopping after first error.
	 *		vix 31mar87
	 */
	Set_LineNum(1 - NHEADER_LINES)
	CheckErrorCount = 0;  eof = FALSE;
	while (!CheckErrorCount && !eof) {
		switch (load_env(envstr, tmp)) {
		case ERR:
			eof = TRUE;
			break;
		case FALSE:
			e = load_entry(tmp, check_error, pw, envp);
			if (e)
				free(e);
			break;
		case TRUE:
			break;
		}
	}

	if (CheckErrorCount != 0) {
		fprintf(stderr, "errors in crontab file, can't install.\n");
		fclose(tmp);  unlink(tn);
		/* Give up privilege, in case we loop. */
		if (setreuid(saved_uid, -1) < 0)
			return (-2);
		return (-1);
	}

#ifdef HAS_FCHOWN
	if (fchown(fileno(tmp), ROOT_UID, -1) < OK)
#else
	if (chown(tn, ROOT_UID, -1) < OK)
#endif
	{
		perror("chown");
		fclose(tmp);  unlink(tn);
		return (-2);
	}

#ifdef HAS_FCHMOD
	if (fchmod(fileno(tmp), 0600) < OK)
#else
	if (chmod(tn, 0600) < OK)
#endif
	{
		perror("chown");
		fclose(tmp);  unlink(tn);
		return (-2);
	}

	if (fclose(tmp) == EOF) {
		perror("fclose");
		unlink(tn);
		return (-2);
	}

	(void) snprintf(n, sizeof(n), CRON_TAB(User));
	if (rename(tn, n)) {
		fprintf(stderr, "%s: error renaming %s to %s\n",
			ProgramName, tn, n);
		perror("rename");
		unlink(tn);
		return (-2);
	}
	log_it(RealUser, Pid, "REPLACE", User);

	poke_daemon();

	/* Give up privilege, just in case. */
	/* Don't need to check for error; nothing happens beyond here but a log entry,
	   and the failure message is incorrect after the rename above. */
	setreuid(saved_uid, -1);

	return (0);
}


static void
poke_daemon() {
#ifdef USE_UTIMES
	struct timeval tvs[2];
	struct timezone tz;

	(void) gettimeofday(&tvs[0], &tz);
	tvs[1] = tvs[0];
	if (utimes(SPOOL_DIR, tvs) < OK) {
		fprintf(stderr, "crontab: can't update mtime on spooldir\n");
		perror(SPOOL_DIR);
		return;
	}
#else
	if (utime(SPOOL_DIR, NULL) < OK) {
		fprintf(stderr, "crontab: can't update mtime on spooldir\n");
		perror(SPOOL_DIR);
		return;
	}
#endif /*USE_UTIMES*/
}
