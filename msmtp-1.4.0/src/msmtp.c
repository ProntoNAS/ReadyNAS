/*
 * msmtp.c
 *
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2000, 2003, 2004, 2005
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   msmtp is released under the GPL with the additional exemption that
 *   compiling, linking, and/or using OpenSSL is allowed.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
extern int errno;
#include <time.h>
#include <getopt.h>
extern char *optarg;
extern int optind;
#include <sysexits.h>
#ifndef EX_OK
#define EX_OK 0
#endif
#ifdef HAVE_SYSLOG
#include <syslog.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef __MINGW32__
#include <windows.h>
#include <lmcons.h>
#include <winsock2.h>
#elif defined DJGPP
#include <netdb.h>
#include <arpa/inet.h>
#else /* UNIX */
#include <pwd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "xalloc.h"

#include "list.h"
#include "paths.h"
#include "conf.h"
#include "net.h"
#include "netrc.h"
#include "smtp.h"
#ifdef HAVE_SSL
#include "tls.h"
#endif /* HAVE_SSL */

/* Default file names. */
#ifdef __MINGW32__
#define SYSCONFFILE	"msmtprc.txt"
#define USERCONFFILE	"msmtprc.txt"
#elif defined (DJGPP)
#define SYSCONFFILE	"msmtprc"
#define USERCONFFILE	"_msmtprc"
#define NETRCFILE	"_netrc"
#else /* UNIX */
#define SYSCONFFILE	"msmtprc"
#define USERCONFFILE	".msmtprc"
#define NETRCFILE	".netrc"
#endif

/* The size of all error message buffers */
#define ERRSTR_BUFSIZE 512
const size_t errstr_bufsize = ERRSTR_BUFSIZE;
char errstr[ERRSTR_BUFSIZE];

/* The name of this program */
char *prgname;


/* 
 * Die if memory allocation fails
 */

void xalloc_die(void)
{
    fprintf(stderr, "%s: FATAL: %s", prgname, strerror(errno));
    exit(EX_OSERR);
}


/*
 * Translate error codes from net.h, tls.h or smtp.h 
 * to error codes from sysexits.h
 */

int exitcode_net(int net_error_code)
{
    switch (net_error_code)
    {
	case NET_EHOSTNOTFOUND:
	    return EX_NOHOST;

	case NET_ESOCKET:
	    return EX_OSERR;

	case NET_ECONNECT:
	    return EX_TEMPFAIL;	    

	case NET_EIO:
	    return EX_IOERR;
	    
	case NET_ELIBFAILED:
	default:
	    return EX_SOFTWARE;
    }
}

#ifdef HAVE_SSL
int exitcode_tls(int tls_error_code)
{
    switch (tls_error_code)
    {
	case TLS_EIO:
	    return EX_IOERR;

	case TLS_EFILE:
	    return EX_NOINPUT;

	case TLS_EHANDSHAKE:
	    return EX_PROTOCOL;

	case TLS_ECERT:
	    /* did not find anything better... */
	    return EX_UNAVAILABLE;

	case TLS_ELIBFAILED:
	case TLS_ESEED:
	default:
	    return EX_SOFTWARE;
    }
}
#endif /* HAVE_SSL */

int exitcode_smtp(int smtp_error_code)
{
    switch (smtp_error_code)
    {
	case SMTP_EIO:
	    return EX_IOERR;

	case SMTP_EPROTO:
	    return EX_PROTOCOL;

	case SMTP_EINVAL:
	    return EX_DATAERR;
	    
	case SMTP_EAUTHFAIL:
	    return EX_NOPERM;

	case SMTP_EINSECURE:
	case SMTP_EUNAVAIL:
	    return EX_UNAVAILABLE;

	case SMTP_ELIBFAILED:
	default:
	    return EX_SOFTWARE;
    }
}


/* 
 * Return the name of a sysexits.h exitcode
 */
const char *exitcode_to_string(int exitcode)
{
    switch (exitcode)
    {
	case EX_OK:
	    return "EX_OK";

	case EX_USAGE:
	    return "EX_USAGE";
	    
	case EX_DATAERR:
	    return "EX_DATAERR";

	case EX_NOINPUT:
	    return "EX_NOINPUT";

	case EX_NOUSER:
	    return "EX_NOUSER";

	case EX_NOHOST:
	    return "EX_NOHOST";

	case EX_UNAVAILABLE:
	    return "EX_UNAVAILABLE";

	case EX_SOFTWARE:
	    return "EX_SOFTWARE";

	case EX_OSERR:
	    return "EX_OSERR";

	case EX_OSFILE:
	    return "EX_OSFILE";
	    
	case EX_CANTCREAT:
	    return "EX_CANTCREAT";
	    
	case EX_IOERR:
	    return "EX_IOERR";

	case EX_TEMPFAIL:
	    return "EX_TEMPFAIL";
	    
	case EX_PROTOCOL:
	    return "EX_PROTOCOL";

	case EX_NOPERM:
	    return "EX_NOPERM";

	case EX_CONFIG:
	    return "EX_CONFIG";

	default:
	    return "BUG:UNKNOWN";
    }
}


/* 
 * msmtp_sanitize_string()
 *
 * Replaces all control characters in the string with a question mark
 */

char *msmtp_sanitize_string(char *str)
{
    char *p = str;
    
    while (*p != '\0')
    {
	if (iscntrl((unsigned char)*p))
	{
    	    *p = '?';
	}
	p++;
    }

    return str;
}


/* 
 * msmtp_password_callback()
 *
 * This function will be called by smtp_auth() to get a password if none was
 * given. It tries to read a password from .netrc, and if that fails reads a 
 * password with getpass().
 * It must return NULL on failure or a password in an allocated buffer.
 */

char *msmtp_password_callback(const char *hostname, const char *user)
{
#ifdef __MINGW32__
    /* Windows has neither a .netrc nor a getpass() substitute ... */
    return NULL;
#else
	
    char errstr[ERRSTR_BUFSIZE];
    char *netrc_filename;
    netrc_entry *netrc_hostlist;
    netrc_entry *netrc_host;
    char *prompt;
    size_t prompt_len;
    char *gpw;
    char *password = NULL;

    
    if (get_default_user_conffile(NETRCFILE, &netrc_filename, errstr)
	    == PATHS_EOK)
    {
	if ((netrc_hostlist = parse_netrc(netrc_filename)))
	{
	    if ((netrc_host = search_netrc(netrc_hostlist, hostname, user)))
	    {
		password = xstrdup(netrc_host->password);
	    }
	    free_netrc_entry_list(netrc_hostlist);
	}
	free(netrc_filename);
    }
    
    if (!password)
    {
	prompt_len = 13 + strlen(user) + 4 + strlen(hostname) + 2;
	prompt = xmalloc((prompt_len + 1) * sizeof(char));
	snprintf(prompt, prompt_len + 1, "password for %s at %s: ", 
		user, hostname);
	gpw = getpass(prompt);
	free(prompt);
	if (gpw)
	{
	    password = xstrdup(gpw);
	    memset(gpw, 0, strlen(gpw));
	}
    }
    
    return password;

#endif
}


/*
 * msmtp_print_tls_cert_info()
 *
 * Prints information about a TLS certificate.
 */

#ifdef HAVE_SSL
void msmtp_print_tls_cert_info(tls_cert_info_t *tci)
{
    const char *info_fieldname[6] = { "Common Name", "Organization", 
	"Organizational unit", "Locality", "State or Province", "Country" };
    char hex[] = "0123456789ABCDEF";
    char sha1_fingerprint_string[60];
    char md5_fingerprint_string[48];
    char *tmp;
    int i;
    
    for (i = 0; i < 20; i++)
    {
	sha1_fingerprint_string[3 * i] = 
	    hex[(tci->sha1_fingerprint[i] & 0xf0) >> 4];
	sha1_fingerprint_string[3 * i + 1] = 
	    hex[tci->sha1_fingerprint[i] & 0x0f];
	sha1_fingerprint_string[3 * i + 2] = ':';
    }
    sha1_fingerprint_string[59] = '\0';
    for (i = 0; i < 16; i++)
    {
	md5_fingerprint_string[3 * i] = 
	    hex[(tci->md5_fingerprint[i] & 0xf0) >> 4];
	md5_fingerprint_string[3 * i + 1] = 
	    hex[tci->md5_fingerprint[i] & 0x0f];
	md5_fingerprint_string[3 * i + 2] = ':';
    }
    md5_fingerprint_string[47] = '\0';
    
    printf("TLS certificate information:\n");
    printf("    Owner:\n");
    for (i = 0; i < 6; i++)
    {
	if (tci->owner_info[i])
	{
	    tmp = xstrdup(tci->owner_info[i]);
	    printf("        %s: %s\n", info_fieldname[i], 
		    msmtp_sanitize_string(tmp));
	    free(tmp);
	}
    }
    printf("    Issuer:\n");
    for (i = 0; i < 6; i++)
    {
	if (tci->issuer_info[i])
	{
	    tmp = xstrdup(tci->issuer_info[i]);
	    printf("        %s: %s\n", info_fieldname[i], 
		    msmtp_sanitize_string(tmp));
	    free(tmp);
	}
    }
    printf("    Validity:\n");
    printf("        Activation time: %s", ctime(&tci->activation_time));
    printf("        Expiration time: %s", ctime(&tci->expiration_time));
    printf("    Fingerprints:\n");
    printf("        SHA1: %s\n", sha1_fingerprint_string);
    printf("        MD5:  %s\n", md5_fingerprint_string);
}
#endif


/*
 * msmtp_endsession()
 *
 * Quit an SMTP session and close the connection.
 * QUIT is only sent when the flag 'quit' is set.
 */

void msmtp_endsession(smtp_server_t *srv, int quit)
{
    char tmp_errstr[ERRSTR_BUFSIZE];
    
    if (quit)
    {
	(void)smtp_quit(srv, tmp_errstr);
    }
    smtp_close(srv);
}


/*
 * msmtp_rmqs()
 *
 * Sends an ETRN request to the SMTP server specified in the account 'acc'.
 * 'errstr' must be 'errstr_bufsize' characters long.
 * If an error occured, 'errstr' may contain a descriptive message (or an empty
 * string) and 'msg' may contain the offending message from the SMTP server (or
 * NULL).
 */

int msmtp_rmqs(account_t *acc, int debug, const char *rmqs_argument, 
	list_t **msg, char *errstr)
{
    smtp_server_t srv;
    int e;
#ifdef HAVE_SSL
    tls_cert_info_t *tci = NULL;
#endif /* HAVE_SSL */
    
    errstr[0] = '\0';
    *msg = NULL;
    
    /* create a new smtp_server_t */
    srv = smtp_new(debug ? stdout : NULL, acc->protocol);

    /* prepare tls */
#ifdef HAVE_SSL
    if (acc->tls)
    {
	if ((e = smtp_tls_init(&srv, acc->tls_key_file, acc->tls_cert_file, 
			acc->tls_trust_file, errstr)) != TLS_EOK)
	{
	    return exitcode_tls(e);
	}
    }
#endif /* HAVE_SSL */

    /* connect */
    if ((e = smtp_connect(&srv, acc->host, acc->port, acc->connect_timeout,
		    errstr)) != NET_EOK)
    {
	return exitcode_net(e);
    }

    /* start tls for ssmtp servers */
#ifdef HAVE_SSL
    if (acc->tls && acc->tls_nostarttls)
    {
	if (debug)
	{
	    tci = tls_cert_info_new();
	}
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    if (debug)
	    {
		tls_cert_info_free(tci);
	    }
	    msmtp_endsession(&srv, 0);
	    return exitcode_tls(e);
	}
	if (debug)
	{
	    msmtp_print_tls_cert_info(tci);
	    tls_cert_info_free(tci);
	}
    }
