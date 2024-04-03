/*
 * paths.c
 *
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2004, 2005
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
#include <string.h>
#include <errno.h>
extern int errno;
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __MINGW32__
#include <windows.h>
#include <io.h>
#include <time.h>
#elif defined DJGPP
#include <unistd.h>
#include <time.h>
#else /* UNIX */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <pwd.h>
#endif /* UNIX */

#include "xalloc.h"

#include "paths.h"


#ifdef __MINGW32__
#define PATH_SEP '\\'
#else /* UNIX or DJGPP */
#define PATH_SEP '/'
#endif


/*
 * get_prgname()
 *
 * see paths.h
 */

char *get_prgname(char *argv0)
{
    char *prgname;
    
    prgname = strrchr(argv0, PATH_SEP);
    if (!prgname)
    {
	prgname = argv0;
    }
    else
    {
	prgname++;
    }
    
    return prgname;
}


/*
 * get_sysconfdir()
 *
 * Get the system configuration directory (or something similar, depending 
 * on the OS). Returns a pointer to an allocated string.
 * Cannot fail because it uses safe defaults as fallback.
 */

#ifdef SYSCONFDIR
char *get_sysconfdir(void)
{
#ifdef __MINGW32__

    BYTE sysconfdir[MAX_PATH + 1];
    HKEY hkey;
    DWORD len;
    DWORD type;
    long l;
    
    l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
	    "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
	    "Shell Folders", 0, KEY_READ, &hkey);
    if (l != ERROR_SUCCESS)
    {
	return xstrdup("C:");
    }
    len = MAX_PATH;
    l = RegQueryValueEx(hkey, "Common AppData", NULL, &type, sysconfdir, &len);
    if (l != ERROR_SUCCESS || len >= MAX_PATH)
    {
	if (l != ERROR_SUCCESS || len >= MAX_PATH)
	{
	    return xstrdup("C:");
	}
    }
    RegCloseKey(hkey);
    return xstrdup((char *)sysconfdir);

#else /* UNIX or DJGPP */
    
    return xstrdup(SYSCONFDIR);

#endif
}
#endif


/*
 * get_homedir()
 *
 * Get the users home directory (or something similar, depending on the OS).
 * Returns a pointer to an allocated string in 'h'.
 * Used error codes: PATHS_ENOHOME
 */

int get_homedir(char **h, char *errstr)
{
#ifdef __MINGW32__

    BYTE home[MAX_PATH + 1];
    HKEY hkey;
    DWORD len;
    DWORD type;
    long l;
    
    l = RegOpenKeyEx(HKEY_CURRENT_USER,
	    "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
	    "Shell Folders", 0, KEY_READ, &hkey);
    if (l != ERROR_SUCCESS)
    {
	snprintf(errstr, errstr_bufsize, "cannot read registry");
	return PATHS_ENOHOME;
    }
    len = MAX_PATH;
    l = RegQueryValueEx(hkey, "AppData", NULL, &type, home, &len);
    if (l != ERROR_SUCCESS || len >= MAX_PATH)
    {
	RegCloseKey(hkey);
	snprintf(errstr, errstr_bufsize, "cannot read registry");
	return PATHS_ENOHOME;
    }
    RegCloseKey(hkey);
    *h = xstrdup((char *)home);
    return PATHS_EOK;

#elif defined DJGPP
    
    char *home;
    
    if (!(home = getenv("HOME")))
    {
	snprintf(errstr, errstr_bufsize, 
		"no environment variable HOME");
	return PATHS_ENOHOME;
    }
    *h = xstrdup(home);
    return PATHS_EOK;

#else /* UNIX */

    char *home;
    struct passwd *pw;
    
    if (!(home = getenv("HOME")))
    {
	pw = getpwuid(getuid());
	if (!pw || !(pw->pw_dir))
	{
	    snprintf(errstr, errstr_bufsize, 
		    "cannot get user's home directory");
	    return PATHS_ENOHOME;
	}
	home = pw->pw_dir;
    }
    *h = xstrdup(home);
    return PATHS_EOK;

#endif
}


/*
 * get_default_system_conffile()
 *
 * see paths.h
 */

