obj-$(CONFIG_EFI) 		+= efi.o efi_$(BITS).o efi_stub_$(BITS).o
ccflags-$(CONFIG_XEN)		+= -fshort-wchar
disabled-obj-$(CONFIG_XEN)	:= efi_%$(BITS).o
