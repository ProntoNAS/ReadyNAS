#serial 6

AC_DEFUN(jm_FUNC_MEMCMP,
[AC_REQUIRE([AC_FUNC_MEMCMP])dnl
 if test $ac_cv_func_memcmp_working = no; then
   AC_DEFINE_UNQUOTED(memcmp, rpl_memcmp,
     [Define to rpl_memcmp if the replacement function should be used.])
 fi
])