#ifdef SYSCONFDIR
char *get_default_system_conffile(const char *base)
{
    char *conffile;
    size_t len;
    
    conffile = get_sysconfdir();
    len = strlen(conffile);
    conffile = xrealloc(conffile, (len + strlen(base) + 2) * sizeof(char));
    if (len > 0 && conffile[len - 1] != PATH_SEP)
    {
	conffile[len++] = PATH_SEP;
    }
    strcpy(conffile + len, base);

    return conffile;
}
#endif


/*
 * get_default_user_conffile()
 *
 * see paths.h
 */

int get_default_user_conffile(const char *base, char **dst, char *errstr)
{
    int e;
    char *conffile;
    size_t len;
    
    if ((e = get_homedir(&conffile, errstr)) != PATHS_EOK)
    {
	return e;
    }
    len = strlen(conffile);
    conffile = xrealloc(conffile, (len + strlen(base) + 2) * sizeof(char));
    if (len > 0 && conffile[len - 1] != PATH_SEP)
    {
	conffile[len++] = PATH_SEP;
    }
    strcpy(conffile + len, base);

    *dst = conffile;
    return PATHS_EOK;
}


/*
 * expand_tilde()
 *
 * see paths.h
 */

int expand_tilde(const char *filename, char **new_filename, char *errstr)
{
    int e;
    size_t homedirlen;
    
    if (filename[0] == '~')
    {
	if ((e = get_homedir(new_filename, errstr)) != PATHS_EOK)
	{
	    return e;
	}
	homedirlen = strlen(*new_filename);
	*new_filename = xrealloc(*new_filename, 
		(homedirlen + strlen(filename)) * sizeof(char));
	strcpy(*new_filename + homedirlen, filename + 1);
    }
    else 
    {
	*new_filename = xstrdup(filename);
    }
    return PATHS_EOK;
}


/*
 * [DJGPP and Windows only] mkstemp_unlink()
 *
 * This function does on DOS/Windows what mkstemp() followed by unlink() do on
 * UNIX.
 *
 * 1. unlink() on DOS and Windows is not POSIX conformant: it does not wait
 *    until the last file descriptor is closed before unlinking the file. 
 *    Instead, it fails (Windows) or may even mess up the file system (DOS).
 * 2. Windows does not have mkstemp.
 * 3. If a file is opened with O_TEMPORARY on Windows or DOS, it will be deleted
 *    after the last file descriptor is closed. This is what this function does.
 *
 * Return value: file descriptor, or -1 on error (errno will be set).
 */

#if defined(__MINGW32__) || defined(DJGPP)
int mkstemp_unlink(char *template)
{
    size_t templatelen;
    char *X;
    int i;
    int try;
    int ret;
    /* DOS/Windows file systems are case insensitive, there's no point in using
     * lower case characters. */
    const char alnum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 

    templatelen = strlen(template);
    if (templatelen < 6)
    {
	errno = EINVAL;
	return -1;
    }
    X = template + templatelen - 6;
    if (strcmp(X, "XXXXXX") != 0)
    {
	errno = EINVAL;
	return -1;
    }
    
    srand((unsigned int)time(NULL));

    /* We have 36^6 possible filenames. We try 36^2=1296 times. */
    ret = -1;
    for (try = 0; ret == -1 && try < 1296; try++)
    {
	for (i = 0; i < 6; i++)
	{
	    X[i] = alnum[rand() % 36];
	}
#ifdef __MINGW__
	ret = _open(template, _O_CREAT | _O_EXCL | _O_RDWR | _O_TEMPORARY, 
		_S_IREAD | _S_IWRITE);
#else /* DJGPP */
	ret = open(template, O_CREAT | O_EXCL | O_RDWR | O_TEMPORARY, 
		S_IRUSR | S_IWUSR);
#endif /* DJGPP */
    }

    return ret;
}
#endif /* __MINGW32__ or DJGPP */


/*
 * tempfile()
 *
 * see paths.h
 */

