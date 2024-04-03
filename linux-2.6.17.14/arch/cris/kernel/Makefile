# $Id: Makefile,v 1.1.1.1 2006-08-28 17:52:07 bzhang Exp $
#
# Makefile for the linux kernel.
#

extra-y	:= vmlinux.lds

obj-y   := process.o traps.o irq.o ptrace.o setup.o \
	   time.o sys_cris.o semaphore.o

obj-$(CONFIG_MODULES)    += crisksyms.o
obj-$(CONFIG_MODULES)	 += module.o
obj-$(CONFIG_SYSTEM_PROFILER)   += profile.o

clean:

