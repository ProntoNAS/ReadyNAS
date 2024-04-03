# $Id: Makefile,v 1.1.1.1 2006-08-28 17:52:12 bzhang Exp $
# Makefile for the linux kernel.
#

extra-y		:= head.o init_task.o vmlinux.lds

EXTRA_AFLAGS	:= -ansi

obj-y    := entry.o wof.o wuf.o etrap.o rtrap.o traps.o irq.o \
	    process.o signal.o ioport.o setup.o sys_sparc.o systbls.o \
	    time.o windows.o ptrace.o unaligned.o muldiv.o semaphore.o lpram.o

obj-$(CONFIG_PCI) += pcic.o
obj-$(CONFIG_MODULES) += module.o sparc_ksyms.o

