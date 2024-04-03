# These are based on Colin's Build System
DEB_PATCH_SUFFIX = .patch
DEB_SRCDIR = .
DEB_PATCHDIRS = debian/patches
DEB_PATCHES = $(foreach dir,$(DEB_PATCHDIRS),$(shell echo $(wildcard $(dir)/*$(DEB_PATCH_SUFFIX)) $(wildcard $(dir)/*.diff)))

pre-build:
	# Emptly rule to please CBS patch rules

# The patch subsystem 
apply-patches: pre-build debian/stamp-patched
debian/stamp-patched: $(DEB_PATCHES)
debian/stamp-patched reverse-patches:
	@echo "patches: $(DEB_PATCHES)"
	@set -e ; reverse=""; patches="$(DEB_PATCHES)"; \
	  if [ "$@" = "reverse-patches" ]; then \
	    if [ ! -e debian/stamp-patched ]; then \
	      echo "Not reversing not applied patches."; \
	      exit 0; \
	    fi; \
	    reverse="-R"; \
	    for patch in $$patches; do reversepatches="$$patch $$reversepatches"; done; \
	    patches="$$reversepatches"; \
	  fi; \
	  for patch in $$patches; do \
          case $$patch in \
            *.gz) cat=zcat ;; \
            *.bz2) cat=bzcat ;; \
            *) cat=cat ;; \
          esac; \
	  level=$$(head $$patch | egrep '^#DPATCHLEVEL=' | cut -f 2 -d '='); \
	  reverse=""; \
	  if [ "$@" = "reverse-patches" ]; then reverse="-R"; fi; \
	  success=""; \
	  if [ -z "$$level" ]; then \
	    echo -n "Trying "; if test -n "$$reverse"; then echo -n "reversed "; fi; echo -n "patch $$patch at level "; \
	    for level in 0 1 2; do \
	      if test -z "$$success"; then \
	        echo -n "$$level..."; \
		if [ "$(DEB_PATCHDIRS_READONLY)" = "yes" ] ; then \
		  logfile="/dev/null" ; \
		else \
		  logfile="$$patch.level-$$level.log" ; \
		fi ; \
	        if $$cat $$patch | patch -d $(DEB_SRCDIR) $$reverse -E --dry-run -p$$level 1>$$logfile 2>&1; then \
	          if $$cat $$patch | patch -d $(DEB_SRCDIR) $$reverse -E --no-backup-if-mismatch -V never -p$$level 1>$$logfile 2>&1; then \
	            success=yes; \
	            touch debian/stamp-patch-$$(basename $$patch); \
	            echo "success."; \
                  fi; \
	        fi; \
	      fi; \
            done; \
	    if test -z "$$success"; then \
	      if test -z "$$reverse"; then \
	        echo "failure."; \
	        exit 1; \
	       else \
	         echo "failure (ignored)."; \
               fi \
	    fi; \
	  else \
	    echo -n "Trying patch $$patch at level $$level..."; \
	        echo -n "$$level..."; \
	    if [ "$(DEB_PATCHDIRS_READONLY)" = "yes" ] ; then \
	      logfile="/dev/null" ; \
	    else \
	      logfile="$$patch.log" ; \
	    fi ; \
	    if $$cat $$patch | patch -d $(DEB_SRCDIR) $$reverse -E --no-backup-if-mismatch -V never -p$$level 1>$$logfile 2>&1; then \
              touch debian/stamp-patch-$$(basename $$patch); \
	      echo "success."; \
	    else \
	      echo "failure:"; \
	      cat $$logfile; \
	      if test -z "$$reverse"; then exit 1; fi; \
            fi; \
	  fi; \
	done
	if [ "$@" = "debian/stamp-patched" ]; then touch debian/stamp-patched; fi

post-patches:: apply-patches

patch-clean: reverse-patches
	rm -f debian/stamp-patch*
	rm -f debian/patches/*.log

