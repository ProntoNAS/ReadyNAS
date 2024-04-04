/*
   Unix SMB/CIFS implementation.
   Build Options for Samba Suite
   Copyright (C) Vance Lankhaar <vlankhaar@linux.ca> 2003
   Copyright (C) Andrew Bartlett <abartlet@samba.org> 2001

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"
#include "dynconfig/dynconfig.h"
#include "lib/cluster_support.h"

static int output(bool screen, const char *format, ...) PRINTF_ATTRIBUTE(2,3);
void build_options(bool screen);


/****************************************************************************
helper function for build_options
****************************************************************************/
static int output(bool screen, const char *format, ...)
{
       char *ptr = NULL;
       int ret = 0;
       va_list ap;
       
       va_start(ap, format);
       ret = vasprintf(&ptr,format,ap);
       va_end(ap);

       if (screen) {
              d_printf("%s", ptr ? ptr : "");
       } else {
              DEBUG(4,("%s", ptr ? ptr : ""));
       }
       
       SAFE_FREE(ptr);
       return ret;
}

/****************************************************************************
options set at build time for the samba suite
****************************************************************************/
void build_options(bool screen)
{
       if ((DEBUGLEVEL < 4) && (!screen)) {
              return;
       }


       /* Output various paths to files and directories */
       output(screen,"\nPaths:\n");
       output(screen,"   SBINDIR: %s\n", get_dyn_SBINDIR());
       output(screen,"   BINDIR: %s\n", get_dyn_BINDIR());
       output(screen,"   CONFIGFILE: %s\n", get_dyn_CONFIGFILE());
       output(screen,"   LOGFILEBASE: %s\n", get_dyn_LOGFILEBASE());
       output(screen,"   LMHOSTSFILE: %s\n",get_dyn_LMHOSTSFILE());
       output(screen,"   LIBDIR: %s\n",get_dyn_LIBDIR());
       output(screen,"   MODULESDIR: %s\n",get_dyn_MODULESDIR());
       output(screen,"   SHLIBEXT: %s\n",get_dyn_SHLIBEXT());
       output(screen,"   LOCKDIR: %s\n",get_dyn_LOCKDIR());
       output(screen,"   STATEDIR: %s\n",get_dyn_STATEDIR());
       output(screen,"   CACHEDIR: %s\n",get_dyn_CACHEDIR());
       output(screen,"   PIDDIR: %s\n", get_dyn_PIDDIR());
       output(screen,"   SMB_PASSWD_FILE: %s\n",get_dyn_SMB_PASSWD_FILE());
       output(screen,"   PRIVATE_DIR: %s\n",get_dyn_PRIVATE_DIR());
       output(screen,"   BINDDNS_DIR: %s\n",get_dyn_BINDDNS_DIR());


	/* Show System Headers */
       output(screen, "\nSystem Headers:\n");

#ifdef HAVE_SYS_ACL_H
       output(screen, "   HAVE_SYS_ACL_H\n");
#endif
#ifdef HAVE_SYS_AUXV_H
       output(screen, "   HAVE_SYS_AUXV_H\n");
#endif
#ifdef HAVE_SYS_CAPABILITY_H
       output(screen, "   HAVE_SYS_CAPABILITY_H\n");
#endif
#ifdef HAVE_SYS_CDEFS_H
       output(screen, "   HAVE_SYS_CDEFS_H\n");
#endif
#ifdef HAVE_SYS_DIR_H
       output(screen, "   HAVE_SYS_DIR_H\n");
#endif
#ifdef HAVE_SYS_EPOLL_H
       output(screen, "   HAVE_SYS_EPOLL_H\n");
#endif
#ifdef HAVE_SYS_EVENTFD_H
       output(screen, "   HAVE_SYS_EVENTFD_H\n");
#endif
#ifdef HAVE_SYS_FCNTL_H
       output(screen, "   HAVE_SYS_FCNTL_H\n");
#endif
#ifdef HAVE_SYS_FILE_H
       output(screen, "   HAVE_SYS_FILE_H\n");
#endif
#ifdef HAVE_SYS_INOTIFY_H
       output(screen, "   HAVE_SYS_INOTIFY_H\n");
#endif
#ifdef HAVE_SYS_IOCTL_H
       output(screen, "   HAVE_SYS_IOCTL_H\n");
#endif
#ifdef HAVE_SYS_IPC_H
       output(screen, "   HAVE_SYS_IPC_H\n");
#endif
#ifdef HAVE_SYS_MMAN_H
       output(screen, "   HAVE_SYS_MMAN_H\n");
#endif
#ifdef HAVE_SYS_MOUNT_H
       output(screen, "   HAVE_SYS_MOUNT_H\n");
#endif
#ifdef HAVE_SYS_PARAM_H
       output(screen, "   HAVE_SYS_PARAM_H\n");
#endif
#ifdef HAVE_SYS_PRCTL_H
       output(screen, "   HAVE_SYS_PRCTL_H\n");
#endif
#ifdef HAVE_SYS_RESOURCE_H
       output(screen, "   HAVE_SYS_RESOURCE_H\n");
#endif
#ifdef HAVE_SYS_SELECT_H
       output(screen, "   HAVE_SYS_SELECT_H\n");
#endif
#ifdef HAVE_SYS_SENDFILE_H
       output(screen, "   HAVE_SYS_SENDFILE_H\n");
#endif
#ifdef HAVE_SYS_SHM_H
       output(screen, "   HAVE_SYS_SHM_H\n");
#endif
#ifdef HAVE_SYS_SOCKET_H
       output(screen, "   HAVE_SYS_SOCKET_H\n");
#endif
#ifdef HAVE_SYS_STATFS_H
       output(screen, "   HAVE_SYS_STATFS_H\n");
#endif
#ifdef HAVE_SYS_STATVFS_H
       output(screen, "   HAVE_SYS_STATVFS_H\n");
#endif
#ifdef HAVE_SYS_STAT_H
       output(screen, "   HAVE_SYS_STAT_H\n");
#endif
#ifdef HAVE_SYS_SYSCALL_H
       output(screen, "   HAVE_SYS_SYSCALL_H\n");
#endif
#ifdef HAVE_SYS_SYSCTL_H
       output(screen, "   HAVE_SYS_SYSCTL_H\n");
#endif
#ifdef HAVE_SYS_SYSLOG_H
       output(screen, "   HAVE_SYS_SYSLOG_H\n");
#endif
#ifdef HAVE_SYS_SYSMACROS_H
       output(screen, "   HAVE_SYS_SYSMACROS_H\n");
#endif
#ifdef HAVE_SYS_TIME_H
       output(screen, "   HAVE_SYS_TIME_H\n");
#endif
#ifdef HAVE_SYS_TYPES_H
       output(screen, "   HAVE_SYS_TYPES_H\n");
#endif
#ifdef HAVE_SYS_UCONTEXT_H
       output(screen, "   HAVE_SYS_UCONTEXT_H\n");
#endif
#ifdef HAVE_SYS_UIO_H
       output(screen, "   HAVE_SYS_UIO_H\n");
#endif
#ifdef HAVE_SYS_UNISTD_H
       output(screen, "   HAVE_SYS_UNISTD_H\n");
#endif
#ifdef HAVE_SYS_UN_H
       output(screen, "   HAVE_SYS_UN_H\n");
#endif
#ifdef HAVE_SYS_UTSNAME_H
       output(screen, "   HAVE_SYS_UTSNAME_H\n");
#endif
#ifdef HAVE_SYS_VFS_H
       output(screen, "   HAVE_SYS_VFS_H\n");
#endif
#ifdef HAVE_SYS_WAIT_H
       output(screen, "   HAVE_SYS_WAIT_H\n");
#endif
#ifdef HAVE_SYS_XATTR_H
       output(screen, "   HAVE_SYS_XATTR_H\n");
#endif


	/* Show Headers */
       output(screen, "\nHeaders:\n");

#ifdef HAVE_ACL_LIBACL_H
       output(screen, "   HAVE_ACL_LIBACL_H\n");
#endif
#ifdef HAVE_AIO_H
       output(screen, "   HAVE_AIO_H\n");
#endif
#ifdef HAVE_ALLOCA_H
       output(screen, "   HAVE_ALLOCA_H\n");
#endif
#ifdef HAVE_ARCHIVE_H
       output(screen, "   HAVE_ARCHIVE_H\n");
#endif
#ifdef HAVE_ARPA_INET_H
       output(screen, "   HAVE_ARPA_INET_H\n");
#endif
#ifdef HAVE_ARPA_NAMESER_H
       output(screen, "   HAVE_ARPA_NAMESER_H\n");
#endif
#ifdef HAVE_ASM_TYPES_H
       output(screen, "   HAVE_ASM_TYPES_H\n");
#endif
#ifdef HAVE_ASM_UNISTD_H
       output(screen, "   HAVE_ASM_UNISTD_H\n");
#endif
#ifdef HAVE_ASSERT_H
       output(screen, "   HAVE_ASSERT_H\n");
#endif
#ifdef HAVE_ATTR_ATTRIBUTES_H
       output(screen, "   HAVE_ATTR_ATTRIBUTES_H\n");
#endif
#ifdef HAVE_ATTR_XATTR_H
       output(screen, "   HAVE_ATTR_XATTR_H\n");
#endif
#ifdef HAVE_AVAHI_CLIENT_CLIENT_H
       output(screen, "   HAVE_AVAHI_CLIENT_CLIENT_H\n");
#endif
#ifdef HAVE_AVAHI_COMMON_WATCH_H
       output(screen, "   HAVE_AVAHI_COMMON_WATCH_H\n");
#endif
#ifdef HAVE_BSD_LIBUTIL_H
       output(screen, "   HAVE_BSD_LIBUTIL_H\n");
#endif
#ifdef HAVE_BSD_MD5_H
       output(screen, "   HAVE_BSD_MD5_H\n");
#endif
#ifdef HAVE_BSD_STRING_H
       output(screen, "   HAVE_BSD_STRING_H\n");
#endif
#ifdef HAVE_BSD_UNISTD_H
       output(screen, "   HAVE_BSD_UNISTD_H\n");
#endif
#ifdef HAVE_COM_ERR_H
       output(screen, "   HAVE_COM_ERR_H\n");
#endif
#ifdef HAVE_CONFIG_H
       output(screen, "   HAVE_CONFIG_H\n");
#endif
#ifdef HAVE_CRYPT_H
       output(screen, "   HAVE_CRYPT_H\n");
#endif
#ifdef HAVE_CTYPE_H
       output(screen, "   HAVE_CTYPE_H\n");
#endif
#ifdef HAVE_CUPS_CUPS_H
       output(screen, "   HAVE_CUPS_CUPS_H\n");
#endif
#ifdef HAVE_CUPS_LANGUAGE_H
       output(screen, "   HAVE_CUPS_LANGUAGE_H\n");
#endif
#ifdef HAVE_DBUS_DBUS_H
       output(screen, "   HAVE_DBUS_DBUS_H\n");
#endif
#ifdef HAVE_DIRENT_H
       output(screen, "   HAVE_DIRENT_H\n");
#endif
#ifdef HAVE_DLFCN_H
       output(screen, "   HAVE_DLFCN_H\n");
#endif
#ifdef HAVE_ENDIAN_H
       output(screen, "   HAVE_ENDIAN_H\n");
#endif
#ifdef HAVE_ERRNO_H
       output(screen, "   HAVE_ERRNO_H\n");
#endif
#ifdef HAVE_EXECINFO_H
       output(screen, "   HAVE_EXECINFO_H\n");
#endif
#ifdef HAVE_FCNTL_H
       output(screen, "   HAVE_FCNTL_H\n");
#endif
#ifdef HAVE_FLOAT_H
       output(screen, "   HAVE_FLOAT_H\n");
#endif
#ifdef HAVE_FNMATCH_H
       output(screen, "   HAVE_FNMATCH_H\n");
#endif
#ifdef HAVE_FORM_H
       output(screen, "   HAVE_FORM_H\n");
#endif
#ifdef HAVE_FTW_H
       output(screen, "   HAVE_FTW_H\n");
#endif
#ifdef HAVE_FUSE_FUSE_LOWLEVEL_H
       output(screen, "   HAVE_FUSE_FUSE_LOWLEVEL_H\n");
#endif
#ifdef HAVE_GETOPT_H
       output(screen, "   HAVE_GETOPT_H\n");
#endif
#ifdef HAVE_GLOB_H
       output(screen, "   HAVE_GLOB_H\n");
#endif
#ifdef HAVE_GNUTLS_GNUTLS_H
       output(screen, "   HAVE_GNUTLS_GNUTLS_H\n");
#endif
#ifdef HAVE_GNUTLS_X509_H
       output(screen, "   HAVE_GNUTLS_X509_H\n");
#endif
#ifdef HAVE_GRP_H
       output(screen, "   HAVE_GRP_H\n");
#endif
#ifdef HAVE_GSSAPI_GSSAPI_EXT_H
       output(screen, "   HAVE_GSSAPI_GSSAPI_EXT_H\n");
#endif
#ifdef HAVE_GSSAPI_GSSAPI_GENERIC_H
       output(screen, "   HAVE_GSSAPI_GSSAPI_GENERIC_H\n");
#endif
#ifdef HAVE_GSSAPI_GSSAPI_H
       output(screen, "   HAVE_GSSAPI_GSSAPI_H\n");
#endif
#ifdef HAVE_GSSAPI_GSSAPI_KRB5_H
       output(screen, "   HAVE_GSSAPI_GSSAPI_KRB5_H\n");
#endif
#ifdef HAVE_GSSAPI_H
       output(screen, "   HAVE_GSSAPI_H\n");
#endif
#ifdef HAVE_ICONV_H
       output(screen, "   HAVE_ICONV_H\n");
#endif
#ifdef HAVE_IFADDRS_H
       output(screen, "   HAVE_IFADDRS_H\n");
#endif
#ifdef HAVE_INTTYPES_H
       output(screen, "   HAVE_INTTYPES_H\n");
#endif
#ifdef HAVE_KDB_H
       output(screen, "   HAVE_KDB_H\n");
#endif
#ifdef HAVE_KRB5_H
       output(screen, "   HAVE_KRB5_H\n");
#endif
#ifdef HAVE_KRB5_LOCALAUTH_PLUGIN_H
       output(screen, "   HAVE_KRB5_LOCALAUTH_PLUGIN_H\n");
#endif
#ifdef HAVE_KRB5_LOCATE_PLUGIN_H
       output(screen, "   HAVE_KRB5_LOCATE_PLUGIN_H\n");
#endif
#ifdef HAVE_LANGINFO_H
       output(screen, "   HAVE_LANGINFO_H\n");
#endif
#ifdef HAVE_LASTLOG_H
       output(screen, "   HAVE_LASTLOG_H\n");
#endif
#ifdef HAVE_LBER_H
       output(screen, "   HAVE_LBER_H\n");
#endif
#ifdef HAVE_LDAP_H
       output(screen, "   HAVE_LDAP_H\n");
#endif
#ifdef HAVE_LIBAIO_H
       output(screen, "   HAVE_LIBAIO_H\n");
#endif
#ifdef HAVE_LIBGEN_H
       output(screen, "   HAVE_LIBGEN_H\n");
#endif
#ifdef HAVE_LIBINTL_H
       output(screen, "   HAVE_LIBINTL_H\n");
#endif
#ifdef HAVE_LIMITS_H
       output(screen, "   HAVE_LIMITS_H\n");
#endif
#ifdef HAVE_LINUX_ETHTOOL_H
       output(screen, "   HAVE_LINUX_ETHTOOL_H\n");
#endif
#ifdef HAVE_LINUX_FALLOC_H
       output(screen, "   HAVE_LINUX_FALLOC_H\n");
#endif
#ifdef HAVE_LINUX_FCNTL_H
       output(screen, "   HAVE_LINUX_FCNTL_H\n");
#endif
#ifdef HAVE_LINUX_FS_H
       output(screen, "   HAVE_LINUX_FS_H\n");
#endif
#ifdef HAVE_LINUX_IOCTL_H
       output(screen, "   HAVE_LINUX_IOCTL_H\n");
#endif
#ifdef HAVE_LINUX_NETLINK_H
       output(screen, "   HAVE_LINUX_NETLINK_H\n");
#endif
#ifdef HAVE_LINUX_RTNETLINK_H
       output(screen, "   HAVE_LINUX_RTNETLINK_H\n");
#endif
#ifdef HAVE_LINUX_SOCKIOS_H
       output(screen, "   HAVE_LINUX_SOCKIOS_H\n");
#endif
#ifdef HAVE_LINUX_TYPES_H
       output(screen, "   HAVE_LINUX_TYPES_H\n");
#endif
#ifdef HAVE_LOCALE_H
       output(screen, "   HAVE_LOCALE_H\n");
#endif
#ifdef HAVE_MALLOC_H
       output(screen, "   HAVE_MALLOC_H\n");
#endif
#ifdef HAVE_MEMORY_H
       output(screen, "   HAVE_MEMORY_H\n");
#endif
#ifdef HAVE_MENU_H
       output(screen, "   HAVE_MENU_H\n");
#endif
#ifdef HAVE_MNTENT_H
       output(screen, "   HAVE_MNTENT_H\n");
#endif
#ifdef HAVE_NCURSES_H
       output(screen, "   HAVE_NCURSES_H\n");
#endif
#ifdef HAVE_NETDB_H
       output(screen, "   HAVE_NETDB_H\n");
#endif
#ifdef HAVE_NETINET_IN_H
       output(screen, "   HAVE_NETINET_IN_H\n");
#endif
#ifdef HAVE_NETINET_IN_SYSTM_H
       output(screen, "   HAVE_NETINET_IN_SYSTM_H\n");
#endif
#ifdef HAVE_NETINET_IP_H
       output(screen, "   HAVE_NETINET_IP_H\n");
#endif
#ifdef HAVE_NETINET_TCP_H
       output(screen, "   HAVE_NETINET_TCP_H\n");
#endif
#ifdef HAVE_NET_IF_H
       output(screen, "   HAVE_NET_IF_H\n");
#endif
#ifdef HAVE_NSS_H
       output(screen, "   HAVE_NSS_H\n");
#endif
#ifdef HAVE_PANEL_H
       output(screen, "   HAVE_PANEL_H\n");
#endif
#ifdef HAVE_POLL_H
       output(screen, "   HAVE_POLL_H\n");
#endif
#ifdef HAVE_POPT_H
       output(screen, "   HAVE_POPT_H\n");
#endif
#ifdef HAVE_PROFILE_H
       output(screen, "   HAVE_PROFILE_H\n");
#endif
#ifdef HAVE_PTHREAD_H
       output(screen, "   HAVE_PTHREAD_H\n");
#endif
#ifdef HAVE_PTY_H
       output(screen, "   HAVE_PTY_H\n");
#endif
#ifdef HAVE_PWD_H
       output(screen, "   HAVE_PWD_H\n");
#endif
#ifdef HAVE_PYTHON_H
       output(screen, "   HAVE_PYTHON_H\n");
#endif
#ifdef HAVE_READLINE_HISTORY_H
       output(screen, "   HAVE_READLINE_HISTORY_H\n");
#endif
#ifdef HAVE_READLINE_READLINE_H
       output(screen, "   HAVE_READLINE_READLINE_H\n");
#endif
#ifdef HAVE_RESOLV_H
       output(screen, "   HAVE_RESOLV_H\n");
#endif
#ifdef HAVE_RPCSVC_NIS_H
       output(screen, "   HAVE_RPCSVC_NIS_H\n");
#endif
#ifdef HAVE_RPCSVC_YPCLNT_H
       output(screen, "   HAVE_RPCSVC_YPCLNT_H\n");
#endif
#ifdef HAVE_RPCSVC_YP_PROT_H
       output(screen, "   HAVE_RPCSVC_YP_PROT_H\n");
#endif
#ifdef HAVE_RPC_RPC_H
       output(screen, "   HAVE_RPC_RPC_H\n");
#endif
#ifdef HAVE_RPC_XDR_H
       output(screen, "   HAVE_RPC_XDR_H\n");
#endif
#ifdef HAVE_SECURITY_PAM_APPL_H
       output(screen, "   HAVE_SECURITY_PAM_APPL_H\n");
#endif
#ifdef HAVE_SECURITY_PAM_EXT_H
       output(screen, "   HAVE_SECURITY_PAM_EXT_H\n");
#endif
#ifdef HAVE_SECURITY_PAM_MODULES_H
       output(screen, "   HAVE_SECURITY_PAM_MODULES_H\n");
#endif
#ifdef HAVE_SECURITY__PAM_MACROS_H
       output(screen, "   HAVE_SECURITY__PAM_MACROS_H\n");
#endif
#ifdef HAVE_SETJMP_H
       output(screen, "   HAVE_SETJMP_H\n");
#endif
#ifdef HAVE_SHADOW_H
       output(screen, "   HAVE_SHADOW_H\n");
#endif
#ifdef HAVE_SIGNAL_H
       output(screen, "   HAVE_SIGNAL_H\n");
#endif
#ifdef HAVE_STDARG_H
       output(screen, "   HAVE_STDARG_H\n");
#endif
#ifdef HAVE_STDBOOL_H
       output(screen, "   HAVE_STDBOOL_H\n");
#endif
#ifdef HAVE_STDDEF_H
       output(screen, "   HAVE_STDDEF_H\n");
#endif
#ifdef HAVE_STDINT_H
       output(screen, "   HAVE_STDINT_H\n");
#endif
#ifdef HAVE_STDIO_H
       output(screen, "   HAVE_STDIO_H\n");
#endif
#ifdef HAVE_STDLIB_H
       output(screen, "   HAVE_STDLIB_H\n");
#endif
#ifdef HAVE_STRINGS_H
       output(screen, "   HAVE_STRINGS_H\n");
#endif
#ifdef HAVE_STRING_H
       output(screen, "   HAVE_STRING_H\n");
#endif
#ifdef HAVE_STROPTS_H
       output(screen, "   HAVE_STROPTS_H\n");
#endif
#ifdef HAVE_SYSCALL_H
       output(screen, "   HAVE_SYSCALL_H\n");
#endif
#ifdef HAVE_SYSLOG_H
       output(screen, "   HAVE_SYSLOG_H\n");
#endif
#ifdef HAVE_TERMIOS_H
       output(screen, "   HAVE_TERMIOS_H\n");
#endif
#ifdef HAVE_TERMIO_H
       output(screen, "   HAVE_TERMIO_H\n");
#endif
#ifdef HAVE_TIME_H
       output(screen, "   HAVE_TIME_H\n");
#endif
#ifdef HAVE_UNISTD_H
       output(screen, "   HAVE_UNISTD_H\n");
#endif
#ifdef HAVE_UTIME_H
       output(screen, "   HAVE_UTIME_H\n");
#endif
#ifdef HAVE_ZLIB_H
       output(screen, "   HAVE_ZLIB_H\n");
#endif


	/* Show UTMP Options */
       output(screen, "\nUTMP Options:\n");

#ifdef HAVE_GETUTMPX
       output(screen, "   HAVE_GETUTMPX\n");
#endif
#ifdef HAVE_UTMPX_H
       output(screen, "   HAVE_UTMPX_H\n");
#endif
#ifdef HAVE_UTMP_H
       output(screen, "   HAVE_UTMP_H\n");
#endif
#ifdef HAVE_UT_UT_EXIT
       output(screen, "   HAVE_UT_UT_EXIT\n");
#endif
#ifdef HAVE_UT_UT_HOST
       output(screen, "   HAVE_UT_UT_HOST\n");
#endif
#ifdef HAVE_UT_UT_ID
       output(screen, "   HAVE_UT_UT_ID\n");
#endif
#ifdef HAVE_UT_UT_NAME
       output(screen, "   HAVE_UT_UT_NAME\n");
#endif
#ifdef HAVE_UT_UT_PID
       output(screen, "   HAVE_UT_UT_PID\n");
#endif
#ifdef HAVE_UT_UT_TIME
       output(screen, "   HAVE_UT_UT_TIME\n");
#endif
#ifdef HAVE_UT_UT_TV
       output(screen, "   HAVE_UT_UT_TV\n");
#endif
#ifdef HAVE_UT_UT_TYPE
       output(screen, "   HAVE_UT_UT_TYPE\n");
#endif
#ifdef HAVE_UT_UT_USER
       output(screen, "   HAVE_UT_UT_USER\n");
#endif
#ifdef PUTUTLINE_RETURNS_UTMP
       output(screen, "   PUTUTLINE_RETURNS_UTMP\n");
#endif
#ifdef SIZEOF_UTMP_UT_LINE
       output(screen, "   SIZEOF_UTMP_UT_LINE\n");
#endif
#ifdef WITH_UTMP
       output(screen, "   WITH_UTMP\n");
#endif


	/* Show HAVE_* Defines */
       output(screen, "\nHAVE_* Defines:\n");

#ifdef HAVE_ACL_EVERYONE
       output(screen, "   HAVE_ACL_EVERYONE\n");
#endif
#ifdef HAVE_ACL_GET_FILE
       output(screen, "   HAVE_ACL_GET_FILE\n");
#endif
#ifdef HAVE_ACL_GET_PERM_NP
       output(screen, "   HAVE_ACL_GET_PERM_NP\n");
#endif
#ifdef HAVE_ADDRTYPE_IN_KRB5_ADDRESS
       output(screen, "   HAVE_ADDRTYPE_IN_KRB5_ADDRESS\n");
#endif
#ifdef HAVE_AESNI_INTEL
       output(screen, "   HAVE_AESNI_INTEL\n");
#endif
#ifdef HAVE_AP_OPTS_USE_SUBKEY
       output(screen, "   HAVE_AP_OPTS_USE_SUBKEY\n");
#endif
#ifdef HAVE_ARCHIVE_READ_FREE
       output(screen, "   HAVE_ARCHIVE_READ_FREE\n");
#endif
#ifdef HAVE_ARCHIVE_READ_SUPPORT_FILTER_ALL
       output(screen, "   HAVE_ARCHIVE_READ_SUPPORT_FILTER_ALL\n");
#endif
#ifdef HAVE_ASPRINTF
       output(screen, "   HAVE_ASPRINTF\n");
#endif
#ifdef HAVE_ATEXIT
       output(screen, "   HAVE_ATEXIT\n");
#endif
#ifdef HAVE_ATOMIC_ADD_32
       output(screen, "   HAVE_ATOMIC_ADD_32\n");
#endif
#ifdef HAVE_AUDIT
       output(screen, "   HAVE_AUDIT\n");
#endif
#ifdef HAVE_AVAHI_CLIENT_NEW
       output(screen, "   HAVE_AVAHI_CLIENT_NEW\n");
#endif
#ifdef HAVE_AVAHI_STRERROR
       output(screen, "   HAVE_AVAHI_STRERROR\n");
#endif
#ifdef HAVE_BACKTRACE
       output(screen, "   HAVE_BACKTRACE\n");
#endif
#ifdef HAVE_BACKTRACE_SYMBOLS
       output(screen, "   HAVE_BACKTRACE_SYMBOLS\n");
#endif
#ifdef HAVE_BASENAME
       output(screen, "   HAVE_BASENAME\n");
#endif
#ifdef HAVE_BER_SCANF
       output(screen, "   HAVE_BER_SCANF\n");
#endif
#ifdef HAVE_BER_SOCKBUF_ADD_IO
       output(screen, "   HAVE_BER_SOCKBUF_ADD_IO\n");
#endif
#ifdef HAVE_BER_TAG_T
       output(screen, "   HAVE_BER_TAG_T\n");
#endif
#ifdef HAVE_BIG_ENDIAN
       output(screen, "   HAVE_BIG_ENDIAN\n");
#endif
#ifdef HAVE_BINDTEXTDOMAIN
       output(screen, "   HAVE_BINDTEXTDOMAIN\n");
#endif
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
       output(screen, "   HAVE_BIND_TEXTDOMAIN_CODESET\n");
#endif
#ifdef HAVE_BLKCNT_T
       output(screen, "   HAVE_BLKCNT_T\n");
#endif
#ifdef HAVE_BLKSIZE_T
       output(screen, "   HAVE_BLKSIZE_T\n");
#endif
#ifdef HAVE_BOOL
       output(screen, "   HAVE_BOOL\n");
#endif
#ifdef HAVE_BSD_STRTOLL
       output(screen, "   HAVE_BSD_STRTOLL\n");
#endif
#ifdef HAVE_BZERO
       output(screen, "   HAVE_BZERO\n");
#endif
#ifdef HAVE_C99_VSNPRINTF
       output(screen, "   HAVE_C99_VSNPRINTF\n");
#endif
#ifdef HAVE_CAP_GET_PROC
       output(screen, "   HAVE_CAP_GET_PROC\n");
#endif
#ifdef HAVE_CC_MD5_INIT
       output(screen, "   HAVE_CC_MD5_INIT\n");
#endif
#ifdef HAVE_CHARSET_CP850
       output(screen, "   HAVE_CHARSET_CP850\n");
#endif
#ifdef HAVE_CHARSET_UTF_8
       output(screen, "   HAVE_CHARSET_UTF_8\n");
#endif
#ifdef HAVE_CHFLAGS
       output(screen, "   HAVE_CHFLAGS\n");
#endif
#ifdef HAVE_CHMOD
       output(screen, "   HAVE_CHMOD\n");
#endif
#ifdef HAVE_CHOWN
       output(screen, "   HAVE_CHOWN\n");
#endif
#ifdef HAVE_CHROOT
       output(screen, "   HAVE_CHROOT\n");
#endif
#ifdef HAVE_CHSIZE
       output(screen, "   HAVE_CHSIZE\n");
#endif
#ifdef HAVE_CLEARENV
       output(screen, "   HAVE_CLEARENV\n");
#endif
#ifdef HAVE_CLOCK_GETTIME
       output(screen, "   HAVE_CLOCK_GETTIME\n");
#endif
#ifdef HAVE_CLOCK_MONOTONIC
       output(screen, "   HAVE_CLOCK_MONOTONIC\n");
#endif
#ifdef HAVE_CLOCK_PROCESS_CPUTIME_ID
       output(screen, "   HAVE_CLOCK_PROCESS_CPUTIME_ID\n");
#endif
#ifdef HAVE_CLOCK_REALTIME
       output(screen, "   HAVE_CLOCK_REALTIME\n");
#endif
#ifdef HAVE_CLOSEFROM
       output(screen, "   HAVE_CLOSEFROM\n");
#endif
#ifdef HAVE_COMPARISON_FN_T
       output(screen, "   HAVE_COMPARISON_FN_T\n");
#endif
#ifdef HAVE_COMPILER_WILL_OPTIMIZE_OUT_FNS
       output(screen, "   HAVE_COMPILER_WILL_OPTIMIZE_OUT_FNS\n");
#endif
#ifdef HAVE_COM_ERR
       output(screen, "   HAVE_COM_ERR\n");
#endif
#ifdef HAVE_CONNECT
       output(screen, "   HAVE_CONNECT\n");
#endif
#ifdef HAVE_CONSTRUCTOR_ATTRIBUTE
       output(screen, "   HAVE_CONSTRUCTOR_ATTRIBUTE\n");
#endif
#ifdef HAVE_COPY_AUTHENTICATOR
       output(screen, "   HAVE_COPY_AUTHENTICATOR\n");
#endif
#ifdef HAVE_CPPFUNCTION
       output(screen, "   HAVE_CPPFUNCTION\n");
#endif
#ifdef HAVE_CRYPT
       output(screen, "   HAVE_CRYPT\n");
#endif
#ifdef HAVE_CRYPT16
       output(screen, "   HAVE_CRYPT16\n");
#endif
#ifdef HAVE_CRYPT_R
       output(screen, "   HAVE_CRYPT_R\n");
#endif
#ifdef HAVE_CUPS
       output(screen, "   HAVE_CUPS\n");
#endif
#ifdef HAVE_DARWIN_INITGROUPS
       output(screen, "   HAVE_DARWIN_INITGROUPS\n");
#endif
#ifdef HAVE_DBUS
       output(screen, "   HAVE_DBUS\n");
#endif
#ifdef HAVE_DBUS_1
       output(screen, "   HAVE_DBUS_1\n");
#endif
#ifdef HAVE_DECL_ASPRINTF
       output(screen, "   HAVE_DECL_ASPRINTF\n");
#endif
#ifdef HAVE_DECL_BINDTEXTDOMAIN
       output(screen, "   HAVE_DECL_BINDTEXTDOMAIN\n");
#endif
#ifdef HAVE_DECL_BIND_TEXTDOMAIN_CODESET
       output(screen, "   HAVE_DECL_BIND_TEXTDOMAIN_CODESET\n");
#endif
#ifdef HAVE_DECL_DGETTEXT
       output(screen, "   HAVE_DECL_DGETTEXT\n");
#endif
#ifdef HAVE_DECL_DLOPEN
       output(screen, "   HAVE_DECL_DLOPEN\n");
#endif
#ifdef HAVE_DECL_EWOULDBLOCK
       output(screen, "   HAVE_DECL_EWOULDBLOCK\n");
#endif
#ifdef HAVE_DECL_FDATASYNC
       output(screen, "   HAVE_DECL_FDATASYNC\n");
#endif
#ifdef HAVE_DECL_FS_COMPR_FL
       output(screen, "   HAVE_DECL_FS_COMPR_FL\n");
#endif
#ifdef HAVE_DECL_FS_IOC_GETFLAGS
       output(screen, "   HAVE_DECL_FS_IOC_GETFLAGS\n");
#endif
#ifdef HAVE_DECL_GETGRENT_R
       output(screen, "   HAVE_DECL_GETGRENT_R\n");
#endif
#ifdef HAVE_DECL_GETPWENT_R
       output(screen, "   HAVE_DECL_GETPWENT_R\n");
#endif
#ifdef HAVE_DECL_GETTEXT
       output(screen, "   HAVE_DECL_GETTEXT\n");
#endif
#ifdef HAVE_DECL_GETTIMEOFDAY
       output(screen, "   HAVE_DECL_GETTIMEOFDAY\n");
#endif
#ifdef HAVE_DECL_GNUTLS_CERT_EXPIRED
       output(screen, "   HAVE_DECL_GNUTLS_CERT_EXPIRED\n");
#endif
#ifdef HAVE_DECL_GNUTLS_CERT_NOT_ACTIVATED
       output(screen, "   HAVE_DECL_GNUTLS_CERT_NOT_ACTIVATED\n");
#endif
#ifdef HAVE_DECL_GNUTLS_CERT_UNEXPECTED_OWNER
       output(screen, "   HAVE_DECL_GNUTLS_CERT_UNEXPECTED_OWNER\n");
#endif
#ifdef HAVE_DECL_KRB5_AUTH_CON_SET_REQ_CKSUMTYPE
       output(screen, "   HAVE_DECL_KRB5_AUTH_CON_SET_REQ_CKSUMTYPE\n");
#endif
#ifdef HAVE_DECL_KRB5_GET_CREDENTIALS_FOR_USER
       output(screen, "   HAVE_DECL_KRB5_GET_CREDENTIALS_FOR_USER\n");
#endif
#ifdef HAVE_DECL_MALLOC
       output(screen, "   HAVE_DECL_MALLOC\n");
#endif
#ifdef HAVE_DECL_MEMALIGN
       output(screen, "   HAVE_DECL_MEMALIGN\n");
#endif
#ifdef HAVE_DECL_PTHREAD_MUTEX_ROBUST
       output(screen, "   HAVE_DECL_PTHREAD_MUTEX_ROBUST\n");
#endif
#ifdef HAVE_DECL_READAHEAD
       output(screen, "   HAVE_DECL_READAHEAD\n");
#endif
#ifdef HAVE_DECL_RL_EVENT_HOOK
       output(screen, "   HAVE_DECL_RL_EVENT_HOOK\n");
#endif
#ifdef HAVE_DECL_SNPRINTF
       output(screen, "   HAVE_DECL_SNPRINTF\n");
#endif
#ifdef HAVE_DECL_STRPTIME
       output(screen, "   HAVE_DECL_STRPTIME\n");
#endif
#ifdef HAVE_DECL_TEXTDOMAIN
       output(screen, "   HAVE_DECL_TEXTDOMAIN\n");
#endif
#ifdef HAVE_DECL_VASPRINTF
       output(screen, "   HAVE_DECL_VASPRINTF\n");
#endif
#ifdef HAVE_DECL_VSNPRINTF
       output(screen, "   HAVE_DECL_VSNPRINTF\n");
#endif
#ifdef HAVE_DESTRUCTOR_ATTRIBUTE
       output(screen, "   HAVE_DESTRUCTOR_ATTRIBUTE\n");
#endif
#ifdef HAVE_DES_PCBC_ENCRYPT
       output(screen, "   HAVE_DES_PCBC_ENCRYPT\n");
#endif
#ifdef HAVE_DES_SET_KEY
       output(screen, "   HAVE_DES_SET_KEY\n");
#endif
#ifdef HAVE_DEVICE_MAJOR_FN
       output(screen, "   HAVE_DEVICE_MAJOR_FN\n");
#endif
#ifdef HAVE_DEVICE_MINOR_FN
       output(screen, "   HAVE_DEVICE_MINOR_FN\n");
#endif
#ifdef HAVE_DEVNM
       output(screen, "   HAVE_DEVNM\n");
#endif
#ifdef HAVE_DGETTEXT
       output(screen, "   HAVE_DGETTEXT\n");
#endif
#ifdef HAVE_DIRENT_D_OFF
       output(screen, "   HAVE_DIRENT_D_OFF\n");
#endif
#ifdef HAVE_DIRFD
       output(screen, "   HAVE_DIRFD\n");
#endif
#ifdef HAVE_DIRFD_DECL
       output(screen, "   HAVE_DIRFD_DECL\n");
#endif
#ifdef HAVE_DIRNAME
       output(screen, "   HAVE_DIRNAME\n");
#endif
#ifdef HAVE_DLCLOSE
       output(screen, "   HAVE_DLCLOSE\n");
#endif
#ifdef HAVE_DLERROR
       output(screen, "   HAVE_DLERROR\n");
#endif
#ifdef HAVE_DLOPEN
       output(screen, "   HAVE_DLOPEN\n");
#endif
#ifdef HAVE_DLSYM
       output(screen, "   HAVE_DLSYM\n");
#endif
#ifdef HAVE_DM_GET_EVENTLIST
       output(screen, "   HAVE_DM_GET_EVENTLIST\n");
#endif
#ifdef HAVE_DNSSERVICEREGISTER
       output(screen, "   HAVE_DNSSERVICEREGISTER\n");
#endif
#ifdef HAVE_DN_EXPAND
       output(screen, "   HAVE_DN_EXPAND\n");
#endif
#ifdef HAVE_DPRINTF
       output(screen, "   HAVE_DPRINTF\n");
#endif
#ifdef HAVE_DUP2
       output(screen, "   HAVE_DUP2\n");
#endif
#ifdef HAVE_ENCTYPE_AES128_CTS_HMAC_SHA1_96
       output(screen, "   HAVE_ENCTYPE_AES128_CTS_HMAC_SHA1_96\n");
#endif
#ifdef HAVE_ENCTYPE_AES256_CTS_HMAC_SHA1_96
       output(screen, "   HAVE_ENCTYPE_AES256_CTS_HMAC_SHA1_96\n");
#endif
#ifdef HAVE_ENCTYPE_ARCFOUR_HMAC
       output(screen, "   HAVE_ENCTYPE_ARCFOUR_HMAC\n");
#endif
#ifdef HAVE_ENCTYPE_ARCFOUR_HMAC_EXP
       output(screen, "   HAVE_ENCTYPE_ARCFOUR_HMAC_EXP\n");
#endif
#ifdef HAVE_ENDMNTENT
       output(screen, "   HAVE_ENDMNTENT\n");
#endif
#ifdef HAVE_ENDNETGRENT
       output(screen, "   HAVE_ENDNETGRENT\n");
#endif
#ifdef HAVE_ENDNETGRENT_PROTOTYPE
       output(screen, "   HAVE_ENDNETGRENT_PROTOTYPE\n");
#endif
#ifdef HAVE_ENVIRON_DECL
       output(screen, "   HAVE_ENVIRON_DECL\n");
#endif
#ifdef HAVE_EPOLL
       output(screen, "   HAVE_EPOLL\n");
#endif
#ifdef HAVE_EPOLL_CREATE
       output(screen, "   HAVE_EPOLL_CREATE\n");
#endif
#ifdef HAVE_ERRNO_DECL
       output(screen, "   HAVE_ERRNO_DECL\n");
#endif
#ifdef HAVE_ETHTOOL
       output(screen, "   HAVE_ETHTOOL\n");
#endif
#ifdef HAVE_EVENTFD
       output(screen, "   HAVE_EVENTFD\n");
#endif
#ifdef HAVE_EXECL
       output(screen, "   HAVE_EXECL\n");
#endif
#ifdef HAVE_FALLOCATE
       output(screen, "   HAVE_FALLOCATE\n");
#endif
#ifdef HAVE_FALLOC_FL_PUNCH_HOLE
       output(screen, "   HAVE_FALLOC_FL_PUNCH_HOLE\n");
#endif
#ifdef HAVE_FCHMOD
       output(screen, "   HAVE_FCHMOD\n");
#endif
#ifdef HAVE_FCHOWN
       output(screen, "   HAVE_FCHOWN\n");
#endif
#ifdef HAVE_FCNTL_LOCK
       output(screen, "   HAVE_FCNTL_LOCK\n");
#endif
#ifdef HAVE_FCVT
       output(screen, "   HAVE_FCVT\n");
#endif
#ifdef HAVE_FCVTL
       output(screen, "   HAVE_FCVTL\n");
#endif
#ifdef HAVE_FDATASYNC
       output(screen, "   HAVE_FDATASYNC\n");
#endif
#ifdef HAVE_FDATASYNC_DECL
       output(screen, "   HAVE_FDATASYNC_DECL\n");
#endif
#ifdef HAVE_FDOPENDIR
       output(screen, "   HAVE_FDOPENDIR\n");
#endif
#ifdef HAVE_FGETXATTR
       output(screen, "   HAVE_FGETXATTR\n");
#endif
#ifdef HAVE_FLAGS_IN_KRB5_CREDS
       output(screen, "   HAVE_FLAGS_IN_KRB5_CREDS\n");
#endif
#ifdef HAVE_FLISTEA
       output(screen, "   HAVE_FLISTEA\n");
#endif
#ifdef HAVE_FLISTXATTR
       output(screen, "   HAVE_FLISTXATTR\n");
#endif
#ifdef HAVE_FREEADDRINFO
       output(screen, "   HAVE_FREEADDRINFO\n");
#endif
#ifdef HAVE_FREEIFADDRS
       output(screen, "   HAVE_FREEIFADDRS\n");
#endif
#ifdef HAVE_FREMOVEXATTR
       output(screen, "   HAVE_FREMOVEXATTR\n");
#endif
#ifdef HAVE_FRSIZE
       output(screen, "   HAVE_FRSIZE\n");
#endif
#ifdef HAVE_FSEEKO
       output(screen, "   HAVE_FSEEKO\n");
#endif
#ifdef HAVE_FSETXATTR
       output(screen, "   HAVE_FSETXATTR\n");
#endif
#ifdef HAVE_FSID_INT
       output(screen, "   HAVE_FSID_INT\n");
#endif
#ifdef HAVE_FSTATAT
       output(screen, "   HAVE_FSTATAT\n");
#endif
#ifdef HAVE_FSYNC
       output(screen, "   HAVE_FSYNC\n");
#endif
#ifdef HAVE_FTRUNCATE
       output(screen, "   HAVE_FTRUNCATE\n");
#endif
#ifdef HAVE_FTRUNCATE_EXTEND
       output(screen, "   HAVE_FTRUNCATE_EXTEND\n");
#endif
#ifdef HAVE_FUNCTION_MACRO
       output(screen, "   HAVE_FUNCTION_MACRO\n");
#endif
#ifdef HAVE_FUTIMENS
       output(screen, "   HAVE_FUTIMENS\n");
#endif
#ifdef HAVE_FUTIMES
       output(screen, "   HAVE_FUTIMES\n");
#endif
#ifdef HAVE_F_SETLEASE_DECL
       output(screen, "   HAVE_F_SETLEASE_DECL\n");
#endif
#ifdef HAVE_GAI_STRERROR
       output(screen, "   HAVE_GAI_STRERROR\n");
#endif
#ifdef HAVE_GCRY_CONTROL
       output(screen, "   HAVE_GCRY_CONTROL\n");
#endif
#ifdef HAVE_GETADDRINFO
       output(screen, "   HAVE_GETADDRINFO\n");
#endif
#ifdef HAVE_GETAUTHUID
       output(screen, "   HAVE_GETAUTHUID\n");
#endif
#ifdef HAVE_GETAUXVAL
       output(screen, "   HAVE_GETAUXVAL\n");
#endif
#ifdef HAVE_GETCWD
       output(screen, "   HAVE_GETCWD\n");
#endif
#ifdef HAVE_GETDENTS
       output(screen, "   HAVE_GETDENTS\n");
#endif
#ifdef HAVE_GETDIRENTRIES
       output(screen, "   HAVE_GETDIRENTRIES\n");
#endif
#ifdef HAVE_GETGRENT
       output(screen, "   HAVE_GETGRENT\n");
#endif
#ifdef HAVE_GETGRENT_R
       output(screen, "   HAVE_GETGRENT_R\n");
#endif
#ifdef HAVE_GETGRENT_R_DECL
       output(screen, "   HAVE_GETGRENT_R_DECL\n");
#endif
#ifdef HAVE_GETGRGID_R
       output(screen, "   HAVE_GETGRGID_R\n");
#endif
#ifdef HAVE_GETGRNAM
       output(screen, "   HAVE_GETGRNAM\n");
#endif
#ifdef HAVE_GETGRNAM_R
       output(screen, "   HAVE_GETGRNAM_R\n");
#endif
#ifdef HAVE_GETGROUPLIST
       output(screen, "   HAVE_GETGROUPLIST\n");
#endif
#ifdef HAVE_GETGRSET
       output(screen, "   HAVE_GETGRSET\n");
#endif
#ifdef HAVE_GETHOSTBYNAME
       output(screen, "   HAVE_GETHOSTBYNAME\n");
#endif
#ifdef HAVE_GETIFADDRS
       output(screen, "   HAVE_GETIFADDRS\n");
#endif
#ifdef HAVE_GETMNTENT
       output(screen, "   HAVE_GETMNTENT\n");
#endif
#ifdef HAVE_GETNAMEINFO
       output(screen, "   HAVE_GETNAMEINFO\n");
#endif
#ifdef HAVE_GETNETGRENT
       output(screen, "   HAVE_GETNETGRENT\n");
#endif
#ifdef HAVE_GETNETGRENT_PROTOTYPE
       output(screen, "   HAVE_GETNETGRENT_PROTOTYPE\n");
#endif
#ifdef HAVE_GETPAGESIZE
       output(screen, "   HAVE_GETPAGESIZE\n");
#endif
#ifdef HAVE_GETPEEREID
       output(screen, "   HAVE_GETPEEREID\n");
#endif
#ifdef HAVE_GETPGRP
       output(screen, "   HAVE_GETPGRP\n");
#endif
#ifdef HAVE_GETPWANAM
       output(screen, "   HAVE_GETPWANAM\n");
#endif
#ifdef HAVE_GETPWENT_R
       output(screen, "   HAVE_GETPWENT_R\n");
#endif
#ifdef HAVE_GETPWENT_R_DECL
       output(screen, "   HAVE_GETPWENT_R_DECL\n");
#endif
#ifdef HAVE_GETPWNAM
       output(screen, "   HAVE_GETPWNAM\n");
#endif
#ifdef HAVE_GETPWNAM_R
       output(screen, "   HAVE_GETPWNAM_R\n");
#endif
#ifdef HAVE_GETPWUID_R
       output(screen, "   HAVE_GETPWUID_R\n");
#endif
#ifdef HAVE_GETRLIMIT
       output(screen, "   HAVE_GETRLIMIT\n");
#endif
#ifdef HAVE_GETSPNAM
       output(screen, "   HAVE_GETSPNAM\n");
#endif
#ifdef HAVE_GETTEXT
       output(screen, "   HAVE_GETTEXT\n");
#endif
#ifdef HAVE_GETTIMEOFDAY_TZ
       output(screen, "   HAVE_GETTIMEOFDAY_TZ\n");
#endif
#ifdef HAVE_GETTIMEOFDAY_TZ_VOID
       output(screen, "   HAVE_GETTIMEOFDAY_TZ_VOID\n");
#endif
#ifdef HAVE_GETUTXENT
       output(screen, "   HAVE_GETUTXENT\n");
#endif
#ifdef HAVE_GETXATTR
       output(screen, "   HAVE_GETXATTR\n");
#endif
#ifdef HAVE_GET_CURRENT_DIR_NAME
       output(screen, "   HAVE_GET_CURRENT_DIR_NAME\n");
#endif
#ifdef HAVE_GLOB
       output(screen, "   HAVE_GLOB\n");
#endif
#ifdef HAVE_GLUSTERFS
       output(screen, "   HAVE_GLUSTERFS\n");
#endif
#ifdef HAVE_GNUTLS
       output(screen, "   HAVE_GNUTLS\n");
#endif
#ifdef HAVE_GNUTLS3
       output(screen, "   HAVE_GNUTLS3\n");
#endif
#ifdef HAVE_GNUTLS_AEAD_CIPHER_INIT
       output(screen, "   HAVE_GNUTLS_AEAD_CIPHER_INIT\n");
#endif
#ifdef HAVE_GNUTLS_CERTIFICATE_VERIFY_PEERS3
       output(screen, "   HAVE_GNUTLS_CERTIFICATE_VERIFY_PEERS3\n");
#endif
#ifdef HAVE_GNUTLS_DATUM
       output(screen, "   HAVE_GNUTLS_DATUM\n");
#endif
#ifdef HAVE_GNUTLS_DATUM_T
       output(screen, "   HAVE_GNUTLS_DATUM_T\n");
#endif
#ifdef HAVE_GNUTLS_GLOBAL_INIT
       output(screen, "   HAVE_GNUTLS_GLOBAL_INIT\n");
#endif
#ifdef HAVE_GNUTLS_X509_CRT_SET_SUBJECT_KEY_ID
       output(screen, "   HAVE_GNUTLS_X509_CRT_SET_SUBJECT_KEY_ID\n");
#endif
#ifdef HAVE_GNUTLS_X509_CRT_SET_VERSION
       output(screen, "   HAVE_GNUTLS_X509_CRT_SET_VERSION\n");
#endif
#ifdef HAVE_GPGME_NEW
       output(screen, "   HAVE_GPGME_NEW\n");
#endif
#ifdef HAVE_GPG_ERR_CODE_FROM_ERRNO
       output(screen, "   HAVE_GPG_ERR_CODE_FROM_ERRNO\n");
#endif
#ifdef HAVE_GRANTPT
       output(screen, "   HAVE_GRANTPT\n");
#endif
#ifdef HAVE_GSSAPI
       output(screen, "   HAVE_GSSAPI\n");
#endif
#ifdef HAVE_GSSKRB5_EXTRACT_AUTHZ_DATA_FROM_SEC_CONTEXT
       output(screen, "   HAVE_GSSKRB5_EXTRACT_AUTHZ_DATA_FROM_SEC_CONTEXT\n");
#endif
#ifdef HAVE_GSS_ACQUIRE_CRED_FROM
       output(screen, "   HAVE_GSS_ACQUIRE_CRED_FROM\n");
#endif
#ifdef HAVE_GSS_DISPLAY_STATUS
       output(screen, "   HAVE_GSS_DISPLAY_STATUS\n");
#endif
#ifdef HAVE_GSS_EXPORT_CRED
       output(screen, "   HAVE_GSS_EXPORT_CRED\n");
#endif
#ifdef HAVE_GSS_GET_NAME_ATTRIBUTE
       output(screen, "   HAVE_GSS_GET_NAME_ATTRIBUTE\n");
#endif
#ifdef HAVE_GSS_IMPORT_CRED
       output(screen, "   HAVE_GSS_IMPORT_CRED\n");
#endif
#ifdef HAVE_GSS_INQUIRE_SEC_CONTEXT_BY_OID
       output(screen, "   HAVE_GSS_INQUIRE_SEC_CONTEXT_BY_OID\n");
#endif
#ifdef HAVE_GSS_KRB5_CRED_NO_CI_FLAGS_X
       output(screen, "   HAVE_GSS_KRB5_CRED_NO_CI_FLAGS_X\n");
#endif
#ifdef HAVE_GSS_KRB5_EXPORT_LUCID_SEC_CONTEXT
       output(screen, "   HAVE_GSS_KRB5_EXPORT_LUCID_SEC_CONTEXT\n");
#endif
#ifdef HAVE_GSS_KRB5_IMPORT_CRED
       output(screen, "   HAVE_GSS_KRB5_IMPORT_CRED\n");
#endif
#ifdef HAVE_GSS_MECH_KRB5
       output(screen, "   HAVE_GSS_MECH_KRB5\n");
#endif
#ifdef HAVE_GSS_OID_EQUAL
       output(screen, "   HAVE_GSS_OID_EQUAL\n");
#endif
#ifdef HAVE_GSS_WRAP_IOV
       output(screen, "   HAVE_GSS_WRAP_IOV\n");
#endif
#ifdef HAVE_HISTORY_LIST
       output(screen, "   HAVE_HISTORY_LIST\n");
#endif
#ifdef HAVE_HSTRERROR
       output(screen, "   HAVE_HSTRERROR\n");
#endif
#ifdef HAVE_HTTPCONNECT
       output(screen, "   HAVE_HTTPCONNECT\n");
#endif
#ifdef HAVE_HTTPCONNECTENCRYPT
       output(screen, "   HAVE_HTTPCONNECTENCRYPT\n");
#endif
#ifdef HAVE_ICONV
       output(screen, "   HAVE_ICONV\n");
#endif
#ifdef HAVE_ICONV_ERRNO_ILLEGAL_MULTIBYTE
       output(screen, "   HAVE_ICONV_ERRNO_ILLEGAL_MULTIBYTE\n");
#endif
#ifdef HAVE_ICONV_OPEN
       output(screen, "   HAVE_ICONV_OPEN\n");
#endif
#ifdef HAVE_IFACE_GETIFADDRS
       output(screen, "   HAVE_IFACE_GETIFADDRS\n");
#endif
#ifdef HAVE_IF_NAMETOINDEX
       output(screen, "   HAVE_IF_NAMETOINDEX\n");
#endif
#ifdef HAVE_IMMEDIATE_STRUCTURES
       output(screen, "   HAVE_IMMEDIATE_STRUCTURES\n");
#endif
#ifdef HAVE_INCOHERENT_MMAP
       output(screen, "   HAVE_INCOHERENT_MMAP\n");
#endif
#ifdef HAVE_INET_ATON
       output(screen, "   HAVE_INET_ATON\n");
#endif
#ifdef HAVE_INET_NTOA
       output(screen, "   HAVE_INET_NTOA\n");
#endif
#ifdef HAVE_INET_NTOP
       output(screen, "   HAVE_INET_NTOP\n");
#endif
#ifdef HAVE_INET_PTON
       output(screen, "   HAVE_INET_PTON\n");
#endif
#ifdef HAVE_INITGROUPS
       output(screen, "   HAVE_INITGROUPS\n");
#endif
#ifdef HAVE_INITIALIZE_KRB5_ERROR_TABLE
       output(screen, "   HAVE_INITIALIZE_KRB5_ERROR_TABLE\n");
#endif
#ifdef HAVE_INITSCR
       output(screen, "   HAVE_INITSCR\n");
#endif
#ifdef HAVE_INNETGR
       output(screen, "   HAVE_INNETGR\n");
#endif
#ifdef HAVE_INOTIFY
       output(screen, "   HAVE_INOTIFY\n");
#endif
#ifdef HAVE_INOTIFY_INIT
       output(screen, "   HAVE_INOTIFY_INIT\n");
#endif
#ifdef HAVE_INO_T
       output(screen, "   HAVE_INO_T\n");
#endif
#ifdef HAVE_INT16_T
       output(screen, "   HAVE_INT16_T\n");
#endif
#ifdef HAVE_INT32_T
       output(screen, "   HAVE_INT32_T\n");
#endif
#ifdef HAVE_INT64_T
       output(screen, "   HAVE_INT64_T\n");
#endif
#ifdef HAVE_INT8_T
       output(screen, "   HAVE_INT8_T\n");
#endif
#ifdef HAVE_INTPTR_T
       output(screen, "   HAVE_INTPTR_T\n");
#endif
#ifdef HAVE_IPRINT
       output(screen, "   HAVE_IPRINT\n");
#endif
#ifdef HAVE_IPV6
       output(screen, "   HAVE_IPV6\n");
#endif
#ifdef HAVE_IPV6_V6ONLY
       output(screen, "   HAVE_IPV6_V6ONLY\n");
#endif
#ifdef HAVE_ISATTY
       output(screen, "   HAVE_ISATTY\n");
#endif
#ifdef HAVE_ITEM_COUNT
       output(screen, "   HAVE_ITEM_COUNT\n");
#endif
#ifdef HAVE_KADM5_INIT
       output(screen, "   HAVE_KADM5_INIT\n");
#endif
#ifdef HAVE_KADM_SERVER
       output(screen, "   HAVE_KADM_SERVER\n");
#endif
#ifdef HAVE_KDB5
       output(screen, "   HAVE_KDB5\n");
#endif
#ifdef HAVE_KERNEL_CHANGE_NOTIFY
       output(screen, "   HAVE_KERNEL_CHANGE_NOTIFY\n");
#endif
#ifdef HAVE_KERNEL_OPLOCKS_IRIX
       output(screen, "   HAVE_KERNEL_OPLOCKS_IRIX\n");
#endif
#ifdef HAVE_KERNEL_OPLOCKS_LINUX
       output(screen, "   HAVE_KERNEL_OPLOCKS_LINUX\n");
#endif
#ifdef HAVE_KERNEL_SHARE_MODES
       output(screen, "   HAVE_KERNEL_SHARE_MODES\n");
#endif
#ifdef HAVE_KRB5
       output(screen, "   HAVE_KRB5\n");
#endif
#ifdef HAVE_KRB5_AUTH_CON_GETRECVSUBKEY
       output(screen, "   HAVE_KRB5_AUTH_CON_GETRECVSUBKEY\n");
#endif
#ifdef HAVE_KRB5_AUTH_CON_GETSENDSUBKEY
       output(screen, "   HAVE_KRB5_AUTH_CON_GETSENDSUBKEY\n");
#endif
#ifdef HAVE_KRB5_AUTH_CON_SETKEY
       output(screen, "   HAVE_KRB5_AUTH_CON_SETKEY\n");
#endif
#ifdef HAVE_KRB5_AUTH_CON_SETUSERUSERKEY
       output(screen, "   HAVE_KRB5_AUTH_CON_SETUSERUSERKEY\n");
#endif
#ifdef HAVE_KRB5_AUTH_CON_SET_REQ_CKSUMTYPE
       output(screen, "   HAVE_KRB5_AUTH_CON_SET_REQ_CKSUMTYPE\n");
#endif
#ifdef HAVE_KRB5_BUILD_PRINCIPAL_ALLOC_VA
       output(screen, "   HAVE_KRB5_BUILD_PRINCIPAL_ALLOC_VA\n");
#endif
#ifdef HAVE_KRB5_CC_COPY_CREDS
       output(screen, "   HAVE_KRB5_CC_COPY_CREDS\n");
#endif
#ifdef HAVE_KRB5_CC_GET_LIFETIME
       output(screen, "   HAVE_KRB5_CC_GET_LIFETIME\n");
#endif
#ifdef HAVE_KRB5_CC_RETRIEVE_CRED
       output(screen, "   HAVE_KRB5_CC_RETRIEVE_CRED\n");
#endif
#ifdef HAVE_KRB5_CONFIG_GET_BOOL_DEFAULT
       output(screen, "   HAVE_KRB5_CONFIG_GET_BOOL_DEFAULT\n");
#endif
#ifdef HAVE_KRB5_CREATE_CHECKSUM
       output(screen, "   HAVE_KRB5_CREATE_CHECKSUM\n");
#endif
#ifdef HAVE_KRB5_CRYPTO_DESTROY
       output(screen, "   HAVE_KRB5_CRYPTO_DESTROY\n");
#endif
#ifdef HAVE_KRB5_CRYPTO_INIT
       output(screen, "   HAVE_KRB5_CRYPTO_INIT\n");
#endif
#ifdef HAVE_KRB5_C_MAKE_CHECKSUM
       output(screen, "   HAVE_KRB5_C_MAKE_CHECKSUM\n");
#endif
#ifdef HAVE_KRB5_C_STRING_TO_KEY
       output(screen, "   HAVE_KRB5_C_STRING_TO_KEY\n");
#endif
#ifdef HAVE_KRB5_C_VERIFY_CHECKSUM
       output(screen, "   HAVE_KRB5_C_VERIFY_CHECKSUM\n");
#endif
#ifdef HAVE_KRB5_DATA_COPY
       output(screen, "   HAVE_KRB5_DATA_COPY\n");
#endif
#ifdef HAVE_KRB5_DEPRECATED_WITH_IDENTIFIER
       output(screen, "   HAVE_KRB5_DEPRECATED_WITH_IDENTIFIER\n");
#endif
#ifdef HAVE_KRB5_ENCRYPT_BLOCK
       output(screen, "   HAVE_KRB5_ENCRYPT_BLOCK\n");
#endif
#ifdef HAVE_KRB5_ENCRYPT_DATA
       output(screen, "   HAVE_KRB5_ENCRYPT_DATA\n");
#endif
#ifdef HAVE_KRB5_ENCTYPE_TO_STRING
       output(screen, "   HAVE_KRB5_ENCTYPE_TO_STRING\n");
#endif
#ifdef HAVE_KRB5_ENCTYPE_TO_STRING_WITH_KRB5_CONTEXT_ARG
       output(screen, "   HAVE_KRB5_ENCTYPE_TO_STRING_WITH_KRB5_CONTEXT_ARG\n");
#endif
#ifdef HAVE_KRB5_ENCTYPE_TO_STRING_WITH_SIZE_T_ARG
       output(screen, "   HAVE_KRB5_ENCTYPE_TO_STRING_WITH_SIZE_T_ARG\n");
#endif
#ifdef HAVE_KRB5_FREE_CHECKSUM_CONTENTS
       output(screen, "   HAVE_KRB5_FREE_CHECKSUM_CONTENTS\n");
#endif
#ifdef HAVE_KRB5_FREE_DATA_CONTENTS
       output(screen, "   HAVE_KRB5_FREE_DATA_CONTENTS\n");
#endif
#ifdef HAVE_KRB5_FREE_ERROR_CONTENTS
       output(screen, "   HAVE_KRB5_FREE_ERROR_CONTENTS\n");
#endif
#ifdef HAVE_KRB5_FREE_HOST_REALM
       output(screen, "   HAVE_KRB5_FREE_HOST_REALM\n");
#endif
#ifdef HAVE_KRB5_FREE_KEYTAB_ENTRY_CONTENTS
       output(screen, "   HAVE_KRB5_FREE_KEYTAB_ENTRY_CONTENTS\n");
#endif
#ifdef HAVE_KRB5_FREE_UNPARSED_NAME
       output(screen, "   HAVE_KRB5_FREE_UNPARSED_NAME\n");
#endif
#ifdef HAVE_KRB5_FWD_TGT_CREDS
       output(screen, "   HAVE_KRB5_FWD_TGT_CREDS\n");
#endif
#ifdef HAVE_KRB5_GET_CREDENTIALS_FOR_USER
       output(screen, "   HAVE_KRB5_GET_CREDENTIALS_FOR_USER\n");
#endif
#ifdef HAVE_KRB5_GET_CREDS
       output(screen, "   HAVE_KRB5_GET_CREDS\n");
#endif
#ifdef HAVE_KRB5_GET_CREDS_OPT_ALLOC
       output(screen, "   HAVE_KRB5_GET_CREDS_OPT_ALLOC\n");
#endif
#ifdef HAVE_KRB5_GET_CREDS_OPT_SET_IMPERSONATE
       output(screen, "   HAVE_KRB5_GET_CREDS_OPT_SET_IMPERSONATE\n");
#endif
#ifdef HAVE_KRB5_GET_DEFAULT_IN_TKT_ETYPES
       output(screen, "   HAVE_KRB5_GET_DEFAULT_IN_TKT_ETYPES\n");
#endif
#ifdef HAVE_KRB5_GET_HOST_REALM
       output(screen, "   HAVE_KRB5_GET_HOST_REALM\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_KEYBLOCK
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_KEYBLOCK\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_KEYTAB
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_KEYTAB\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_OPT_ALLOC
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_OPT_ALLOC\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_OPT_FREE
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_OPT_FREE\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_OPT_GET_ERROR
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_OPT_GET_ERROR\n");
#endif
#ifdef HAVE_KRB5_GET_INIT_CREDS_OPT_SET_PAC_REQUEST
       output(screen, "   HAVE_KRB5_GET_INIT_CREDS_OPT_SET_PAC_REQUEST\n");
#endif
#ifdef HAVE_KRB5_GET_PERMITTED_ENCTYPES
       output(screen, "   HAVE_KRB5_GET_PERMITTED_ENCTYPES\n");
#endif
#ifdef HAVE_KRB5_GET_PROFILE
       output(screen, "   HAVE_KRB5_GET_PROFILE\n");
#endif
#ifdef HAVE_KRB5_GET_PROMPT_TYPES
       output(screen, "   HAVE_KRB5_GET_PROMPT_TYPES\n");
#endif
#ifdef HAVE_KRB5_GET_PW_SALT
       output(screen, "   HAVE_KRB5_GET_PW_SALT\n");
#endif
#ifdef HAVE_KRB5_GET_RENEWED_CREDS
       output(screen, "   HAVE_KRB5_GET_RENEWED_CREDS\n");
#endif
#ifdef HAVE_KRB5_INIT_KEYBLOCK
       output(screen, "   HAVE_KRB5_INIT_KEYBLOCK\n");
#endif
#ifdef HAVE_KRB5_KEYTAB_ENTRY_KEY
       output(screen, "   HAVE_KRB5_KEYTAB_ENTRY_KEY\n");
#endif
#ifdef HAVE_KRB5_KEYTAB_ENTRY_KEYBLOCK
       output(screen, "   HAVE_KRB5_KEYTAB_ENTRY_KEYBLOCK\n");
#endif
#ifdef HAVE_KRB5_KEYUSAGE_APP_DATA_CKSUM
       output(screen, "   HAVE_KRB5_KEYUSAGE_APP_DATA_CKSUM\n");
#endif
#ifdef HAVE_KRB5_KRBHST_GET_ADDRINFO
       output(screen, "   HAVE_KRB5_KRBHST_GET_ADDRINFO\n");
#endif
#ifdef HAVE_KRB5_KRBHST_INIT
       output(screen, "   HAVE_KRB5_KRBHST_INIT\n");
#endif
#ifdef HAVE_KRB5_KT_COMPARE
       output(screen, "   HAVE_KRB5_KT_COMPARE\n");
#endif
#ifdef HAVE_KRB5_KT_FREE_ENTRY
       output(screen, "   HAVE_KRB5_KT_FREE_ENTRY\n");
#endif
#ifdef HAVE_KRB5_KU_OTHER_CKSUM
       output(screen, "   HAVE_KRB5_KU_OTHER_CKSUM\n");
#endif
#ifdef HAVE_KRB5_MAKE_PRINCIPAL
       output(screen, "   HAVE_KRB5_MAKE_PRINCIPAL\n");
#endif
#ifdef HAVE_KRB5_MK_REQ_EXTENDED
       output(screen, "   HAVE_KRB5_MK_REQ_EXTENDED\n");
#endif
#ifdef HAVE_KRB5_PARSE_NAME_NOREALM
       output(screen, "   HAVE_KRB5_PARSE_NAME_NOREALM\n");
#endif
#ifdef HAVE_KRB5_PDU_NONE_DECL
       output(screen, "   HAVE_KRB5_PDU_NONE_DECL\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL2SALT
       output(screen, "   HAVE_KRB5_PRINCIPAL2SALT\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_COMPARE_ANY_REALM
       output(screen, "   HAVE_KRB5_PRINCIPAL_COMPARE_ANY_REALM\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_GET_COMP_STRING
       output(screen, "   HAVE_KRB5_PRINCIPAL_GET_COMP_STRING\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_GET_REALM
       output(screen, "   HAVE_KRB5_PRINCIPAL_GET_REALM\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_GET_TYPE
       output(screen, "   HAVE_KRB5_PRINCIPAL_GET_TYPE\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_SET_REALM
       output(screen, "   HAVE_KRB5_PRINCIPAL_SET_REALM\n");
#endif
#ifdef HAVE_KRB5_PRINCIPAL_SET_TYPE
       output(screen, "   HAVE_KRB5_PRINCIPAL_SET_TYPE\n");
#endif
#ifdef HAVE_KRB5_PRINC_COMPONENT
       output(screen, "   HAVE_KRB5_PRINC_COMPONENT\n");
#endif
#ifdef HAVE_KRB5_PRINC_REALM
       output(screen, "   HAVE_KRB5_PRINC_REALM\n");
#endif
#ifdef HAVE_KRB5_PRINC_SIZE
       output(screen, "   HAVE_KRB5_PRINC_SIZE\n");
#endif
#ifdef HAVE_KRB5_PROMPT_TYPE
       output(screen, "   HAVE_KRB5_PROMPT_TYPE\n");
#endif
#ifdef HAVE_KRB5_SET_DEFAULT_IN_TKT_ETYPES
       output(screen, "   HAVE_KRB5_SET_DEFAULT_IN_TKT_ETYPES\n");
#endif
#ifdef HAVE_KRB5_SET_DEFAULT_TGS_ENCTYPES
       output(screen, "   HAVE_KRB5_SET_DEFAULT_TGS_ENCTYPES\n");
#endif
#ifdef HAVE_KRB5_SET_DEFAULT_TGS_KTYPES
       output(screen, "   HAVE_KRB5_SET_DEFAULT_TGS_KTYPES\n");
#endif
#ifdef HAVE_KRB5_STRING_TO_KEY_SALT
       output(screen, "   HAVE_KRB5_STRING_TO_KEY_SALT\n");
#endif
#ifdef HAVE_KRB5_WARNX
       output(screen, "   HAVE_KRB5_WARNX\n");
#endif
#ifdef HAVE_KV5M_KEYTAB
       output(screen, "   HAVE_KV5M_KEYTAB\n");
#endif
#ifdef HAVE_LARGEFILE
       output(screen, "   HAVE_LARGEFILE\n");
#endif
#ifdef HAVE_LBER_LOG_PRINT_FN
       output(screen, "   HAVE_LBER_LOG_PRINT_FN\n");
#endif
#ifdef HAVE_LCHOWN
       output(screen, "   HAVE_LCHOWN\n");
#endif
#ifdef HAVE_LDAP
       output(screen, "   HAVE_LDAP\n");
#endif
#ifdef HAVE_LDAP_ADD_RESULT_ENTRY
       output(screen, "   HAVE_LDAP_ADD_RESULT_ENTRY\n");
#endif
#ifdef HAVE_LDAP_INIT
       output(screen, "   HAVE_LDAP_INIT\n");
#endif
#ifdef HAVE_LDAP_INITIALIZE
       output(screen, "   HAVE_LDAP_INITIALIZE\n");
#endif
#ifdef HAVE_LDAP_INIT_FD
       output(screen, "   HAVE_LDAP_INIT_FD\n");
#endif
#ifdef HAVE_LDAP_OPT_SOCKBUF
       output(screen, "   HAVE_LDAP_OPT_SOCKBUF\n");
#endif
#ifdef HAVE_LDAP_SASL_WRAPPING
       output(screen, "   HAVE_LDAP_SASL_WRAPPING\n");
#endif
#ifdef HAVE_LDAP_SET_REBIND_PROC
       output(screen, "   HAVE_LDAP_SET_REBIND_PROC\n");
#endif
#ifdef HAVE_LDB
       output(screen, "   HAVE_LDB\n");
#endif
#ifdef HAVE_LDWRAP
       output(screen, "   HAVE_LDWRAP\n");
#endif
#ifdef HAVE_LIBACL
       output(screen, "   HAVE_LIBACL\n");
#endif
#ifdef HAVE_LIBARCHIVE
       output(screen, "   HAVE_LIBARCHIVE\n");
#endif
#ifdef HAVE_LIBATTR
       output(screen, "   HAVE_LIBATTR\n");
#endif
#ifdef HAVE_LIBAUDIT
       output(screen, "   HAVE_LIBAUDIT\n");
#endif
#ifdef HAVE_LIBAVAHI_CLIENT
       output(screen, "   HAVE_LIBAVAHI_CLIENT\n");
#endif
#ifdef HAVE_LIBAVAHI_COMMON
       output(screen, "   HAVE_LIBAVAHI_COMMON\n");
#endif
#ifdef HAVE_LIBBSD
       output(screen, "   HAVE_LIBBSD\n");
#endif
#ifdef HAVE_LIBCAP
       output(screen, "   HAVE_LIBCAP\n");
#endif
#ifdef HAVE_LIBCRYPT
       output(screen, "   HAVE_LIBCRYPT\n");
#endif
#ifdef HAVE_LIBCUPS
       output(screen, "   HAVE_LIBCUPS\n");
#endif
#ifdef HAVE_LIBDBUS_1
       output(screen, "   HAVE_LIBDBUS_1\n");
#endif
#ifdef HAVE_LIBDL
       output(screen, "   HAVE_LIBDL\n");
#endif
#ifdef HAVE_LIBFORM
       output(screen, "   HAVE_LIBFORM\n");
#endif
#ifdef HAVE_LIBGNUTLS
       output(screen, "   HAVE_LIBGNUTLS\n");
#endif
#ifdef HAVE_LIBKADM5SRV_MIT
       output(screen, "   HAVE_LIBKADM5SRV_MIT\n");
#endif
#ifdef HAVE_LIBLBER
       output(screen, "   HAVE_LIBLBER\n");
#endif
#ifdef HAVE_LIBLDAP
       output(screen, "   HAVE_LIBLDAP\n");
#endif
#ifdef HAVE_LIBMENU
       output(screen, "   HAVE_LIBMENU\n");
#endif
#ifdef HAVE_LIBNCURSES
       output(screen, "   HAVE_LIBNCURSES\n");
#endif
#ifdef HAVE_LIBNSL
       output(screen, "   HAVE_LIBNSL\n");
#endif
#ifdef HAVE_LIBPAM
       output(screen, "   HAVE_LIBPAM\n");
#endif
#ifdef HAVE_LIBPANEL
       output(screen, "   HAVE_LIBPANEL\n");
#endif
#ifdef HAVE_LIBPOPT
       output(screen, "   HAVE_LIBPOPT\n");
#endif
#ifdef HAVE_LIBPTHREAD
       output(screen, "   HAVE_LIBPTHREAD\n");
#endif
#ifdef HAVE_LIBREADLINE
       output(screen, "   HAVE_LIBREADLINE\n");
#endif
#ifdef HAVE_LIBREPLACE
       output(screen, "   HAVE_LIBREPLACE\n");
#endif
#ifdef HAVE_LIBRESOLV
       output(screen, "   HAVE_LIBRESOLV\n");
#endif
#ifdef HAVE_LIBRT
       output(screen, "   HAVE_LIBRT\n");
#endif
#ifdef HAVE_LIBSYSTEMD_DAEMON
       output(screen, "   HAVE_LIBSYSTEMD_DAEMON\n");
#endif
#ifdef HAVE_LIBSYSTEMD_JOURNAL
       output(screen, "   HAVE_LIBSYSTEMD_JOURNAL\n");
#endif
#ifdef HAVE_LIBUTIL
       output(screen, "   HAVE_LIBUTIL\n");
#endif
#ifdef HAVE_LIBZ
       output(screen, "   HAVE_LIBZ\n");
#endif
#ifdef HAVE_LINK
       output(screen, "   HAVE_LINK\n");
#endif
#ifdef HAVE_LINUX_FALLOCATE
       output(screen, "   HAVE_LINUX_FALLOCATE\n");
#endif
#ifdef HAVE_LINUX_INOTIFY
       output(screen, "   HAVE_LINUX_INOTIFY\n");
#endif
#ifdef HAVE_LINUX_IOCTL
       output(screen, "   HAVE_LINUX_IOCTL\n");
#endif
#ifdef HAVE_LINUX_READAHEAD
       output(screen, "   HAVE_LINUX_READAHEAD\n");
#endif
#ifdef HAVE_LINUX_SPLICE
       output(screen, "   HAVE_LINUX_SPLICE\n");
#endif
#ifdef HAVE_LISTXATTR
       output(screen, "   HAVE_LISTXATTR\n");
#endif
#ifdef HAVE_LITTLE_ENDIAN
       output(screen, "   HAVE_LITTLE_ENDIAN\n");
#endif
#ifdef HAVE_LLSEEK
       output(screen, "   HAVE_LLSEEK\n");
#endif
#ifdef HAVE_LOFF_T
       output(screen, "   HAVE_LOFF_T\n");
#endif
#ifdef HAVE_LONGJMP
       output(screen, "   HAVE_LONGJMP\n");
#endif
#ifdef HAVE_LONG_LONG
       output(screen, "   HAVE_LONG_LONG\n");
#endif
#ifdef HAVE_LSEEK_HOLE_DATA
       output(screen, "   HAVE_LSEEK_HOLE_DATA\n");
#endif
#ifdef HAVE_LSTAT
       output(screen, "   HAVE_LSTAT\n");
#endif
#ifdef HAVE_LUTIMES
       output(screen, "   HAVE_LUTIMES\n");
#endif
#ifdef HAVE_MAGIC_IN_KRB5_ADDRESS
       output(screen, "   HAVE_MAGIC_IN_KRB5_ADDRESS\n");
#endif
#ifdef HAVE_MAKEDEV
       output(screen, "   HAVE_MAKEDEV\n");
#endif
#ifdef HAVE_MD5INIT
       output(screen, "   HAVE_MD5INIT\n");
#endif
#ifdef HAVE_MEMALIGN
       output(screen, "   HAVE_MEMALIGN\n");
#endif
#ifdef HAVE_MEMCPY
       output(screen, "   HAVE_MEMCPY\n");
#endif
#ifdef HAVE_MEMMEM
       output(screen, "   HAVE_MEMMEM\n");
#endif
#ifdef HAVE_MEMMOVE
       output(screen, "   HAVE_MEMMOVE\n");
#endif
#ifdef HAVE_MEMSET
       output(screen, "   HAVE_MEMSET\n");
#endif
#ifdef HAVE_MKDIR_MODE
       output(screen, "   HAVE_MKDIR_MODE\n");
#endif
#ifdef HAVE_MKDTEMP
       output(screen, "   HAVE_MKDTEMP\n");
#endif
#ifdef HAVE_MKNOD
       output(screen, "   HAVE_MKNOD\n");
#endif
#ifdef HAVE_MKTIME
       output(screen, "   HAVE_MKTIME\n");
#endif
#ifdef HAVE_MLOCK
       output(screen, "   HAVE_MLOCK\n");
#endif
#ifdef HAVE_MLOCKALL
       output(screen, "   HAVE_MLOCKALL\n");
#endif
#ifdef HAVE_MMAP
       output(screen, "   HAVE_MMAP\n");
#endif
#ifdef HAVE_MREMAP
       output(screen, "   HAVE_MREMAP\n");
#endif
#ifdef HAVE_MUNLOCK
       output(screen, "   HAVE_MUNLOCK\n");
#endif
#ifdef HAVE_MUNLOCKALL
       output(screen, "   HAVE_MUNLOCKALL\n");
#endif
#ifdef HAVE_NANOSLEEP
       output(screen, "   HAVE_NANOSLEEP\n");
#endif
#ifdef HAVE_NATIVE_ICONV
       output(screen, "   HAVE_NATIVE_ICONV\n");
#endif
#ifdef HAVE_NCURSES
       output(screen, "   HAVE_NCURSES\n");
#endif
#ifdef HAVE_NETGROUP
       output(screen, "   HAVE_NETGROUP\n");
#endif
#ifdef HAVE_NEW_FIELD
       output(screen, "   HAVE_NEW_FIELD\n");
#endif
#ifdef HAVE_NEW_FORM
       output(screen, "   HAVE_NEW_FORM\n");
#endif
#ifdef HAVE_NEW_LIBREADLINE
       output(screen, "   HAVE_NEW_LIBREADLINE\n");
#endif
#ifdef HAVE_NEW_PANEL
       output(screen, "   HAVE_NEW_PANEL\n");
#endif
#ifdef HAVE_NFTW
       output(screen, "   HAVE_NFTW\n");
#endif
#ifdef HAVE_NSCD_FLUSH_CACHE
       output(screen, "   HAVE_NSCD_FLUSH_CACHE\n");
#endif
#ifdef HAVE_NSS_XBYY_KEY_IPNODE
       output(screen, "   HAVE_NSS_XBYY_KEY_IPNODE\n");
#endif
#ifdef HAVE_OFD_LOCKS
       output(screen, "   HAVE_OFD_LOCKS\n");
#endif
#ifdef HAVE_OFFSET_T
       output(screen, "   HAVE_OFFSET_T\n");
#endif
#ifdef HAVE_OPENAT
       output(screen, "   HAVE_OPENAT\n");
#endif
#ifdef HAVE_OPENPTY
       output(screen, "   HAVE_OPENPTY\n");
#endif
#ifdef HAVE_OPEN_O_DIRECT
       output(screen, "   HAVE_OPEN_O_DIRECT\n");
#endif
#ifdef HAVE_PAM_GET_DATA
       output(screen, "   HAVE_PAM_GET_DATA\n");
#endif
#ifdef HAVE_PAM_RADIO_TYPE
       output(screen, "   HAVE_PAM_RADIO_TYPE\n");
#endif
#ifdef HAVE_PAM_RHOST
       output(screen, "   HAVE_PAM_RHOST\n");
#endif
#ifdef HAVE_PAM_START
       output(screen, "   HAVE_PAM_START\n");
#endif
#ifdef HAVE_PAM_TTY
       output(screen, "   HAVE_PAM_TTY\n");
#endif
#ifdef HAVE_PAM_VSYSLOG
       output(screen, "   HAVE_PAM_VSYSLOG\n");
#endif
#ifdef HAVE_PASSWD_PW_AGE
       output(screen, "   HAVE_PASSWD_PW_AGE\n");
#endif
#ifdef HAVE_PASSWD_PW_COMMENT
       output(screen, "   HAVE_PASSWD_PW_COMMENT\n");
#endif
#ifdef HAVE_PATHCONF
       output(screen, "   HAVE_PATHCONF\n");
#endif
#ifdef HAVE_PEERCRED
       output(screen, "   HAVE_PEERCRED\n");
#endif
#ifdef HAVE_PERL_MAKEMAKER
       output(screen, "   HAVE_PERL_MAKEMAKER\n");
#endif
#ifdef HAVE_PIPE
       output(screen, "   HAVE_PIPE\n");
#endif
#ifdef HAVE_POLL
       output(screen, "   HAVE_POLL\n");
#endif
#ifdef HAVE_POPT
       output(screen, "   HAVE_POPT\n");
#endif
#ifdef HAVE_POPTGETCONTEXT
       output(screen, "   HAVE_POPTGETCONTEXT\n");
#endif
#ifdef HAVE_PORT_CREATE
       output(screen, "   HAVE_PORT_CREATE\n");
#endif
#ifdef HAVE_POSIX_ACLS
       output(screen, "   HAVE_POSIX_ACLS\n");
#endif
#ifdef HAVE_POSIX_CAPABILITIES
       output(screen, "   HAVE_POSIX_CAPABILITIES\n");
#endif
#ifdef HAVE_POSIX_FADVISE
       output(screen, "   HAVE_POSIX_FADVISE\n");
#endif
#ifdef HAVE_POSIX_FALLOCATE
       output(screen, "   HAVE_POSIX_FALLOCATE\n");
#endif
#ifdef HAVE_POSIX_MEMALIGN
       output(screen, "   HAVE_POSIX_MEMALIGN\n");
#endif
#ifdef HAVE_POSIX_OPENPT
       output(screen, "   HAVE_POSIX_OPENPT\n");
#endif
#ifdef HAVE_PRCTL
       output(screen, "   HAVE_PRCTL\n");
#endif
#ifdef HAVE_PREAD
       output(screen, "   HAVE_PREAD\n");
#endif
#ifdef HAVE_PREAD_DECL
       output(screen, "   HAVE_PREAD_DECL\n");
#endif
#ifdef HAVE_PRINTF
       output(screen, "   HAVE_PRINTF\n");
#endif
#ifdef HAVE_PTHREAD
       output(screen, "   HAVE_PTHREAD\n");
#endif
#ifdef HAVE_PTHREAD_ATTR_INIT
       output(screen, "   HAVE_PTHREAD_ATTR_INIT\n");
#endif
#ifdef HAVE_PTHREAD_CREATE
       output(screen, "   HAVE_PTHREAD_CREATE\n");
#endif
#ifdef HAVE_PTHREAD_MUTEXATTR_SETROBUST
       output(screen, "   HAVE_PTHREAD_MUTEXATTR_SETROBUST\n");
#endif
#ifdef HAVE_PTHREAD_MUTEX_CONSISTENT
       output(screen, "   HAVE_PTHREAD_MUTEX_CONSISTENT\n");
#endif
#ifdef HAVE_PTRDIFF_T
       output(screen, "   HAVE_PTRDIFF_T\n");
#endif
#ifdef HAVE_PUTUTLINE
       output(screen, "   HAVE_PUTUTLINE\n");
#endif
#ifdef HAVE_PUTUTXLINE
       output(screen, "   HAVE_PUTUTXLINE\n");
#endif
#ifdef HAVE_PWRITE
       output(screen, "   HAVE_PWRITE\n");
#endif
#ifdef HAVE_PWRITE_DECL
       output(screen, "   HAVE_PWRITE_DECL\n");
#endif
#ifdef HAVE_PYLDB_UTIL
       output(screen, "   HAVE_PYLDB_UTIL\n");
#endif
#ifdef HAVE_PYTALLOC_UTIL
       output(screen, "   HAVE_PYTALLOC_UTIL\n");
#endif
#ifdef HAVE_RAND
       output(screen, "   HAVE_RAND\n");
#endif
#ifdef HAVE_RANDOM
       output(screen, "   HAVE_RANDOM\n");
#endif
#ifdef HAVE_RDCHK
       output(screen, "   HAVE_RDCHK\n");
#endif
#ifdef HAVE_READAHEAD_DECL
       output(screen, "   HAVE_READAHEAD_DECL\n");
#endif
#ifdef HAVE_READLINE_READLINE_WORKAROUND
       output(screen, "   HAVE_READLINE_READLINE_WORKAROUND\n");
#endif
#ifdef HAVE_READLINK
       output(screen, "   HAVE_READLINK\n");
#endif
#ifdef HAVE_REALPATH
       output(screen, "   HAVE_REALPATH\n");
#endif
#ifdef HAVE_REMOVEXATTR
       output(screen, "   HAVE_REMOVEXATTR\n");
#endif
#ifdef HAVE_RENAME
       output(screen, "   HAVE_RENAME\n");
#endif
#ifdef HAVE_RES_SEARCH
       output(screen, "   HAVE_RES_SEARCH\n");
#endif
#ifdef HAVE_RL_COMPLETION_FUNC_T
       output(screen, "   HAVE_RL_COMPLETION_FUNC_T\n");
#endif
#ifdef HAVE_RL_COMPLETION_MATCHES
       output(screen, "   HAVE_RL_COMPLETION_MATCHES\n");
#endif
#ifdef HAVE_ROBUST_MUTEXES
       output(screen, "   HAVE_ROBUST_MUTEXES\n");
#endif
#ifdef HAVE_ROKEN_GETADDRINFO_HOSTSPEC
       output(screen, "   HAVE_ROKEN_GETADDRINFO_HOSTSPEC\n");
#endif
#ifdef HAVE_RPC_RPC_HEADERS
       output(screen, "   HAVE_RPC_RPC_HEADERS\n");
#endif
#ifdef HAVE_SA_FAMILY_T
       output(screen, "   HAVE_SA_FAMILY_T\n");
#endif
#ifdef HAVE_SA_SIGINFO_DECL
       output(screen, "   HAVE_SA_SIGINFO_DECL\n");
#endif
#ifdef HAVE_SECURE_MKSTEMP
       output(screen, "   HAVE_SECURE_MKSTEMP\n");
#endif
#ifdef HAVE_SELECT
       output(screen, "   HAVE_SELECT\n");
#endif
#ifdef HAVE_SENDFILE
       output(screen, "   HAVE_SENDFILE\n");
#endif
#ifdef HAVE_SETBUFFER
       output(screen, "   HAVE_SETBUFFER\n");
#endif
#ifdef HAVE_SETEGID
       output(screen, "   HAVE_SETEGID\n");
#endif
#ifdef HAVE_SETENV
       output(screen, "   HAVE_SETENV\n");
#endif
#ifdef HAVE_SETENV_DECL
       output(screen, "   HAVE_SETENV_DECL\n");
#endif
#ifdef HAVE_SETEUID
       output(screen, "   HAVE_SETEUID\n");
#endif
#ifdef HAVE_SETGID
       output(screen, "   HAVE_SETGID\n");
#endif
#ifdef HAVE_SETGIDX
       output(screen, "   HAVE_SETGIDX\n");
#endif
#ifdef HAVE_SETGROUPS
       output(screen, "   HAVE_SETGROUPS\n");
#endif
#ifdef HAVE_SETLINEBUF
       output(screen, "   HAVE_SETLINEBUF\n");
#endif
#ifdef HAVE_SETLOCALE
       output(screen, "   HAVE_SETLOCALE\n");
#endif
#ifdef HAVE_SETLUID
       output(screen, "   HAVE_SETLUID\n");
#endif
#ifdef HAVE_SETMNTENT
       output(screen, "   HAVE_SETMNTENT\n");
#endif
#ifdef HAVE_SETNETGRENT
       output(screen, "   HAVE_SETNETGRENT\n");
#endif
#ifdef HAVE_SETNETGRENT_PROTOTYPE
       output(screen, "   HAVE_SETNETGRENT_PROTOTYPE\n");
#endif
#ifdef HAVE_SETPGID
       output(screen, "   HAVE_SETPGID\n");
#endif
#ifdef HAVE_SETPRIV
       output(screen, "   HAVE_SETPRIV\n");
#endif
#ifdef HAVE_SETPROCTITLE
       output(screen, "   HAVE_SETPROCTITLE\n");
#endif
#ifdef HAVE_SETPROCTITLE_INIT
       output(screen, "   HAVE_SETPROCTITLE_INIT\n");
#endif
#ifdef HAVE_SETREGID
       output(screen, "   HAVE_SETREGID\n");
#endif
#ifdef HAVE_SETRESGID
       output(screen, "   HAVE_SETRESGID\n");
#endif
#ifdef HAVE_SETRESGID_DECL
       output(screen, "   HAVE_SETRESGID_DECL\n");
#endif
#ifdef HAVE_SETRESUID
       output(screen, "   HAVE_SETRESUID\n");
#endif
#ifdef HAVE_SETRESUID_DECL
       output(screen, "   HAVE_SETRESUID_DECL\n");
#endif
#ifdef HAVE_SETREUID
       output(screen, "   HAVE_SETREUID\n");
#endif
#ifdef HAVE_SETSID
       output(screen, "   HAVE_SETSID\n");
#endif
#ifdef HAVE_SETUID
       output(screen, "   HAVE_SETUID\n");
#endif
#ifdef HAVE_SETUIDX
       output(screen, "   HAVE_SETUIDX\n");
#endif
#ifdef HAVE_SETXATTR
       output(screen, "   HAVE_SETXATTR\n");
#endif
#ifdef HAVE_SET_MENU_ITEMS
       output(screen, "   HAVE_SET_MENU_ITEMS\n");
#endif
#ifdef HAVE_SHA1_UPDATE
       output(screen, "   HAVE_SHA1_UPDATE\n");
#endif
#ifdef HAVE_SHA256_UPDATE
       output(screen, "   HAVE_SHA256_UPDATE\n");
#endif
#ifdef HAVE_SHA512_UPDATE
       output(screen, "   HAVE_SHA512_UPDATE\n");
#endif
#ifdef HAVE_SHARED_MMAP
       output(screen, "   HAVE_SHARED_MMAP\n");
#endif
#ifdef HAVE_SHL_FINDSYM
       output(screen, "   HAVE_SHL_FINDSYM\n");
#endif
#ifdef HAVE_SHL_LOAD
       output(screen, "   HAVE_SHL_LOAD\n");
#endif
#ifdef HAVE_SHL_UNLOAD
       output(screen, "   HAVE_SHL_UNLOAD\n");
#endif
#ifdef HAVE_SHMGET
       output(screen, "   HAVE_SHMGET\n");
#endif
#ifdef HAVE_SHM_OPEN
       output(screen, "   HAVE_SHM_OPEN\n");
#endif
#ifdef HAVE_SHOW_PANEL
       output(screen, "   HAVE_SHOW_PANEL\n");
#endif
#ifdef HAVE_SIGACTION
       output(screen, "   HAVE_SIGACTION\n");
#endif
#ifdef HAVE_SIGBLOCK
       output(screen, "   HAVE_SIGBLOCK\n");
#endif
#ifdef HAVE_SIGGETMASK
       output(screen, "   HAVE_SIGGETMASK\n");
#endif
#ifdef HAVE_SIGLONGJMP
       output(screen, "   HAVE_SIGLONGJMP\n");
#endif
#ifdef HAVE_SIGPROCMASK
       output(screen, "   HAVE_SIGPROCMASK\n");
#endif
#ifdef HAVE_SIGSET
       output(screen, "   HAVE_SIGSET\n");
#endif
#ifdef HAVE_SIGSETMASK
       output(screen, "   HAVE_SIGSETMASK\n");
#endif
#ifdef HAVE_SIG_ATOMIC_T_TYPE
       output(screen, "   HAVE_SIG_ATOMIC_T_TYPE\n");
#endif
#ifdef HAVE_SIMPLE_C_PROG
       output(screen, "   HAVE_SIMPLE_C_PROG\n");
#endif
#ifdef HAVE_SIZE_T
       output(screen, "   HAVE_SIZE_T\n");
#endif
#ifdef HAVE_SNPRINTF
       output(screen, "   HAVE_SNPRINTF\n");
#endif
#ifdef HAVE_SOCKADDR_SA_LEN
       output(screen, "   HAVE_SOCKADDR_SA_LEN\n");
#endif
#ifdef HAVE_SOCKETPAIR
       output(screen, "   HAVE_SOCKETPAIR\n");
#endif
#ifdef HAVE_SOCKLEN_T
       output(screen, "   HAVE_SOCKLEN_T\n");
#endif
#ifdef HAVE_SOCK_SIN6_LEN
       output(screen, "   HAVE_SOCK_SIN6_LEN\n");
#endif
#ifdef HAVE_SOCK_SIN_LEN
       output(screen, "   HAVE_SOCK_SIN_LEN\n");
#endif
#ifdef HAVE_SPLICE_DECL
       output(screen, "   HAVE_SPLICE_DECL\n");
#endif
#ifdef HAVE_SRAND
       output(screen, "   HAVE_SRAND\n");
#endif
#ifdef HAVE_SRANDOM
       output(screen, "   HAVE_SRANDOM\n");
#endif
#ifdef HAVE_SSIZE_T
       output(screen, "   HAVE_SSIZE_T\n");
#endif
#ifdef HAVE_SS_FAMILY
       output(screen, "   HAVE_SS_FAMILY\n");
#endif
#ifdef HAVE_STATFS_F_FSID
       output(screen, "   HAVE_STATFS_F_FSID\n");
#endif
#ifdef HAVE_STATVFS
       output(screen, "   HAVE_STATVFS\n");
#endif
#ifdef HAVE_STATVFS_F_FLAG
       output(screen, "   HAVE_STATVFS_F_FLAG\n");
#endif
#ifdef HAVE_STATVFS_F_FLAGS
       output(screen, "   HAVE_STATVFS_F_FLAGS\n");
#endif
#ifdef HAVE_STAT_HIRES_TIMESTAMPS
       output(screen, "   HAVE_STAT_HIRES_TIMESTAMPS\n");
#endif
#ifdef HAVE_STAT_ST_BLKSIZE
       output(screen, "   HAVE_STAT_ST_BLKSIZE\n");
#endif
#ifdef HAVE_STAT_ST_BLOCKS
       output(screen, "   HAVE_STAT_ST_BLOCKS\n");
#endif
#ifdef HAVE_STAT_ST_FLAGS
       output(screen, "   HAVE_STAT_ST_FLAGS\n");
#endif
#ifdef HAVE_STAT_TV_NSEC
       output(screen, "   HAVE_STAT_TV_NSEC\n");
#endif
#ifdef HAVE_STRCASECMP
       output(screen, "   HAVE_STRCASECMP\n");
#endif
#ifdef HAVE_STRCASESTR
       output(screen, "   HAVE_STRCASESTR\n");
#endif
#ifdef HAVE_STRCHR
       output(screen, "   HAVE_STRCHR\n");
#endif
#ifdef HAVE_STRCPY
       output(screen, "   HAVE_STRCPY\n");
#endif
#ifdef HAVE_STRDUP
       output(screen, "   HAVE_STRDUP\n");
#endif
#ifdef HAVE_STRERROR
       output(screen, "   HAVE_STRERROR\n");
#endif
#ifdef HAVE_STRERROR_R
       output(screen, "   HAVE_STRERROR_R\n");
#endif
#ifdef HAVE_STRFTIME
       output(screen, "   HAVE_STRFTIME\n");
#endif
#ifdef HAVE_STRLCAT
       output(screen, "   HAVE_STRLCAT\n");
#endif
#ifdef HAVE_STRLCPY
       output(screen, "   HAVE_STRLCPY\n");
#endif
#ifdef HAVE_STRNCPY
       output(screen, "   HAVE_STRNCPY\n");
#endif
#ifdef HAVE_STRNDUP
       output(screen, "   HAVE_STRNDUP\n");
#endif
#ifdef HAVE_STRNLEN
       output(screen, "   HAVE_STRNLEN\n");
#endif
#ifdef HAVE_STRPBRK
       output(screen, "   HAVE_STRPBRK\n");
#endif
#ifdef HAVE_STRPTIME
       output(screen, "   HAVE_STRPTIME\n");
#endif
#ifdef HAVE_STRSEP
       output(screen, "   HAVE_STRSEP\n");
#endif
#ifdef HAVE_STRSIGNAL
       output(screen, "   HAVE_STRSIGNAL\n");
#endif
#ifdef HAVE_STRTOK_R
       output(screen, "   HAVE_STRTOK_R\n");
#endif
#ifdef HAVE_STRTOL
       output(screen, "   HAVE_STRTOL\n");
#endif
#ifdef HAVE_STRTOLL
       output(screen, "   HAVE_STRTOLL\n");
#endif
#ifdef HAVE_STRTOQ
       output(screen, "   HAVE_STRTOQ\n");
#endif
#ifdef HAVE_STRTOULL
       output(screen, "   HAVE_STRTOULL\n");
#endif
#ifdef HAVE_STRTOUQ
       output(screen, "   HAVE_STRTOUQ\n");
#endif
#ifdef HAVE_STRUCT_ADDRINFO
       output(screen, "   HAVE_STRUCT_ADDRINFO\n");
#endif
#ifdef HAVE_STRUCT_IFADDRS
       output(screen, "   HAVE_STRUCT_IFADDRS\n");
#endif
#ifdef HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
       output(screen, "   HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS\n");
#endif
#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL
       output(screen, "   HAVE_STRUCT_MSGHDR_MSG_CONTROL\n");
#endif
#ifdef HAVE_STRUCT_SIGEVENT
       output(screen, "   HAVE_STRUCT_SIGEVENT\n");
#endif
#ifdef HAVE_STRUCT_SIGEVENT_SIGEV_VALUE_SIGVAL_PTR
       output(screen, "   HAVE_STRUCT_SIGEVENT_SIGEV_VALUE_SIGVAL_PTR\n");
#endif
#ifdef HAVE_STRUCT_SIGEVENT_SIGEV_VALUE_SIVAL_PTR
       output(screen, "   HAVE_STRUCT_SIGEVENT_SIGEV_VALUE_SIVAL_PTR\n");
#endif
#ifdef HAVE_STRUCT_SOCKADDR
       output(screen, "   HAVE_STRUCT_SOCKADDR\n");
#endif
#ifdef HAVE_STRUCT_SOCKADDR_IN6
       output(screen, "   HAVE_STRUCT_SOCKADDR_IN6\n");
#endif
#ifdef HAVE_STRUCT_SOCKADDR_STORAGE
       output(screen, "   HAVE_STRUCT_SOCKADDR_STORAGE\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
       output(screen, "   HAVE_STRUCT_STAT_ST_BIRTHTIME\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC
       output(screen, "   HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC
       output(screen, "   HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_MTIMENSEC
       output(screen, "   HAVE_STRUCT_STAT_ST_MTIMENSEC\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC
       output(screen, "   HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_MTIME_N
       output(screen, "   HAVE_STRUCT_STAT_ST_MTIME_N\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC
       output(screen, "   HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_RDEV
       output(screen, "   HAVE_STRUCT_STAT_ST_RDEV\n");
#endif
#ifdef HAVE_STRUCT_STAT_ST_UMTIME
       output(screen, "   HAVE_STRUCT_STAT_ST_UMTIME\n");
#endif
#ifdef HAVE_STRUCT_TIMESPEC
       output(screen, "   HAVE_STRUCT_TIMESPEC\n");
#endif
#ifdef HAVE_STRUPR
       output(screen, "   HAVE_STRUPR\n");
#endif
#ifdef HAVE_ST_RDEV
       output(screen, "   HAVE_ST_RDEV\n");
#endif
#ifdef HAVE_SYMLINK
       output(screen, "   HAVE_SYMLINK\n");
#endif
#ifdef HAVE_SYSCALL
       output(screen, "   HAVE_SYSCALL\n");
#endif
#ifdef HAVE_SYSCONF
       output(screen, "   HAVE_SYSCONF\n");
#endif
#ifdef HAVE_SYSCTL
       output(screen, "   HAVE_SYSCTL\n");
#endif
#ifdef HAVE_SYSCTLBYNAME
       output(screen, "   HAVE_SYSCTLBYNAME\n");
#endif
#ifdef HAVE_SYSLOG
       output(screen, "   HAVE_SYSLOG\n");
#endif
#ifdef HAVE_TALLOC
       output(screen, "   HAVE_TALLOC\n");
#endif
#ifdef HAVE_TDB
       output(screen, "   HAVE_TDB\n");
#endif
#ifdef HAVE_TEVENT
       output(screen, "   HAVE_TEVENT\n");
#endif
#ifdef HAVE_TEXTDOMAIN
       output(screen, "   HAVE_TEXTDOMAIN\n");
#endif
#ifdef HAVE_TGETENT
       output(screen, "   HAVE_TGETENT\n");
#endif
#ifdef HAVE_TICKET_POINTER_IN_KRB5_AP_REQ
       output(screen, "   HAVE_TICKET_POINTER_IN_KRB5_AP_REQ\n");
#endif
#ifdef HAVE_TIMEGM
       output(screen, "   HAVE_TIMEGM\n");
#endif
#ifdef HAVE_TRACKER_SPARQL_1_0
       output(screen, "   HAVE_TRACKER_SPARQL_1_0\n");
#endif
#ifdef HAVE_UCONTEXT_T
       output(screen, "   HAVE_UCONTEXT_T\n");
#endif
#ifdef HAVE_UINT16_T
       output(screen, "   HAVE_UINT16_T\n");
#endif
#ifdef HAVE_UINT32_T
       output(screen, "   HAVE_UINT32_T\n");
#endif
#ifdef HAVE_UINT64_T
       output(screen, "   HAVE_UINT64_T\n");
#endif
#ifdef HAVE_UINT8_T
       output(screen, "   HAVE_UINT8_T\n");
#endif
#ifdef HAVE_UINTPTR_T
       output(screen, "   HAVE_UINTPTR_T\n");
#endif
#ifdef HAVE_UINT_T
       output(screen, "   HAVE_UINT_T\n");
#endif
#ifdef HAVE_UNIXSOCKET
       output(screen, "   HAVE_UNIXSOCKET\n");
#endif
#ifdef HAVE_UNSETENV
       output(screen, "   HAVE_UNSETENV\n");
#endif
#ifdef HAVE_UPDWTMP
       output(screen, "   HAVE_UPDWTMP\n");
#endif
#ifdef HAVE_UPDWTMPX
       output(screen, "   HAVE_UPDWTMPX\n");
#endif
#ifdef HAVE_USLEEP
       output(screen, "   HAVE_USLEEP\n");
#endif
#ifdef HAVE_UTIMBUF
       output(screen, "   HAVE_UTIMBUF\n");
#endif
#ifdef HAVE_UTIME
       output(screen, "   HAVE_UTIME\n");
#endif
#ifdef HAVE_UTIMENSAT
       output(screen, "   HAVE_UTIMENSAT\n");
#endif
#ifdef HAVE_UTIMES
       output(screen, "   HAVE_UTIMES\n");
#endif
#ifdef HAVE_UX_UT_SYSLEN
       output(screen, "   HAVE_UX_UT_SYSLEN\n");
#endif
#ifdef HAVE_VASPRINTF
       output(screen, "   HAVE_VASPRINTF\n");
#endif
#ifdef HAVE_VA_COPY
       output(screen, "   HAVE_VA_COPY\n");
#endif
#ifdef HAVE_VDPRINTF
       output(screen, "   HAVE_VDPRINTF\n");
#endif
#ifdef HAVE_VISIBILITY_ATTR
       output(screen, "   HAVE_VISIBILITY_ATTR\n");
#endif
#ifdef HAVE_VOLATILE
       output(screen, "   HAVE_VOLATILE\n");
#endif
#ifdef HAVE_VSNPRINTF
       output(screen, "   HAVE_VSNPRINTF\n");
#endif
#ifdef HAVE_VSYSLOG
       output(screen, "   HAVE_VSYSLOG\n");
#endif
#ifdef HAVE_WAIT4
       output(screen, "   HAVE_WAIT4\n");
#endif
#ifdef HAVE_WAITPID
       output(screen, "   HAVE_WAITPID\n");
#endif
#ifdef HAVE_WNO_FORMAT_TRUNCATION
       output(screen, "   HAVE_WNO_FORMAT_TRUNCATION\n");
#endif
#ifdef HAVE_WNO_UNUSED_FUNCTION
       output(screen, "   HAVE_WNO_UNUSED_FUNCTION\n");
#endif
#ifdef HAVE_WORKING_READLINE_READLINE_WITH_STRICT_PROTO
       output(screen, "   HAVE_WORKING_READLINE_READLINE_WITH_STRICT_PROTO\n");
#endif
#ifdef HAVE_WORKING_STRPTIME
       output(screen, "   HAVE_WORKING_STRPTIME\n");
#endif
#ifdef HAVE_WRFILE_KEYTAB
       output(screen, "   HAVE_WRFILE_KEYTAB\n");
#endif
#ifdef HAVE_XATTR_SUPPORT
       output(screen, "   HAVE_XATTR_SUPPORT\n");
#endif
#ifdef HAVE_YP_GET_DEFAULT_DOMAIN
       output(screen, "   HAVE_YP_GET_DEFAULT_DOMAIN\n");
#endif
#ifdef HAVE_Z
       output(screen, "   HAVE_Z\n");
#endif
#ifdef HAVE_ZLIBVERSION
       output(screen, "   HAVE_ZLIBVERSION\n");
#endif
#ifdef HAVE__ACL
       output(screen, "   HAVE__ACL\n");
#endif
#ifdef HAVE__Bool
       output(screen, "   HAVE__Bool\n");
#endif
#ifdef HAVE__CHDIR
       output(screen, "   HAVE__CHDIR\n");
#endif
#ifdef HAVE__CLOSE
       output(screen, "   HAVE__CLOSE\n");
#endif
#ifdef HAVE__CLOSEDIR
       output(screen, "   HAVE__CLOSEDIR\n");
#endif
#ifdef HAVE__DN_EXPAND
       output(screen, "   HAVE__DN_EXPAND\n");
#endif
#ifdef HAVE__DUP
       output(screen, "   HAVE__DUP\n");
#endif
#ifdef HAVE__DUP2
       output(screen, "   HAVE__DUP2\n");
#endif
#ifdef HAVE__ET_LIST
       output(screen, "   HAVE__ET_LIST\n");
#endif
#ifdef HAVE__FACL
       output(screen, "   HAVE__FACL\n");
#endif
#ifdef HAVE__FCHDIR
       output(screen, "   HAVE__FCHDIR\n");
#endif
#ifdef HAVE__FCNTL
       output(screen, "   HAVE__FCNTL\n");
#endif
#ifdef HAVE__FORK
       output(screen, "   HAVE__FORK\n");
#endif
#ifdef HAVE__FSTAT
       output(screen, "   HAVE__FSTAT\n");
#endif
#ifdef HAVE__GETCWD
       output(screen, "   HAVE__GETCWD\n");
#endif
#ifdef HAVE__LLSEEK
       output(screen, "   HAVE__LLSEEK\n");
#endif
#ifdef HAVE__LSEEK
       output(screen, "   HAVE__LSEEK\n");
#endif
#ifdef HAVE__LSTAT
       output(screen, "   HAVE__LSTAT\n");
#endif
#ifdef HAVE__OPEN
       output(screen, "   HAVE__OPEN\n");
#endif
#ifdef HAVE__OPENDIR
       output(screen, "   HAVE__OPENDIR\n");
#endif
#ifdef HAVE__PREAD
       output(screen, "   HAVE__PREAD\n");
#endif
#ifdef HAVE__PWRITE
       output(screen, "   HAVE__PWRITE\n");
#endif
#ifdef HAVE__READ
       output(screen, "   HAVE__READ\n");
#endif
#ifdef HAVE__READDIR
       output(screen, "   HAVE__READDIR\n");
#endif
#ifdef HAVE__SEEKDIR
       output(screen, "   HAVE__SEEKDIR\n");
#endif
#ifdef HAVE__STAT
       output(screen, "   HAVE__STAT\n");
#endif
#ifdef HAVE__TELLDIR
       output(screen, "   HAVE__TELLDIR\n");
#endif
#ifdef HAVE__VA_ARGS__MACRO
       output(screen, "   HAVE__VA_ARGS__MACRO\n");
#endif
#ifdef HAVE__WRITE
       output(screen, "   HAVE__WRITE\n");
#endif
#ifdef HAVE___ACL
       output(screen, "   HAVE___ACL\n");
#endif
#ifdef HAVE___ATTRIBUTE__
       output(screen, "   HAVE___ATTRIBUTE__\n");
#endif
#ifdef HAVE___CHDIR
       output(screen, "   HAVE___CHDIR\n");
#endif
#ifdef HAVE___CLOSE
       output(screen, "   HAVE___CLOSE\n");
#endif
#ifdef HAVE___CLOSEDIR
       output(screen, "   HAVE___CLOSEDIR\n");
#endif
#ifdef HAVE___DN_EXPAND
       output(screen, "   HAVE___DN_EXPAND\n");
#endif
#ifdef HAVE___DUP
       output(screen, "   HAVE___DUP\n");
#endif
#ifdef HAVE___DUP2
       output(screen, "   HAVE___DUP2\n");
#endif
#ifdef HAVE___FACL
       output(screen, "   HAVE___FACL\n");
#endif
#ifdef HAVE___FCHDIR
       output(screen, "   HAVE___FCHDIR\n");
#endif
#ifdef HAVE___FCNTL
       output(screen, "   HAVE___FCNTL\n");
#endif
#ifdef HAVE___FORK
       output(screen, "   HAVE___FORK\n");
#endif
#ifdef HAVE___FSTAT
       output(screen, "   HAVE___FSTAT\n");
#endif
#ifdef HAVE___FXSTAT
       output(screen, "   HAVE___FXSTAT\n");
#endif
#ifdef HAVE___GETCWD
       output(screen, "   HAVE___GETCWD\n");
#endif
#ifdef HAVE___GETDENTS
       output(screen, "   HAVE___GETDENTS\n");
#endif
#ifdef HAVE___LLSEEK
       output(screen, "   HAVE___LLSEEK\n");
#endif
#ifdef HAVE___LSEEK
       output(screen, "   HAVE___LSEEK\n");
#endif
#ifdef HAVE___LSTAT
       output(screen, "   HAVE___LSTAT\n");
#endif
#ifdef HAVE___LXSTAT
       output(screen, "   HAVE___LXSTAT\n");
#endif
#ifdef HAVE___OPEN
       output(screen, "   HAVE___OPEN\n");
#endif
#ifdef HAVE___OPENDIR
       output(screen, "   HAVE___OPENDIR\n");
#endif
#ifdef HAVE___PREAD
       output(screen, "   HAVE___PREAD\n");
#endif
#ifdef HAVE___PWRITE
       output(screen, "   HAVE___PWRITE\n");
#endif
#ifdef HAVE___READ
       output(screen, "   HAVE___READ\n");
#endif
#ifdef HAVE___READDIR
       output(screen, "   HAVE___READDIR\n");
#endif
#ifdef HAVE___SEEKDIR
       output(screen, "   HAVE___SEEKDIR\n");
#endif
#ifdef HAVE___SS_FAMILY
       output(screen, "   HAVE___SS_FAMILY\n");
#endif
#ifdef HAVE___STAT
       output(screen, "   HAVE___STAT\n");
#endif
#ifdef HAVE___STRTOLL
       output(screen, "   HAVE___STRTOLL\n");
#endif
#ifdef HAVE___STRTOULL
       output(screen, "   HAVE___STRTOULL\n");
#endif
#ifdef HAVE___SYNC_FETCH_AND_ADD
       output(screen, "   HAVE___SYNC_FETCH_AND_ADD\n");
#endif
#ifdef HAVE___SYS_LLSEEK
       output(screen, "   HAVE___SYS_LLSEEK\n");
#endif
#ifdef HAVE___TELLDIR
       output(screen, "   HAVE___TELLDIR\n");
#endif
#ifdef HAVE___WRITE
       output(screen, "   HAVE___WRITE\n");
#endif
#ifdef HAVE___XSTAT
       output(screen, "   HAVE___XSTAT\n");
#endif


	/* Show --with Options */
       output(screen, "\n--with Options:\n");

#ifdef WITH_ADS
       output(screen, "   WITH_ADS\n");
#endif
#ifdef WITH_AUTOMOUNT
       output(screen, "   WITH_AUTOMOUNT\n");
#endif
#ifdef WITH_AVAHI_SUPPORT
       output(screen, "   WITH_AVAHI_SUPPORT\n");
#endif
#ifdef WITH_DNS_UPDATES
       output(screen, "   WITH_DNS_UPDATES\n");
#endif
#ifdef WITH_PAM
       output(screen, "   WITH_PAM\n");
#endif
#ifdef WITH_PAM_MODULES
       output(screen, "   WITH_PAM_MODULES\n");
#endif
#ifdef WITH_PTHREADPOOL
       output(screen, "   WITH_PTHREADPOOL\n");
#endif
#ifdef WITH_SENDFILE
       output(screen, "   WITH_SENDFILE\n");
#endif
#ifdef WITH_SPOTLIGHT
       output(screen, "   WITH_SPOTLIGHT\n");
#endif
#ifdef WITH_SYSLOG
       output(screen, "   WITH_SYSLOG\n");
#endif
#ifdef WITH_WINBIND
       output(screen, "   WITH_WINBIND\n");
#endif


	/* Show Build Options */
       output(screen, "\nBuild Options:\n");

#ifdef BROKEN_NISPLUS_INCLUDE_FILES
       output(screen, "   BROKEN_NISPLUS_INCLUDE_FILES\n");
#endif
#ifdef BSD_STYLE_STATVFS
       output(screen, "   BSD_STYLE_STATVFS\n");
#endif
#ifdef CHECK_BUNDLED_SYSTEM_Z
       output(screen, "   CHECK_BUNDLED_SYSTEM_Z\n");
#endif
#ifdef COMPILER_SUPPORTS_LL
       output(screen, "   COMPILER_SUPPORTS_LL\n");
#endif
#ifdef CONFIG_H_IS_FROM_SAMBA
       output(screen, "   CONFIG_H_IS_FROM_SAMBA\n");
#endif
#ifdef DEFAULT_DOS_CHARSET
       output(screen, "   DEFAULT_DOS_CHARSET\n");
#endif
#ifdef DEFAULT_UNIX_CHARSET
       output(screen, "   DEFAULT_UNIX_CHARSET\n");
#endif
#ifdef DLOPEN_TAKES_UNSIGNED_FLAGS
       output(screen, "   DLOPEN_TAKES_UNSIGNED_FLAGS\n");
#endif
#ifdef ENABLE_GNUTLS
       output(screen, "   ENABLE_GNUTLS\n");
#endif
#ifdef EXPECTED_SYSTEM_LDB_VERSION_MAJOR
       output(screen, "   EXPECTED_SYSTEM_LDB_VERSION_MAJOR\n");
#endif
#ifdef EXPECTED_SYSTEM_LDB_VERSION_MINOR
       output(screen, "   EXPECTED_SYSTEM_LDB_VERSION_MINOR\n");
#endif
#ifdef EXPECTED_SYSTEM_LDB_VERSION_RELEASE
       output(screen, "   EXPECTED_SYSTEM_LDB_VERSION_RELEASE\n");
#endif
#ifdef GETCWD_TAKES_NULL
       output(screen, "   GETCWD_TAKES_NULL\n");
#endif
#ifdef INLINE_MACRO
       output(screen, "   INLINE_MACRO\n");
#endif
#ifdef KRB5_CREDS_OPT_FREE_REQUIRES_CONTEXT
       output(screen, "   KRB5_CREDS_OPT_FREE_REQUIRES_CONTEXT\n");
#endif
#ifdef LDAP_DEPRECATED
       output(screen, "   LDAP_DEPRECATED\n");
#endif
#ifdef LDAP_SET_REBIND_PROC_ARGS
       output(screen, "   LDAP_SET_REBIND_PROC_ARGS\n");
#endif
#ifdef LIBREPLACE_NETWORK_CHECKS
       output(screen, "   LIBREPLACE_NETWORK_CHECKS\n");
#endif
#ifdef LINUX
       output(screen, "   LINUX\n");
#endif
#ifdef LINUX_SENDFILE_API
       output(screen, "   LINUX_SENDFILE_API\n");
#endif
#ifdef REALPATH_TAKES_NULL
       output(screen, "   REALPATH_TAKES_NULL\n");
#endif
#ifdef RETSIGTYPE
       output(screen, "   RETSIGTYPE\n");
#endif
#ifdef RETSIGTYPE_INT
       output(screen, "   RETSIGTYPE_INT\n");
#endif
#ifdef SEEKDIR_RETURNS_VOID
       output(screen, "   SEEKDIR_RETURNS_VOID\n");
#endif
#ifdef SHLIBEXT
       output(screen, "   SHLIBEXT\n");
#endif
#ifdef SIZEOF_BLKCNT_T_4
       output(screen, "   SIZEOF_BLKCNT_T_4\n");
#endif
#ifdef SIZEOF_BLKCNT_T_8
       output(screen, "   SIZEOF_BLKCNT_T_8\n");
#endif
#ifdef SIZEOF_BOOL
       output(screen, "   SIZEOF_BOOL\n");
#endif
#ifdef SIZEOF_CHAR
       output(screen, "   SIZEOF_CHAR\n");
#endif
#ifdef SIZEOF_DEV_T
       output(screen, "   SIZEOF_DEV_T\n");
#endif
#ifdef SIZEOF_INO_T
       output(screen, "   SIZEOF_INO_T\n");
#endif
#ifdef SIZEOF_INT
       output(screen, "   SIZEOF_INT\n");
#endif
#ifdef SIZEOF_INT16_T
       output(screen, "   SIZEOF_INT16_T\n");
#endif
#ifdef SIZEOF_INT32_T
       output(screen, "   SIZEOF_INT32_T\n");
#endif
#ifdef SIZEOF_INT64_T
       output(screen, "   SIZEOF_INT64_T\n");
#endif
#ifdef SIZEOF_INT8_T
       output(screen, "   SIZEOF_INT8_T\n");
#endif
#ifdef SIZEOF_LONG
       output(screen, "   SIZEOF_LONG\n");
#endif
#ifdef SIZEOF_LONG_LONG
       output(screen, "   SIZEOF_LONG_LONG\n");
#endif
#ifdef SIZEOF_OFF_T
       output(screen, "   SIZEOF_OFF_T\n");
#endif
#ifdef SIZEOF_SHORT
       output(screen, "   SIZEOF_SHORT\n");
#endif
#ifdef SIZEOF_SIZE_T
       output(screen, "   SIZEOF_SIZE_T\n");
#endif
#ifdef SIZEOF_SSIZE_T
       output(screen, "   SIZEOF_SSIZE_T\n");
#endif
#ifdef SIZEOF_TIME_T
       output(screen, "   SIZEOF_TIME_T\n");
#endif
#ifdef SIZEOF_UINT16_T
       output(screen, "   SIZEOF_UINT16_T\n");
#endif
#ifdef SIZEOF_UINT32_T
       output(screen, "   SIZEOF_UINT32_T\n");
#endif
#ifdef SIZEOF_UINT64_T
       output(screen, "   SIZEOF_UINT64_T\n");
#endif
#ifdef SIZEOF_UINT8_T
       output(screen, "   SIZEOF_UINT8_T\n");
#endif
#ifdef SIZEOF_VOID_P
       output(screen, "   SIZEOF_VOID_P\n");
#endif
#ifdef SOLARIS_GETGRENT_R
       output(screen, "   SOLARIS_GETGRENT_R\n");
#endif
#ifdef SOLARIS_GETPWENT_R
       output(screen, "   SOLARIS_GETPWENT_R\n");
#endif
#ifdef SRCDIR
       output(screen, "   SRCDIR\n");
#endif
#ifdef STAT_STATVFS
       output(screen, "   STAT_STATVFS\n");
#endif
#ifdef STAT_ST_BLOCKSIZE
       output(screen, "   STAT_ST_BLOCKSIZE\n");
#endif
#ifdef STDC_HEADERS
       output(screen, "   STDC_HEADERS\n");
#endif
#ifdef STRERROR_R_XSI_NOT_GNU
       output(screen, "   STRERROR_R_XSI_NOT_GNU\n");
#endif
#ifdef STRING_STATIC_MODULES
       output(screen, "   STRING_STATIC_MODULES\n");
#endif
#ifdef SUMMARY_PASSES
       output(screen, "   SUMMARY_PASSES\n");
#endif
#ifdef SYSCONF_SC_NGROUPS_MAX
       output(screen, "   SYSCONF_SC_NGROUPS_MAX\n");
#endif
#ifdef SYSCONF_SC_NPROCESSORS_ONLN
       output(screen, "   SYSCONF_SC_NPROCESSORS_ONLN\n");
#endif
#ifdef SYSCONF_SC_NPROC_ONLN
       output(screen, "   SYSCONF_SC_NPROC_ONLN\n");
#endif
#ifdef SYSCONF_SC_PAGESIZE
       output(screen, "   SYSCONF_SC_PAGESIZE\n");
#endif
#ifdef SYSTEM_UNAME_MACHINE
       output(screen, "   SYSTEM_UNAME_MACHINE\n");
#endif
#ifdef SYSTEM_UNAME_RELEASE
       output(screen, "   SYSTEM_UNAME_RELEASE\n");
#endif
#ifdef SYSTEM_UNAME_SYSNAME
       output(screen, "   SYSTEM_UNAME_SYSNAME\n");
#endif
#ifdef SYSTEM_UNAME_VERSION
       output(screen, "   SYSTEM_UNAME_VERSION\n");
#endif
#ifdef TALLOC_BUILD_VERSION_MAJOR
       output(screen, "   TALLOC_BUILD_VERSION_MAJOR\n");
#endif
#ifdef TALLOC_BUILD_VERSION_MINOR
       output(screen, "   TALLOC_BUILD_VERSION_MINOR\n");
#endif
#ifdef TALLOC_BUILD_VERSION_RELEASE
       output(screen, "   TALLOC_BUILD_VERSION_RELEASE\n");
#endif
#ifdef TIME_T_MAX
       output(screen, "   TIME_T_MAX\n");
#endif
#ifdef TIME_WITH_SYS_TIME
       output(screen, "   TIME_WITH_SYS_TIME\n");
#endif
#ifdef USE_LINUX_32BIT_SYSCALLS
       output(screen, "   USE_LINUX_32BIT_SYSCALLS\n");
#endif
#ifdef USE_LINUX_THREAD_CREDENTIALS
       output(screen, "   USE_LINUX_THREAD_CREDENTIALS\n");
#endif
#ifdef USING_SYSTEM_KRB5
       output(screen, "   USING_SYSTEM_KRB5\n");
#endif
#ifdef USING_SYSTEM_LDB
       output(screen, "   USING_SYSTEM_LDB\n");
#endif
#ifdef USING_SYSTEM_PARSE_YAPP_DRIVER
       output(screen, "   USING_SYSTEM_PARSE_YAPP_DRIVER\n");
#endif
#ifdef USING_SYSTEM_POPT
       output(screen, "   USING_SYSTEM_POPT\n");
#endif
#ifdef USING_SYSTEM_PYLDB_UTIL
       output(screen, "   USING_SYSTEM_PYLDB_UTIL\n");
#endif
#ifdef USING_SYSTEM_PYTALLOC_UTIL
       output(screen, "   USING_SYSTEM_PYTALLOC_UTIL\n");
#endif
#ifdef USING_SYSTEM_PYTDB
       output(screen, "   USING_SYSTEM_PYTDB\n");
#endif
#ifdef USING_SYSTEM_TALLOC
       output(screen, "   USING_SYSTEM_TALLOC\n");
#endif
#ifdef USING_SYSTEM_TDB
       output(screen, "   USING_SYSTEM_TDB\n");
#endif
#ifdef USING_SYSTEM_TEVENT
       output(screen, "   USING_SYSTEM_TEVENT\n");
#endif
#ifdef USING_SYSTEM_ZLIB
       output(screen, "   USING_SYSTEM_ZLIB\n");
#endif
#ifdef VALUEOF_NSIG
       output(screen, "   VALUEOF_NSIG\n");
#endif
#ifdef VALUEOF_SIGRTMAX
       output(screen, "   VALUEOF_SIGRTMAX\n");
#endif
#ifdef VALUEOF_SIGRTMIN
       output(screen, "   VALUEOF_SIGRTMIN\n");
#endif
#ifdef VALUEOF__NSIG
       output(screen, "   VALUEOF__NSIG\n");
#endif
#ifdef WORKING_GETCONF_LFS_CFLAGS
       output(screen, "   WORKING_GETCONF_LFS_CFLAGS\n");
#endif
#ifdef XATTR_ADDITIONAL_OPTIONS
       output(screen, "   XATTR_ADDITIONAL_OPTIONS\n");
#endif
#ifdef XSLTPROC_MANPAGES
       output(screen, "   XSLTPROC_MANPAGES\n");
#endif
#ifdef _GNU_SOURCE
       output(screen, "   _GNU_SOURCE\n");
#endif
#ifdef _HAVE_ENCTYPE_ARCFOUR_HMAC_MD5
       output(screen, "   _HAVE_ENCTYPE_ARCFOUR_HMAC_MD5\n");
#endif
#ifdef _HAVE_ENCTYPE_ARCFOUR_HMAC_MD5_56
       output(screen, "   _HAVE_ENCTYPE_ARCFOUR_HMAC_MD5_56\n");
#endif
#ifdef _HAVE_KEYTYPE_ARCFOUR_56
       output(screen, "   _HAVE_KEYTYPE_ARCFOUR_56\n");
#endif
#ifdef _HAVE_SENDFILE
       output(screen, "   _HAVE_SENDFILE\n");
#endif
#ifdef _POSIX_FALLOCATE_CAPABLE_LIBC
       output(screen, "   _POSIX_FALLOCATE_CAPABLE_LIBC\n");
#endif
#ifdef _SAMBA_BUILD_
       output(screen, "   _SAMBA_BUILD_\n");
#endif
#ifdef _XOPEN_SOURCE_EXTENDED
       output(screen, "   _XOPEN_SOURCE_EXTENDED\n");
#endif
#ifdef __TIME_T_MAX
       output(screen, "   __TIME_T_MAX\n");
#endif
#ifdef auth_script_init
       output(screen, "   auth_script_init\n");
#endif
#ifdef idmap_ad_init
       output(screen, "   idmap_ad_init\n");
#endif
#ifdef idmap_autorid_init
       output(screen, "   idmap_autorid_init\n");
#endif
#ifdef idmap_hash_init
       output(screen, "   idmap_hash_init\n");
#endif
#ifdef idmap_rfc2307_init
       output(screen, "   idmap_rfc2307_init\n");
#endif
#ifdef idmap_rid_init
       output(screen, "   idmap_rid_init\n");
#endif
#ifdef idmap_script_init
       output(screen, "   idmap_script_init\n");
#endif
#ifdef idmap_tdb2_init
       output(screen, "   idmap_tdb2_init\n");
#endif
#ifdef offset_t
       output(screen, "   offset_t\n");
#endif
#ifdef static_decl_auth
       output(screen, "   static_decl_auth\n");
#endif
#ifdef static_decl_charset
       output(screen, "   static_decl_charset\n");
#endif
#ifdef static_decl_gpext
       output(screen, "   static_decl_gpext\n");
#endif
#ifdef static_decl_idmap
       output(screen, "   static_decl_idmap\n");
#endif
#ifdef static_decl_nss_info
       output(screen, "   static_decl_nss_info\n");
#endif
#ifdef static_decl_pdb
       output(screen, "   static_decl_pdb\n");
#endif
#ifdef static_decl_perfcount
       output(screen, "   static_decl_perfcount\n");
#endif
#ifdef static_decl_rpc
       output(screen, "   static_decl_rpc\n");
#endif
#ifdef static_decl_vfs
       output(screen, "   static_decl_vfs\n");
#endif
#ifdef static_init_auth
       output(screen, "   static_init_auth\n");
#endif
#ifdef static_init_charset
       output(screen, "   static_init_charset\n");
#endif
#ifdef static_init_gpext
       output(screen, "   static_init_gpext\n");
#endif
#ifdef static_init_idmap
       output(screen, "   static_init_idmap\n");
#endif
#ifdef static_init_nss_info
       output(screen, "   static_init_nss_info\n");
#endif
#ifdef static_init_pdb
       output(screen, "   static_init_pdb\n");
#endif
#ifdef static_init_perfcount
       output(screen, "   static_init_perfcount\n");
#endif
#ifdef static_init_rpc
       output(screen, "   static_init_rpc\n");
#endif
#ifdef static_init_vfs
       output(screen, "   static_init_vfs\n");
#endif
#ifdef uint_t
       output(screen, "   uint_t\n");
#endif
#ifdef vfs_acl_tdb_init
       output(screen, "   vfs_acl_tdb_init\n");
#endif
#ifdef vfs_acl_xattr_init
       output(screen, "   vfs_acl_xattr_init\n");
#endif
#ifdef vfs_aio_fork_init
       output(screen, "   vfs_aio_fork_init\n");
#endif
#ifdef vfs_aio_pthread_init
       output(screen, "   vfs_aio_pthread_init\n");
#endif
#ifdef vfs_audit_init
       output(screen, "   vfs_audit_init\n");
#endif
#ifdef vfs_btrfs_init
       output(screen, "   vfs_btrfs_init\n");
#endif
#ifdef vfs_cap_init
       output(screen, "   vfs_cap_init\n");
#endif
#ifdef vfs_catia_init
       output(screen, "   vfs_catia_init\n");
#endif
#ifdef vfs_commit_init
       output(screen, "   vfs_commit_init\n");
#endif
#ifdef vfs_crossrename_init
       output(screen, "   vfs_crossrename_init\n");
#endif
#ifdef vfs_default_quota_init
       output(screen, "   vfs_default_quota_init\n");
#endif
#ifdef vfs_dirsort_init
       output(screen, "   vfs_dirsort_init\n");
#endif
#ifdef vfs_expand_msdfs_init
       output(screen, "   vfs_expand_msdfs_init\n");
#endif
#ifdef vfs_extd_audit_init
       output(screen, "   vfs_extd_audit_init\n");
#endif
#ifdef vfs_fake_perms_init
       output(screen, "   vfs_fake_perms_init\n");
#endif
#ifdef vfs_fileid_init
       output(screen, "   vfs_fileid_init\n");
#endif
#ifdef vfs_fruit_init
       output(screen, "   vfs_fruit_init\n");
#endif
#ifdef vfs_full_audit_init
       output(screen, "   vfs_full_audit_init\n");
#endif
#ifdef vfs_glusterfs_fuse_init
       output(screen, "   vfs_glusterfs_fuse_init\n");
#endif
#ifdef vfs_linux_xfs_sgid_init
       output(screen, "   vfs_linux_xfs_sgid_init\n");
#endif
#ifdef vfs_media_harmony_init
       output(screen, "   vfs_media_harmony_init\n");
#endif
#ifdef vfs_netatalk_init
       output(screen, "   vfs_netatalk_init\n");
#endif
#ifdef vfs_offline_init
       output(screen, "   vfs_offline_init\n");
#endif
#ifdef vfs_preopen_init
       output(screen, "   vfs_preopen_init\n");
#endif
#ifdef vfs_readahead_init
       output(screen, "   vfs_readahead_init\n");
#endif
#ifdef vfs_readonly_init
       output(screen, "   vfs_readonly_init\n");
#endif
#ifdef vfs_recycle_init
       output(screen, "   vfs_recycle_init\n");
#endif
#ifdef vfs_shadow_copy2_init
       output(screen, "   vfs_shadow_copy2_init\n");
#endif
#ifdef vfs_shadow_copy_init
       output(screen, "   vfs_shadow_copy_init\n");
#endif
#ifdef vfs_shell_snap_init
       output(screen, "   vfs_shell_snap_init\n");
#endif
#ifdef vfs_snapper_init
       output(screen, "   vfs_snapper_init\n");
#endif
#ifdef vfs_streams_depot_init
       output(screen, "   vfs_streams_depot_init\n");
#endif
#ifdef vfs_streams_xattr_init
       output(screen, "   vfs_streams_xattr_init\n");
#endif
#ifdef vfs_syncops_init
       output(screen, "   vfs_syncops_init\n");
#endif
#ifdef vfs_time_audit_init
       output(screen, "   vfs_time_audit_init\n");
#endif
#ifdef vfs_unityed_media_init
       output(screen, "   vfs_unityed_media_init\n");
#endif
#ifdef vfs_virusfilter_init
       output(screen, "   vfs_virusfilter_init\n");
#endif
#ifdef vfs_worm_init
       output(screen, "   vfs_worm_init\n");
#endif
#ifdef vfs_xattr_tdb_init
       output(screen, "   vfs_xattr_tdb_init\n");
#endif

       /* Output the sizes of the various cluster features */
       output(screen, "\n%s", cluster_support_features());

       /* Output the sizes of the various types */
       output(screen, "\nType sizes:\n");
       output(screen, "   sizeof(char):         %lu\n",(unsigned long)sizeof(char));
       output(screen, "   sizeof(int):          %lu\n",(unsigned long)sizeof(int));
       output(screen, "   sizeof(long):         %lu\n",(unsigned long)sizeof(long));
       output(screen, "   sizeof(long long):    %lu\n",(unsigned long)sizeof(long long));
       output(screen, "   sizeof(uint8_t):      %lu\n",(unsigned long)sizeof(uint8_t));
       output(screen, "   sizeof(uint16_t):     %lu\n",(unsigned long)sizeof(uint16_t));
       output(screen, "   sizeof(uint32_t):     %lu\n",(unsigned long)sizeof(uint32_t));
       output(screen, "   sizeof(short):        %lu\n",(unsigned long)sizeof(short));
       output(screen, "   sizeof(void*):        %lu\n",(unsigned long)sizeof(void*));
       output(screen, "   sizeof(size_t):       %lu\n",(unsigned long)sizeof(size_t));
       output(screen, "   sizeof(off_t):        %lu\n",(unsigned long)sizeof(off_t));
       output(screen, "   sizeof(ino_t):        %lu\n",(unsigned long)sizeof(ino_t));
       output(screen, "   sizeof(dev_t):        %lu\n",(unsigned long)sizeof(dev_t));

       output(screen, "\nBuiltin modules:\n");
       output(screen, "   %s\n", STRING_STATIC_MODULES);
}
