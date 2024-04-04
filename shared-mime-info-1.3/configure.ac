AC_INIT([shared-mime-info],[1.3],[https://bugs.freedesktop.org/enter_bug.cgi?product=shared-mime-info])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-xz check-news])

AC_CONFIG_HEADERS([config.h])

# Enable silent build when available (Automake 1.11)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

dnl Checks for programs.
AC_PROG_CC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=shared-mime-info
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

AC_CHECK_FUNCS(fdatasync)

dnl Check for cross compiling
AM_CONDITIONAL(CROSS_COMPILING, test $cross_compiling = yes)

dnl Check whether libxml and glib are present is installed
PKG_CHECK_MODULES(ALL, \
		  libxml-2.0 >= 2.4 \
                  glib-2.0 >= 2.6.0)
AC_SUBST(ALL_CFLAGS)
AC_SUBST(ALL_LIBS)

dnl Build the tree magic test if gio is present
PKG_CHECK_MODULES(GIO, gio-2.0, HAVE_GIO=yes, AC_MSG_RESULT(no))

AM_CONDITIONAL([HAVE_GIO], test "x$HAVE_GIO" = "xyes")

dnl Use -Wall, etc if possible
[
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -Wmissing-prototypes -Wwrite-strings" ;;
  esac
fi
]

AC_ARG_ENABLE(default-make-check,
   AC_HELP_STRING([--disable-default-make-check],
                   [run make check by default [default=yes]]),,
    enable_default_make_check=yes)
AM_CONDITIONAL(ENABLE_DEFAULT_MAKE_CHECK, test x$enable_default_make_check = xyes)

AC_ARG_ENABLE(update-mimedb,
   AC_HELP_STRING([--disable-update-mimedb],
                   [disable the update-mime-database after install [default=no]]),,
    enable_update_mimedb=yes)

if test $cross_compiling = yes && test x$enable_update_mimedb = xyes; then
  AC_PATH_PROG(UPDATE_MIME_DATABASE, update-mime-database, no)
  if test x$UPDATE_MIME_DATABASE = xno; then
    enable_update_mimedb=no
    AC_MSG_WARN(Could not find the update-mime-database program in your PATH. Your mime database will not be updated upon install.)
  fi
fi

AM_CONDITIONAL(ENABLE_UPDATE_MIMEDB, test x$enable_update_mimedb = xyes)

AC_PATH_PROG([XMLLINT],[xmllint])

AC_CONFIG_FILES([
Makefile
shared-mime-info.pc
po/Makefile.in
])

AC_OUTPUT
