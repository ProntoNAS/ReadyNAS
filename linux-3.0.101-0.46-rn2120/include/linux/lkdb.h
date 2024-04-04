#ifndef _LKDB_H
#define _LKDB_H

/*
 * Kernel Debugger Architecture Independent Global Headers
 * for legacy KDB
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 2000-2007 Silicon Graphics, Inc.  All Rights Reserved.
 * Copyright (C) 2000 Stephane Eranian <eranian@hpl.hp.com>
 * Copyright (C) 2009 Jason Wessel <jason.wessel@windriver.com>
 */

#if defined(CONFIG_KDB)
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/slab.h>

#ifdef CONFIG_KDB
/* These are really private, but they must be defined before including
 * asm-$(ARCH)/kdb.h, so make them public and put them here.
 */
extern int kdb_getuserarea_size(void *, unsigned long, size_t);
extern int kdb_putuserarea_size(unsigned long, void *, size_t);

// include <asm/kdb.h>

#define LKDB_MAJOR_VERSION	4
#define LKDB_MINOR_VERSION	4
#define LKDB_TEST_VERSION	""

#endif

#define LKDB_POLL_FUNC_MAX	5
extern int kdb_poll_idx;

/*
 * lkdb_initial_cpu is initialized to -1, and is set to the cpu
 * number whenever the kernel debugger is entered.
 */
extern int lkdb_initial_cpu;
extern atomic_t lkdb_event;

#ifdef CONFIG_KDB
extern atomic_t kdb_8250;
#define LKDB_IS_RUNNING() (lkdb_initial_cpu != -1)
#define LKDB_8250() (atomic_read(&kdb_8250) != 0)
#else
#define LKDB_IS_RUNNING() (0)
#define LKDB_8250() (0)
#endif /* CONFIG_KDB */

#ifdef CONFIG_KDB
/*
 * kdb_on
 *
 *     Defines whether kdb is on or not.  Default value
 *     is set by CONFIG_KDB_OFF.  Boot with kdb=on/off/on-nokey
 *     or echo "[012]" > /proc/sys/kernel/kdb to change it.
 */
extern int kdb_on;

#if defined(CONFIG_SERIAL_8250_CONSOLE) || defined(CONFIG_SERIAL_SGI_L1_CONSOLE)
/*
 * kdb_serial.iobase is initialized to zero, and is set to the I/O
 * address of the serial port when the console is setup in
 * serial_console_setup.
 */
extern struct kdb_serial {
       int io_type;
       unsigned long iobase;
       unsigned long ioreg_shift;
} kdb_serial;
#endif
#endif /* CONFIG_KDB */

/* Types and messages used for dynamically added kdb shell commands */

#define LKDB_MAXARGS    16 /* Maximum number of arguments to a function  */

typedef enum {
	LKDB_REPEAT_NONE = 0,	/* Do not repeat this command */
	LKDB_REPEAT_NO_ARGS,	/* Repeat the command without arguments */
	LKDB_REPEAT_WITH_ARGS,	/* Repeat the command including its arguments */
} lkdb_repeat_t;

typedef int (*lkdb_func_t)(int, const char **);

/* KDB return codes from a command or internal kdb function */
#define LKDB_NOTFOUND	(-1)
#define LKDB_ARGCOUNT	(-2)
#define LKDB_BADWIDTH	(-3)
#define LKDB_BADRADIX	(-4)
#define LKDB_NOTENV	(-5)
#define LKDB_NOENVVALUE	(-6)
#define LKDB_NOTIMP	(-7)
#define LKDB_ENVFULL	(-8)
#define LKDB_ENVBUFFULL	(-9)
#define LKDB_TOOMANYBPT	(-10)
#define LKDB_TOOMANYDBREGS (-11)
#define LKDB_DUPBPT	(-12)
#define LKDB_BPTNOTFOUND	(-13)
#define LKDB_BADMODE	(-14)
#define LKDB_BADINT	(-15)
#define LKDB_INVADDRFMT  (-16)
#define LKDB_BADREG      (-17)
#define LKDB_BADCPUNUM   (-18)
#define LKDB_BADLENGTH	(-19)
#define LKDB_NOBP	(-20)
#define LKDB_BADADDR	(-21)

/*
 * lkdb_diemsg
 *
 *	Contains a pointer to the last string supplied to the
 *	kernel 'die' panic function.
 */
extern const char *lkdb_diemsg;

#define LKDB_FLAG_EARLYKDB	(1 << 0) /* set from boot parameter kdb=early */
#define LKDB_FLAG_CATASTROPHIC	(1 << 1) /* A catastrophic event has occurred */
#define LKDB_FLAG_CMD_INTERRUPT	(1 << 2) /* Previous command was interrupted */
#define LKDB_FLAG_NOIPI		(1 << 3) /* Do not send IPIs */
#define LKDB_FLAG_ONLY_DO_DUMP	(1 << 4) /* Only do a dump, used when
					  * kdb is off */
#define LKDB_FLAG_NO_CONSOLE	(1 << 5) /* No console is available,
					  * kdb is disabled */
#define LKDB_FLAG_NO_VT_CONSOLE	(1 << 6) /* No VT console is available, do
					  * not use keyboard */
