#serial 13, with several lines deleted for fileutils.

dnl Misc type-related macros for fileutils, sh-utils, textutils.

AC_DEFUN(jm_MACROS,
[
  AC_PREREQ(2.14a)

  GNU_PACKAGE="GNU $PACKAGE"
  AC_DEFINE_UNQUOTED(GNU_PACKAGE, "$GNU_PACKAGE",
    [The concatenation of the strings `GNU ', and PACKAGE.])
  AC_SUBST(GNU_PACKAGE)

  AC_SUBST(OPTIONAL_BIN_PROGS)
  AC_SUBST(OPTIONAL_BIN_ZCRIPTS)
  AC_SUBST(MAN)
  AC_SUBST(DF_PROG)

  dnl This macro actually runs replacement code.  See isc-posix.m4.
  AC_REQUIRE([AC_ISC_POSIX])dnl

  AC_CHECK_HEADERS( \
    errno.h  \
    fcntl.h \
    fenv.h \
    float.h \
    limits.h \
    memory.h \
    mntent.h \
    mnttab.h \
    netdb.h \
    paths.h \
    stdlib.h \
    stddef.h \
    string.h \
    sys/acl.h \
    sys/filsys.h \
    sys/fs/s5param.h \
    sys/fs_types.h \
    sys/fstyp.h \
    sys/ioctl.h \
    sys/mntent.h \
    sys/mount.h \
    sys/param.h \
    sys/socket.h \
    sys/statfs.h \
    sys/statvfs.h \
    sys/systeminfo.h \
    sys/time.h \
    sys/timeb.h \
    sys/vfs.h \
    sys/wait.h \
    syslog.h \
    termios.h \
    unistd.h \
    values.h \
  )

  jm_INCLUDED_REGEX([lib/regex.c])

  AC_REQUIRE([jm_ASSERT])
  AC_REQUIRE([jm_AC_HEADER_INTTYPES_H])
  AC_REQUIRE([jm_CHECK_TYPE_STRUCT_DIRENT_D_TYPE])
  AC_REQUIRE([jm_CHECK_TYPE_STRUCT_DIRENT_D_INO])
  AC_REQUIRE([jm_CHECK_DECLS])

  AC_REQUIRE([jm_PREREQ])

  AC_REQUIRE([jm_FUNC_MKTIME])
  AC_REQUIRE([jm_FUNC_LSTAT])
  AC_REQUIRE([jm_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK])
  AC_REQUIRE([jm_FUNC_STAT])
  AC_REQUIRE([jm_FUNC_REALLOC])
  AC_REQUIRE([jm_FUNC_MALLOC])
  AC_REQUIRE([AC_FUNC_STRERROR_R])
  AC_REQUIRE([jm_FUNC_READDIR])
  AC_REQUIRE([jm_FUNC_MEMCMP])
  AC_REQUIRE([jm_FUNC_GLIBC_UNLOCKED_IO])
  AC_REQUIRE([kd_FUNC_FNMATCH_CASE_RPL])
  AC_REQUIRE([jm_AFS])
  AC_REQUIRE([jm_AC_PREREQ_XSTRTOUMAX])
  AC_REQUIRE([jm_AC_FUNC_LINK_FOLLOWS_SYMLINK])
  AC_REQUIRE([AM_FUNC_ERROR_AT_LINE])
  AC_REQUIRE([jm_FUNC_GNU_STRFTIME])
  AC_REQUIRE([jm_FUNC_MKTIME])

  AC_REQUIRE([AC_FUNC_VPRINTF])
  AC_REQUIRE([AC_FUNC_ALLOCA])

  AC_REPLACE_FUNCS(strncasecmp)
  AC_REPLACE_FUNCS(stpcpy strstr strtol strtoul)
  AC_REPLACE_FUNCS(memcmp rpmatch)

  dnl used by e.g. intl/*domain.c and lib/canon-host.c
  AC_REPLACE_FUNCS(strdup)

  AC_REPLACE_FUNCS(memcpy memset)
  AC_CHECK_FUNCS(getpagesize)

  # By default, argmatch should fail calling usage (1).
  AC_DEFINE(ARGMATCH_DIE, [usage (1)],
	    [Define to the function xargmatch calls on failures.])
  AC_DEFINE(ARGMATCH_DIE_DECL, [extern void usage ()],
	    [Define to the declaration of the xargmatch failure function.])

  dnl Used to define SETVBUF in sys2.h.
  dnl This evokes the following warning from autoconf:
  dnl ...: warning: AC_TRY_RUN called without default to allow cross compiling
  AC_FUNC_SETVBUF_REVERSED

  AC_CHECK_FUNCS(getdelim)

  AC_REQUIRE([AC_FUNC_CLOSEDIR_VOID])

  AC_CHECK_FUNCS( \
    acl \
    bcopy \
    endgrent \
    endpwent \
    fchdir \
    fdatasync \
    fseeko \
    ftime \
    getcwd \
    gethrtime \
    getmntinfo \
    hasmntopt \
    isascii \
    listmntent \
    localeconv \
    memcpy \
    mempcpy \
    mkfifo \
    realpath \
    resolvepath \
    sethostname \
    strchr \
    strerror \
    strrchr \
    sysinfo \
    tzset \
  )

  AM_FUNC_GETLINE
  if test $am_cv_func_working_getline != yes; then
    AC_CHECK_FUNCS(getdelim)
  fi

  jm_GLIBC21
  jm_ICONV

  # These tests are for df.
  jm_FSTYPENAME

])

AC_DEFUN(jm_CHECK_ALL_TYPES,
[
  dnl Checks for typedefs, structures, and compiler characteristics.
  AC_REQUIRE([AC_C_BIGENDIAN])
  AC_REQUIRE([AC_PROG_CC_STDC])
  AC_REQUIRE([AC_C_CONST])
  AC_REQUIRE([AC_C_VOLATILE])
  AC_REQUIRE([AC_C_INLINE])
  AC_REQUIRE([AC_C_LONG_DOUBLE])

  AC_REQUIRE([AC_HEADER_DIRENT])
  AC_REQUIRE([AC_HEADER_STDC])
  AC_CHECK_MEMBERS([struct stat.st_blksize],,,[$ac_includes_default
#include <sys/stat.h>
  ])
  AC_REQUIRE([AC_STRUCT_ST_BLOCKS])

  AC_REQUIRE([AC_STRUCT_TM])
  AC_REQUIRE([AC_STRUCT_TIMEZONE])
  AC_REQUIRE([AC_HEADER_STAT])
  AC_REQUIRE([AC_STRUCT_ST_MTIM_NSEC])
  AC_REQUIRE([AC_STRUCT_ST_DM_MODE])
  AC_REQUIRE([jm_CHECK_TYPE_STRUCT_TIMESPEC])

  AC_REQUIRE([AC_TYPE_GETGROUPS])
  AC_REQUIRE([AC_TYPE_MODE_T])
  AC_REQUIRE([AC_TYPE_OFF_T])
  AC_REQUIRE([AC_TYPE_PID_T])
  AC_REQUIRE([AC_TYPE_SIGNAL])
  AC_REQUIRE([AC_TYPE_SIZE_T])
  AC_REQUIRE([AC_TYPE_UID_T])
  AC_CHECK_TYPE(ino_t, unsigned long)

  dnl This relies on the fact that autoconf 2.14a's implementation of
  dnl AC_CHECK_TYPE checks includes unistd.h.
  AC_CHECK_TYPE(ssize_t, int)

  AC_REQUIRE([jm_AC_TYPE_UINTMAX_T])
  AC_REQUIRE([jm_AC_TYPE_UNSIGNED_LONG_LONG])

  AC_REQUIRE([AC_HEADER_MAJOR])
  AC_REQUIRE([AC_HEADER_DIRENT])

])
