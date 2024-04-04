# ----------------------------------------------------------------------
#    Copyright (c) 1999, 2004-2009 NOVELL (All rights reserved)
#    Copyright (c) 2010-2014 Canonical Ltd.
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of version 2 of the GNU General Public
#    License published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, contact Novell, Inc.
# ----------------------------------------------------------------------
NAME 		= apparmor-utils
all:
COMMONDIR=../../common/

include $(COMMONDIR)/Make.rules

ifdef USE_SYSTEM
    LD_LIBRARY_PATH=
    PYTHONPATH=
else
    # PYTHON_DIST_BUILD_PATH based on libapparmor/swig/python/test/Makefile.am
    PYTHON_DIST_BUILD_PATH = ../../libraries/libapparmor/swig/python/build/$$($(PYTHON) -c "import distutils.util; import platform; print(\"lib.%s-%s\" %(distutils.util.get_platform(), platform.python_version()[:3]))")
    LD_LIBRARY_PATH=../../libraries/libapparmor/src/.libs/
    PYTHONPATH=..:$(PYTHON_DIST_BUILD_PATH)
endif

.PHONY: __libapparmor
__libapparmor:
ifndef USE_SYSTEM
	@if [ ! -f $(LD_LIBRARY_PATH)libapparmor.so ]; then \
		echo "error: $(LD_LIBRARY_PATH)libapparmor.so is missing. Pick one of these possible solutions:" 1>&2; \
		echo "  1) Build against the in-tree libapparmor by building it first and then trying again. See the top-level README for help." 1>&2; \
		echo "  2) Build against the system libapparmor by adding USE_SYSTEM=1 to your make command." 1>&2; \
		return 1; \
	fi
endif

COVERAGE_OMIT=test-*.py,common_test.py
ifneq ($(COVERAGE_OUT), )
HTML_COVR_ARGS=-d $(COVERAGE_OUT)
endif

# use   make COVERAGE_IGNORE_FAILURES=true coverage   to build coverage data even if some tests fail
ifeq ($(COVERAGE_IGNORE_FAILURES), true)
  COVERAGE_IGNORE_FAILURES_CMD=true
else
  COVERAGE_IGNORE_FAILURES_CMD=set -e
endif

.PHONY: clean check coverage coverage-report coverage-html
ifndef VERBOSE
.SILENT: clean check .coverage coverage coverage-report coverage-html
endif

clean:
	rm -rf __pycache__/ .coverage htmlcov

check: __libapparmor
	export PYTHONPATH=$(PYTHONPATH) ; export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ; $(foreach test, $(wildcard test-*.py), echo ; echo === $(test) === ; $(call pyalldo, $(test)))

.coverage: $(wildcard ../aa-* ../apparmor/*.py test-*.py) __libapparmor
	export PYTHONPATH=$(PYTHONPATH) ; export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH); $(COVERAGE_IGNORE_FAILURES_CMD) ; $(foreach test, $(wildcard test-*.py), echo ; echo === $(test) === ; $(PYTHON) -m coverage run --branch -p $(test); )
	$(PYTHON) -m coverage combine

coverage: .coverage

coverage-report: .coverage
	$(PYTHON) -m coverage report --omit="$(COVERAGE_OMIT)"

coverage-html: .coverage
	$(PYTHON) -m coverage html --omit="$(COVERAGE_OMIT)" $(HTML_COVR_ARGS)