int tempfile(const char *base, FILE **f, char *errstr)
{
    size_t baselen;
    const char *dir;
    size_t dirlen;
    char *template = NULL;
    size_t templatelen;
    char *template_bak = NULL;
    int fd = -1;
    int e;

    
    /* the directory for the temp file */
    if (!(dir = getenv("TMPDIR")))
    {
	/* system dependent default location */
#ifdef __MINGW32__
	/* there is no registry key for this (?) */
	if (!(dir = getenv("TEMP")))
	{
	    if (!(dir = getenv("TMP")))
	    {
		dir = "C:";
	    }		    
	}
#elif defined DJGPP
	dir = "C:";
#else /* UNIX */
#ifdef P_tmpdir
	dir = P_tmpdir;
#else
	dir = "/tmp";
#endif
#endif /* UNIX */
    }    
    dirlen = strlen(dir);

    /* the proposed file name */
    baselen = strlen(base);
#ifdef DJGPP
    /* shorten the base to two characters because of 8.3 filenames */
    if (baselen > 2)
    {
	baselen = 2;
    }
#endif
    
    /* build the template */
    templatelen = dirlen + 1 + baselen + 6;
    template = xmalloc((templatelen + 1) * sizeof(char));
    template_bak = xmalloc((templatelen + 1) * sizeof(char));
    strncpy(template, dir, dirlen);
    template[dirlen] = PATH_SEP;
    /* template is long enough */
    strncpy(template + dirlen + 1, base, baselen);
    strcpy(template + dirlen + 1 + baselen, "XXXXXX");
    strcpy(template_bak, template);

    /* create the file */
#if defined(__MINGW32__) || defined(DJGPP)
    if ((fd = mkstemp_unlink(template)) == -1)
#else /* UNIX */
    if ((fd = mkstemp(template)) == -1)
#endif /* UNIX */
    {
	/* we need template_bak because the contents of template are undefined
	 * when certain errors occur */
	snprintf(errstr, errstr_bufsize,
		"cannot create temporary file from template %s: %s",
		template_bak, strerror(errno));
	e = PATHS_ECANTCREAT;
	goto error_exit;
    }
    free(template_bak);
    template_bak = NULL;

    /* UNIX only: set the permissions (not every mkstemp() sets them to 0600)
     * and unlink the file so that it gets deleted when the caller closes it */
#ifndef DJGPP
#ifndef __MINGW32__
    if (fchmod(fd, S_IRUSR | S_IWUSR) == -1)
    {
	snprintf(errstr, errstr_bufsize,
		"cannot set mode of temporary file %s: %s",
		template, strerror(errno));
	e = PATHS_ECANTCREAT;
	goto error_exit;
    }
    if (unlink(template) != 0)
    {
	snprintf(errstr, errstr_bufsize,
		"cannot unlink temporary file %s: %s",
		template, strerror(errno));
	e = PATHS_ECANTCREAT;
	goto error_exit;
    }
#endif /* not __MINGW32__ */
#endif /* not DJGPP */

    /* get the stream from the filedescriptor */
    if (!(*f = fdopen(fd, "w+")))
    {
	snprintf(errstr, errstr_bufsize,
		"cannot access temporary file %s: %s",
		template, strerror(errno));
	e = PATHS_ECANTCREAT;
	goto error_exit;
    }
    free(template);

    return PATHS_EOK;

error_exit:
    if (fd >= 0)
    {
	close(fd);
    }
    free(template_bak);
    if (template)
    {
	(void)remove(template);
	free(template);
    }
    *f = NULL;
    return e;
}


/*
 * check_secure()
 *
 * see paths.h
 *
 * This code is largely borrowed from fetchmail-6.2.5, rcfile_y.y.
 * (Licensed under the terms of the GNU GPL).
 */

int check_secure(const char *pathname, char *errstr)
{
#if defined(__MINGW32__) || defined(DJGPP)
    
    return PATHS_EOK;

#else /* UNIX */

    struct stat statbuf;

    if (stat(pathname, &statbuf) < 0)
    {
	snprintf(errstr, errstr_bufsize, "%s", strerror(errno));
	return PATHS_EUNKNOWN;
    }
    
    if (!S_ISREG(statbuf.st_mode))
    {
	snprintf(errstr, errstr_bufsize, "must be a regular file");
	return PATHS_EINSECURE;
    }
    if (statbuf.st_mode & (S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP 
		| S_IROTH | S_IWOTH | S_IXOTH))
    {
	snprintf(errstr, errstr_bufsize, 
		"must have no more than user read/write permissions");
	return PATHS_EINSECURE;
    }
    if (statbuf.st_uid != geteuid())
    {
	snprintf(errstr, errstr_bufsize, "must be owned by you");
	return PATHS_EINSECURE;
    }

    return PATHS_EOK;

#endif /* UNIX */
}
