/* A function to determine if a particular line is in /etc/securietty*/


#define SECURETTY_FILE "/etc/securetty"
#define TTY_PREFIX     "/dev/"

/* This function taken out of pam_security by Sam Hartman <hartmans@debian.org>*/
/*
 * by Elliot Lee <sopwith@redhat.com>, Red Hat Software.
 * July 25, 1996.
 * Slight modifications AGM. 1996/12/3
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <security/pam_modules.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/syslog.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <security/_pam_modutil.h>

static void _pam_log(int err,  const char *format,...)
{
	va_list args;
	const char tag[] = "(pam_securetty) ";
	char *mod_format;
	int free_mod_format = 1;

	mod_format = malloc( 1 + sizeof(tag) + strlen(format));
	if(mod_format == NULL) {
	  free_mod_format = 0;
	  mod_format = (char *) format;
	} else {
	  strcpy(mod_format, tag);
	  strcat( mod_format, format);
	}

	va_start(args, format);
	vsyslog(err | LOG_AUTH, mod_format, args);
	va_end(args);

	if (free_mod_format) free(mod_format);
}


int _pammodutil_tty_secure(const char *uttyname)
{
    int retval = PAM_AUTH_ERR;
    char ttyfileline[256];
    struct stat ttyfileinfo;
    FILE *ttyfile;
    /* The PAM_TTY item may be prefixed with "/dev/" - skip that */
    if (strncmp(TTY_PREFIX, uttyname, sizeof(TTY_PREFIX)-1) == 0)
	uttyname += sizeof(TTY_PREFIX)-1;

    if (stat(SECURETTY_FILE, &ttyfileinfo)) {
	_pam_log(LOG_NOTICE, "Couldn't open " SECURETTY_FILE);
	return PAM_SUCCESS; /* for compatibility with old securetty handling,
			       this needs to succeed.  But we still log the
			       error. */
    }

    if ((ttyfileinfo.st_mode & S_IWOTH)
	|| !S_ISREG(ttyfileinfo.st_mode)) {
	/* If the file is world writable or is not a
	   normal file, return error */
	_pam_log(LOG_ERR, SECURETTY_FILE
		 " is either world writable or not a normal file");
	return PAM_AUTH_ERR;
    }

    ttyfile = fopen(SECURETTY_FILE,"r");
    if(ttyfile == NULL) { /* Check that we opened it successfully */
	_pam_log(LOG_ERR,
		 "Error opening " SECURETTY_FILE);
	return PAM_SERVICE_ERR;
    }
    /* There should be no more errors from here on */
    retval=PAM_AUTH_ERR;
    /* This loop assumes that PAM_SUCCESS == 0
       and PAM_AUTH_ERR != 0 */
    while((fgets(ttyfileline,sizeof(ttyfileline)-1, ttyfile) != NULL) 
	  && retval) {
	if(ttyfileline[strlen(ttyfileline) - 1] == '\n')
	    ttyfileline[strlen(ttyfileline) - 1] = '\0';
	retval = strcmp(ttyfileline,uttyname);
    }
    fclose(ttyfile);
    if(retval) {
      _pam_log(LOG_WARNING, "access denied: tty '%s' is not secure !",
		     uttyname);
	retval = PAM_AUTH_ERR;
    }
    return retval;
}
