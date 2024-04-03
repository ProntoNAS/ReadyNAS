## Process this file with automake to produce Makefile.in -*-Makefile-*-.

TESTS = nice help-version
TESTS_ENVIRONMENT = \
  all_programs="$(bin_SCRIPTS) $(bin_PROGRAMS)"

EXTRA_DIST = README Makefile.am.in $(TESTS) .env-warn mk-script Fetish.pm
SUBDIRS = basename date factor seq stty test

TESTS_ENVIRONMENT = \
  top_srcdir=$(top_srcdir) \
  srcdir=$(srcdir) \
  PATH=../src:$$PATH

check-recursive: envvar-check

# Warn when `make check' is run with POSIXLY_CORRECT or CDPATH set.
envvar-check:
	test "$${POSIXLY_CORRECT+set}" = set \
	  && sed s/%%/POSIXLY_CORRECT/ $(srcdir)/.env-warn || :
	test "$${CDPATH+set}" = set \
	  && sed s/%%/CDPATH/ $(srcdir)/.env-warn || :