#endif /* HAVE_SSL */

    /* get greeting */
    if ((e = smtp_get_greeting(&srv, msg, NULL, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	return exitcode_smtp(e);
    }
		    
    /* initialize session */
    if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	return exitcode_smtp(e);
    }
    
    /* start tls for starttls servers */
#ifdef HAVE_SSL
    if (acc->tls && !acc->tls_nostarttls)
    {
	if (!(srv.cap.flags & SMTP_CAP_STARTTLS))
	{
	    snprintf(errstr, errstr_bufsize,
		    "the server does not support TLS "
		    "via the STARTTLS command");
	    msmtp_endsession(&srv, 1);
	    return EX_UNAVAILABLE;
	}
	if ((e = smtp_tls_starttls(&srv, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    return exitcode_smtp(e);
	}
	if (debug)
	{
	    tci = tls_cert_info_new();
	}
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    if (debug)
	    {
		tls_cert_info_free(tci);
	    }
	    msmtp_endsession(&srv, 0);
	    return exitcode_tls(e);
	}
	if (debug)
	{
	    msmtp_print_tls_cert_info(tci);
	    tls_cert_info_free(tci);
	}
	/* initialize again */
	if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    return exitcode_smtp(e);
	}
    }
#endif /* HAVE_SSL */

    if (!(srv.cap.flags & SMTP_CAP_ETRN))
    {
	snprintf(errstr, errstr_bufsize,
		"the server does not support "
		"Remote Message Queue Starting");
	msmtp_endsession(&srv, 1);
	return EX_UNAVAILABLE;
    }

    /* authenticate */
    if (acc->auth_mech)
    {
	if (!(srv.cap.flags & SMTP_CAP_AUTH))
	{
	    snprintf(errstr, errstr_bufsize, 
		    "the server does not support authentication");
	    msmtp_endsession(&srv, 1);
	    return EX_UNAVAILABLE;
	}
	if ((e = smtp_auth(&srv, acc->host, acc->username, acc->password, 
			acc->ntlmdomain, acc->auth_mech, 
			msmtp_password_callback, msg, errstr)) 
		!= SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    return exitcode_smtp(e);
	}
    }
    
    /* send the ETRN request */
    if ((e = smtp_etrn(&srv, rmqs_argument, msg, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	return exitcode_smtp(e);
    }

    /* end session */
    msmtp_endsession(&srv, 1);
    return EX_OK;
}


/*
 * msmtp_serverinfo()
 *
 * Prints information about the SMTP server specified in the account 'acc'.
 * 'errstr' must be 'errstr_bufsize' characters long.
 * If an error occured, 'errstr' may contain a descriptive message (or an empty
 * string) and 'msg' may contain the offending message from the SMTP server (or
 * NULL).
 */

int msmtp_serverinfo(account_t *acc, int debug, list_t **msg, char *errstr)
{
    smtp_server_t srv;
    char *server_greeting = NULL;
    int e;
#ifdef HAVE_SSL
    tls_cert_info_t *tci = NULL;
#endif /* HAVE_SSL */
    
    errstr[0] = '\0';
    *msg = NULL;
    
    /* create a new smtp_server_t */
    srv = smtp_new(debug ? stdout : NULL, acc->protocol);

    /* prepare tls */
#ifdef HAVE_SSL
    if (acc->tls)
    {
	tci = tls_cert_info_new();
	if ((e = smtp_tls_init(&srv, acc->tls_key_file, acc->tls_cert_file, 
			acc->tls_trust_file, errstr)) != TLS_EOK)
	{
	    e = exitcode_tls(e);
	    goto error_exit;
	}
    }
#endif /* HAVE_SSL */

    /* connect */
    if ((e = smtp_connect(&srv, acc->host, acc->port, acc->connect_timeout,
		    errstr)) != NET_EOK)
    {
	e = exitcode_net(e);
       	goto error_exit;
    }

    /* start tls for ssmtp servers */
#ifdef HAVE_SSL
    if (acc->tls && acc->tls_nostarttls)
    {
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_tls(e);
	    goto error_exit;
	}
    }
#endif /* HAVE_SSL */

    /* get greeting */
    if ((e = smtp_get_greeting(&srv, msg, &server_greeting, 
		    errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
       	goto error_exit;
    }
		    
    /* initialize session */
    if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
       	goto error_exit;
    }
    
    /* start tls for starttls servers */
#ifdef HAVE_SSL
    if (acc->tls && !acc->tls_nostarttls)
    {
	if (!(srv.cap.flags & SMTP_CAP_STARTTLS))
	{
	    snprintf(errstr, errstr_bufsize, 
		    "the server does not support TLS "
		    "via the STARTTLS command");
	    msmtp_endsession(&srv, 1);
	    e = EX_UNAVAILABLE;
	    goto error_exit;
	}
	if ((e = smtp_tls_starttls(&srv, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_tls(e);
	    goto error_exit;
	}
	/* initialize again */
	if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
    }
#endif /* HAVE_SSL */

    /* end session */
    msmtp_endsession(&srv, 1);

    /* print results */
    printf("%cMTP server at %s, port %d:\n", 
	    (acc->protocol == SMTP_PROTO_SMTP ? 'S' : 'L'), 
	    acc->host, acc->port);
    if (*server_greeting != '\0')
    {
	printf("    %s\n", msmtp_sanitize_string(server_greeting));
    }
#ifdef HAVE_SSL
    if (acc->tls)
    {
	msmtp_print_tls_cert_info(tci);
    }
#endif /* HAVE_SSL */
#ifdef HAVE_SSL
    if (srv.cap.flags == 0 && !(acc->tls && !acc->tls_nostarttls))
#else /* not HAVE_SSL */
    if (srv.cap.flags == 0)
#endif /* not HAVE_SSL */
    {
	printf("No special capabilities.\n");
    }
    else
    {
	printf("Capabilities:\n");
	if (srv.cap.flags & SMTP_CAP_SIZE)
	{
	    printf("    SIZE %ld:\n        Maximum message size is ", 
		    srv.cap.size);
	    if (srv.cap.size == 0)
	    {
		printf("unlimited\n");
	    }
	    else
	    {
		printf("%ld bytes", srv.cap.size);
		if (srv.cap.size > 1024 * 1024)
		{
		    printf(" = %.2f MB", 
			    (float)srv.cap.size / (1024.0 * 1024.0));
		}
		else if (srv.cap.size > 1024)
		{
		    printf(" = %.2f KB", (float)srv.cap.size / 1024.0);
		}
		printf("\n");
	    }
       	}
	if (srv.cap.flags & SMTP_CAP_PIPELINING)
	{
    	    printf("    PIPELINING:\n        Support for command grouping "
		    "for faster transmission\n");
       	}
	if (srv.cap.flags & SMTP_CAP_ETRN)
	{
    	    printf("    ETRN:\n        Support for RMQS "
		    "(Remote Message Queue Starting)\n");
       	}
	if (srv.cap.flags & SMTP_CAP_DSN)
	{
    	    printf("    DSN:\n        Support for "
		    "Delivery Status Notifications\n");
       	}
#ifdef HAVE_SSL
	if ((acc->tls && !acc->tls_nostarttls) 
		|| (srv.cap.flags & SMTP_CAP_STARTTLS))
#else /* not HAVE_SSL */
        if (srv.cap.flags & SMTP_CAP_STARTTLS)
#endif /* not HAVE_SSL */
	{
    	    printf("    STARTTLS:\n        Support for TLS encryption "
		    "via the STARTTLS command\n");
       	}
	if (srv.cap.flags & SMTP_CAP_AUTH)
	{
    	    printf("    AUTH:\n        Supported authentication methods:\n"
		    "        ");
    	    if (srv.cap.flags & SMTP_CAP_AUTH_PLAIN)
    	    {
    		printf("PLAIN ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_CRAM_MD5)
    	    {
    		printf("CRAM-MD5 ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_DIGEST_MD5)
    	    {
    		printf("DIGEST-MD5 ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_GSSAPI)
    	    {
    		printf("GSSAPI ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_EXTERNAL)
    	    {
    		printf("EXTERNAL ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_LOGIN)
    	    {
    		printf("LOGIN ");
    	    }
    	    if (srv.cap.flags & SMTP_CAP_AUTH_NTLM)
    	    {
    		printf("NTLM ");
    	    }
	    printf("\n");
       	}
#ifdef HAVE_SSL
	if ((srv.cap.flags & SMTP_CAP_STARTTLS) && !acc->tls)
#else /* not HAVE_SSL */
	if (srv.cap.flags & SMTP_CAP_STARTTLS)
#endif /* not HAVE_SSL */
	{
    	    printf("This server might advertise more or other "
		    "capabilities when TLS is active.\n");
       	}
    }

    e = EX_OK;
    
error_exit:
#ifdef HAVE_SSL
    if (tci)
    {
	tls_cert_info_free(tci);
    }
#endif /* HAVE_SSL */
    free(server_greeting);
    return e;
}


/*
 * msmtp_read_recipients()
 *
 * Copies the headers of the mail from 'mailf' to a temporary file '*tmpfile',
 * including the blank line that separates the header from the body of the mail.
 * Extracts all recipients from the To, Cc, and Bcc headers and adds them
 * to 'recipients'.
 * This function rewinds '*tmpfile' after writing the headers to it.
 * 
 * See RFC2822, section 3 for the format of these headers.
 * 
 * Return codes: EX_OK, EX_IOERR
 */

#define STATE_LINESTART_FRESH		0	/* a new line started; the 
						   previous line was not a 
						   recipient header */
#define STATE_LINESTART_AFTER_RCPTHDR	1	/* a new line started; the 
						   previous line was a 
						   recipient header */
#define STATE_OTHER_HDR			2	/* a header we don't 
						   care about */
#define STATE_TO			3	/* we saw "^T" */
#define STATE_CC			4	/* we saw "^C" */
#define STATE_BCC1			5	/* we saw "^B" */
#define STATE_BCC2			6	/* we saw "^Bc" */
#define STATE_RCPTHDR_ALMOST		7	/* we saw "^To", "^Cc" 
						   or "^Bcc" */
#define STATE_RCPTHDR_DEFAULT		8	/* in_rcpt_hdr and in_rcpt 
						   state our position */
#define STATE_RCPTHDR_DQUOTE		9	/* duoble quotes */
#define STATE_RCPTHDR_BRACKETS_START	10	/* entering <...> */
#define STATE_RCPTHDR_IN_BRACKETS	11	/* an address inside <> */
#define STATE_RCPTHDR_PARENTH_START	12	/* entering (...) */
#define STATE_RCPTHDR_IN_PARENTH	13	/* a comment inside () */
#define STATE_RCPTHDR_IN_ADDRESS	14	/* a bare address */
#define STATE_RCPTHDR_BACKQUOTE		15	/* we saw a '\\' */
#define STATE_HEADERS_END		16	/* we saw "^$", the blank line 
						   between headers and body */

int msmtp_read_recipients(FILE *mailf, list_t *recipients, FILE **tmpfile, 
	char *errstr)
{
    int c;
    int state = STATE_LINESTART_FRESH;
    int oldstate = STATE_LINESTART_FRESH;
    int backquote_savestate = STATE_LINESTART_FRESH;
    int parentheses_depth = 0;
    int parentheses_savestate = STATE_LINESTART_FRESH;
    int folded_rcpthdr_savestate = STATE_LINESTART_FRESH;
    char *current_recipient = NULL;
    size_t current_recipient_len = 0;
    int forget_current_recipient = 0;
    int finish_current_recipient = 0;
    size_t bufsize = 0;
    /* The buffer that is filled with the current recipient grows by 
     * 'bufsize_step' if the remaining space becomes too small. This value must
     * be at least 2. Wasted characters are at most (bufsize_step - 1). A value 
     * of 10 means low wasted space and a low number of realloc()s per 
     * recipient. */
    const size_t bufsize_step = 10; 
    

    if (tempfile(PACKAGE_NAME, tmpfile, errstr) != PATHS_EOK)
    {
	goto error_exit;
    }
    
    for (;;)
    {
	c = fgetc(mailf);
	/* Convert CRLF to LF. According to RFC 2822, CRs may only occur in a
	 * mail when they are followed by LF, so just ignoring CRs is ok. */
	if (c == '\r')
	{
	    continue;
	}
	oldstate = state;
	if (c == EOF)
	{
	    state = STATE_HEADERS_END;
	    if (current_recipient)
		finish_current_recipient = 1;
	}
	else
	{
	    switch (state)
	    {
		case STATE_LINESTART_FRESH:
		    parentheses_depth = 0;
		    if (c == 't' || c == 'T')
			state = STATE_TO;
		    else if (c == 'c' || c == 'C')
			state = STATE_CC;
		    else if (c == 'b' || c == 'B')
			state = STATE_BCC1;
		    else if (c == '\n')
			state = STATE_HEADERS_END;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_LINESTART_AFTER_RCPTHDR:
		    if (c != ' ' && c != '\t' && current_recipient)
			finish_current_recipient = 1;
		    if (c == ' ' || c == '\t')
			state = folded_rcpthdr_savestate;
		    else if (c == 't' || c == 'T')
			state = STATE_TO;
		    else if (c == 'c' || c == 'C')
			state = STATE_CC;
		    else if (c == 'b' || c == 'B')
			state = STATE_BCC1;
		    else if (c == '\n')
			state = STATE_HEADERS_END;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_OTHER_HDR:
		    if (c == '\n')
			state = STATE_LINESTART_FRESH;
		    break;

		case STATE_TO:
	    	    if (c == 'o' || c == 'O')
	    		state = STATE_RCPTHDR_ALMOST;
	    	    else if (c == '\n')
	    		state = STATE_LINESTART_FRESH;
	    	    else
	    		state = STATE_OTHER_HDR;
	    	    break;

		case STATE_CC:
		    if (c == 'c' || c == 'C')
			state = STATE_RCPTHDR_ALMOST;
		    else if (c == '\n')
			state = STATE_LINESTART_FRESH;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_BCC1:
		    if (c == 'c' || c == 'C')
			state = STATE_BCC2;
		    else if (c == '\n')
			state = STATE_LINESTART_FRESH;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_BCC2:
		    if (c == 'c' || c == 'C')
			state = STATE_RCPTHDR_ALMOST;
		    else if (c == '\n')
			state = STATE_LINESTART_FRESH;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_RCPTHDR_ALMOST:
		    if (c == ':')
			state = STATE_RCPTHDR_DEFAULT;
		    else if (c == '\n')
			state = STATE_LINESTART_FRESH;
		    else
			state = STATE_OTHER_HDR;
		    break;

		case STATE_RCPTHDR_DEFAULT:
	    	    if (c == '\n')
	    	    {
	    		if (current_recipient)
	    		    finish_current_recipient = 1;
	    		folded_rcpthdr_savestate = state;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == '\\')
	    	    {
	    		backquote_savestate = state;
	    		state = STATE_RCPTHDR_BACKQUOTE;
	    	    }
	    	    else if (c == '(')
	    	    {
	    		parentheses_savestate = state;
	    		state = STATE_RCPTHDR_PARENTH_START;
	    	    }
	    	    else if (c == '"')
	    	    {
	    		if (current_recipient)
	    		    forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_DQUOTE;
	    	    }
	    	    else if (c == '<')
	    	    {
	    		if (current_recipient)
	    		    forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_BRACKETS_START;
	    	    }
	    	    else if (c == ' ' || c == '\t')
	    		; /* keep state */
	    	    else if (c == ':')
	    	    {
	    		if (current_recipient)
	    		    forget_current_recipient = 1;
	    	    }
	    	    else if (c == ';' || c == ',')
	    	    {
	    		if (current_recipient)
	    		    finish_current_recipient = 1;
	    	    }
	    	    else
	    	    {
	    		if (current_recipient)
	    		    forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_IN_ADDRESS;
	    	    }
	    	    break;

		case STATE_RCPTHDR_DQUOTE:
	    	    if (c == '\n')
	    	    {
	    		folded_rcpthdr_savestate = state;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == '\\')
	    	    {
	    		backquote_savestate = state;
	    		state = STATE_RCPTHDR_BACKQUOTE;
	    	    }
	    	    else if (c == '"')
	    		state = STATE_RCPTHDR_DEFAULT;
	    	    break;

		case STATE_RCPTHDR_BRACKETS_START:
		    if (c == '\n')
		    {
		       	folded_rcpthdr_savestate = state;
			state = STATE_LINESTART_AFTER_RCPTHDR;
		    }
		    else if (c == '(')
		    {
		       	parentheses_savestate = state;
			state = STATE_RCPTHDR_PARENTH_START;
		    }
		    else if (c == '>')
			state = STATE_RCPTHDR_DEFAULT;
		    else
			state = STATE_RCPTHDR_IN_BRACKETS;
		    break;

		case STATE_RCPTHDR_IN_BRACKETS:
	    	    if (c == '\n')
	    	    {
	    		folded_rcpthdr_savestate = state;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == '\\')
	    	    {
	    		backquote_savestate = state;
	    		state = STATE_RCPTHDR_BACKQUOTE;
	    	    }
	    	    else if (c == '(')
	    	    {
	    		parentheses_savestate = state;
	    		state = STATE_RCPTHDR_PARENTH_START;
	    	    }
	    	    else if (c == '>')
	    	    {
	    		finish_current_recipient = 1;
	    		state = STATE_RCPTHDR_DEFAULT;
	    	    }
	    	    break;

		case STATE_RCPTHDR_PARENTH_START:
	    	    if (c == '\n')
	    	    {
	    		folded_rcpthdr_savestate = state;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == ')')
	    		state = parentheses_savestate;
	    	    else
	    	    {
	    		parentheses_depth++;
	    		state = STATE_RCPTHDR_IN_PARENTH;
	    	    }
	    	    break;

		case STATE_RCPTHDR_IN_PARENTH:
	    	    if (c == '\n')
	    	    {
	    		folded_rcpthdr_savestate = state;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == '\\')
	    	    {
	    		backquote_savestate = state;
	    		state = STATE_RCPTHDR_BACKQUOTE;
	    	    }
	    	    else if (c == '(')
	    		state = STATE_RCPTHDR_PARENTH_START;
	    	    else if (c == ')')
	    	    {
	    		parentheses_depth--;
	    		if (parentheses_depth == 0)
	    		    state = parentheses_savestate;
	    	    }
	    	    break;

		case STATE_RCPTHDR_IN_ADDRESS:
	    	    if (c == '\n')
	    	    {
	    		folded_rcpthdr_savestate = STATE_RCPTHDR_DEFAULT;
	    		state = STATE_LINESTART_AFTER_RCPTHDR;
	    	    }
	    	    else if (c == '\\')
	    	    {
	    		backquote_savestate = state;
	    		state = STATE_RCPTHDR_BACKQUOTE;
	    	    }
	    	    else if (c == '"')
	    	    {
	    		forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_DQUOTE;
	    	    }
	    	    else if (c == '(')
	    	    {
	    		parentheses_savestate = state;
	    		state = STATE_RCPTHDR_PARENTH_START;
	    	    }
	    	    else if (c == '<')
	    	    {
	    		forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_BRACKETS_START;
	    	    }
	    	    else if (c == ' ' || c == '\t')
	    		state = STATE_RCPTHDR_DEFAULT;
	    	    else if (c == ':')
	    	    {
	    		forget_current_recipient = 1;
	    		state = STATE_RCPTHDR_DEFAULT;
	    	    }
	    	    else if (c == ',' || c == ';')
	    	    {
	    		finish_current_recipient = 1;
	    		state = STATE_RCPTHDR_DEFAULT;
	    	    }
	    	    break;

		case STATE_RCPTHDR_BACKQUOTE:
		    if (c == '\n')
		    {
		       	folded_rcpthdr_savestate = STATE_RCPTHDR_DEFAULT;
			state = STATE_LINESTART_AFTER_RCPTHDR;
		    }
		    else
			state = backquote_savestate;
		    break;
	    }
	}

    	if (c != EOF && fputc(c, *tmpfile) == EOF)
	{
    	    snprintf(errstr, errstr_bufsize,
		    "cannot write mail headers to temporary file: "
		    "output error");
	    goto error_exit;
	}
	
    	if (forget_current_recipient)
	{
	    /* this was just junk */
	    current_recipient_len = 0;
	    current_recipient = NULL;
	    forget_current_recipient = 0;
	}
	if (finish_current_recipient)
	{
	    /* The current recipient just ended. Add it to the list */
	    current_recipient[current_recipient_len] = '\0';
	    list_insert(recipients, current_recipient);
	    recipients = recipients->next;
	    /* Reset for the next recipient */
	    current_recipient_len = 0;
	    current_recipient = NULL;
	    finish_current_recipient = 0;
	}
	if ((state == STATE_RCPTHDR_IN_ADDRESS 
		    || state == STATE_RCPTHDR_IN_BRACKETS)
		&& oldstate != STATE_RCPTHDR_PARENTH_START
		&& oldstate != STATE_RCPTHDR_IN_PARENTH
		&& oldstate != STATE_LINESTART_AFTER_RCPTHDR)
	{
    	    /* Add this character to the current recipient */
	    current_recipient_len++;
	    if (bufsize < current_recipient_len + 1)
	    {
		bufsize += bufsize_step;
	    }
	    current_recipient = xrealloc(current_recipient, 
		    bufsize * sizeof(char));
	    /* sanitize characters */
	    if (!iscntrl((unsigned char)c) && !isspace((unsigned char)c))
	    {
		current_recipient[current_recipient_len - 1] = (char)c;
	    }
	    else
	    {
		current_recipient[current_recipient_len - 1] = '_';
	    }
	}
	
	if (state == STATE_HEADERS_END)
	{
	    break;
	}
    }
    
    if (ferror(mailf))
    {
	snprintf(errstr, errstr_bufsize, 
		"input error while reading the mail");
	goto error_exit;
    }
    
    if (fseek(*tmpfile, 0L, SEEK_SET) != 0)
    {
	snprintf(errstr, errstr_bufsize,
		"cannot rewind temporary file: %s", strerror(errno));
	goto error_exit;
    }

    return EX_OK;

error_exit:
    if (*tmpfile)
    {
	(void)fclose(*tmpfile);
	*tmpfile = NULL;
    }
    free(current_recipient);
    return EX_IOERR;
}

 
/*
 * msmtp_sendmail()
 *
 * Sends a mail. Returns a value from sysexits.h.
 * If 'read_recipients' is true, recipients are extracted from the To, Cc,
 * and Bcc headers *in addition* to the recipients in 'recipients'. 
 * 'errstr' must be 'errstr_bufsize' characters long.
 * If an error occured, 'errstr' may contain a descriptive message (or an empty
 * string) and 'msg' may contain the offending message from the SMTP server (or
 * NULL).
 * In case of success, 'mailsize' contains the number of bytes of the mail 
 * transferred to the SMTP server. In case of failure, its contents are
 * undefined.
 */

int msmtp_sendmail(account_t *acc, list_t *recipients, int read_recipients, 
	FILE *f, int debug, long *mailsize, 
	list_t **lmtp_errstrs, list_t **lmtp_error_msgs,
	list_t **msg, char *errstr)
{
    smtp_server_t srv;
    FILE *tmpfile = NULL;
    int e;
#ifdef HAVE_SSL
    tls_cert_info_t *tci = NULL;
#endif /* HAVE_SSL */
    
    errstr[0] = '\0';
    *msg = NULL;
    *lmtp_errstrs = NULL;
    *lmtp_error_msgs = NULL;
    
    /* Read recipients from the mail as soon as possible. Important for 
     * error reporting/logging. */
    if (read_recipients)
    {
	if ((e = msmtp_read_recipients(f, list_last(recipients), &tmpfile, 
			errstr)) != EX_OK)
	{
	    goto error_exit;
	}
	if (list_is_empty(recipients))
	{
	    snprintf(errstr, errstr_bufsize, "no recipients found");
	    e = EX_DATAERR;
	    goto error_exit;
	}
    }
    /* create a new smtp_server_t */
    srv = smtp_new(debug ? stdout : NULL, acc->protocol);

    /* prepare tls */
#ifdef HAVE_SSL
    if (acc->tls)
    {
	if ((e = smtp_tls_init(&srv, acc->tls_key_file, acc->tls_cert_file, 
			acc->tls_trust_file, errstr)) != TLS_EOK)
	{
	    e = exitcode_tls(e);
	    goto error_exit;
	}
    }
#endif /* HAVE_SSL */

    /* connect */
    if ((e = smtp_connect(&srv, acc->host, acc->port, acc->connect_timeout,
		    errstr)) != NET_EOK)
    {
	e = exitcode_net(e);
	goto error_exit;
    }

    /* start tls for ssmtp servers */
#ifdef HAVE_SSL
    if (acc->tls && acc->tls_nostarttls)
    {
	if (debug)
	{
	    tci = tls_cert_info_new();
	}
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    if (debug)
	    {
		tls_cert_info_free(tci);
	    }
	    msmtp_endsession(&srv, 0);
	    e = exitcode_tls(e);
	    goto error_exit;
	}
	if (debug)
	{
	    msmtp_print_tls_cert_info(tci);
	    tls_cert_info_free(tci);
	}
    }
#endif /* HAVE_SSL */

    /* get greeting */
    if ((e = smtp_get_greeting(&srv, msg, NULL, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
	goto error_exit;
    }
		    
    /* initialize session */
    if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
	goto error_exit;
    }

    /* start tls for starttls servers */
#ifdef HAVE_SSL
    if (acc->tls && !acc->tls_nostarttls)
    {
	if (!(srv.cap.flags & SMTP_CAP_STARTTLS))
	{
	    snprintf(errstr, errstr_bufsize,
		    "the server does not support TLS "
		    "via the STARTTLS command");
	    msmtp_endsession(&srv, 1);
	    e = EX_UNAVAILABLE;
	    goto error_exit;
	}
	if ((e = smtp_tls_starttls(&srv, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
	if (debug)
	{
	    tci = tls_cert_info_new();
	}
	if ((e = smtp_tls(&srv, acc->host, acc->tls_nocertcheck, tci, errstr))
		!= TLS_EOK)
	{
	    if (debug)
	    {
		tls_cert_info_free(tci);
	    }
	    msmtp_endsession(&srv, 0);
	    e = exitcode_tls(e);
	    goto error_exit;
	}
	if (debug)
	{
	    msmtp_print_tls_cert_info(tci);
	    tls_cert_info_free(tci);
	}
	/* initialize again */
	if ((e = smtp_init(&srv, acc->domain, msg, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
    }
#endif /* HAVE_SSL */

    /* test for needed features */
    if ((acc->dsn_return || acc->dsn_notify) && !(srv.cap.flags & SMTP_CAP_DSN))
    {
	snprintf(errstr, errstr_bufsize, 
		"the server does not support DSN");
	msmtp_endsession(&srv, 1);
	e = EX_UNAVAILABLE;
	goto error_exit;
    }
    /* authenticate */
    if (acc->auth_mech)
    {
	if (!(srv.cap.flags & SMTP_CAP_AUTH))
	{
	    snprintf(errstr, errstr_bufsize, 
		    "the server does not support authentication");
	    msmtp_endsession(&srv, 1);
	    e = EX_UNAVAILABLE;
	    goto error_exit;
	}
	if ((e = smtp_auth(&srv, acc->host, acc->username, acc->password, 
			acc->ntlmdomain, acc->auth_mech, 
			msmtp_password_callback, msg, errstr)) 
		!= SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
    }
    
    /* send the envelope */
    if ((e = smtp_send_envelope(&srv, acc->from, recipients, 
		    acc->dsn_notify, acc->dsn_return, msg, errstr)) != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
	goto error_exit;
    }
    /* send header and body */
    *mailsize = 0;
    if (read_recipients)
    {
	/* first the headers from the temp file */
	if ((e = smtp_send_mail(&srv, tmpfile, acc->keepbcc, mailsize, errstr))
	     	!= SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
	(void)fclose(tmpfile);
	tmpfile = NULL;
	/* then the body from the original file */
	if ((e = smtp_send_mail(&srv, f, 1, mailsize, errstr)) != SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
    }
    else
    {
	if ((e = smtp_send_mail(&srv, f, acc->keepbcc, mailsize, errstr)) 
	     	!= SMTP_EOK)
	{
	    msmtp_endsession(&srv, 0);
	    e = exitcode_smtp(e);
	    goto error_exit;
	}
    }
    /* end the mail */
    if (acc->protocol == SMTP_PROTO_SMTP)
    {
	e = smtp_end_mail(&srv, msg, errstr);
    }
    else
    {
	e = smtp_end_mail_lmtp(&srv, recipients, 
		lmtp_errstrs, lmtp_error_msgs, errstr);
    }
    if (e != SMTP_EOK)
    {
	msmtp_endsession(&srv, 0);
	e = exitcode_smtp(e);
	goto error_exit;
    }

    /* end session */
    msmtp_endsession(&srv, 1);

    e = EX_OK;

error_exit:
    if (tmpfile)
    {
	(void)fclose(tmpfile);
    }
    return e;
}


/*
 * print_error()
 *
 * Print an error message
 */

/* make gcc print format warnings for this function: 
void print_error(const char *format, ...) 
    __attribute__ ((format (printf, 1, 2)));
*/
void print_error(const char *format, ...)
{
    va_list args;
    fprintf(stderr, "%s: ", prgname);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}


/*
 * msmtp_get_log_info()
 *
 * Gather log information for syslog or logfile and put it in a string:
 * - host=%s
 * - tls=on|off
 * - auth=on|off
 * - user=%s (only if auth == on and username != NULL)
 * - from=%s
 * - recipients=%s,%s,... 
 * - mailsize=%s (only if exitcode == EX_OK)
 * - smtpstatus=%s (only if exitcode != EX_OK and a smtp msg is available)
 * - smtpmsg='%s' (only if exitcode != EX_OK and a smtp msg is available)
 * - errormsg='%s' (only if exitcode != EX_OK and an error msg is available)
 * - exitcode=%s 
 * 'exitcode' must be one of the sysexits.h exitcodes.
 * This function cannot fail.
 */

char *msmtp_get_log_info(account_t *acc, list_t *recipients, long mailsize,
	list_t *errmsg, char *errstr, int exitcode)
{
    int i;
    size_t s;
    list_t *l;
    char *line;
    int n;
    char *p;
    char *tmp;
    /* temporary strings: */
    char mailsize_str[80];	/* sufficient for 256 bit longs */
    const char *exitcode_str;
    char smtpstatus_str[4];
    char *smtperrmsg_str = NULL;
    
    
    /* gather information */
    
    line = NULL;
    /* mailsize */
    if (exitcode == EX_OK)
    {
	(void)snprintf(mailsize_str, 80, "%ld", mailsize);
    }	
    /* exitcode */
    exitcode_str = exitcode_to_string(exitcode);
    /* smtp status and smtp error message */
    if (exitcode != EX_OK && errmsg)
    {
	(void)snprintf(smtpstatus_str, 4, "%d", smtp_msg_status(errmsg));
	l = errmsg;
	s = 0;
	while (!list_is_empty(l))
	{
	    l = l->next;
	    s += strlen(l->data) + 2;
	}
	s += 1;
	smtperrmsg_str = xmalloc(s * sizeof(char));
	smtperrmsg_str[0] = '\'';
	i = 1;
	l = errmsg;
	while (!list_is_empty(l))
	{
	    l = l->next;
	    p = msmtp_sanitize_string(l->data);
	    while (*p != '\0')
	    {
		/* hide single quotes to make the info easy to parse */
		smtperrmsg_str[i] = (*p == '\'') ? '?' : *p;
		p++;
		i++;
	    }
	    smtperrmsg_str[i++] = '\\';
	    smtperrmsg_str[i++] = 'n';
	}
	i -= 2;
	smtperrmsg_str[i++] = '\'';
	smtperrmsg_str[i++] = '\0';
    }
    
    /* calculate the length of the log line */
    
    s = 0;
    /* "host=%s " */
    s += 5 + strlen(acc->host) + 1;
    /* "tls=on|off " */
    s += 4 + (acc->tls ? 2 : 3) + 1;
    /* "auth=on|off " */
    s += 5 + (acc->auth_mech ? 2 : 3) + 1;
    /* "user=%s " */
    if (acc->auth_mech && acc->username)
    {
	s += 5 + strlen(acc->username) + 1;
    }
    /* "from=%s " */
    s += 5 + strlen(acc->from) + 1;
    /* "recipients=%s,%s,... " */
    s += 11;
    l = recipients;
    while (!list_is_empty(l))
    {
	l = l->next;
	s += strlen(l->data) + 1;
    }
    /* "mailsize=%s " */
    if (exitcode == EX_OK)
    {
	s += 9 + strlen(mailsize_str) + 1;
    }
    /* "smtpstatus=%s smtpmsg=%s " */
    if (exitcode != EX_OK && errmsg)
    {
	s += 11 + strlen(smtpstatus_str) + 1 + 8 + strlen(smtperrmsg_str) + 1;
    }
    /* "errormsg='%s' */
    if (exitcode != EX_OK && errstr[0] != '\0')
    {
	s += 10 + strlen(errstr) + 2;
    }
    /* "exitcode=%s" */
    s += 9 + strlen(exitcode_str);
    /* '\0' */
    s++;
    
    line = xmalloc(s * sizeof(char));
    
    /* build the log line */
    
    p = line;
    n = snprintf(p, s, "host=%s tls=%s auth=%s ",
	    acc->host, (acc->tls ? "on" : "off"), 
	    (acc->auth_mech ? "on" : "off"));
    s -= n;
    p += n;
    if (acc->auth_mech && acc->username)
    {
	n = snprintf(p, s, "user=%s ", acc->username);
	s -= n;
	p += n;
    }
    n = snprintf(p, s, "from=%s recipients=", acc->from);
    s -= n;
    p += n;
    l = recipients;
    while (!list_is_empty(l))
    {
	l = l->next;
	n = snprintf(p, s, "%s,", (char *)(l->data));
	s -= n;
	p += n;
    }
    /* delete the last ',' */
    *(p - 1) = ' ';
    if (exitcode == EX_OK)
    {
	n = snprintf(p, s, "mailsize=%s ", mailsize_str);
	s -= n;
	p += n;
    }
    if (exitcode != EX_OK && errmsg)
    {
	n = snprintf(p, s, "smtpstatus=%s smtpmsg=%s ", 
		smtpstatus_str, smtperrmsg_str);
	s -= n;
	p += n;
    }
    if (exitcode != EX_OK && errstr[0] != '\0')
    {
	/* hide single quotes to make the info easy to parse */
	tmp = errstr;
	while (*tmp)
	{
	    if (*tmp == '\'')
	    {
		*tmp = '?';
	    }
	    tmp++;
	}
	n = snprintf(p, s, "errormsg='%s' ", msmtp_sanitize_string(errstr));
	s -= n;
	p += n;
    }
    (void)snprintf(p, s, "exitcode=%s", exitcode_str);
    
    return line;
}


/*
 * msmtp_log_to_file()
 *
 * Append a log entry to 'acc->logfile' with the following information:
 * - date/time 
 * - the log line as delivered by msmtp_get_log_info
 */

void msmtp_log_to_file(const char *logfile, const char *loginfo)
{
    FILE *f;
    time_t t;
    struct tm *tm;
    const char *failure_reason;
    char time_str[64];
    
    
    /* get time */
    if ((t = time(NULL)) < 0)
    {
	failure_reason = strerror(errno);
	goto log_failure;
    }
    if (!(tm = localtime(&t)))
    {
	failure_reason = "cannot convert UTC time to local time";
	goto log_failure;
    }
    if (strftime(time_str, 64, "%b %d %H:%M:%S", tm) > 63)
    {
	failure_reason = "BUG: time_str too short";
	goto log_failure;
    }
    
    /* write log to file */
    if (strcmp(logfile, "-") == 0)
    {
	f = stdout;
    }
    else
    {
	if (!(f = fopen(logfile, "a")))
	{
	    failure_reason = strerror(errno);
	    goto log_failure;
	}
    }
    if ((fputs(time_str, f) == EOF) || (fputc(' ', f) == EOF)
	|| (fputs(loginfo, f) == EOF) || (fputc('\n', f) == EOF))
    {
	failure_reason = "output error";
	goto log_failure;
    }
    if (f != stdout && fclose(f) != 0)
    {
	failure_reason = strerror(errno);
	goto log_failure;
    }

    return;
    
    /* error exit target */
log_failure:
    print_error("cannot log to %s: %s", logfile, failure_reason);
    if (loginfo)
    {
	print_error("log info was: %s", loginfo);
    }
}


/*
 * msmtp_log_to_syslog()
 *
 * Log the information delivered by msmtp_get_log_info() to syslog
 * the facility_str must be one of "LOG_MAIL", "LOG_USER", "LOG_LOCAL0", ...
 * "LOG_LOCAL7"
 * If 'error' is set, LOG_ERR is used, else LOG_INFO is used.
 */

#ifdef HAVE_SYSLOG
void msmtp_log_to_syslog(const char *facility_str, 
	const char *loginfo, int error)
{
    int facility;

    if (facility_str[4] == 'M')
    {
	facility = LOG_MAIL;
    }
    else if (facility_str[4] == 'U')
    {
	facility = LOG_USER;
    }
    else if (facility_str[9] == '0')
    {
	facility = LOG_LOCAL0;
    }
    else if (facility_str[9] == '1')
    {
	facility = LOG_LOCAL1;
    }
    else if (facility_str[9] == '2')
    {
	facility = LOG_LOCAL2;
    }
    else if (facility_str[9] == '3')
    {
	facility = LOG_LOCAL3;
    }
    else if (facility_str[9] == '4')
    {
	facility = LOG_LOCAL4;
    }
    else if (facility_str[9] == '5')
    {
	facility = LOG_LOCAL5;
    }
    else if (facility_str[9] == '6')
    {
	facility = LOG_LOCAL6;
    }
    else
    {
	facility = LOG_LOCAL7;
    }
    
    openlog(PACKAGE_NAME, 0, facility);
    syslog(error ? LOG_ERR : LOG_INFO, "%s", loginfo);
    closelog();
}
#endif /* HAVE_SYSLOG */


/*
 * msmtp_construct_env_from()
 *
 * Build an envelope from address for the current user.
 * If maildomain is not NULL, it will be the domain part of the address.
 * If maildomain is NULL, the address won't have a domain part.
 */

char *msmtp_construct_env_from(const char *maildomain)
{
    char *envelope_from = NULL;
    size_t len;
#ifdef __MINGW32__
    DWORD size = UNLEN + 1;
    TCHAR buf[UNLEN + 1];
#elif defined DJGPP
#else /* UNIX */
    struct passwd *pw;
#endif
	 
    envelope_from = getenv("USER");
    if (envelope_from)
    {
	envelope_from = xstrdup(envelope_from);
    }
    else
    {	
	envelope_from = getenv("LOGNAME");
	if (envelope_from)
	{
	    envelope_from = xstrdup(envelope_from);
	}
	else
	{
#ifdef __MINGW32__
	    if (GetUserName(buf, &size))
	    {
		envelope_from = xstrdup((char *)buf);
	    }
	    else
	    {
		/* last resort */
		envelope_from = xstrdup("unknown");
	    }
#elif defined DJGPP
	    /* DJGPP's getlogin() checks USER, then LOGNAME, and then uses 
	     * "dosuser" as a last resort. We already checked USER and LOGNAME
	     * and choose "unknown" as a last resort to be consistent with the
	     * other systems. */
	    envelope_from = xstrdup("unknown");
#else /* UNIX */
	    envelope_from = getlogin();
	    if (envelope_from)
	    {
		envelope_from = xstrdup(envelope_from);
	    }
	    else
	    {
	    	pw = getpwuid(getuid());		
		if (pw && pw->pw_name)
		{
		    envelope_from = xstrdup(pw->pw_name);
	    	}
		else
		{
		    /* last resort */
		    envelope_from = xstrdup("unknown");
		}
	    }
#endif
	}
    }
    
    if (maildomain)
    {
	len = strlen(envelope_from);
	envelope_from = xrealloc(envelope_from, 
		((len + 1 + strlen(maildomain) + 1) * sizeof(char)));
	envelope_from[len] = '@';
	strcpy(envelope_from + len + 1, maildomain);
    }

    return envelope_from;
}


/*
 * msmtp_print_version()
 *
 * Print --version information
 */

void msmtp_print_version(void)
{
    char *system_conffile;
    char *user_conffile;
    
    printf("%s version %s\nTLS/SSL library: %s\n"
    	    "Authentication library: %s\n"
	    "Supported authentication methods:\n",
	    PACKAGE_NAME, VERSION,
#ifdef HAVE_GNUTLS
	    "GnuTLS"
#elif defined (HAVE_OPENSSL)
	    "OpenSSL"
#else
	    "none"
#endif
	    ,
#ifdef USE_GSASL
	    "GNU SASL"
#else
	    "built-in"
#endif /* USE_GSASL */
	  );
    if (smtp_client_supports_authmech("PLAIN"))
    {
	printf("plain ");
    }
    if (smtp_client_supports_authmech("CRAM-MD5"))
    {
	printf("cram-md5 ");
    }
    if (smtp_client_supports_authmech("DIGEST-MD5"))
    {
	printf("digest-md5 ");
    }
    if (smtp_client_supports_authmech("GSSAPI"))
    {
	printf("gssapi ");
    }
    if (smtp_client_supports_authmech("EXTERNAL"))
    {
	printf("external ");
    }
    if (smtp_client_supports_authmech("LOGIN"))
    {
	printf("login ");
    }
    if (smtp_client_supports_authmech("NTLM"))
    {
	printf("ntlm ");
    }
    printf("\n");
    system_conffile = get_default_system_conffile(SYSCONFFILE);
    printf("System configuration file name: %s\n", system_conffile);
    free(system_conffile);
    if (get_default_user_conffile(USERCONFFILE, &user_conffile, errstr) 
	    != CONF_EOK)
    {
	printf("Cannot get user configuration file name: %s\n", errstr);
    }
    else
    {
	printf("User configuration file name: %s\n", user_conffile);
    }
    free(user_conffile);
    printf("\nCopyright (C) 2004, 2005 Martin Lambers and others.\n"
	    "This is free software; see the source for copying conditions.  "
	    "There is NO\n"
	    "warranty; not even for MERCHANTABILITY or FITNESS FOR A "
	    "PARTICULAR PURPOSE.\n");
}


/*
 * msmtp_print_help()
 *
 * Print --help information
 */

void msmtp_print_help(void)
{
    printf("USAGE:\n\n"
	    "Sendmail mode (default):\n"
	    "  %s [option...] [--] recipient...\n"
	    "  %s [option...] -t [--] [recipient...]\n"
	    "  Read a mail from standard input and transmit it to an SMTP "
	    	"or LMTP server.\n"
	    "Server information mode:\n"
	    "  %s [option...] --serverinfo\n"
	    "  Print information about a server.\n"
	    "Remote Message Queue Starting mode:\n"
	    "  %s [option...] --rmqs=host|@domain|#queue\n"
	    "  Send a Remote Message Queue Starting request to a server\n"
            "\nOPTIONS:\n\n"
	    "General options:\n"
	    "  --version                    print version\n"
	    "  --help                       print help\n"
	    "  -P, --pretend                print configuration info and exit\n"
	    "  -d, --debug                  print debugging information\n"
	    "Changing the mode of operation:\n"
	    "  -S, --serverinfo             print information about the "
	    	"server\n"
	    "  --rmqs=host|@domain|#queue   send a Remote Message Queue "
	    	"Starting request\n"
            "Configuration options:\n"
	    "  -C, --file=filename          set configuration file\n"
	    "  -a, --account=id             use the given account instead of "
	        "the account\n"
	    "                               named \"default\"; its settings may "
	        "be changed\n"
	    "                               with command line options\n"
	    "  --host=hostname              set the server, use only command "
	    	"line settings;\n"
	    "                               do not use any configuration file "
	    	"data\n"
	    "  --port=number                set port number\n"
	    "  --connect-timeout=(off|t)    set/unset connect timeout in "
		"seconds\n"
	    "  --protocol=(smtp|lmtp)       use the given sub protocol\n"
	    "  --domain=string              set the argument of EHLO or LHLO "
	    	"command\n"
	    "  --auth[=(on|off|method)]     enable/disable authentication and "
	    	"optionally\n"
	    "                               choose the method\n"
	    "  --user=[username]            set/unset user name for "
	    	"authentication\n"
	    "  --tls[=(on|off)]             enable/disable TLS encryption\n"
	    "  --tls-trust-file=[file]      set/unset trust file for TLS\n"
            "  --tls-key-file=[file]        set/unset private key file for "
	    	"TLS\n"
	    "  --tls-cert-file=[file]       set/unset private cert file for "
	    	"TLS\n"
	    "  --tls-certcheck[=(on|off)]   enable/disable server certificate "
	    	"checks for TLS\n"
	    "  --tls-starttls[=(on|off)]    enable/disable STARTTLS for TLS\n"
            "Options specific to sendmail mode:\n"
	    "  -f, --from=address           set envelope from address\n"
	    "  --maildomain=[domain]        set/unset the domain used to "
	    	"create envelope\n"
	    "                               from addresses\n"
	    "  -N, --dsn-notify=(off|cond)  set/unset DSN notification "
	    	"conditions\n"
	    "  -R, --dsn-return=(off|ret)   set/unset DSN notification amount\n"
	    "  --keepbcc[=(on|off)]         enable/disable preservation of the "
	    	"Bcc header\n"
            "  -X, --logfile=[file]         set/unset log file\n"
	    "  --syslog[=(on|off|facility)] enable/disable/configure syslog "
	    	"logging\n"
	    "  -t, --read-recipients        read additional recipients from "
	    	"the mail\n"
	    "  --                           end of options\n"
            "Accepted but ignored: -A, -B, -bm, -G, -h, -i, -L, -m, -n, -O, "
	    	"-o, -v\n"
	    "\nReport bugs to <%s>.\n",
	    prgname, prgname, prgname, prgname, PACKAGE_BUGREPORT);
}


/*
 * msmtp_cmdline()
 *
 * Process the command line
 */

typedef struct
{
    /* the configuration */
    int print_version;
    int print_help;
    int print_conf;
    int debug;
    int pretend;
    int read_recipients;
    /* mode of operation */
    int sendmail;
    int serverinfo;
    int rmqs;
    char *rmqs_argument;
    /* account information from the command line */
    account_t *cmdline_account;
    const char *account_id;
    char *user_conffile;
    /* the list of recipients */
    list_t *recipients;
} msmtp_cmdline_conf_t;

/* long options without a corresponding short option */
#define LONGONLYOPT_VERSION 		0
#define LONGONLYOPT_HELP    		1
#define LONGONLYOPT_HOST		2
#define LONGONLYOPT_PORT		3
#define LONGONLYOPT_CONNECT_TIMEOUT	4
#define LONGONLYOPT_AUTH		5
#define LONGONLYOPT_USER		6
#define LONGONLYOPT_TLS			7
#define LONGONLYOPT_TLS_TRUST_FILE	8
#define LONGONLYOPT_TLS_KEY_FILE	9
#define LONGONLYOPT_TLS_CERT_FILE	10
#define LONGONLYOPT_TLS_CERTCHECK	11
#define LONGONLYOPT_TLS_STARTTLS	12
#define LONGONLYOPT_PROTOCOL		13
#define LONGONLYOPT_DOMAIN		14
#define LONGONLYOPT_KEEPBCC		15
#define LONGONLYOPT_RMQS		16
#define LONGONLYOPT_SYSLOG		17
#define LONGONLYOPT_MAILDOMAIN		18

int msmtp_cmdline(msmtp_cmdline_conf_t *conf, int argc, char *argv[])
{
    int option_index;
    struct option options[] =
    {
	{ "version",         no_argument,       0, LONGONLYOPT_VERSION },
	{ "help",            no_argument,       0, LONGONLYOPT_HELP },
	{ "pretend",         no_argument,       0, 'P' },
	/* accept an optional argument for sendmail compatibility: */
	{ "debug",           optional_argument, 0, 'd' },
	{ "serverinfo",      no_argument,       0, 'S' },
	{ "rmqs",            required_argument, 0, LONGONLYOPT_RMQS },
	{ "file",            required_argument, 0, 'C' },
	{ "account",         required_argument, 0, 'a' },
	{ "host",            required_argument, 0, LONGONLYOPT_HOST },
	{ "port",            required_argument, 0, LONGONLYOPT_PORT },
	{ "connect-timeout", required_argument, 0, LONGONLYOPT_CONNECT_TIMEOUT},
	{ "from",            required_argument, 0, 'f' },
	{ "maildomain",      required_argument, 0, LONGONLYOPT_MAILDOMAIN },
	{ "auth",            optional_argument, 0, LONGONLYOPT_AUTH },
	{ "user",            required_argument, 0, LONGONLYOPT_USER },
	{ "tls",             optional_argument, 0, LONGONLYOPT_TLS },
	{ "tls-trust-file",  required_argument, 0, LONGONLYOPT_TLS_TRUST_FILE },
	{ "tls-key-file",    required_argument, 0, LONGONLYOPT_TLS_KEY_FILE },
	{ "tls-cert-file",   required_argument, 0, LONGONLYOPT_TLS_CERT_FILE },
	{ "tls-certcheck",   optional_argument, 0, LONGONLYOPT_TLS_CERTCHECK },
	{ "tls-starttls",    optional_argument, 0, LONGONLYOPT_TLS_STARTTLS },
	{ "dsn-notify",      required_argument, 0, 'N' },
	{ "dsn-return",      required_argument, 0, 'R' },
	{ "protocol",        required_argument, 0, LONGONLYOPT_PROTOCOL },
	{ "domain",          required_argument, 0, LONGONLYOPT_DOMAIN },
	{ "keepbcc",         optional_argument, 0, LONGONLYOPT_KEEPBCC },
	{ "logfile",         required_argument, 0, 'X' },
	{ "syslog",          optional_argument, 0, LONGONLYOPT_SYSLOG },
	{ "read-recipients", no_argument,       0, 't' },
	{ 0, 0, 0, 0 }
    };
    int rcptc;
    char **rcptv;
    int error_code;
    int c;
    int i;
    list_t *lp;

    /* the program name */
    prgname = get_prgname(argv[0]);
    /* the configuration */
    conf->print_version = 0;
    conf->print_help = 0;
    conf->print_conf = 0;
    conf->debug = 0;
    conf->pretend = 0;
    conf->read_recipients = 0;
    /* mode of operation */
    conf->sendmail = 1;
    conf->serverinfo = 0;
    conf->rmqs = 0;
    conf->rmqs_argument = NULL;
    /* account information from the command line */
    conf->cmdline_account = account_new(NULL, NULL);
    conf->account_id = NULL;
    conf->user_conffile = NULL;
    /* the recipients */
    conf->recipients = NULL;
    
    /* process the command line */
    error_code = 0;
    for (;;)
    {
	c = getopt_long(argc, argv, "Pd::SC:a:f:N:R:X:tA:B:b:Gh:iL:mnO:o:v", 
		options, &option_index);
	if (c == -1)
	{
	    break;
	}
	switch(c)
	{
	    case LONGONLYOPT_VERSION:
		conf->print_version = 1;
    		conf->sendmail = 0;
		conf->serverinfo = 0;
		break;

	    case LONGONLYOPT_HELP:
		conf->print_help = 1;
		conf->sendmail = 0;
		conf->serverinfo = 0;
		break;

	    case 'P':
		conf->print_conf = 1;
		conf->pretend = 1;
		break;
		
	    case 'd':
		conf->print_conf = 1;
		conf->debug = 1;
		/* only care about the optional argument if it's "0.1", which is
		 * the only argument that's documented for sendmail: it prints
		 * version information */
		if (optarg && strcmp(optarg, "0.1") == 0)
		{
	    	    conf->print_version = 1;
		}
		break;
		
	    case 'S':
		if (conf->rmqs)
		{
		    print_error("cannot use both --serverinfo and --rmqs");
		    error_code = 1;
		}
		else
		{
		    conf->serverinfo = 1;
		    conf->sendmail = 0;
		    conf->rmqs = 0;
		}
		break;

	    case LONGONLYOPT_RMQS:
		if (conf->serverinfo)
		{
		    print_error("cannot use both --serverinfo and --rmqs");
		    error_code = 1;
		}
		else
		{
		    conf->rmqs = 1;
		    conf->rmqs_argument = optarg;
		    conf->sendmail = 0;
		    conf->serverinfo = 0;
		}
		break;
		
	    case 'C':
		free(conf->user_conffile);
		conf->user_conffile = xstrdup(optarg);
		break;

	    case 'a':
		if (conf->cmdline_account->host)
		{
		    print_error("cannot use both --host and --account");
		    error_code = 1;
		}
		else
		{
		    conf->account_id = optarg;
		}
		break;

	    case LONGONLYOPT_HOST:
		if (conf->account_id)
		{
		    print_error("cannot use both --host and --account");
		    error_code = 1;
		}
		else
		{
		    free(conf->cmdline_account->host);
		    conf->cmdline_account->host = xstrdup(optarg);
		    conf->cmdline_account->mask |= ACC_HOST;
		}
		break;
		
	    case LONGONLYOPT_PORT:
		conf->cmdline_account->port = get_pos_int(optarg);
		if (conf->cmdline_account->port < 1 
			|| conf->cmdline_account->port > 65535)
		{
		    print_error("invalid argument %s for %s", optarg, "--port");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_PORT;
		break;		
		
	    case LONGONLYOPT_CONNECT_TIMEOUT:
		if (is_off(optarg))
		{
		    conf->cmdline_account->connect_timeout = 0;
		}
		else
		{
		    conf->cmdline_account->connect_timeout = 
			get_pos_int(optarg);
		    if (conf->cmdline_account->connect_timeout < 1)
		    {
			print_error("invalid argument %s for %s",
				optarg, "--connect-timeout");
			error_code = 1;
		    }
		}
		conf->cmdline_account->mask |= ACC_CONNECT_TIMEOUT;
		break;
		
	    case 'f':
		free(conf->cmdline_account->from);
		conf->cmdline_account->from = 
		    (*optarg == '\0') ? NULL : xstrdup(optarg);
		conf->cmdline_account->mask |= ACC_FROM;
		break;

	    case LONGONLYOPT_MAILDOMAIN:
		free(conf->cmdline_account->maildomain);
		conf->cmdline_account->maildomain = 
		    (*optarg == '\0') ? NULL : xstrdup(optarg);
		conf->cmdline_account->mask |= ACC_MAILDOMAIN;
		break;

	    case LONGONLYOPT_AUTH:
		free(conf->cmdline_account->auth_mech);
		if (!optarg || is_on(optarg))
		{
		    conf->cmdline_account->auth_mech = xstrdup("");
		}
		else if (is_off(optarg))
    		{
		    conf->cmdline_account->auth_mech = NULL;
		}
		else if (check_auth_arg(optarg) == 0)
		{
		    conf->cmdline_account->auth_mech = xstrdup(optarg);
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--auth");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_AUTH_MECH;
		break;

	    case LONGONLYOPT_USER:
		free(conf->cmdline_account->username);
		conf->cmdline_account->username =
		    (*optarg == '\0') ? NULL : xstrdup(optarg);
		conf->cmdline_account->mask |= ACC_USERNAME;
		break;

	    case LONGONLYOPT_TLS:
	    	if (!optarg || is_on(optarg))
    		{
		    conf->cmdline_account->tls = 1;
		}
		else if (is_off(optarg))
		{
		    conf->cmdline_account->tls = 0;
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--tls");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_TLS;
		break;

	    case LONGONLYOPT_TLS_TRUST_FILE:
		free(conf->cmdline_account->tls_trust_file);
		if (*optarg)
		{
		    if (expand_tilde(optarg, 
				&(conf->cmdline_account->tls_trust_file), 
				errstr) != PATHS_EOK)
		    {
			print_error("%s", errstr);
			error_code = 1;
		    }
		}
		else
		{
		    conf->cmdline_account->tls_trust_file = NULL;
		}
		conf->cmdline_account->mask |= ACC_TLS_TRUST_FILE;
		break;

	    case LONGONLYOPT_TLS_KEY_FILE:
		free(conf->cmdline_account->tls_key_file);
		if (*optarg)
		{
		    if (expand_tilde(optarg, 
				&(conf->cmdline_account->tls_key_file),
				errstr) != PATHS_EOK)
		    {
			print_error("%s", errstr);
			error_code = 1;
		    }
		}
		else
		{
		    conf->cmdline_account->tls_key_file = NULL;
		}
		conf->cmdline_account->mask |= ACC_TLS_KEY_FILE;
		break;

	    case LONGONLYOPT_TLS_CERT_FILE:
		free(conf->cmdline_account->tls_cert_file);
		if (*optarg)
		{
		    if (expand_tilde(optarg, 
				&(conf->cmdline_account->tls_cert_file),
				errstr) != PATHS_EOK)
		    {
			print_error("%s", errstr);
			error_code = 1;
		    }
		}
		else
		{
		    conf->cmdline_account->tls_cert_file = NULL;
		}
		conf->cmdline_account->mask |= ACC_TLS_CERT_FILE;
		break;

	    case LONGONLYOPT_TLS_CERTCHECK:
	   	if (!optarg || is_on(optarg))
    		{
		    conf->cmdline_account->tls_nocertcheck = 0;
		}
		else if (is_off(optarg))
		{
		    conf->cmdline_account->tls_nocertcheck = 1;
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--tls-certcheck");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_TLS_NOCERTCHECK;
		break;

	    case LONGONLYOPT_TLS_STARTTLS:
	    	if (!optarg || is_on(optarg))
    		{
		    conf->cmdline_account->tls_nostarttls = 0;
		}
		else if (is_off(optarg))
		{
		    conf->cmdline_account->tls_nostarttls = 1;
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--tls-starttls");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_TLS_NOSTARTTLS;
		break;

	    case 'N':
		free(conf->cmdline_account->dsn_notify);
		if (is_off(optarg))
		{
		    conf->cmdline_account->dsn_notify = NULL;
		}
		else if (check_dsn_notify_arg(optarg) == 0)
		{
		    conf->cmdline_account->dsn_notify = xstrdup(optarg);
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--dsn-notify");
	    	    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_DSN_NOTIFY;
		break;

	    case 'R':
		/* be compatible to both sendmail and the dsn_notify command */
		free(conf->cmdline_account->dsn_return);
		if (is_off(optarg))
		{
		    conf->cmdline_account->dsn_return = NULL;
		}
		else if (strcmp(optarg, "hdrs") == 0 
			|| strcmp(optarg, "headers") == 0)
		{
		    conf->cmdline_account->dsn_return = xstrdup("HDRS");
		}
		else if (strcmp(optarg, "full") == 0)
		{
		    conf->cmdline_account->dsn_return = xstrdup("FULL");
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--dsn-return");
	    	    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_DSN_RETURN;
		break;
		
	    case LONGONLYOPT_PROTOCOL:
		conf->cmdline_account->mask |= ACC_PROTOCOL;
		if (strcmp(optarg, "smtp") == 0)
		{
		    conf->cmdline_account->protocol = SMTP_PROTO_SMTP;
		}
		else if (strcmp(optarg, "lmtp") == 0)
		{
		    conf->cmdline_account->protocol = SMTP_PROTO_LMTP;
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--protocol");
	    	    error_code = 1;
		}
		break;

	    case LONGONLYOPT_DOMAIN:
		free(conf->cmdline_account->domain);
		conf->cmdline_account->domain = xstrdup(optarg);
		conf->cmdline_account->mask |= ACC_DOMAIN;
		break;

	    case LONGONLYOPT_KEEPBCC:
	    	if (!optarg || is_on(optarg))
    		{
		    conf->cmdline_account->keepbcc = 1;
		}
		else if (is_off(optarg))
		{
		    conf->cmdline_account->keepbcc = 0;
		}
		else
		{
		    print_error("invalid argument %s for %s", 
			    optarg, "--keepbcc");
		    error_code = 1;
		}
		conf->cmdline_account->mask |= ACC_KEEPBCC;
		break;

	    case 'X':
		free(conf->cmdline_account->logfile);
		if (*optarg)
		{
		    if (expand_tilde(optarg, 
				&(conf->cmdline_account->logfile),
				errstr) != PATHS_EOK)
		    {
			print_error("%s", errstr);
			error_code = 1;
		    }
		}
		else
		{
		    conf->cmdline_account->logfile = NULL;
		}
		conf->cmdline_account->mask |= ACC_LOGFILE;
		break;

	    case LONGONLYOPT_SYSLOG:
		free(conf->cmdline_account->syslog);
		if (!optarg || is_on(optarg))
		{
		    conf->cmdline_account->syslog = 
			get_default_syslog_facility();
		}
		else if (is_off(optarg))
		{
		    conf->cmdline_account->syslog = NULL;
		}
		else
		{
		    if (check_syslog_arg(optarg) != 0)
		    {
			print_error("invalid argument %s for %s", 
				optarg, "--syslog");
			error_code = 1;
		    }
		    else
		    {
			conf->cmdline_account->syslog = xstrdup(optarg);
		    }
		}
		conf->cmdline_account->mask |= ACC_SYSLOG;
		break;

	    case 't':
		conf->read_recipients = 1;
		break;

	    case 'b':
		/* only m makes sense */
		if (strcmp(optarg, "m") != 0)
		{
		    print_error("unsupported operation mode b%s", optarg);
		    error_code = 1;
		}		    
		break;

	    case 'A':
	    case 'B':
	    case 'G':
	    case 'h':
	    case 'i':
	    case 'L':
	    case 'm':
	    case 'n':
	    case 'O':
	    case 'o':
	    case 'v':
		break;
		
	    /* unknown option */
	    default:
		error_code = 1;
		break;
	}
	if (error_code)
	{
	    break;
	}
    }
    if (error_code)
    {
	return EX_USAGE;
    }

    /* the list of recipients */
    rcptc = argc - optind;
    rcptv = &(argv[optind]);
    conf->recipients = list_new();
    lp = conf->recipients;
    for (i = 0; i < rcptc; i++)
    {
	list_insert(lp, xstrdup(rcptv[i]));
	lp = lp->next;
    }

    return EX_OK;
}


/*
 * msmtp_get_conffile_accounts()
 * Read the system and user configuration files and merge the data
 */

int msmtp_get_conffile_accounts(list_t **account_list, 
	int print_info, const char *user_conffile, 
	char **loaded_system_conffile, char **loaded_user_conffile)
{
    char *system_conffile;
    char *real_user_conffile;
    list_t *system_account_list;
    list_t *user_account_list;
    list_t *lps;
    list_t *lpu;
    int e;


    *loaded_system_conffile = NULL;
    *loaded_user_conffile = NULL;

    /* Read the system configuration file.
     * It is not an error if system_conffile cannot be opened,
     * but it is an error is the file content is invalid */ 
    system_conffile = get_default_system_conffile(SYSCONFFILE);
    if ((e = get_conf(system_conffile, 0, &system_account_list, errstr)) 
	    != CONF_EOK)
    {
	if (e == CONF_ECANTOPEN)
	{
	    if (print_info)
	    {
		printf("ignoring system configuration file %s: %s\n",
			system_conffile, msmtp_sanitize_string(errstr));
	    }
	}
	else
	{
	    print_error("%s: %s", system_conffile, 
		    msmtp_sanitize_string(errstr));
	    return (e == CONF_EIO) ? EX_IOERR : EX_CONFIG;
	}
    }
    else
    {
	if (print_info)
	{
	    printf("loaded system configuration file %s\n", system_conffile);
	}
	*loaded_system_conffile = xstrdup(system_conffile);
    }
    free(system_conffile);

    /* Read the user configuration file.
     * It is not an error if user_conffile cannot be opened (unless it was
     * chosen with -C/--file), but it is an error is the file content is 
     * invalid */ 
    real_user_conffile = NULL;
    if (!user_conffile)
    {
	if (get_default_user_conffile(USERCONFFILE, &real_user_conffile, 
		    errstr) != PATHS_EOK)
	{
	    if (print_info)
	    {
		printf("ignoring user configuration file: %s\n", 
			msmtp_sanitize_string(errstr));
	    }
	}
    }
    else
    {
	real_user_conffile = xstrdup(user_conffile);
    }
    if (real_user_conffile)
    {
	if ((e = get_conf(real_user_conffile, 1, &user_account_list, errstr)) 
		!= CONF_EOK)
	{
	    if (e == CONF_ECANTOPEN)
	    {
		/* If the configuration file was set with -C/--file, it is an
		 * error if we cannot open it */
		if (user_conffile)
		{
		    print_error("%s: %s", real_user_conffile, 
			    msmtp_sanitize_string(errstr));
		    return EX_IOERR;
		}
		/* otherwise, we can ignore it */
		if (print_info)
		{
		    printf("ignoring user configuration file %s: %s\n",
			    real_user_conffile, msmtp_sanitize_string(errstr));
		}
	    }
	    else
	    {
		print_error("%s: %s", real_user_conffile, 
			msmtp_sanitize_string(errstr));
		return (e == CONF_EIO) ? EX_IOERR : EX_CONFIG;
	    }
	}
	else
	{
	    if (print_info)
	    {
		printf("loaded user configuration file %s\n", 
			real_user_conffile);
	    }
	    *loaded_user_conffile = xstrdup(real_user_conffile);
	}
    }
    free(real_user_conffile);

    /* Merge system_account_list and user_account_list into account_list.
     * If an account exist in both files, only the one from the user conffile is
     * kept. It is important that the order of accounts is maintained, so that
     * --from can choose the *first* account with a matching envelope from 
     * address. */
    if (*loaded_system_conffile && *loaded_user_conffile)
    {
	lpu = user_account_list;
	lps = system_account_list;
	while (!list_is_empty(lps))
	{
	    lps = lps->next;
	    if (!find_account(user_account_list, ((account_t *)lps->data)->id))
	    {
		list_insert(lpu, account_copy(lps->data));
		lpu = lpu->next;
	    }
	}
	*account_list = user_account_list;
	list_xfree(system_account_list, account_free);
    }
    else if (*loaded_system_conffile)
    {
	*account_list = system_account_list;
    }
    else if (*loaded_user_conffile)
    {
	*account_list = user_account_list;
    }
    else
    {
	*account_list = list_new();
    }

    return EX_OK;
}

    
/*
 * msmtp_print_conf
 *
 * Print configuration information, for example for --pretend
 */

void msmtp_print_conf(msmtp_cmdline_conf_t conf, account_t *account)
{
    if (account->id && account->conffile)
    {
	printf("using account %s from %s\n", 
		account->id, account->conffile);
    }
    printf("host            = %s\n"
	    "port            = %d\n",
	    account->host,
	    account->port);
    printf("connect timeout = ");
    if (account->connect_timeout <= 0)
    {
	printf("off\n");
    }
    else
    {
	printf("%d second%c\n", account->connect_timeout, 
		account->connect_timeout > 1 ? 's' : ' ');
    }
    printf("protocol        = %s\n"
	    "domain          = %s\n", 
	    account->protocol == SMTP_PROTO_SMTP ? "smtp" : "lmtp",
	    account->domain);
    printf("auth            = ");
    if (!account->auth_mech)
    {
	printf("none\n");
    }
    else if (account->auth_mech[0] == '\0')
    {
	printf("choose\n");
    }
    else
    {
	printf("%s\n", account->auth_mech);
    }
    printf("user            = %s\n"
	    "password        = %s\n"
	    "ntlmdomain      = %s\n"
	    "tls             = %s\n"
	    "tls_trust_file  = %s\n"
	    "tls_key_file    = %s\n"
	    "tls_cert_file   = %s\n"
	    "tls_starttls    = %s\n"
	    "tls_certcheck   = %s\n",
	    account->username ? account->username : "(not set)",
	    account->password ? "*" : "(not set)",
	    account->ntlmdomain ? account->ntlmdomain : "(not set)",
	    account->tls ? "on" : "off", 
	    account->tls_trust_file ? account->tls_trust_file : "(not set)",
	    account->tls_key_file ? account->tls_key_file : "(not set)",
	    account->tls_cert_file ? account->tls_cert_file : "(not set)",
	    account->tls_nostarttls ? "off" : "on",
	    account->tls_nocertcheck ? "off" : "on");
    if (conf.sendmail)
    {
	printf("from            = %s\n"
		"maildomain      = %s\n"
		"dsn_notify      = %s\n"
		"dsn_return      = %s\n"
		"keepbcc         = %s\n"
		"logfile         = %s\n"
		"syslog          = %s\n",
		account->from ? account->from : "(not set)",
		account->maildomain ? account->maildomain : "(not set)",
		account->dsn_notify ? account->dsn_notify : "(not set)",
		account->dsn_return ? account->dsn_return : "(not set)",
		account->keepbcc ? "on" : "off",
		account->logfile ? account->logfile : "(not set)",
		account->syslog ? account->syslog : "(not set)");
	printf("reading recipients from the command line %s\n",
		conf.read_recipients ? "and the mail" : "");
    }
    if (conf.rmqs)
    {
	printf("RMQS argument   = %s\n", conf.rmqs_argument);
    }
}


/*
 * The main function.
 * It returns values from sysexits.h (like sendmail does).
 */

int main(int argc, char *argv[])
{
    msmtp_cmdline_conf_t conf;
    /* account information from the configuration file(s) */
    list_t *account_list = NULL;
    char *loaded_system_conffile = NULL;
    char *loaded_user_conffile = NULL;
    /* the account data that will be used */
    account_t *account = NULL;
    /* error handling */
    int error_code;
    int e;
    list_t *errmsg;
    list_t *lp;
    /* misc */
#ifdef HAVE_SSL
    int tls_lib_initialized = 0;
#endif
    int net_lib_initialized = 0;
    /* the size of a sent mail */
    long mailsize;
    /* special LMTP error info */
    list_t *lmtp_errstrs;
    list_t *lmtp_error_msgs;
    list_t *lp_lmtp_errstrs;
    list_t *lp_lmtp_error_msgs;
    /* log information */
    char *log_info;
    /* needed to get the default port */
    struct servent *se;    


    /* the command line */
    if ((error_code = msmtp_cmdline(&conf, argc, argv)) != EX_OK)
    {
	goto exit;
    }

    if (conf.print_version)
    {
	msmtp_print_version();
    }
    if (conf.print_help)
    {
	msmtp_print_help();
    }
	    
    if (!conf.sendmail && !conf.serverinfo && !conf.rmqs && !conf.print_conf)
    {
	error_code = EX_OK;
	goto exit;
    }

    /* check the list of recipients */
    if (conf.sendmail && list_is_empty(conf.recipients) 
	    && !conf.read_recipients && !conf.pretend)
    {
	print_error("no recipients given");
	error_code = EX_USAGE;
	goto exit;
    }
    if ((conf.serverinfo || conf.rmqs) && !list_is_empty(conf.recipients))
    {
	print_error("too many arguments");
	error_code = EX_USAGE;
	goto exit;
    }

    /* get the account to be used, either from the conffile(s) or from the 
     * command line */
    if (!conf.cmdline_account->host)
    {	
	if ((error_code = msmtp_get_conffile_accounts(&account_list, 
			(conf.pretend || conf.debug), conf.user_conffile, 
			&loaded_system_conffile, &loaded_user_conffile)) 
		!= EX_OK)
	{
	    goto exit;
	}
	if (!conf.account_id)
	{
	    if (conf.cmdline_account->from)
	    {
		/* No account was chosen, but the envelope from address is 
		 * given. Choose the right account with this address.
		 */
		account = account_copy(find_account_by_envelope_from(
			    account_list, conf.cmdline_account->from));
	    }
	    if (!account)
	    {
		/* No envelope from address or no matching account. 
		 * Use default. */
		conf.account_id = "default";
	    }
	}
	if (!account && !(account = 
		    account_copy(find_account(account_list, conf.account_id))))
	{
	    if (loaded_system_conffile && loaded_user_conffile)
	    {		
		print_error("account %s not found in %s and %s",
		     	conf.account_id, loaded_system_conffile, 
			loaded_user_conffile);
	    }
	    else if (loaded_system_conffile)
	    {
		print_error("account %s not found in %s", conf.account_id, 
			loaded_system_conffile);
	    }
	    else if (loaded_user_conffile)
	    {
		print_error("account %s not found in %s", conf.account_id, 
			loaded_user_conffile);
	    }
	    else /* no conffile was read */
	    {
		print_error("account %s not found: "
			"no configuration file available",
		     	conf.account_id);
	    }
	    error_code = EX_CONFIG;
    	    goto exit;
	}
	override_account(account, conf.cmdline_account);
    }
    else
    {
	account = account_copy(conf.cmdline_account);
    }
    
    /* OK, we're using the settings in 'account'. Complete them and check 
     * them. */
    if (account->port == 0)
    {
	if (account->protocol == SMTP_PROTO_SMTP)
	{
	    if (account->tls && account->tls_nostarttls)
	    {
		se = getservbyname("ssmtp", NULL);
		account->port = se ? ntohs(se->s_port) : 465;
	    }
	    else
	    {
		se = getservbyname("smtp", NULL);
		account->port = se ? ntohs(se->s_port) : 25;
	    }
	}
	else /* LMTP */
	{
	    se = getservbyname("lmtp", NULL);
	    account->port = se ? ntohs(se->s_port) : 2003;
	}
    }
    if (check_account(account, errstr) != CONF_EOK)
    {
	if (account->id && account->conffile)
	{
	    print_error("account %s from %s: %s", account->id, 
		    account->conffile, msmtp_sanitize_string(errstr));
	}
	else
	{
	    print_error("%s", msmtp_sanitize_string(errstr));
	}
	error_code = EX_CONFIG;
	goto exit;
    }

    /* construct an envelope from address if necessary */
    if (conf.sendmail && !account->from)
    {
	account->from = msmtp_construct_env_from(account->maildomain);
    }
    
    /* print configuration */
    if (conf.print_conf)
    {
	msmtp_print_conf(conf, account);
    }

    /* stop if there's nothing to do */
    if (conf.pretend || (!conf.sendmail && !conf.serverinfo && !conf.rmqs))
    {
	error_code = EX_OK;
	goto exit;
    }
    
    /* initialize libraries */
#ifndef HAVE_SYSLOG
    if (conf.sendmail && account->syslog)
    {
	print_error("this platform does not support syslog logging");
	error_code = EX_UNAVAILABLE;
	goto exit;
    }
#endif /* not HAVE_SYSLOG */
    if ((conf.sendmail || conf.rmqs) /* serverinfo does not use auth */
	    && account->auth_mech && (strcmp(account->auth_mech, "") != 0) 
	    && !smtp_client_supports_authmech(account->auth_mech))
    {
	print_error("support for authentication method %s is not compiled in", 
		account->auth_mech);
	error_code = EX_UNAVAILABLE;
	goto exit;
    }
    if ((e = net_lib_init(errstr)) != NET_EOK)
    {
	print_error("cannot initialize networking: %s", 
		msmtp_sanitize_string(errstr));
	error_code = EX_SOFTWARE;
	goto exit;	
    }
    net_lib_initialized = 1;
    if (account->tls)
    {
#ifdef HAVE_SSL
	if ((e = tls_lib_init(errstr)) != TLS_EOK)
	{
	    print_error("cannot initialize TLS library: %s", 
		    msmtp_sanitize_string(errstr));
	    error_code = EX_SOFTWARE;
	    goto exit;
	}
	tls_lib_initialized = 1;
#else /* not HAVE_SSL */
	print_error("support for TLS is not compiled in");
	error_code = EX_UNAVAILABLE;
	goto exit;
#endif /* not HAVE_SSL */
    }

    /* do the work */
    if (conf.sendmail)
    {
	if ((error_code = msmtp_sendmail(account, conf.recipients,
			conf.read_recipients, stdin, conf.debug, &mailsize, 
			&lmtp_errstrs, &lmtp_error_msgs,
			&errmsg, errstr)) != EX_OK)
	{
	    if (account->protocol == SMTP_PROTO_LMTP && lmtp_errstrs)
	    {
		lp_lmtp_errstrs = lmtp_errstrs;
		lp_lmtp_error_msgs = lmtp_error_msgs;
		while (!list_is_empty(lp_lmtp_errstrs))
		{
		    lp_lmtp_errstrs = lp_lmtp_errstrs->next;
		    lp_lmtp_error_msgs = lp_lmtp_error_msgs->next;
		    if (lp_lmtp_errstrs->data)
		    {
			print_error("%s", msmtp_sanitize_string(
				    lp_lmtp_errstrs->data));
			if ((lp = lp_lmtp_error_msgs->data))
			{
			    while (!list_is_empty(lp))
			    {
				lp = lp->next;
				print_error("LMTP server message: %s",
					msmtp_sanitize_string(lp->data));
			    }
			    list_xfree(lp_lmtp_error_msgs->data, free);
			}
		    }
		}
		list_xfree(lmtp_errstrs, free);
		list_free(lmtp_error_msgs);
		if (account->id && account->conffile)
		{
		    print_error("could not send mail to all recipients "
			    "(account %s from %s)", 
		    	    account->id, account->conffile);
		}
		else
		{
		    print_error("could not send mail to all recipients");
		}
	    }
	    else
	    {
		if (errstr[0] != '\0')
		{
		    print_error("%s", msmtp_sanitize_string(errstr));
		}
		if (errmsg)
		{
		    lp = errmsg;
		    while (!list_is_empty(lp))
		    {
			lp = lp->next;
			print_error("Server message: %s", 
				msmtp_sanitize_string(lp->data));
		    }
		}
		if (account->id && account->conffile)
		{
		    print_error("could not send mail (account %s from %s)", 
		    	    account->id, account->conffile);
		}
		else
		{
		    print_error("could not send mail");
		}
	    }
	}
	if (account->logfile || account->syslog)
	{
	    if (account->protocol == SMTP_PROTO_LMTP && lmtp_errstrs)
	    {
		/* errstr is empty; print short info to it */
		snprintf(errstr, errstr_bufsize, 
			"delivery to one or more recipients failed");
		/* we know that errmsg is NULL. that's ok. */
	    }
	    log_info = msmtp_get_log_info(account, conf.recipients, mailsize, 
		    errmsg, errstr, error_code);
	    if (account->logfile)
	    {
		msmtp_log_to_file(account->logfile, log_info);
	    }
#ifdef HAVE_SYSLOG
	    if (account->syslog)
	    {
		msmtp_log_to_syslog(account->syslog, log_info, 
			(error_code != EX_OK));
	    }
#endif
	    free(log_info);
	}
    }
    else if (conf.serverinfo)
    {
	if ((error_code = msmtp_serverinfo(account, conf.debug, 
			&errmsg, errstr)) != EX_OK)
	{
	    if (errstr[0] != '\0')
	    {
		print_error("%s", msmtp_sanitize_string(errstr));
	    }
	    if (errmsg)
	    {
		lp = errmsg;
		while (!list_is_empty(lp))
		{
		    lp = lp->next;
		    print_error("Server message: %s", 
			    msmtp_sanitize_string(lp->data));
		}
	    }		    
	}
    }
    else /* rmqs */
    {
	if ((error_code = msmtp_rmqs(account, conf.debug, conf.rmqs_argument, 
			&errmsg, errstr)) != EX_OK)
	{
	    if (errstr[0] != '\0')
	    {
		print_error("%s", msmtp_sanitize_string(errstr));
	    }
	    if (errmsg)
	    {
		lp = errmsg;
		while (!list_is_empty(lp))
		{
		    lp = lp->next;
		    print_error("Server message: %s", 
			    msmtp_sanitize_string(lp->data));
		}
	    }		    
	}
    }

    
exit:

    /* clean up */
    free(loaded_system_conffile);
    free(loaded_user_conffile);
#ifdef HAVE_SSL
    if (tls_lib_initialized) 
    {
	tls_lib_deinit();
    }
#endif /* HAVE_SSL */
    if (net_lib_initialized)
    {
	net_lib_deinit();
    }
    if (account_list)
    {
	list_xfree(account_list, account_free);
    }
    account_free(conf.cmdline_account);
    account_free(account);
    if (conf.recipients)
    {
	list_xfree(conf.recipients, free);
    }

    return error_code;
}
