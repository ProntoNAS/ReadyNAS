#serial 15

dnl This macro is intended to be used solely in this file.
dnl These are the prerequisite macros for GNU's strftime.c replacement.
AC_DEFUN(_jm_STRFTIME_PREREQS,
[
 dnl strftime.c uses localtime_r and the underyling system strftime
 dnl if they exist.
 AC_CHECK_FUNCS(localtime_r strftime)

 AC_CHECK_HEADERS(limits.h)
 AC_CHECK_FUNCS(bcopy tzset mempcpy memcpy memset)

 # This defines (or not) HAVE_TZNAME and HAVE_TM_ZONE.
 AC_STRUCT_TIMEZONE

 AC_CHECK_FUNCS(mblen mbrlen)

 AC_CHECK_MEMBER([struct tm.tm_gmtoff],
                 [AC_DEFINE(HAVE_TM_GMTOFF, 1,
                            [Define if struct tm has the tm_gmtoff member.])],
                 ,
                 [#include <time.h>])
])

dnl Determine if the strftime function has all the features of the GNU one.
dnl
dnl From Jim Meyering.
dnl
AC_DEFUN(jm_FUNC_GNU_STRFTIME,
[AC_REQUIRE([AC_HEADER_TIME])dnl

 _jm_STRFTIME_PREREQS

 AC_REQUIRE([AC_C_CONST])dnl
 AC_REQUIRE([AC_HEADER_STDC])dnl
 AC_CHECK_HEADERS(sys/time.h)
 AC_CACHE_CHECK([for working GNU strftime], jm_cv_func_working_gnu_strftime,
  [AC_TRY_RUN(
[ /* Ulrich Drepper provided parts of the test program.  */
#if STDC_HEADERS
# include <stdlib.h>
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

static int
compare (const char *fmt, const struct tm *tm, const char *expected)
{
  char buf[99];
  strftime (buf, 99, fmt, tm);
  if (strcmp (buf, expected))
    {
#ifdef SHOW_FAILURES
      printf ("fmt: \"%s\", expected \"%s\", got \"%s\"\n",
	      fmt, expected, buf);
#endif
      return 1;
    }
  return 0;
}

int
main ()
{
  int n_fail = 0;
  struct tm *tm;
  time_t t = 738367; /* Fri Jan  9 13:06:07 1970 */
  tm = gmtime (&t);

  /* Undefine this in case the configure-time putenv test has defined it
     to something else.  The use we make of this function here doesn't
     require the added functionality of the replacement one.  */
#undef putenv

  /* This is necessary to make strftime give consistent zone strings and
     e.g., seconds since the epoch (%s).  */
  putenv ("TZ=GMT0");

#undef CMP
#define CMP(Fmt, Expected) n_fail += compare ((Fmt), tm, (Expected))

  CMP ("%-m", "1");		/* GNU */
  CMP ("%A", "Friday");
  CMP ("%^A", "FRIDAY");	/* The ^ is a GNU extension.  */
  CMP ("%B", "January");
  CMP ("%^B", "JANUARY");
  CMP ("%C", "19");		/* POSIX.2 */
  CMP ("%D", "01/09/70");	/* POSIX.2 */
  CMP ("%F", "1970-01-09");
  CMP ("%G", "1970");		/* GNU */
  CMP ("%H", "13");
  CMP ("%I", "01");
  CMP ("%M", "06");
  CMP ("%M", "06");
  CMP ("%R", "13:06");		/* POSIX.2 */
  CMP ("%S", "07");
  CMP ("%T", "13:06:07");	/* POSIX.2 */
  CMP ("%U", "01");
  CMP ("%V", "02");
  CMP ("%W", "01");
  CMP ("%X", "13:06:07");
  CMP ("%Y", "1970");
  CMP ("%Z", "GMT");
  CMP ("%_m", " 1");		/* GNU */
  CMP ("%a", "Fri");
  CMP ("%^a", "FRI");
  CMP ("%b", "Jan");
  CMP ("%^b", "JAN");
  CMP ("%c", "Fri Jan  9 13:06:07 1970");
  CMP ("%^c", "FRI JAN  9 13:06:07 1970");
  CMP ("%d", "09");
  CMP ("%e", " 9");		/* POSIX.2 */
  CMP ("%g", "70");		/* GNU */
  CMP ("%h", "Jan");		/* POSIX.2 */
  CMP ("%^h", "JAN");
  CMP ("%j", "009");
  CMP ("%k", "13");		/* GNU */
  CMP ("%l", " 1");		/* GNU */
  CMP ("%m", "01");
  CMP ("%n", "\n");		/* POSIX.2 */
  CMP ("%p", "PM");
  CMP ("%r", "01:06:07 PM");	/* POSIX.2 */
  CMP ("%s", "738367");		/* GNU */
  CMP ("%t", "\t");		/* POSIX.2 */
  CMP ("%u", "5");		/* POSIX.2 */
  CMP ("%w", "5");
  CMP ("%x", "01/09/70");
  CMP ("%y", "70");
  CMP ("%z", "+0000");		/* GNU */

  exit (n_fail ? 1 : 0);
}],
	     jm_cv_func_working_gnu_strftime=yes,
             jm_cv_func_working_gnu_strftime=no,
	     dnl When crosscompiling, assume strftime is missing or broken.
	     jm_cv_func_working_gnu_strftime=no)
  ])
  if test $jm_cv_func_working_gnu_strftime = no; then
    AC_LIBOBJ(strftime)
    AC_DEFINE_UNQUOTED(strftime, gnu_strftime,
      [Define to gnu_strftime if the replacement function should be used.])
  fi
])

AC_DEFUN(jm_FUNC_STRFTIME,
[
  _jm_STRFTIME_PREREQS
  AC_REPLACE_FUNCS(strftime)
])
