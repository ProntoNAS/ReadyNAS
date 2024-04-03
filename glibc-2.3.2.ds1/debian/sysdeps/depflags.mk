depflags = libc=$(libc) glibc=glibc \
	  DEB_HOST_GNU_SYSTEM=$(DEB_HOST_GNU_SYSTEM) \
	  DEB_HOST_GNU_TYPE=$(DEB_HOST_GNU_TYPE) \
	  perl debian/sysdeps/depflags.pl

libc_control_flags = $(shell $(depflags) libc)
libc_dev_control_flags = $(shell $(depflags) libc_dev)

# If there's a -DDepends for libc-dev, add this to it.  If there isn't
# then the control file's depends line (which contains this) will be used.
# This is gross and another reason why depflags.pl must die.
libc_dev_dep = $(libc) (= $(DEB_VERSION)),
libc_dev_control_flags := $(subst Depends=",Depends="$(libc_dev_dep) ,$(libc_dev_control_flags))

$(libc)_control_flags = $(libc_control_flags)
$(libc)-dev_control_flags = $(libc_dev_control_flags)

test_depflags:
	@echo
	@echo "$(libc):"
	@echo '  $(libc_control_flags)'
	@echo
	@echo "$(libc)-dev:"
	@echo '  $(libc_dev_control_flags)'
	@echo
