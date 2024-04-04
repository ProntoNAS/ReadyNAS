#! /usr/bin/make --no-print-directory -f

## Default target
RUBY_VERSIONS := $(shell dh_ruby --print-supported)
all: $(RUBY_VERSIONS)

## Targets share the same output files, so must be run serially
.NOTPARALLEL:
.PHONY: all $(RUBY_VERSIONS)

## Helper variables
RUBY_PLATFORM = $(shell $@ -e 'print RUBY_PLATFORM')
RUBY_HDRDIR = $(shell $@ -e 'print RbConfig::CONFIG["rubyhdrdir"]')
RUBY_ARCH_HDRDIR = $(shell $@ -e 'print RbConfig::CONFIG["rubyarchhdrdir"]')
RUBY_VENDOR_ARCHLIB = $(shell $@ -e 'print RbConfig::CONFIG["vendorarchdir"]')

## SELinux does not have a very nice build process
extra_ruby_args  = RUBYLIBVER=$*
extra_ruby_args += RUBYPLATFORM=$(RUBY_PLATFORM)
extra_ruby_args += RUBYINC="-I$(RUBY_HDRDIR) -I$(RUBY_ARCH_HDRDIR)"
extra_ruby_args += RUBYINSTALL=$(DESTDIR)$(RUBY_VENDOR_ARCHLIB)

## How to build and install each individually-versioned copy
$(RUBY_VERSIONS): ruby%:
	+$(MAKE) $(extra_ruby_args) clean-rubywrap
	+$(MAKE) $(extra_ruby_args) install-rubywrap