#define LKDB_FLAG_NO_I8042	(1 << 7) /* No i8042 chip is available, do
					  * not use keyboard */
#define LKDB_FLAG_RECOVERY	(1 << 8) /* kdb is being entered for an error which has been recovered */

extern int lkdb_flags;	/* Global flags, see kdb_state for per cpu state */

extern void lkdb_save_flags(void);
extern void lkdb_restore_flags(void);

#define LKDB_FLAG(flag)		(lkdb_flags & LKDB_FLAG_##flag)
#define LKDB_FLAG_SET(flag)	((void)(lkdb_flags |= LKDB_FLAG_##flag))
#define LKDB_FLAG_CLEAR(flag)	((void)(lkdb_flags &= ~LKDB_FLAG_##flag))

/*
 * External entry point for the kernel debugger.  The pt_regs
 * at the time of entry are supplied along with the reason for
 * entry to the kernel debugger.
 */

typedef enum {
	LKDB_REASON_ENTER = 1,	/* KDB_ENTER() trap/fault - regs valid */
	LKDB_REASON_ENTER_SLAVE,	/* KDB_ENTER_SLAVE() trap/fault - regs valid */
	LKDB_REASON_BREAK,	/* Breakpoint inst. - regs valid */
	LKDB_REASON_DEBUG,	/* Debug Fault - regs valid */
	LKDB_REASON_OOPS,	/* Kernel Oops - regs valid */
	LKDB_REASON_SWITCH,	/* CPU switch - regs valid*/
	LKDB_REASON_KEYBOARD,	/* Keyboard entry - regs valid */
	LKDB_REASON_NMI,		/* Non-maskable interrupt; regs valid */
	LKDB_REASON_RECURSE,	/* Recursive entry to kdb;
				 * regs probably valid */
	LKDB_REASON_SSTEP,	/* Single Step trap. - regs valid */
	LKDB_REASON_CPU_UP,	/* Add one cpu to kdb; regs invalid */
	LKDB_REASON_SILENT,	/* Silent entry/exit to kdb; regs invalid - internal only */
} lkdb_reason_t;

#ifdef CONFIG_KDB
extern int kdb(lkdb_reason_t, int, struct pt_regs *);
#else
static inline int kdb(lkdb_reason_t, int, struct pt_regs *) { return 0; }
#endif

#ifdef CONFIG_KDB
extern int kdb_trap_printk;
extern int vkdb_printf(const char *fmt, va_list args)
	    __attribute__ ((format (printf, 1, 0)));
extern void lkdb_printf(const char *fmt, ...)
	    __attribute__ ((format (printf, 1, 2)));
typedef void (*lkdb_printf_t)(const char *, ...)
             __attribute__ ((format (printf, 1, 2)));
#endif

#include <asm/kdb.h>

#ifdef CONFIG_KDB
extern void legacy_kdb_init(void);

#if defined(CONFIG_SMP)
/*
 * Kernel debugger non-maskable IPI handler.
 */
extern int kdb_ipi(struct pt_regs *, void (*ack_interrupt)(void));
extern void smp_kdb_stop(void);
#else  /* CONFIG_SMP */
#define        smp_kdb_stop()
#endif /* CONFIG_SMP */
#endif /* KDB */

/* Access to kdb specific polling devices */
typedef int (*lget_char_func)(void);
extern lget_char_func kdb_poll_funcs[];
extern int lkdb_get_kbd_char(void);

static inline
int lkdb_process_cpu(const struct task_struct *p)
{
	unsigned int cpu = task_thread_info(p)->cpu;
	if (cpu > num_possible_cpus())
		cpu = 0;
	return cpu;
}

#ifdef CONFIG_KDB
extern const char kdb_serial_str[];

#ifdef CONFIG_KDB_KDUMP
/* Define values for kdb_kdump_state */
extern int lkdb_kdump_state;    /* KDB kdump state */
#define LKDB_KDUMP_RESET		0
#define LKDB_KDUMP_KDUMP		1

void kdba_kdump_prepare(struct pt_regs *);
void machine_crash_shutdown(struct pt_regs *);
void machine_crash_shutdown_begin(void);
void machine_crash_shutdown_end(struct pt_regs *);

#endif /* CONFIG_KDB_KDUMP */
#endif /* KDB */

/* kdb access to register set for stack dumping */
extern struct pt_regs *lkdb_current_regs;
#ifdef CONFIG_KALLSYMS
extern const char *lkdb_walk_kallsyms(loff_t *pos);
#else /* ! CONFIG_KALLSYMS */
static inline const char *lkdb_walk_kallsyms(loff_t *pos)
{
	return NULL;
}
#endif /* ! CONFIG_KALLSYMS */

/* Dynamic kdb shell command registration */
extern int lkdb_register(char *, lkdb_func_t, char *, char *, short);
extern int lkdb_register_repeat(char *, lkdb_func_t, char *, char *,
			       short, lkdb_repeat_t);
extern int lkdb_unregister(char *);
#endif

enum {
	LKDB_NOT_INITIALIZED,
	LKDB_INIT_EARLY,
	LKDB_INIT_FULL,
};

#ifdef CONFIG_KDB
extern int lkdbgetintenv(const char *, int *);
extern int lkdb_set(int, const char **);
#endif

#endif	/* !_LKDB_H */
