/*
 * Kernel Debugger Architecture Independent Main Code
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1999-2004 Silicon Graphics, Inc.  All Rights Reserved.
 * Copyright (C) 2000 Stephane Eranian <eranian@hpl.hp.com>
 * Xscale (R) modifications copyright (C) 2003 Intel Corporation.
 */

/*
 * Updated for Xscale (R) architecture support
 * Eddie Dong <eddie.dong@intel.com> 8 Jan 03
 */

#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/sched.h>
#include <linux/sysrq.h>
#include <linux/smp.h>
#include <linux/utsname.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/notifier.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/nmi.h>
#include <linux/ptrace.h>
#include <linux/sysctl.h>
#if defined(CONFIG_LKCD_DUMP) || defined(CONFIG_LKCD_DUMP_MODULE)
#include <linux/dump.h>
#endif
#include <linux/cpu.h>
#include <linux/kdebug.h>
#ifdef CONFIG_KDB_KDUMP
#include <linux/kexec.h>
#endif
#include <asm/uv/uv.h>


#include <acpi/acpi_bus.h>

#include <asm/system.h>
#include <asm/kdebug.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
char kdb_debug_info_filename[256] = {""};
EXPORT_SYMBOL(kdb_debug_info_filename);
#define GREP_LEN 256
char lkdb_grep_string[GREP_LEN];
int lkdb_grepping_flag;
EXPORT_SYMBOL(lkdb_grepping_flag);
int lkdb_grep_leading;
int lkdb_grep_trailing;

/*
 * Kernel debugger state flags
 */
int lkdb_flags;
atomic_t lkdb_event;
atomic_t kdb_8250;

/*
 * kdb_lock protects updates to lkdb_initial_cpu.  Used to
 * single thread processors through the kernel debugger.
 */
static DEFINE_SPINLOCK(kdb_lock);
int lkdb_initial_cpu = -1;		/* cpu number that owns kdb */
int lkdb_seqno = 2;				/* how many times kdb has been entered */

volatile int lkdb_nextline = 1;
static volatile int kdb_new_cpu;		/* Which cpu to switch to */

volatile int lkdb_state[NR_CPUS];		/* Per cpu state */

struct task_struct *lkdb_current_task;
EXPORT_SYMBOL(lkdb_current_task);
struct pt_regs *lkdb_current_regs;

#ifdef	CONFIG_KDB_OFF
int kdb_on = 0;				/* Default is off */
#else
int kdb_on = 1;				/* Default is on */
#endif	/* CONFIG_KDB_OFF */

const char *lkdb_diemsg;
static int kdb_go_count;
#ifdef CONFIG_KDB_CONTINUE_CATASTROPHIC
static unsigned int kdb_continue_catastrophic = CONFIG_KDB_CONTINUE_CATASTROPHIC;
#else
static unsigned int kdb_continue_catastrophic = 0;
#endif

#ifdef kdba_setjmp
	/*
	 * Must have a setjmp buffer per CPU.  Switching cpus will
	 * cause the jump buffer to be setup for the new cpu, and
	 * subsequent switches (and pager aborts) will use the
	 * appropriate per-processor values.
	 */
kdb_jmp_buf *kdbjmpbuf;
#endif	/* kdba_setjmp */

	/*
	 * kdb_commands describes the available commands.
	 */
static lkdbtab_t *lkdb_commands;
static int kdb_max_commands;

typedef struct _kdbmsg {
	int	km_diag;	/* kdb diagnostic */
	char	*km_msg;	/* Corresponding message text */
} kdbmsg_t;

#define KDBMSG(msgnum, text) \
	{ LKDB_##msgnum, text }

static kdbmsg_t kdbmsgs[] = {
	KDBMSG(NOTFOUND,"Command Not Found"),
	KDBMSG(ARGCOUNT, "Improper argument count, see usage."),
	KDBMSG(BADWIDTH, "Illegal value for BYTESPERWORD use 1, 2, 4 or 8, 8 is only allowed on 64 bit systems"),
	KDBMSG(BADRADIX, "Illegal value for RADIX use 8, 10 or 16"),
	KDBMSG(NOTENV, "Cannot find environment variable"),
	KDBMSG(NOENVVALUE, "Environment variable should have value"),
	KDBMSG(NOTIMP, "Command not implemented"),
	KDBMSG(ENVFULL, "Environment full"),
	KDBMSG(ENVBUFFULL, "Environment buffer full"),
	KDBMSG(TOOMANYBPT, "Too many breakpoints defined"),
#ifdef  CONFIG_CPU_XSCALE
	KDBMSG(TOOMANYDBREGS, "More breakpoints than ibcr registers defined"),
#else
	KDBMSG(TOOMANYDBREGS, "More breakpoints than db registers defined"),
#endif
	KDBMSG(DUPBPT, "Duplicate breakpoint address"),
	KDBMSG(BPTNOTFOUND, "Breakpoint not found"),
	KDBMSG(BADMODE, "Invalid IDMODE"),
	KDBMSG(BADINT, "Illegal numeric value"),
	KDBMSG(INVADDRFMT, "Invalid symbolic address format"),
	KDBMSG(BADREG, "Invalid register name"),
	KDBMSG(BADCPUNUM, "Invalid cpu number"),
	KDBMSG(BADLENGTH, "Invalid length field"),
	KDBMSG(NOBP, "No Breakpoint exists"),
	KDBMSG(BADADDR, "Invalid address"),
};
#undef KDBMSG

static const int __nkdb_err = sizeof(kdbmsgs) / sizeof(kdbmsg_t);


/*
 * Initial environment.   This is all kept static and local to
 * this file.   We don't want to rely on the memory allocation
 * mechanisms in the kernel, so we use a very limited allocate-only
 * heap for new and altered environment variables.  The entire
 * environment is limited to a fixed number of entries (add more
 * to __env[] if required) and a fixed amount of heap (add more to
 * KDB_ENVBUFSIZE if required).
 */

static char *__env[] = {
#if defined(CONFIG_SMP)
 "PROMPT=[%d]kdb> ",
 "MOREPROMPT=[%d]more> ",
#else
 "PROMPT=kdb> ",
 "MOREPROMPT=more> ",
#endif
 "RADIX=16",
 "LINES=24",
 "COLUMNS=80",
 "MDCOUNT=8",			/* lines of md output */
 "BTARGS=9",			/* 9 possible args in bt */
 LKDB_PLATFORM_ENV,
 "DTABCOUNT=30",
 "NOSECT=1",
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
 (char *)0,
};

static const int __nenv = (sizeof(__env) / sizeof(char *));

/* external commands: */
int kdb_debuginfo_print(int argc, const char **argv);
int kdb_pxhelp(int argc, const char **argv);
int kdb_walkhelp(int argc, const char **argv);
int kdb_walk(int argc, const char **argv);

/*
 * kdb_serial_str is the sequence that the user must enter on a serial
 * console to invoke kdb.  It can be a single character such as "\001"
 * (control-A) or multiple characters such as "\eKDB".  NOTE: All except the
 * last character are passed through to the application reading from the serial
 * console.
 *
 * I tried to make the sequence a CONFIG_ option but most of CML1 cannot cope
 * with '\' in strings.  CML2 would have been able to do it but we lost CML2.
 * KAO.
 */
const char kdb_serial_str[] = "\eKDB";
EXPORT_SYMBOL(kdb_serial_str);

struct task_struct *
lkdb_curr_task(int cpu)
{
	struct task_struct *p = curr_task(cpu);
#ifdef	_TIF_MCA_INIT
	struct lkdb_running_process *krp = lkdb_running_process + cpu;
	if ((task_thread_info(p)->flags & _TIF_MCA_INIT) && krp->p)
		p = krp->p;
#endif
	return p;
}

/*
 * lkdbgetenv
 *
 *	This function will return the character string value of
 *	an environment variable.
 *
 * Parameters:
 *	match	A character string representing an environment variable.
 * Outputs:
 *	None.
 * Returns:
 *	NULL	No environment variable matches 'match'
 *	char*	Pointer to string value of environment variable.
 * Locking:
 *	No locking considerations required.
 * Remarks:
 */
char *
lkdbgetenv(const char *match)
{
	char **ep = __env;
	int matchlen = strlen(match);
	int i;

	for(i=0; i<__nenv; i++) {
		char *e = *ep++;

		if (!e) continue;

		if ((strncmp(match, e, matchlen) == 0)
		 && ((e[matchlen] == '\0')
		   ||(e[matchlen] == '='))) {
			char *cp = strchr(e, '=');
			return (cp ? ++cp :"");
		}
	}
	return NULL;
}

/*
 * kdballocenv
 *
 *	This function is used to allocate bytes for environment entries.
 *
 * Parameters:
 *	match	A character string representing a numeric value
 * Outputs:
 *	*value  the unsigned long represntation of the env variable 'match'
 * Returns:
 *	Zero on success, a kdb diagnostic on failure.
 * Locking:
 *	No locking considerations required.  Must be called with all
 *	processors halted.
 * Remarks:
 *	We use a static environment buffer (envbuffer) to hold the values
 *	of dynamically generated environment variables (see lkdb_set).  Buffer
 *	space once allocated is never free'd, so over time, the amount of space
 *	(currently 512 bytes) will be exhausted if env variables are changed
 *	frequently.
 */
static char *
kdballocenv(size_t bytes)
{
#define	LKDB_ENVBUFSIZE	512
	static char envbuffer[LKDB_ENVBUFSIZE];
	static int envbufsize;
	char *ep = NULL;

	if ((LKDB_ENVBUFSIZE - envbufsize) >= bytes) {
		ep = &envbuffer[envbufsize];
		envbufsize += bytes;
	}
	return ep;
}

/*
 * kdbgetulenv
 *
 *	This function will return the value of an unsigned long-valued
 *	environment variable.
 *
 * Parameters:
 *	match	A character string representing a numeric value
 * Outputs:
 *	*value  the unsigned long represntation of the env variable 'match'
 * Returns:
 *	Zero on success, a kdb diagnostic on failure.
 * Locking:
 *	No locking considerations required.
 * Remarks:
 */

static int
kdbgetulenv(const char *match, unsigned long *value)
{
	char *ep;

	ep = lkdbgetenv(match);
	if (!ep) return LKDB_NOTENV;
	if (strlen(ep) == 0) return LKDB_NOENVVALUE;

	*value = simple_strtoul(ep, NULL, 0);

	return 0;
}

/*
 * lkdbgetintenv
 *
 *	This function will return the value of an integer-valued
 *	environment variable.
 *
 * Parameters:
 *	match	A character string representing an integer-valued env variable
 * Outputs:
 *	*value  the integer representation of the environment variable 'match'
 * Returns:
 *	Zero on success, a kdb diagnostic on failure.
 * Locking:
 *	No locking considerations required.
 * Remarks:
 */

int
lkdbgetintenv(const char *match, int *value) {
	unsigned long val;
	int diag;

	diag = kdbgetulenv(match, &val);
	if (!diag) {
		*value = (int) val;
	}
	return diag;
}

/*
 * lkdbgetularg
 *
 *	This function will convert a numeric string
 *	into an unsigned long value.
 *
 * Parameters:
 *	arg	A character string representing a numeric value
 * Outputs:
 *	*value  the unsigned long represntation of arg.
 * Returns:
 *	Zero on success, a kdb diagnostic on failure.
 * Locking:
 *	No locking considerations required.
 * Remarks:
 */

int
lkdbgetularg(const char *arg, unsigned long *value)
{
	char *endp;
	unsigned long val;

	val = simple_strtoul(arg, &endp, 0);

	if (endp == arg) {
		/*
		 * Try base 16, for us folks too lazy to type the
		 * leading 0x...
		 */
		val = simple_strtoul(arg, &endp, 16);
		if (endp == arg)
			return LKDB_BADINT;
	}

	*value = val;

	return 0;
}

/*
 * lkdb_set
 *
 *	This function implements the 'set' command.  Alter an existing
 *	environment variable or create a new one.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

int
lkdb_set(int argc, const char **argv)
{
	int i;
	char *ep;
	size_t varlen, vallen;

	/*
	 * we can be invoked two ways:
	 *   set var=value    argv[1]="var", argv[2]="value"
	 *   set var = value  argv[1]="var", argv[2]="=", argv[3]="value"
	 * - if the latter, shift 'em down.
	 */
	if (argc == 3) {
		argv[2] = argv[3];
		argc--;
	}

	if (argc != 2)
		return LKDB_ARGCOUNT;

	/*
	 * Check for internal variables
	 */
	if (strcmp(argv[1], "KDBDEBUG") == 0) {
		unsigned int debugflags;
		char *cp;

		debugflags = simple_strtoul(argv[2], &cp, 0);
		if (cp == argv[2] || debugflags & ~KDB_DEBUG_FLAG_MASK) {
			lkdb_printf("kdb: illegal debug flags '%s'\n",
				    argv[2]);
			return 0;
		}
		lkdb_flags = (lkdb_flags & ~(KDB_DEBUG_FLAG_MASK << KDB_DEBUG_FLAG_SHIFT))
			  | (debugflags << KDB_DEBUG_FLAG_SHIFT);

		return 0;
	}

	/*
	 * Tokenizer squashed the '=' sign.  argv[1] is variable
	 * name, argv[2] = value.
	 */
	varlen = strlen(argv[1]);
	vallen = strlen(argv[2]);
	ep = kdballocenv(varlen + vallen + 2);
	if (ep == (char *)0)
		return LKDB_ENVBUFFULL;

	sprintf(ep, "%s=%s", argv[1], argv[2]);

	ep[varlen+vallen+1]='\0';

	for(i=0; i<__nenv; i++) {
		if (__env[i]
		 && ((strncmp(__env[i], argv[1], varlen)==0)
		   && ((__env[i][varlen] == '\0')
		    || (__env[i][varlen] == '=')))) {
			__env[i] = ep;
			return 0;
		}
	}

	/*
	 * Wasn't existing variable.  Fit into slot.
	 */
	for(i=0; i<__nenv-1; i++) {
		if (__env[i] == (char *)0) {
			__env[i] = ep;
			return 0;
		}
	}

	return LKDB_ENVFULL;
}

static int
kdb_check_regs(void)
{
	if (!lkdb_current_regs) {
		lkdb_printf("No current kdb registers."
		           "  You may need to select another task\n");
		return LKDB_BADREG;
	}
	return 0;
}

/*
 * lkdbgetaddrarg
 *
 *	This function is responsible for parsing an
 *	address-expression and returning the value of
 *	the expression, symbol name, and offset to the caller.
 *
 *	The argument may consist of a numeric value (decimal or
 *	hexidecimal), a symbol name, a register name (preceeded
 *	by the percent sign), an environment variable with a numeric
 *	value (preceeded by a dollar sign) or a simple arithmetic
 *	expression consisting of a symbol name, +/-, and a numeric
 *	constant value (offset).
 *
 * Parameters:
 *	argc	- count of arguments in argv
 *	argv	- argument vector
 *	*nextarg - index to next unparsed argument in argv[]
 *	regs	- Register state at time of KDB entry
 * Outputs:
 *	*value	- receives the value of the address-expression
 *	*offset - receives the offset specified, if any
 *	*name   - receives the symbol name, if any
 *	*nextarg - index to next unparsed argument in argv[]
 *
 * Returns:
 *	zero is returned on success, a kdb diagnostic code is
 *      returned on error.
 *
 * Locking:
 *	No locking requirements.
 *
 * Remarks:
 *
 */

int
lkdbgetaddrarg(int argc, const char **argv, int *nextarg,
	      kdb_machreg_t *value,  long *offset,
	      char **name)
{
	kdb_machreg_t addr;
	unsigned long off = 0;
	int positive;
	int diag;
	int found = 0;
	char *symname;
	char symbol = '\0';
	char *cp;
	lkdb_symtab_t symtab;

	/*
	 * Process arguments which follow the following syntax:
	 *
	 *  symbol | numeric-address [+/- numeric-offset]
	 *  %register
	 *  $environment-variable
	 */

	if (*nextarg > argc) {
		return LKDB_ARGCOUNT;
	}

	symname = (char *)argv[*nextarg];

	/*
	 * If there is no whitespace between the symbol
	 * or address and the '+' or '-' symbols, we
	 * remember the character and replace it with a
	 * null so the symbol/value can be properly parsed
	 */
	if ((cp = strpbrk(symname, "+-")) != NULL) {
		symbol = *cp;
		*cp++ = '\0';
	}

	if (symname[0] == '$') {
		diag = kdbgetulenv(&symname[1], &addr);
		if (diag)
			return diag;
	} else if (symname[0] == '%') {
		if ((diag = kdb_check_regs()))
			return diag;
		diag = kdba_getregcontents(&symname[1], lkdb_current_regs, &addr);
		if (diag)
			return diag;
	} else {
		found = lkdbgetsymval(symname, &symtab);
		if (found) {
			addr = symtab.sym_start;
		} else {
			diag = lkdbgetularg(argv[*nextarg], &addr);
			if (diag)
				return diag;
		}
	}

	if (!found)
		found = lkdbnearsym(addr, &symtab);

	(*nextarg)++;

	if (name)
		*name = symname;
	if (value)
		*value = addr;
	if (offset && name && *name)
		*offset = addr - symtab.sym_start;

	if ((*nextarg > argc)
	 && (symbol == '\0'))
		return 0;

	/*
	 * check for +/- and offset
	 */

	if (symbol == '\0') {
		if ((argv[*nextarg][0] != '+')
		 && (argv[*nextarg][0] != '-')) {
			/*
			 * Not our argument.  Return.
			 */
			return 0;
		} else {
			positive = (argv[*nextarg][0] == '+');
			(*nextarg)++;
		}
	} else
		positive = (symbol == '+');

	/*
	 * Now there must be an offset!
	 */
	if ((*nextarg > argc)
	 && (symbol == '\0')) {
		return LKDB_INVADDRFMT;
	}

	if (!symbol) {
		cp = (char *)argv[*nextarg];
		(*nextarg)++;
	}

	diag = lkdbgetularg(cp, &off);
	if (diag)
		return diag;

	if (!positive)
		off = -off;

	if (offset)
		*offset += off;

	if (value)
		*value += off;

	return 0;
}

static void
kdb_cmderror(int diag)
{
	int i;

	if (diag >= 0) {
		lkdb_printf("no error detected (diagnostic is %d)\n", diag);
		return;
	}

	for(i=0; i<__nkdb_err; i++) {
		if (kdbmsgs[i].km_diag == diag) {
			lkdb_printf("diag: %d: %s\n", diag, kdbmsgs[i].km_msg);
			return;
		}
	}

	lkdb_printf("Unknown diag %d\n", -diag);
}

/*
 * kdb_defcmd, kdb_defcmd2
 *
 *	This function implements the 'defcmd' command which defines one
 *	command as a set of other commands, terminated by endefcmd.
 *	kdb_defcmd processes the initial 'defcmd' command, kdb_defcmd2
 *	is invoked from lkdb_parse for the following commands until
 *	'endefcmd'.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

struct defcmd_set {
	int count;
	int usable;
	char *name;
	char *usage;
	char *help;
	char **command;
};
static struct defcmd_set *defcmd_set;
static int defcmd_set_count;
static int defcmd_in_progress;

/* Forward references */
static int kdb_exec_defcmd(int argc, const char **argv);

static int
kdb_defcmd2(const char *cmdstr, const char *argv0)
{
	struct defcmd_set *s = defcmd_set + defcmd_set_count - 1;
	char **save_command = s->command;
	if (strcmp(argv0, "endefcmd") == 0) {
		defcmd_in_progress = 0;
		if (!s->count)
			s->usable = 0;
		if (s->usable)
			lkdb_register(s->name, kdb_exec_defcmd, s->usage, s->help, 0);
		return 0;
	}
	if (!s->usable)
		return LKDB_NOTIMP;
	s->command = kmalloc((s->count + 1) * sizeof(*(s->command)), GFP_KDB);
	if (!s->command) {
		lkdb_printf("Could not allocate new kdb_defcmd table for %s\n", cmdstr);
		s->usable = 0;
		return LKDB_NOTIMP;
	}
	memcpy(s->command, save_command, s->count * sizeof(*(s->command)));
	s->command[s->count++] = lkdb_strdup(cmdstr, GFP_KDB);
	kfree(save_command);
	return 0;
}

static int
kdb_defcmd(int argc, const char **argv)
{
	struct defcmd_set *save_defcmd_set = defcmd_set, *s;
	if (defcmd_in_progress) {
		lkdb_printf("kdb: nested defcmd detected, assuming missing endefcmd\n");
		kdb_defcmd2("endefcmd", "endefcmd");
	}
	if (argc == 0) {
		int i;
		for (s = defcmd_set; s < defcmd_set + defcmd_set_count; ++s) {
			lkdb_printf("defcmd %s \"%s\" \"%s\"\n", s->name, s->usage, s->help);
			for (i = 0; i < s->count; ++i)
				lkdb_printf("%s", s->command[i]);
			lkdb_printf("endefcmd\n");
		}
		return 0;
	}
	if (argc != 3)
		return LKDB_ARGCOUNT;
	defcmd_set = kmalloc((defcmd_set_count + 1) * sizeof(*defcmd_set), GFP_KDB);
	if (!defcmd_set) {
		lkdb_printf("Could not allocate new defcmd_set entry for %s\n", argv[1]);
		defcmd_set = save_defcmd_set;
		return LKDB_NOTIMP;
	}
	memcpy(defcmd_set, save_defcmd_set, defcmd_set_count * sizeof(*defcmd_set));
	kfree(save_defcmd_set);
	s = defcmd_set + defcmd_set_count;
	memset(s, 0, sizeof(*s));
	s->usable = 1;
	s->name = lkdb_strdup(argv[1], GFP_KDB);
	s->usage = lkdb_strdup(argv[2], GFP_KDB);
	s->help = lkdb_strdup(argv[3], GFP_KDB);
	if (s->usage[0] == '"') {
		strcpy(s->usage, s->usage+1);
		s->usage[strlen(s->usage)-1] = '\0';
	}
	if (s->help[0] == '"') {
		strcpy(s->help, s->help+1);
		s->help[strlen(s->help)-1] = '\0';
	}
	++defcmd_set_count;
	defcmd_in_progress = 1;
	return 0;
}

/*
 * kdb_exec_defcmd
 *
 *	Execute the set of commands associated with this defcmd name.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_exec_defcmd(int argc, const char **argv)
{
	int i, ret;
	struct defcmd_set *s;
	if (argc != 0)
		return LKDB_ARGCOUNT;
	for (s = defcmd_set, i = 0; i < defcmd_set_count; ++i, ++s) {
		if (strcmp(s->name, argv[0]) == 0)
			break;
	}
	if (i == defcmd_set_count) {
		lkdb_printf("kdb_exec_defcmd: could not find commands for %s\n", argv[0]);
		return LKDB_NOTIMP;
	}
	for (i = 0; i < s->count; ++i) {
		/* Recursive use of lkdb_parse, do not use argv after this point */
		argv = NULL;
		lkdb_printf("[%s]kdb> %s\n", s->name, s->command[i]);
		if ((ret = lkdb_parse(s->command[i])))
			return ret;
	}
	return 0;
}

/* Command history */
#define KDB_CMD_HISTORY_COUNT	32
#define CMD_BUFLEN		200	/* lkdb_printf: max printline size == 256 */
static unsigned int cmd_head=0, cmd_tail=0;
static unsigned int cmdptr;
static char cmd_hist[KDB_CMD_HISTORY_COUNT][CMD_BUFLEN];
static char cmd_cur[CMD_BUFLEN];

/*
 * The "str" argument may point to something like  | grep xyz
 *
 */
static void
parse_grep(const char *str)
{
	int	len;
	char	*cp = (char *)str, *cp2;

	/* sanity check: we should have been called with the \ first */
	if (*cp != '|')
		return;
	cp++;
	while (isspace(*cp)) cp++;
	if (strncmp(cp,"grep ",5)) {
		lkdb_printf ("invalid 'pipe', see grephelp\n");
		return;
	}
	cp += 5;
	while (isspace(*cp)) cp++;
	cp2 = strchr(cp, '\n');
	if (cp2)
		*cp2 = '\0'; /* remove the trailing newline */
	len = strlen(cp);
	if (len == 0) {
		lkdb_printf ("invalid 'pipe', see grephelp\n");
		return;
	}
	/* now cp points to a nonzero length search string */
	if (*cp == '"') {
		/* allow it be "x y z" by removing the "'s - there must
		   be two of them */
		cp++;
		cp2 = strchr(cp, '"');
		if (!cp2) {
			lkdb_printf ("invalid quoted string, see grephelp\n");
			return;
		}
		*cp2 = '\0'; /* end the string where the 2nd " was */
	}
	lkdb_grep_leading = 0;
	if (*cp == '^') {
		lkdb_grep_leading = 1;
		cp++;
	}
	len = strlen(cp);
	lkdb_grep_trailing = 0;
	if (*(cp+len-1) == '$') {
		lkdb_grep_trailing = 1;
		*(cp+len-1) = '\0';
	}
	len = strlen(cp);
	if (!len) return;
	if (len >= GREP_LEN) {
		lkdb_printf ("search string too long\n");
		return;
	}
	strcpy(lkdb_grep_string, cp);
	lkdb_grepping_flag++;
	return;
}

/*
 * lkdb_parse
 *
 *	Parse the command line, search the command table for a
 *	matching command and invoke the command function.
 *	This function may be called recursively, if it is, the second call
 *	will overwrite argv and cbuf.  It is the caller's responsibility to
 *	save their argv if they recursively call lkdb_parse().
 *
 * Parameters:
 *      cmdstr	The input command line to be parsed.
 *	regs	The registers at the time kdb was entered.
 * Outputs:
 *	None.
 * Returns:
 *	Zero for success, a kdb diagnostic if failure.
 * Locking:
 * 	None.
 * Remarks:
 *	Limited to 20 tokens.
 *
 *	Real rudimentary tokenization. Basically only whitespace
 *	is considered a token delimeter (but special consideration
 *	is taken of the '=' sign as used by the 'set' command).
 *
 *	The algorithm used to tokenize the input string relies on
 *	there being at least one whitespace (or otherwise useless)
 *	character between tokens as the character immediately following
 *	the token is altered in-place to a null-byte to terminate the
 *	token string.
 */

#define MAXARGC	20

int
lkdb_parse(const char *cmdstr)
{
	static char *argv[MAXARGC];
	static int argc = 0;
	static char cbuf[CMD_BUFLEN+2];
	char *cp;
	char *cpp, quoted;
	lkdbtab_t *tp;
	int i, escaped, ignore_errors = 0, check_grep;

	/*
	 * First tokenize the command string.
	 */
	cp = (char *)cmdstr;
	lkdb_grepping_flag = check_grep = 0;

	if (LKDB_FLAG(CMD_INTERRUPT)) {
		/* Previous command was interrupted, newline must not repeat the command */
		LKDB_FLAG_CLEAR(CMD_INTERRUPT);
		argc = 0;	/* no repeat */
	}

	if (*cp != '\n' && *cp != '\0') {
		argc = 0;
		cpp = cbuf;
		while (*cp) {
			/* skip whitespace */
			while (isspace(*cp)) cp++;
			if ((*cp == '\0') || (*cp == '\n') || (*cp == '#' && !defcmd_in_progress))
				break;
			/* special case: check for | grep pattern */
			if (*cp == '|') {
				check_grep++;
				break;
			}
			if (cpp >= cbuf + CMD_BUFLEN) {
				lkdb_printf("lkdb_parse: command buffer overflow, command ignored\n%s\n", cmdstr);
				return LKDB_NOTFOUND;
			}
			if (argc >= MAXARGC - 1) {
				lkdb_printf("lkdb_parse: too many arguments, command ignored\n%s\n", cmdstr);
				return LKDB_NOTFOUND;
			}
			argv[argc++] = cpp;
			escaped = 0;
			quoted = '\0';
			/* Copy to next unquoted and unescaped whitespace or '=' */
			while (*cp && *cp != '\n' && (escaped || quoted || !isspace(*cp))) {
				if (cpp >= cbuf + CMD_BUFLEN)
					break;
				if (escaped) {
					escaped = 0;
					*cpp++ = *cp++;
					continue;
				}
				if (*cp == '\\') {
					escaped = 1;
					++cp;
					continue;
				}
				if (*cp == quoted) {
					quoted = '\0';
				} else if (*cp == '\'' || *cp == '"') {
					quoted = *cp;
				}
				if ((*cpp = *cp++) == '=' && !quoted)
					break;
				++cpp;
			}
			*cpp++ = '\0';	/* Squash a ws or '=' character */
		}
	}
	if (!argc)
		return 0;
	if (check_grep)
		parse_grep(cp);
	if (defcmd_in_progress) {
		int result = kdb_defcmd2(cmdstr, argv[0]);
		if (!defcmd_in_progress) {
			argc = 0;	/* avoid repeat on endefcmd */
			*(argv[0]) = '\0';
		}
		return result;
	}
	if (argv[0][0] == '-' && argv[0][1] && (argv[0][1] < '0' || argv[0][1] > '9')) {
		ignore_errors = 1;
		++argv[0];
	}

	for(tp=lkdb_commands, i=0; i < kdb_max_commands; i++,tp++) {
		if (tp->cmd_name) {
			/*
			 * If this command is allowed to be abbreviated,
			 * check to see if this is it.
			 */

			if (tp->cmd_minlen
			 && (strlen(argv[0]) <= tp->cmd_minlen)) {
				if (strncmp(argv[0],
					    tp->cmd_name,
					    tp->cmd_minlen) == 0) {
					break;
				}
			}

			if (strcmp(argv[0], tp->cmd_name)==0) {
				break;
			}
		}
	}

	/*
	 * If we don't find a command by this name, see if the first
	 * few characters of this match any of the known commands.
	 * e.g., md1c20 should match md.
	 */
	if (i == kdb_max_commands) {
		for(tp=lkdb_commands, i=0; i < kdb_max_commands; i++,tp++) {
			if (tp->cmd_name) {
				if (strncmp(argv[0],
					    tp->cmd_name,
					    strlen(tp->cmd_name))==0) {
					break;
				}
			}
		}
	}

	if (i < kdb_max_commands) {
		int result;
		KDB_STATE_SET(CMD);
		result = (*tp->cmd_func)(argc-1,
				       (const char**)argv);
		if (result && ignore_errors && result > KDB_CMD_GO)
			result = 0;
		KDB_STATE_CLEAR(CMD);
		switch (tp->cmd_repeat) {
		case LKDB_REPEAT_NONE:
			argc = 0;
			if (argv[0])
				*(argv[0]) = '\0';
			break;
		case LKDB_REPEAT_NO_ARGS:
			argc = 1;
			if (argv[1])
				*(argv[1]) = '\0';
			break;
		case LKDB_REPEAT_WITH_ARGS:
			break;
		}
		return result;
	}

	/*
	 * If the input with which we were presented does not
	 * map to an existing command, attempt to parse it as an
	 * address argument and display the result.   Useful for
	 * obtaining the address of a variable, or the nearest symbol
	 * to an address contained in a register.
	 */
	{
		kdb_machreg_t value;
		char *name = NULL;
		long offset;
		int nextarg = 0;

		if (lkdbgetaddrarg(0, (const char **)argv, &nextarg,
				  &value, &offset, &name)) {
			return LKDB_NOTFOUND;
		}

		lkdb_printf("%s = ", argv[0]);
		lkdb_symbol_print(value, NULL, KDB_SP_DEFAULT);
		lkdb_printf("\n");
		return 0;
	}
}


static int
handle_ctrl_cmd(char *cmd)
{
#define CTRL_P	16
#define CTRL_N	14

	/* initial situation */
	if (cmd_head == cmd_tail) return 0;

	switch(*cmd) {
		case CTRL_P:
			if (cmdptr != cmd_tail)
				cmdptr = (cmdptr-1) % KDB_CMD_HISTORY_COUNT;
			strncpy(cmd_cur, cmd_hist[cmdptr], CMD_BUFLEN);
			return 1;
		case CTRL_N:
			if (cmdptr != cmd_head)
				cmdptr = (cmdptr+1) % KDB_CMD_HISTORY_COUNT;
			strncpy(cmd_cur, cmd_hist[cmdptr], CMD_BUFLEN);
			return 1;
	}
	return 0;
}

/*
 * kdb_do_dump
 *
 *	Call the dump() function if the kernel is configured for LKCD.
 * Inputs:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.  dump() may or may not return.
 * Locking:
 *	none.
 * Remarks:
 */

static void
kdb_do_dump(void)
{
#if defined(CONFIG_LKCD_DUMP) || defined(CONFIG_LKCD_DUMP_MODULE)
	lkdb_printf("Forcing dump (if configured)\n");
	console_loglevel = 8;	/* to see the dump messages */
	dump("kdb_do_dump");
#endif
}

/*
 * kdb_reboot
 *
 *	This function implements the 'reboot' command.  Reboot the system
 *	immediately.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	Shouldn't return from this function.
 */

static int
kdb_reboot(int argc, const char **argv)
{
	emergency_restart();
	lkdb_printf("Hmm, kdb_reboot did not reboot, spinning here\n");
	while (1) {};
	/* NOTREACHED */
	return 0;
}

#ifdef CONFIG_KDB_KDUMP

int lkdb_kdump_state = LKDB_KDUMP_RESET;	/* KDB kdump state */

static int kdb_cpu(int argc, const char **argv);

/*
 * kdb_kdump_check
 *
 *	This is where the kdump on monarch cpu is handled.
 *
 */
void kdb_kdump_check(struct pt_regs *regs)
{
	if (lkdb_kdump_state != LKDB_KDUMP_RESET) {
		crash_kexec(regs);

		/* If the call above returned then something
		   didn't work */
		lkdb_printf("kdb_kdump_check: crash_kexec failed!\n");
		lkdb_printf("  Please check if the kdump kernel has been properly loaded\n");
		lkdb_kdump_state = LKDB_KDUMP_RESET;
	}
}


/*
 * kdb_kdump
 *
 *     This function implements the 'kdump' command.
 *
 * Inputs:
 *     argc    argument count
 *     argv    argument vector
 *     envp    environment vector
 *     regs    registers at time kdb was entered.
 * Outputs:
 *     None.
 * Returns:
 *     zero for success, a kdb diagnostic if error
 * Locking:
 *     none.
 * Remarks:
 *     Shouldn't return from this function.
 */

static int
kdb_kdump(int argc, const char **argv)
{
	char cpu_id[6];		/* up to 99,999 cpus */
	const char *cpu_argv[] = {NULL, cpu_id, NULL};
	int ret;

	lkdb_kdump_state = LKDB_KDUMP_KDUMP;
	/* Switch back to the initial cpu before process kdump command */
	if (smp_processor_id() != lkdb_initial_cpu) {
		sprintf(cpu_id, "%d", lkdb_initial_cpu);
		ret = kdb_cpu(1, cpu_argv);
		if (ret != KDB_CMD_CPU) {
			lkdb_printf("kdump: Failed to switch to initial cpu %d;"
				" aborted\n", lkdb_initial_cpu);
			lkdb_kdump_state = LKDB_KDUMP_RESET;
		}
	} else
		ret = KDB_CMD_CPU;

	return ret;
}

#endif /* CONFIG_KDB_KDUMP */

static int
kdb_quiet(int reason)
{
	return (reason == LKDB_REASON_CPU_UP || reason == LKDB_REASON_SILENT);
}

/*
 * kdb_local
 *
 *	The main code for kdb.  This routine is invoked on a specific
 *	processor, it is not global.  The main kdb() routine ensures
 *	that only one processor at a time is in this routine.  This
 *	code is called with the real reason code on the first entry
 *	to a kdb session, thereafter it is called with reason SWITCH,
 *	even if the user goes back to the original cpu.
 *
 * Inputs:
 *	reason		The reason KDB was invoked
 *	error		The hardware-defined error code
 *	regs		The exception frame at time of fault/breakpoint.  NULL
 *			for reason SILENT or CPU_UP, otherwise valid.
 *	db_result	Result code from the break or debug point.
 * Returns:
 *	0	KDB was invoked for an event which it wasn't responsible
 *	1	KDB handled the event for which it was invoked.
 *	DB_CMD_GO	User typed 'go'.
 *	DB_CMD_CPU	User switched to another cpu.
 *	DB_CMD_SS	Single step.
 *	DB_CMD_SSB	Single step until branch.
 * Locking:
 *	none
 * Remarks:
 *	none
 */

static int
kdb_local(lkdb_reason_t reason, int error, struct pt_regs *regs, kdb_dbtrap_t db_result)
{
	char *cmdbuf;
	int diag;
	struct task_struct *kdb_current = lkdb_curr_task(smp_processor_id());

#ifdef CONFIG_KDB_KDUMP
	kdb_kdump_check(regs);
#endif

	/* If kdb has been entered for an event which has been/will be
	 * recovered then silently return.  We have to get this far into kdb in
	 * order to synchronize all the cpus, typically only one cpu (monarch)
	 * knows that the event is recoverable but the other cpus (slaves) may
	 * also be driven into kdb before that decision is made by the monarch.
	 *
	 * To pause in kdb even for recoverable events, 'set RECOVERY_PAUSE 1'
	 */
	KDB_DEBUG_STATE("kdb_local 1", reason);
	if (reason == LKDB_REASON_ENTER
	    && LKDB_FLAG(RECOVERY)
	    && !LKDB_FLAG(CATASTROPHIC)) {
		int recovery_pause = 0;
		lkdbgetintenv("RECOVERY_PAUSE", &recovery_pause);
		if (recovery_pause == 0)
			reason = LKDB_REASON_SILENT;
		else
			lkdb_printf("%s: Recoverable error detected but"
				   " RECOVERY_PAUSE is set, staying in KDB\n",
				   __FUNCTION__);
	}

	KDB_DEBUG_STATE("kdb_local 2", reason);
	kdb_go_count = 0;
	if (kdb_quiet(reason)) {
		/* no message */
	} else if (reason == LKDB_REASON_DEBUG) {
		/* special case below */
	} else {
		lkdb_printf("\nEntering kdb (current=0x%p, pid %d) ", kdb_current, kdb_current->pid);
#if defined(CONFIG_SMP)
		lkdb_printf("on processor %d ", smp_processor_id());
#endif
	}

	switch (reason) {
	case LKDB_REASON_DEBUG:
	{
		/*
		 * If re-entering kdb after a single step
		 * command, don't print the message.
		 */
		switch(db_result) {
		case KDB_DB_BPT:
			lkdb_printf("\nEntering kdb (0x%p, pid %d) ", kdb_current, kdb_current->pid);
#if defined(CONFIG_SMP)
			lkdb_printf("on processor %d ", smp_processor_id());
#endif
			lkdb_printf("due to Debug @ " kdb_machreg_fmt "\n", kdba_getpc(regs));
			break;
		case KDB_DB_SSB:
			/*
			 * In the midst of ssb command. Just return.
			 */
			KDB_DEBUG_STATE("kdb_local 3", reason);
			return KDB_CMD_SSB;	/* Continue with SSB command */

			break;
		case KDB_DB_SS:
			break;
		case KDB_DB_SSBPT:
			KDB_DEBUG_STATE("kdb_local 4", reason);
			return 1;	/* kdba_db_trap did the work */
		default:
			lkdb_printf("kdb: Bad result from kdba_db_trap: %d\n",
				   db_result);
			break;
		}

	}
		break;
	case LKDB_REASON_ENTER:
		if (KDB_STATE(KEYBOARD))
			lkdb_printf("due to Keyboard Entry\n");
		else {
			lkdb_printf("due to KDB_ENTER()\n");
		}
		break;
	case LKDB_REASON_KEYBOARD:
		KDB_STATE_SET(KEYBOARD);
		lkdb_printf("due to Keyboard Entry\n");
		break;
	case LKDB_REASON_ENTER_SLAVE:	/* drop through, slaves only get released via cpu switch */
	case LKDB_REASON_SWITCH:
		lkdb_printf("due to cpu switch\n");
		if (KDB_STATE(GO_SWITCH)) {
			KDB_STATE_CLEAR(GO_SWITCH);
			KDB_DEBUG_STATE("kdb_local 5", reason);
			return KDB_CMD_GO;
		}
		break;
	case LKDB_REASON_OOPS:
		lkdb_printf("Oops: %s\n", lkdb_diemsg);
		lkdb_printf("due to oops @ " kdb_machreg_fmt "\n", kdba_getpc(regs));
		kdba_dumpregs(regs, NULL, NULL);
		break;
	case LKDB_REASON_NMI:
		lkdb_printf("due to NonMaskable Interrupt @ " kdb_machreg_fmt "\n",
			  kdba_getpc(regs));
		kdba_dumpregs(regs, NULL, NULL);
		break;
	case LKDB_REASON_BREAK:
		lkdb_printf("due to Breakpoint @ " kdb_machreg_fmt "\n", kdba_getpc(regs));
		/*
		 * Determine if this breakpoint is one that we
		 * are interested in.
		 */
		if (db_result != KDB_DB_BPT) {
			lkdb_printf("kdb: error return from kdba_bp_trap: %d\n", db_result);
			KDB_DEBUG_STATE("kdb_local 6", reason);
			return 0;	/* Not for us, dismiss it */
		}
		break;
	case LKDB_REASON_RECURSE:
		lkdb_printf("due to Recursion @ " kdb_machreg_fmt "\n", kdba_getpc(regs));
		break;
	case LKDB_REASON_CPU_UP:
	case LKDB_REASON_SILENT:
		KDB_DEBUG_STATE("kdb_local 7", reason);
		if (reason == LKDB_REASON_CPU_UP)
			kdba_cpu_up();
		return KDB_CMD_GO;	/* Silent entry, silent exit */
		break;
	default:
		lkdb_printf("kdb: unexpected reason code: %d\n", reason);
		KDB_DEBUG_STATE("kdb_local 8", reason);
		return 0;	/* Not for us, dismiss it */
	}

	kdba_local_arch_setup();

	kdba_set_current_task(kdb_current);

	while (1) {
		/*
		 * Initialize pager context.
		 */
		lkdb_nextline = 1;
		KDB_STATE_CLEAR(SUPPRESS);
#ifdef kdba_setjmp
		/*
		 * Use kdba_setjmp/kdba_longjmp to break out of
		 * the pager early and to attempt to recover from kdb errors.
		 */
		KDB_STATE_CLEAR(LONGJMP);
		if (kdbjmpbuf) {
			if (kdba_setjmp(&kdbjmpbuf[smp_processor_id()])) {
				/* Command aborted (usually in pager) */
				continue;
			}
			else
				KDB_STATE_SET(LONGJMP);
		}
#endif	/* kdba_setjmp */

		cmdbuf = cmd_cur;
		*cmdbuf = '\0';
		*(cmd_hist[cmd_head])='\0';

		if (LKDB_FLAG(ONLY_DO_DUMP)) {
			/* kdb is off but a catastrophic error requires a dump.
			 * Take the dump and reboot.
			 * Turn on logging so the kdb output appears in the log
			 * buffer in the dump.
			 */
			const char *setargs[] = { "set", "LOGGING", "1" };
			lkdb_set(2, setargs);
			kdb_do_dump();
			kdb_reboot(0, NULL);
			/*NOTREACHED*/
		}

do_full_getstr:
#if defined(CONFIG_SMP)
		snprintf(lkdb_prompt_str, CMD_BUFLEN, lkdbgetenv("PROMPT"), smp_processor_id());
#else
		snprintf(lkdb_prompt_str, CMD_BUFLEN, lkdbgetenv("PROMPT"));
#endif
		if (defcmd_in_progress)
			strncat(lkdb_prompt_str, "[defcmd]", CMD_BUFLEN);

		/*
		 * Fetch command from keyboard
		 */
		cmdbuf = lkdb_getstr(cmdbuf, CMD_BUFLEN, lkdb_prompt_str);
		if (*cmdbuf != '\n') {
			if (*cmdbuf < 32) {
				if(cmdptr == cmd_head) {
					strncpy(cmd_hist[cmd_head], cmd_cur, CMD_BUFLEN);
					*(cmd_hist[cmd_head]+strlen(cmd_hist[cmd_head])-1) = '\0';
				}
				if(!handle_ctrl_cmd(cmdbuf))
					*(cmd_cur+strlen(cmd_cur)-1) = '\0';
				cmdbuf = cmd_cur;
				goto do_full_getstr;
			}
			else
				strncpy(cmd_hist[cmd_head], cmd_cur, CMD_BUFLEN);

			cmd_head = (cmd_head+1) % KDB_CMD_HISTORY_COUNT;
			if (cmd_head == cmd_tail) cmd_tail = (cmd_tail+1) % KDB_CMD_HISTORY_COUNT;

		}

		cmdptr = cmd_head;
		diag = lkdb_parse(cmdbuf);
		if (diag == LKDB_NOTFOUND) {
			lkdb_printf("Unknown kdb command: '%s'\n", cmdbuf);
			diag = 0;
		}
		if (diag == KDB_CMD_GO
		 || diag == KDB_CMD_CPU
		 || diag == KDB_CMD_SS
		 || diag == KDB_CMD_SSB)
			break;

		if (diag)
			kdb_cmderror(diag);
	}

	kdba_local_arch_cleanup();

	KDB_DEBUG_STATE("kdb_local 9", diag);
	return diag;
}


/*
 * lkdb_print_state
 *
 *	Print the state data for the current processor for debugging.
 *
 * Inputs:
 *	text		Identifies the debug point
 *	value		Any integer value to be printed, e.g. reason code.
 * Returns:
 *	None.
 * Locking:
 *	none
 * Remarks:
 *	none
 */

void lkdb_print_state(const char *text, int value)
{
	lkdb_printf("state: %s cpu %d value %d initial %d state %x\n",
		text, smp_processor_id(), value, lkdb_initial_cpu, lkdb_state[smp_processor_id()]);
}

/*
 * kdb_previous_event
 *
 *	Return a count of cpus that are leaving kdb, i.e. the number
 *	of processors that are still handling the previous kdb event.
 *
 * Inputs:
 *	None.
 * Returns:
 *	Count of cpus in previous event.
 * Locking:
 *	none
 * Remarks:
 *	none
 */

static int
kdb_previous_event(void)
{
	int i, leaving = 0;
	for (i = 0; i < NR_CPUS; ++i) {
		if (KDB_STATE_CPU(LEAVING, i))
			++leaving;
	}
	return leaving;
}

/*
 * kdb_wait_for_cpus
 *
 * Invoked once at the start of a kdb event, from the controlling cpu.  Wait a
 * short period for the other cpus to enter kdb state.
 *
 * Inputs:
 *	none
 * Returns:
 *	none
 * Locking:
 *	none
 * Remarks:
 *	none
 */

int lkdb_wait_for_cpus_secs;

static void
kdb_wait_for_cpus(void)
{
#ifdef	CONFIG_SMP
	int online = 0, kdb_data = 0, prev_kdb_data = 0, c, time;
	mdelay(100);
	for (time = 0; time < lkdb_wait_for_cpus_secs; ++time) {
		online = 0;
		kdb_data = 0;
		for_each_online_cpu(c) {
			++online;
			if (lkdb_running_process[c].seqno >= lkdb_seqno - 1)
				++kdb_data;
		}
		if (online == kdb_data)
			break;
		if (prev_kdb_data != kdb_data) {
			lkdb_nextline = 0;	/* no prompt yet */
			lkdb_printf("  %d out of %d cpus in kdb, waiting for the rest, timeout in %d second(s)\n",
				kdb_data, online, lkdb_wait_for_cpus_secs - time);
			prev_kdb_data = kdb_data;
		}
		touch_nmi_watchdog();
		mdelay(1000);
		/* Architectures may want to send a more forceful interrupt */
		if (time == min(lkdb_wait_for_cpus_secs / 2, 5))
			kdba_wait_for_cpus();
		if (time % 4 == 0)
			lkdb_printf(".");
	}
	if (time) {
		int wait = online - kdb_data;
		if (wait == 0)
			lkdb_printf("All cpus are now in kdb\n");
		else
			lkdb_printf("%d cpu%s not in kdb, %s state is unknown\n",
					wait,
					wait == 1 ? " is" : "s are",
					wait == 1 ? "its" : "their");
	}
	/* give back the vector we took over in smp_kdb_stop */
	kdba_giveback_vector(LKDB_VECTOR);
#endif	/* CONFIG_SMP */
}

/*
 * lkdb_main_loop
 *
 * The main kdb loop.  After initial setup and assignment of the controlling
 * cpu, all cpus are in this loop.  One cpu is in control and will issue the kdb
 * prompt, the others will spin until 'go' or cpu switch.
 *
 * To get a consistent view of the kernel stacks for all processes, this routine
 * is invoked from the main kdb code via an architecture specific routine.
 * kdba_main_loop is responsible for making the kernel stacks consistent for all
 * processes, there should be no difference between a blocked process and a
 * running process as far as kdb is concerned.
 *
 * Inputs:
 *	reason		The reason KDB was invoked
 *	error		The hardware-defined error code
 *	reason2		kdb's current reason code.  Initially error but can change
 *			acording to kdb state.
 *	db_result	Result code from break or debug point.
 *	regs		The exception frame at time of fault/breakpoint.  If reason
 *			is SILENT or CPU_UP then regs is NULL, otherwise it
 *			should always be valid.
 * Returns:
 *	0	KDB was invoked for an event which it wasn't responsible
 *	1	KDB handled the event for which it was invoked.
 * Locking:
 *	none
 * Remarks:
 *	none
 */

int
lkdb_main_loop(lkdb_reason_t reason, lkdb_reason_t reason2, int error,
	      kdb_dbtrap_t db_result, struct pt_regs *regs)
{
	int result = 1;
	/* Stay in kdb() until 'go', 'ss[b]' or an error */
	while (1) {
		/*
		 * All processors except the one that is in control
		 * will spin here.
		 */
		KDB_DEBUG_STATE("lkdb_main_loop 1", reason);
		while (KDB_STATE(HOLD_CPU)) {
			/* state KDB is turned off by kdb_cpu to see if the
			 * other cpus are still live, each cpu in this loop
			 * turns it back on.
			 */
			if (!KDB_STATE(KDB)) {
				KDB_STATE_SET(KDB);
			}

#if defined(CONFIG_KDB_KDUMP)
			if (KDB_STATE(KEXEC)) {
				struct pt_regs r;
				if (regs == NULL)
					regs = &r;

				kdba_kdump_shutdown_slave(regs);
				return 0;
			}
#endif
		}

		KDB_STATE_CLEAR(SUPPRESS);
		KDB_DEBUG_STATE("lkdb_main_loop 2", reason);
		if (KDB_STATE(LEAVING))
			break;	/* Another cpu said 'go' */

		if (!kdb_quiet(reason))
			kdb_wait_for_cpus();
		/* Still using kdb, this processor is in control */
		result = kdb_local(reason2, error, regs, db_result);
		KDB_DEBUG_STATE("lkdb_main_loop 3", result);

		if (result == KDB_CMD_CPU) {
			/* Cpu switch, hold the current cpu, release the target one. */
			reason2 = LKDB_REASON_SWITCH;
			KDB_STATE_SET(HOLD_CPU);
			KDB_STATE_CLEAR_CPU(HOLD_CPU, kdb_new_cpu);
			continue;
		}

		if (result == KDB_CMD_SS) {
			KDB_STATE_SET(DOING_SS);
			break;
		}

		if (result == KDB_CMD_SSB) {
			KDB_STATE_SET(DOING_SS);
			KDB_STATE_SET(DOING_SSB);
			break;
		}

		if (result && result != 1 && result != KDB_CMD_GO)
			lkdb_printf("\nUnexpected kdb_local return code %d\n", result);

		KDB_DEBUG_STATE("lkdb_main_loop 4", reason);
		break;
	}
	if (KDB_STATE(DOING_SS))
		KDB_STATE_CLEAR(SSBPT);
	return result;
}

/* iapc_boot_arch was defined in ACPI 2.0, FADT revision 3 onwards.  For any
 * FADT prior to revision 3, we have to assume that we have an i8042 I/O
 * device.  ACPI initialises after KDB initialises but before using KDB, so
 * check iapc_boot_arch on each entry to KDB.
 */
static void
kdb_check_i8042(void)
{
	LKDB_FLAG_CLEAR(NO_I8042);
#ifdef	CONFIG_ACPI
	if (acpi_gbl_FADT.header.revision >= 3 &&
	    (acpi_gbl_FADT.boot_flags & ACPI_FADT_8042) == 0)
		LKDB_FLAG_SET(NO_I8042);
#endif	/* CONFIG_ACPI */
}

/*
 * kdb
 *
 *	This function is the entry point for the kernel debugger.  It
 *	provides a command parser and associated support functions to
 *	allow examination and control of an active kernel.
 *
 *	The breakpoint trap code should invoke this function with
 *	one of KDB_REASON_BREAK (int 03) or KDB_REASON_DEBUG (debug register)
 *
 *	the die_if_kernel function should invoke this function with
 *	KDB_REASON_OOPS.
 *
 *	In single step mode, one cpu is released to run without
 *	breakpoints.   Interrupts and NMI are reset to their original values,
 *	the cpu is allowed to do one instruction which causes a trap
 *	into kdb with KDB_REASON_DEBUG.
 *
 * Inputs:
 *	reason		The reason KDB was invoked
 *	error		The hardware-defined error code
 *	regs		The exception frame at time of fault/breakpoint.  If reason
 *			is SILENT or CPU_UP then regs is NULL, otherwise it
 *			should always be valid.
 * Returns:
 *	0	KDB was invoked for an event which it wasn't responsible
 *	1	KDB handled the event for which it was invoked.
 * Locking:
 *	none
 * Remarks:
 *	No assumptions of system state.  This function may be invoked
 *	with arbitrary locks held.  It will stop all other processors
 *	in an SMP environment, disable all interrupts and does not use
 *	the operating systems keyboard driver.
 *
 *	This code is reentrant but only for cpu switch.  Any other
 *	reentrancy is an error, although kdb will attempt to recover.
 *
 *	At the start of a kdb session the initial processor is running
 *	kdb() and the other processors can be doing anything.  When the
 *	initial processor calls smp_kdb_stop() the other processors are
 *	driven through kdb_ipi which calls kdb() with reason SWITCH.
 *	That brings all processors into this routine, one with a "real"
 *	reason code, the other with SWITCH.
 *
 *	Because the other processors are driven via smp_kdb_stop(),
 *	they enter here from the NMI handler.  Until the other
 *	processors exit from here and exit from kdb_ipi, they will not
 *	take any more NMI requests.  The initial cpu will still take NMI.
 *
 *	Multiple race and reentrancy conditions, each with different
 *	advoidance mechanisms.
 *
 *	Two cpus hit debug points at the same time.
 *
 *	  kdb_lock and lkdb_initial_cpu ensure that only one cpu gets
 *	  control of kdb.  The others spin on lkdb_initial_cpu until
 *	  they are driven through NMI into kdb_ipi.  When the initial
 *	  cpu releases the others from NMI, they resume trying to get
 *	  lkdb_initial_cpu to start a new event.
 *
 *	A cpu is released from kdb and starts a new event before the
 *	original event has completely ended.
 *
 *	  kdb_previous_event() prevents any cpu from entering
 *	  lkdb_initial_cpu state until the previous event has completely
 *	  ended on all cpus.
 *
 *	An exception occurs inside kdb.
 *
 *	  lkdb_initial_cpu detects recursive entry to kdb and attempts
 *	  to recover.  The recovery uses longjmp() which means that
 *	  recursive calls to kdb never return.  Beware of assumptions
 *	  like
 *
 *	    ++depth;
 *	    kdb();
 *	    --depth;
 *
 *	  If the kdb call is recursive then longjmp takes over and
 *	  --depth is never executed.
 *
 *	NMI handling.
 *
 *	  NMI handling is tricky.  The initial cpu is invoked by some kdb event,
 *	  this event could be NMI driven but usually is not.  The other cpus are
 *	  driven into kdb() via kdb_ipi which uses NMI so at the start the other
 *	  cpus will not accept NMI.  Some operations such as SS release one cpu
 *	  but hold all the others.  Releasing a cpu means it drops back to
 *	  whatever it was doing before the kdb event, this means it drops out of
 *	  kdb_ipi and hence out of NMI status.  But the software watchdog uses
 *	  NMI and we do not want spurious watchdog calls into kdb.  kdba_read()
 *	  resets the watchdog counters in its input polling loop, when a kdb
 *	  command is running it is subject to NMI watchdog events.
 *
 *	  Another problem with NMI handling is the NMI used to drive the other
 *	  cpus into kdb cannot be distinguished from the watchdog NMI.  State
 *	  flag WAIT_IPI indicates that a cpu is waiting for NMI via kdb_ipi,
 *	  if not set then software NMI is ignored by kdb_ipi.
 *
 *	Cpu switching.
 *
 *	  All cpus are in kdb (or they should be), all but one are
 *	  spinning on KDB_STATE(HOLD_CPU).  Only one cpu is not in
 *	  HOLD_CPU state, only that cpu can handle commands.
 *
 *	Go command entered.
 *
 *	  If necessary, go will switch to the initial cpu first.  If the event
 *	  was caused by a software breakpoint (assumed to be global) that
 *	  requires single-step to get over the breakpoint then only release the
 *	  initial cpu, after the initial cpu has single-stepped the breakpoint
 *	  then release the rest of the cpus.  If SSBPT is not required then
 *	  release all the cpus at once.
 */

int
kdb(lkdb_reason_t reason, int error, struct pt_regs *regs)
{
	lkdb_intstate_t int_state;	/* Interrupt state */
	lkdb_reason_t reason2 = reason;
	int result = 0;	/* Default is kdb did not handle it */
	int ss_event, old_regs_saved = 0;
	struct pt_regs *old_regs = NULL;
	kdb_dbtrap_t db_result=KDB_DB_NOBPT;
	preempt_disable();
	atomic_inc(&lkdb_event);

	switch(reason) {
	case LKDB_REASON_OOPS:
		LKDB_FLAG_SET(CATASTROPHIC); /* kernel state is dubious now */
		break;
	case LKDB_REASON_NMI:
		if (!is_uv_system())	/* NMI is NOT catastrophic on UV */
			LKDB_FLAG_SET(CATASTROPHIC);
		break;
	default:
		break;
	}
	switch(reason) {
	case LKDB_REASON_ENTER:
	case LKDB_REASON_ENTER_SLAVE:
	case LKDB_REASON_BREAK:
	case LKDB_REASON_DEBUG:
	case LKDB_REASON_OOPS:
	case LKDB_REASON_SWITCH:
	case LKDB_REASON_KEYBOARD:
	case LKDB_REASON_NMI:
		if (regs && regs != get_irq_regs()) {
			old_regs = set_irq_regs(regs);
			old_regs_saved = 1;
		}
		break;
	default:
		break;
	}
	kdb_trap_printk = 0; /* don't intercept kernel printk's */
	if (kdb_continue_catastrophic > 2) {
		lkdb_printf("kdb_continue_catastrophic is out of range, setting to 2\n");
		kdb_continue_catastrophic = 2;
	}
	if (!kdb_on && LKDB_FLAG(CATASTROPHIC) && kdb_continue_catastrophic == 2) {
		LKDB_FLAG_SET(ONLY_DO_DUMP);
	}
	if (!kdb_on && !LKDB_FLAG(ONLY_DO_DUMP))
		goto out;

	KDB_DEBUG_STATE("kdb 1", reason);
	KDB_STATE_CLEAR(SUPPRESS);

	/* Filter out userspace breakpoints first, no point in doing all
	 * the kdb smp fiddling when it is really a gdb trap.
	 * Save the single step status first, kdba_db_trap clears ss status.
	 * kdba_b[dp]_trap sets SSBPT if required.
	 */
	ss_event = KDB_STATE(DOING_SS) || KDB_STATE(SSBPT);
#ifdef  CONFIG_CPU_XSCALE
	if ( KDB_STATE(A_XSC_ICH) ) {
		/* restore changed I_BIT */
		KDB_STATE_CLEAR(A_XSC_ICH);
		kdba_restore_retirq(regs, KDB_STATE(A_XSC_IRQ));
		if ( !ss_event ) {
			lkdb_printf("Stranger!!! Why IRQ bit is changed====\n");
		}
	}
#endif
	if (reason == LKDB_REASON_BREAK) {
		db_result = kdba_bp_trap(regs, error);	/* Only call this once */
	}
	if (reason == LKDB_REASON_DEBUG) {
		db_result = kdba_db_trap(regs, error);	/* Only call this once */
	}

	if ((reason == LKDB_REASON_BREAK || reason == LKDB_REASON_DEBUG)
	 && db_result == KDB_DB_NOBPT) {
		KDB_DEBUG_STATE("kdb 2", reason);
		goto out;	/* Not one of mine */
	}

	/* Turn off single step if it was being used */
	if (ss_event) {
		kdba_clearsinglestep(regs);
		/* Single step after a breakpoint removes the need for a delayed reinstall */
		if (reason == LKDB_REASON_BREAK || reason == LKDB_REASON_DEBUG)
			KDB_STATE_CLEAR(SSBPT);
	}

	/* kdb can validly reenter but only for certain well defined conditions */
	if (reason == LKDB_REASON_DEBUG
	 && !KDB_STATE(HOLD_CPU)
	 && ss_event)
		KDB_STATE_SET(REENTRY);
	else
		KDB_STATE_CLEAR(REENTRY);

	/* Wait for previous kdb event to completely exit before starting
	 * a new event.
	 */
	while (kdb_previous_event())
		;
	KDB_DEBUG_STATE("kdb 3", reason);

	/*
	 * If kdb is already active, print a message and try to recover.
	 * If recovery is not possible and recursion is allowed or
	 * forced recursion without recovery is set then try to recurse
	 * in kdb.  Not guaranteed to work but it makes an attempt at
	 * debugging the debugger.
	 */
	if (reason != LKDB_REASON_SWITCH &&
	    reason != LKDB_REASON_ENTER_SLAVE) {
		if (LKDB_IS_RUNNING() && !KDB_STATE(REENTRY)) {
			int recover = 1;
			unsigned long recurse = 0;
			lkdb_printf("kdb: Debugger re-entered on cpu %d, new reason = %d\n",
				smp_processor_id(), reason);
			/* Should only re-enter from released cpu */

			if (KDB_STATE(HOLD_CPU)) {
				lkdb_printf("     Strange, cpu %d should not be running\n", smp_processor_id());
				recover = 0;
			}
			if (!KDB_STATE(CMD)) {
				lkdb_printf("     Not executing a kdb command\n");
				recover = 0;
			}
			if (!KDB_STATE(LONGJMP)) {
				lkdb_printf("     No longjmp available for recovery\n");
				recover = 0;
			}
			kdbgetulenv("RECURSE", &recurse);
			if (recurse > 1) {
				lkdb_printf("     Forced recursion is set\n");
				recover = 0;
			}
			if (recover) {
				lkdb_printf("     Attempting to abort command and recover\n");
#ifdef kdba_setjmp
				kdba_longjmp(&kdbjmpbuf[smp_processor_id()], 0);
#endif	/* kdba_setjmp */
			}
			if (recurse) {
				if (KDB_STATE(RECURSE)) {
					lkdb_printf("     Already in recursive mode\n");
				} else {
					lkdb_printf("     Attempting recursive mode\n");
					KDB_STATE_SET(RECURSE);
					KDB_STATE_SET(REENTRY);
					reason2 = LKDB_REASON_RECURSE;
					recover = 1;
				}
			}
			if (!recover) {
				lkdb_printf("     Cannot recover, allowing event to proceed\n");
				/*temp*/
				while (LKDB_IS_RUNNING())
					cpu_relax();
				goto out;
			}
		}
	} else if (reason == LKDB_REASON_SWITCH && !LKDB_IS_RUNNING()) {
		lkdb_printf("kdb: CPU switch without kdb running, I'm confused\n");
		goto out;
	}

	/*
	 * Disable interrupts, breakpoints etc. on this processor
	 * during kdb command processing
	 */
	KDB_STATE_SET(KDB);
	kdba_disableint(&int_state);
	if (!KDB_STATE(KDB_CONTROL)) {
		lkdb_bp_remove_local();
		KDB_STATE_SET(KDB_CONTROL);
	}

	/*
	 * If not entering the debugger due to CPU switch or single step
	 * reentry, serialize access here.
	 * The processors may race getting to this point - if,
	 * for example, more than one processor hits a breakpoint
	 * at the same time.   We'll serialize access to kdb here -
	 * other processors will loop here, and the NMI from the stop
	 * IPI will take them into kdb as switch candidates.  Once
	 * the initial processor releases the debugger, the rest of
	 * the processors will race for it.
	 *
	 * The above describes the normal state of affairs, where two or more
	 * cpus that are entering kdb at the "same" time are assumed to be for
	 * separate events.  However some processes such as ia64 MCA/INIT will
	 * drive all the cpus into error processing at the same time.  For that
	 * case, all of the cpus entering kdb at the "same" time are really a
	 * single event.
	 *
	 * That case is handled by the use of KDB_ENTER by one cpu (the
	 * monarch) and KDB_ENTER_SLAVE on the other cpus (the slaves).
	 * KDB_ENTER_SLAVE maps to KDB_REASON_ENTER_SLAVE.  The slave events
	 * will be treated as if they had just responded to the kdb IPI, i.e.
	 * as if they were KDB_REASON_SWITCH.
	 *
	 * Because of races across multiple cpus, ENTER_SLAVE can occur before
	 * the main ENTER.   Hold up ENTER_SLAVE here until the main ENTER
	 * arrives.
	 */

	if (reason == LKDB_REASON_ENTER_SLAVE) {
		spin_lock(&kdb_lock);
		while (!LKDB_IS_RUNNING()) {
			spin_unlock(&kdb_lock);
			while (!LKDB_IS_RUNNING())
				cpu_relax();
			spin_lock(&kdb_lock);
		}
		reason = LKDB_REASON_SWITCH;
		KDB_STATE_SET(HOLD_CPU);
		spin_unlock(&kdb_lock);
	}

	if (reason == LKDB_REASON_SWITCH || KDB_STATE(REENTRY))
		;	/* drop through */
	else {
		KDB_DEBUG_STATE("kdb 4", reason);
		spin_lock(&kdb_lock);
		while (LKDB_IS_RUNNING() || kdb_previous_event()) {
			spin_unlock(&kdb_lock);
			while (LKDB_IS_RUNNING() || kdb_previous_event())
				cpu_relax();
			spin_lock(&kdb_lock);
		}
		KDB_DEBUG_STATE("kdb 5", reason);

		lkdb_initial_cpu = smp_processor_id();
		++lkdb_seqno;
		spin_unlock(&kdb_lock);
		if (!kdb_quiet(reason))
			notify_die(DIE_KDEBUG_ENTER, "KDEBUG ENTER", regs, error, 0, 0);
	}

	if (smp_processor_id() == lkdb_initial_cpu
	 && !KDB_STATE(REENTRY)) {
		KDB_STATE_CLEAR(HOLD_CPU);
		KDB_STATE_CLEAR(WAIT_IPI);
		kdb_check_i8042();
		/*
		 * Remove the global breakpoints.  This is only done
		 * once from the initial processor on initial entry.
		 */
		if (!kdb_quiet(reason) || smp_processor_id() == 0)
			lkdb_bp_remove_global();

		/*
		 * If SMP, stop other processors.  The other processors
		 * will enter kdb() with KDB_REASON_SWITCH and spin in
		 * lkdb_main_loop().
		 */
		KDB_DEBUG_STATE("kdb 6", reason);
		if (NR_CPUS > 1 && !kdb_quiet(reason)) {
			int i;
			for (i = 0; i < NR_CPUS; ++i) {
				if (!cpu_online(i))
					continue;
				if (i != lkdb_initial_cpu) {
					KDB_STATE_SET_CPU(HOLD_CPU, i);
					KDB_STATE_SET_CPU(WAIT_IPI, i);
				}
			}
			KDB_DEBUG_STATE("kdb 7", reason);
			smp_kdb_stop();
			KDB_DEBUG_STATE("kdb 8", reason);
		}
	}

	if (KDB_STATE(GO1)) {
		lkdb_bp_remove_global();		/* They were set for single-step purposes */
		KDB_STATE_CLEAR(GO1);
		reason = LKDB_REASON_SILENT;	/* Now silently go */
	}

	/* Set up a consistent set of process stacks before talking to the user */
	KDB_DEBUG_STATE("kdb 9", result);
	result = kdba_main_loop(reason, reason2, error, db_result, regs);
	reason = reason2;	/* back to original event type */

	KDB_DEBUG_STATE("kdb 10", result);
	kdba_adjust_ip(reason, error, regs);
	KDB_STATE_CLEAR(LONGJMP);
	KDB_DEBUG_STATE("kdb 11", result);
	/* go which requires single-step over a breakpoint must only release
	 * one cpu.
	 */
	if (result == KDB_CMD_GO && KDB_STATE(SSBPT))
		KDB_STATE_SET(GO1);

	if (smp_processor_id() == lkdb_initial_cpu &&
	  !KDB_STATE(DOING_SS) &&
	  !KDB_STATE(RECURSE)) {
		/*
		 * (Re)install the global breakpoints and cleanup the cached
		 * symbol table.  This is only done once from the initial
		 * processor on go.
		 */
		KDB_DEBUG_STATE("kdb 12", reason);
		if (!kdb_quiet(reason) || smp_processor_id() == 0) {
			lkdb_bp_install_global(regs);
			lkdbnearsym_cleanup();
			ldebug_kusage();
		}
		if (!KDB_STATE(GO1)) {
			/*
			 * Release all other cpus which will see KDB_STATE(LEAVING) is set.
			 */
			int i;
			for (i = 0; i < NR_CPUS; ++i) {
				if (KDB_STATE_CPU(KDB, i))
					KDB_STATE_SET_CPU(LEAVING, i);
				KDB_STATE_CLEAR_CPU(WAIT_IPI, i);
				KDB_STATE_CLEAR_CPU(HOLD_CPU, i);
			}
			/* Wait until all the other processors leave kdb */
			while (kdb_previous_event() != 1)
				;
			if (!kdb_quiet(reason))
				notify_die(DIE_KDEBUG_LEAVE, "KDEBUG LEAVE", regs, error, 0, 0);
			lkdb_initial_cpu = -1;	/* release kdb control */
			KDB_DEBUG_STATE("kdb 13", reason);
		}
	}

	KDB_DEBUG_STATE("kdb 14", result);
	kdba_restoreint(&int_state);
#ifdef  CONFIG_CPU_XSCALE
	if ( smp_processor_id() == lkdb_initial_cpu &&
	     ( KDB_STATE(SSBPT) | KDB_STATE(DOING_SS) )
	      ) {
		kdba_setsinglestep(regs);
		// disable IRQ in stack frame
		KDB_STATE_SET(A_XSC_ICH);
		if ( kdba_disable_retirq(regs) ) {
			KDB_STATE_SET(A_XSC_IRQ);
		}
		else {
			KDB_STATE_CLEAR(A_XSC_IRQ);
		}
	}
#endif

	/* Only do this work if we are really leaving kdb */
	if (!(KDB_STATE(DOING_SS) || KDB_STATE(SSBPT) || KDB_STATE(RECURSE))) {
		KDB_DEBUG_STATE("kdb 15", result);
		lkdb_bp_install_local(regs);
		if (old_regs_saved)
			set_irq_regs(old_regs);
		KDB_STATE_CLEAR(KDB_CONTROL);
	}

	KDB_DEBUG_STATE("kdb 16", result);
	LKDB_FLAG_CLEAR(CATASTROPHIC);
	KDB_STATE_CLEAR(IP_ADJUSTED);	/* Re-adjust ip next time in */
	KDB_STATE_CLEAR(KEYBOARD);
	KDB_STATE_CLEAR(KDB);		/* Main kdb state has been cleared */
	KDB_STATE_CLEAR(RECURSE);
	KDB_STATE_CLEAR(LEAVING);	/* No more kdb work after this */
	KDB_DEBUG_STATE("kdb 17", reason);
out:
	atomic_dec(&lkdb_event);
	preempt_enable();
	return result != 0;
}

/*
 * kdb_mdr
 *
 *	This function implements the guts of the 'mdr' command.
 *
 *	mdr  <addr arg>,<byte count>
 *
 * Inputs:
 *	addr	Start address
 *	count	Number of bytes
 * Outputs:
 *	None.
 * Returns:
 *	Always 0.  Any errors are detected and printed by lkdb_getarea.
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_mdr(kdb_machreg_t addr, unsigned int count)
{
	unsigned char c;
	while (count--) {
		if (lkdb_getarea(c, addr))
			return 0;
		lkdb_printf("%02x", c);
		addr++;
	}
	lkdb_printf("\n");
	return 0;
}

/*
 * kdb_md
 *
 *	This function implements the 'md', 'md1', 'md2', 'md4', 'md8'
 *	'mdr' and 'mds' commands.
 *
 *	md|mds  [<addr arg> [<line count> [<radix>]]]
 *	mdWcN	[<addr arg> [<line count> [<radix>]]]
 *		where W = is the width (1, 2, 4 or 8) and N is the count.
 *		for eg., md1c20 reads 20 bytes, 1 at a time.
 *	mdr  <addr arg>,<byte count>
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static void
kdb_md_line(const char *fmtstr, kdb_machreg_t addr,
	    int symbolic, int nosect, int bytesperword,
	    int num, int repeat, int phys)
{
	/* print just one line of data */
	lkdb_symtab_t symtab;
	char cbuf[32];
	char *c = cbuf;
	int i;
	unsigned long word;

	memset(cbuf, '\0', sizeof(cbuf));
	if (phys)
		lkdb_printf("phys " kdb_machreg_fmt0 " ", addr);
	else
		lkdb_printf(kdb_machreg_fmt0 " ", addr);

	for (i = 0; i < num && repeat--; i++) {
		if (phys) {
			if (lkdb_getphysword(&word, addr, bytesperword))
				break;
		} else if (lkdb_getword(&word, addr, bytesperword))
			break;
		lkdb_printf(fmtstr, word);
		if (symbolic)
			lkdbnearsym(word, &symtab);
		else
			memset(&symtab, 0, sizeof(symtab));
		if (symtab.sym_name) {
			lkdb_symbol_print(word, &symtab, 0);
			if (!nosect) {
				lkdb_printf("\n");
				lkdb_printf("                       %s %s "
					   kdb_machreg_fmt " " kdb_machreg_fmt " " kdb_machreg_fmt,
					symtab.mod_name,
					symtab.sec_name,
					symtab.sec_start,
					symtab.sym_start,
					symtab.sym_end);
			}
			addr += bytesperword;
		} else {
			union {
				u64 word;
				unsigned char c[8];
			} wc;
			unsigned char *cp;
#ifdef	__BIG_ENDIAN
			cp = wc.c + 8 - bytesperword;
#else
			cp = wc.c;
#endif
			wc.word = word;
#define printable_char(c) ({unsigned char __c = c; isascii(__c) && isprint(__c) ? __c : '.';})
			switch (bytesperword) {
			case 8:
				*c++ = printable_char(*cp++);
				*c++ = printable_char(*cp++);
				*c++ = printable_char(*cp++);
				*c++ = printable_char(*cp++);
				addr += 4;
			case 4:
				*c++ = printable_char(*cp++);
				*c++ = printable_char(*cp++);
				addr += 2;
			case 2:
				*c++ = printable_char(*cp++);
				addr++;
			case 1:
				*c++ = printable_char(*cp++);
				addr++;
				break;
			}
#undef printable_char
		}
	}
	lkdb_printf("%*s %s\n", (int)((num-i)*(2*bytesperword + 1)+1), " ", cbuf);
}

static int
kdb_md(int argc, const char **argv)
{
	static kdb_machreg_t last_addr;
	static int last_radix, last_bytesperword, last_repeat;
	int radix = 16, mdcount = 8, bytesperword = KDB_WORD_SIZE, repeat;
	int nosect = 0;
	char fmtchar, fmtstr[64];
	kdb_machreg_t addr;
	unsigned long word;
	long offset = 0;
	int symbolic = 0;
	int valid = 0;
	int phys = 0;

	lkdbgetintenv("MDCOUNT", &mdcount);
	lkdbgetintenv("RADIX", &radix);
	lkdbgetintenv("BYTESPERWORD", &bytesperword);

	/* Assume 'md <addr>' and start with environment values */
	repeat = mdcount * 16 / bytesperword;

	if (strcmp(argv[0], "mdr") == 0) {
		if (argc != 2)
			return LKDB_ARGCOUNT;
		valid = 1;
	} else if (isdigit(argv[0][2])) {
		bytesperword = (int)(argv[0][2] - '0');
		if (bytesperword == 0) {
			bytesperword = last_bytesperword;
			if (bytesperword == 0) {
				bytesperword = 4;
			}
		}
		last_bytesperword = bytesperword;
		repeat = mdcount * 16 / bytesperword;
		if (!argv[0][3])
			valid = 1;
		else if (argv[0][3] == 'c' && argv[0][4]) {
			char *p;
			repeat = simple_strtoul(argv[0]+4, &p, 10);
			mdcount = ((repeat * bytesperword) + 15) / 16;
			valid = !*p;
		}
		last_repeat = repeat;
	} else if (strcmp(argv[0], "md") == 0)
		valid = 1;
	else if (strcmp(argv[0], "mds") == 0)
		valid = 1;
	else if (strcmp(argv[0], "mdp") == 0) {
		phys = valid = 1;
	}
	if (!valid)
		return LKDB_NOTFOUND;

	if (argc == 0) {
		if (last_addr == 0)
			return LKDB_ARGCOUNT;
		addr = last_addr;
		radix = last_radix;
		bytesperword = last_bytesperword;
		repeat = last_repeat;
		mdcount = ((repeat * bytesperword) + 15) / 16;
	}

	if (argc) {
		kdb_machreg_t val;
		int diag, nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;
		if (argc > nextarg+2)
			return LKDB_ARGCOUNT;

		if (argc >= nextarg) {
			diag = lkdbgetularg(argv[nextarg], &val);
			if (!diag) {
				mdcount = (int) val;
				repeat = mdcount * 16 / bytesperword;
			}
		}
		if (argc >= nextarg+1) {
			diag = lkdbgetularg(argv[nextarg+1], &val);
			if (!diag)
				radix = (int) val;
		}
	}

	if (strcmp(argv[0], "mdr") == 0) {
		return kdb_mdr(addr, mdcount);
	}

	switch (radix) {
	case 10:
		fmtchar = 'd';
		break;
	case 16:
		fmtchar = 'x';
		break;
	case 8:
		fmtchar = 'o';
		break;
	default:
		return LKDB_BADRADIX;
	}

	last_radix = radix;

	if (bytesperword > KDB_WORD_SIZE)
		return LKDB_BADWIDTH;

	switch (bytesperword) {
	case 8:
		sprintf(fmtstr, "%%16.16l%c ", fmtchar);
		break;
	case 4:
		sprintf(fmtstr, "%%8.8l%c ", fmtchar);
		break;
	case 2:
		sprintf(fmtstr, "%%4.4l%c ", fmtchar);
		break;
	case 1:
		sprintf(fmtstr, "%%2.2l%c ", fmtchar);
		break;
	default:
		return LKDB_BADWIDTH;
	}

	last_repeat = repeat;
	last_bytesperword = bytesperword;

	if (strcmp(argv[0], "mds") == 0) {
		symbolic = 1;
		/* Do not save these changes as last_*, they are temporary mds
		 * overrides.
		 */
		bytesperword = KDB_WORD_SIZE;
		repeat = mdcount;
		lkdbgetintenv("NOSECT", &nosect);
	}

	/* Round address down modulo BYTESPERWORD */

	addr &= ~(bytesperword-1);

	while (repeat > 0) {
		unsigned long a;
		int n, z, num = (symbolic ? 1 : (16 / bytesperword));

		for (a = addr, z = 0; z < repeat; a += bytesperword, ++z) {
			if (phys) {
				if (lkdb_getphysword(&word, a, bytesperword)
						|| word)
					break;
			} else if (lkdb_getword(&word, a, bytesperword) || word)
				break;
		}
		n = min(num, repeat);
		kdb_md_line(fmtstr, addr, symbolic, nosect, bytesperword, num, repeat, phys);
		addr += bytesperword * n;
		repeat -= n;
		z = (z + num - 1) / num;
		if (z > 2) {
			int s = num * (z-2);
			lkdb_printf(kdb_machreg_fmt0 "-" kdb_machreg_fmt0 " zero suppressed\n",
				addr, addr + bytesperword * s - 1);
			addr += bytesperword * s;
			repeat -= s;
		}
	}
	last_addr = addr;

	return 0;
}

/*
 * kdb_mm
 *
 *	This function implements the 'mm' command.
 *
 *	mm address-expression new-value
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	mm works on machine words, mmW works on bytes.
 */

static int
kdb_mm(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset = 0;
	unsigned long contents;
	int nextarg;
	int width;

	if (argv[0][2] && !isdigit(argv[0][2]))
		return LKDB_NOTFOUND;

	if (argc < 2) {
		return LKDB_ARGCOUNT;
	}

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)))
		return diag;

	if (nextarg > argc)
		return LKDB_ARGCOUNT;

	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &contents, NULL, NULL)))
		return diag;

	if (nextarg != argc + 1)
		return LKDB_ARGCOUNT;

	width = argv[0][2] ? (argv[0][2] - '0') : (KDB_WORD_SIZE);
	if ((diag = lkdb_putword(addr, contents, width)))
		return diag;

	lkdb_printf(kdb_machreg_fmt " = " kdb_machreg_fmt "\n", addr, contents);

	return 0;
}

/*
 * kdb_go
 *
 *	This function implements the 'go' command.
 *
 *	go [address-expression]
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	KDB_CMD_GO for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_go(int argc, const char **argv)
{
	kdb_machreg_t addr;
	int diag;
	int nextarg;
	long offset;
	struct pt_regs *regs = get_irq_regs();

	if (argc == 1) {
		if (smp_processor_id() != lkdb_initial_cpu) {
			lkdb_printf("go <address> must be issued from the initial cpu, do cpu %d first\n", lkdb_initial_cpu);
			return LKDB_ARGCOUNT;
		}
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg,
				     &addr, &offset, NULL);
		if (diag)
			return diag;

		kdba_setpc(regs, addr);
	} else if (argc)
		return LKDB_ARGCOUNT;

	diag = KDB_CMD_GO;
	if (LKDB_FLAG(CATASTROPHIC)) {
		lkdb_printf("Catastrophic error detected\n");
		lkdb_printf("kdb_continue_catastrophic=%d, ",
			kdb_continue_catastrophic);
		if (kdb_continue_catastrophic == 0 && kdb_go_count++ == 0) {
			lkdb_printf("type go a second time if you really want to continue\n");
			return 0;
		}
		if (kdb_continue_catastrophic == 2) {
			kdb_do_dump();
			lkdb_printf("forcing reboot\n");
			kdb_reboot(0, NULL);
		}
		lkdb_printf("attempting to continue\n");
		kdb_trap_printk = 0; /* don't intercept kernel printk's */
	}
	if (smp_processor_id() != lkdb_initial_cpu) {
		char buf[80];
		lkdb_printf("go was not issued from initial cpu, switching back to cpu %d\n", lkdb_initial_cpu);
		sprintf(buf, "cpu %d\n", lkdb_initial_cpu);
		/* Recursive use of lkdb_parse, do not use argv after this point */
		argv = NULL;
		diag = lkdb_parse(buf);
		if (diag == KDB_CMD_CPU)
			KDB_STATE_SET_CPU(GO_SWITCH, lkdb_initial_cpu);
	}
	return diag;
}

/*
 * kdb_rd
 *
 *	This function implements the 'rd' command.
 *
 *	rd		display all general registers.
 *	rd  c		display all control registers.
 *	rd  d		display all debug registers.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_rd(int argc, const char **argv)
{
	int diag;
	if (argc == 0) {
		if ((diag = kdb_check_regs()))
			return diag;
		return kdba_dumpregs(lkdb_current_regs, NULL, NULL);
	}

	if (argc > 2) {
		return LKDB_ARGCOUNT;
	}

	if ((diag = kdb_check_regs()))
		return diag;
	return kdba_dumpregs(lkdb_current_regs, argv[1], argc==2 ? argv[2]: NULL);
}

/*
 * kdb_rm
 *
 *	This function implements the 'rm' (register modify)  command.
 *
 *	rm register-name new-contents
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	Currently doesn't allow modification of control or
 *	debug registers.
 */

static int
kdb_rm(int argc, const char **argv)
{
	int diag;
	int ind = 0;
	kdb_machreg_t contents;

	if (argc != 2) {
		return LKDB_ARGCOUNT;
	}

	/*
	 * Allow presence or absence of leading '%' symbol.
	 */

	if (argv[1][0] == '%')
		ind = 1;

	diag = lkdbgetularg(argv[2], &contents);
	if (diag)
		return diag;

	if ((diag = kdb_check_regs()))
		return diag;
	diag = kdba_setregcontents(&argv[1][ind], lkdb_current_regs, contents);
	if (diag)
		return diag;

	return 0;
}

#if defined(CONFIG_MAGIC_SYSRQ)
/*
 * kdb_sr
 *
 *	This function implements the 'sr' (SYSRQ key) command which
 *	interfaces to the soi-disant MAGIC SYSRQ functionality.
 *
 *	sr <magic-sysrq-code>
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	None.
 */
static int
kdb_sr(int argc, const char **argv)
{
	extern int __sysrq_enabled;
	if (argc != 1) {
		return LKDB_ARGCOUNT;
	}
	if (!__sysrq_enabled) {
		lkdb_printf("Auto activating sysrq\n");
		__sysrq_enabled = 1;
	}

	handle_sysrq(*argv[1]);

	return 0;
}
#endif	/* CONFIG_MAGIC_SYSRQ */

/*
 * kdb_ef
 *
 *	This function implements the 'regs' (display exception frame)
 *	command.  This command takes an address and expects to find
 *	an exception frame at that address, formats and prints it.
 *
 *	regs address-expression
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	Not done yet.
 */

static int
kdb_ef(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset;
	int nextarg;

	if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;

		return kdba_dumpregs((struct pt_regs *)addr, NULL, NULL);
	}

	return LKDB_ARGCOUNT;
}

#if defined(CONFIG_MODULES)
extern struct list_head *kdb_modules;
extern void free_module(struct module *);

/*
 * kdb_lsmod
 *
 *	This function implements the 'lsmod' command.  Lists currently
 *	loaded kernel modules.
 *
 *	Mostly taken from userland lsmod.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *
 */

static int
kdb_lsmod(int argc, const char **argv)
{
	struct module *mod, *use;

	if (argc != 0)
		return LKDB_ARGCOUNT;

	lkdb_printf("Module                  Size  modstruct     Used by\n");
	list_for_each_entry(mod, kdb_modules, list) {

		lkdb_printf("%-20s%8u  0x%p ", mod->name,
			   mod->core_size, (void *)mod);
#ifdef CONFIG_MODULE_UNLOAD
		lkdb_printf("%4d ", module_refcount(mod));
#endif
		if (mod->state == MODULE_STATE_GOING)
			lkdb_printf(" (Unloading)");
		else if (mod->state == MODULE_STATE_COMING)
			lkdb_printf(" (Loading)");
		else
			lkdb_printf(" (Live)");

#ifdef CONFIG_MODULE_UNLOAD
		lkdb_printf(" dependents ");
		{
			lkdb_printf("[");
			list_for_each_entry(use, &mod->source_list, list)
				lkdb_printf("%s ", use->name);
			lkdb_printf("]\n");
		}
#endif
	}

	return 0;
}

#endif	/* CONFIG_MODULES */

/*
 * kdb_env
 *
 *	This function implements the 'env' command.  Display the current
 *	environment variables.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_env(int argc, const char **argv)
{
	int i;

	for(i=0; i<__nenv; i++) {
		if (__env[i]) {
			lkdb_printf("%s\n", __env[i]);
		}
	}

	if (KDB_DEBUG(MASK))
		lkdb_printf("KDBFLAGS=0x%x\n", lkdb_flags);

	return 0;
}

/*
 * kdb_dmesg
 *
 *	This function implements the 'dmesg' command to display the contents
 *	of the syslog buffer.
 *
 *	dmesg [lines] [adjust]
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	None.
 */

static int
kdb_dmesg(int argc, const char **argv)
{
	char *syslog_data[4], *start, *end, c = '\0', *p;
	int diag, logging, logsize, lines = 0, adjust = 0, n;

	if (argc > 2)
		return LKDB_ARGCOUNT;
	if (argc) {
		char *cp;
		lines = simple_strtol(argv[1], &cp, 0);
		if (*cp)
			lines = 0;
		if (argc > 1) {
			adjust = simple_strtoul(argv[2], &cp, 0);
			if (*cp || adjust < 0)
				adjust = 0;
		}
	}

	/* disable LOGGING if set */
	diag = lkdbgetintenv("LOGGING", &logging);
	if (!diag && logging) {
		const char *setargs[] = { "set", "LOGGING", "0" };
		lkdb_set(2, setargs);
	}

	/* syslog_data[0,1] physical start, end+1.  syslog_data[2,3] logical start, end+1. */
	kdb_syslog_data(syslog_data);
	if (syslog_data[2] == syslog_data[3])
		return 0;
	logsize = syslog_data[1] - syslog_data[0];
	start = syslog_data[2];
	end = syslog_data[3];
#define KDB_WRAP(p) (((p - syslog_data[0]) % logsize) + syslog_data[0])
	for (n = 0, p = start; p < end; ++p) {
		if ((c = *KDB_WRAP(p)) == '\n')
			++n;
	}
	if (c != '\n')
		++n;
	if (lines < 0) {
		if (adjust >= n)
			lkdb_printf("buffer only contains %d lines, nothing printed\n", n);
		else if (adjust - lines >= n)
			lkdb_printf("buffer only contains %d lines, last %d lines printed\n",
				n, n - adjust);
		if (adjust) {
			for (; start < end && adjust; ++start) {
				if (*KDB_WRAP(start) == '\n')
					--adjust;
			}
			if (start < end)
				++start;
		}
		for (p = start; p < end && lines; ++p) {
			if (*KDB_WRAP(p) == '\n')
				++lines;
		}
		end = p;
	} else if (lines > 0) {
		int skip = n - (adjust + lines);
		if (adjust >= n) {
			lkdb_printf("buffer only contains %d lines, nothing printed\n", n);
			skip = n;
		} else if (skip < 0) {
			lines += skip;
			skip = 0;
			lkdb_printf("buffer only contains %d lines, first %d lines printed\n",
				n, lines);
		}
		for (; start < end && skip; ++start) {
			if (*KDB_WRAP(start) == '\n')
				--skip;
		}
		for (p = start; p < end && lines; ++p) {
			if (*KDB_WRAP(p) == '\n')
				--lines;
		}
		end = p;
	}
	/* Do a line at a time (max 200 chars) to reduce protocol overhead */
	c = '\n';
	while (start != end) {
		char buf[201];
	       	p = buf;
		while (start < end && (c = *KDB_WRAP(start)) && (p - buf) < sizeof(buf)-1) {
			++start;
			*p++ = c;
			if (c == '\n')
				break;
		}
		*p = '\0';
		lkdb_printf("%s", buf);
	}
	if (c != '\n')
		lkdb_printf("\n");

	return 0;
}

/*
 * kdb_cpu
 *
 *	This function implements the 'cpu' command.
 *
 *	cpu	[<cpunum>]
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	KDB_CMD_CPU for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	All cpu's should be spinning in kdb().  However just in case
 *	a cpu did not take the smp_kdb_stop NMI, check that a cpu
 *	entered kdb() before passing control to it.
 */

static void
kdb_cpu_status(void)
{
	int i, start_cpu, first_print = 1;
	char state, prev_state = '?';

	lkdb_printf("Currently on cpu %d\n", smp_processor_id());
	lkdb_printf("Available cpus: ");
	for (start_cpu = -1, i = 0; i < NR_CPUS; i++) {
		if (!cpu_online(i))
			state = 'F';	/* cpu is offline */
		else {
			struct lkdb_running_process *krp = lkdb_running_process+i;
			if (KDB_STATE_CPU(KDB, i)) {
				state = ' ';	/* cpu is responding to kdb */
				if (lkdb_task_state_char(krp->p) == 'I')
					state = 'I';	/* running the idle task */
			} else if (krp->seqno && krp->p && krp->seqno >= lkdb_seqno - 1)
				state = '+';	/* some kdb data, but not responding */
			else
				state = '*';	/* no kdb data */
		}
		if (state != prev_state) {
			if (prev_state != '?') {
				if (!first_print)
					lkdb_printf(", ");
				first_print = 0;
				lkdb_printf("%d", start_cpu);
				if (start_cpu < i-1)
					lkdb_printf("-%d", i-1);
				if (prev_state != ' ')
					lkdb_printf("(%c)", prev_state);
			}
			prev_state = state;
			start_cpu = i;
		}
	}
	/* print the trailing cpus, ignoring them if they are all offline */
	if (prev_state != 'F') {
		if (!first_print)
			lkdb_printf(", ");
		lkdb_printf("%d", start_cpu);
		if (start_cpu < i-1)
			lkdb_printf("-%d", i-1);
		if (prev_state != ' ')
			lkdb_printf("(%c)", prev_state);
	}
	lkdb_printf("\n");
}

static int
kdb_cpu(int argc, const char **argv)
{
	unsigned long cpunum;
	int diag, i;

	/* ask the other cpus if they are still active */
	for (i=0; i<NR_CPUS; i++) {
		if (cpu_online(i))
			KDB_STATE_CLEAR_CPU(KDB, i);
	}
	KDB_STATE_SET(KDB);
	barrier();
	/* wait for the other cpus to notice and set state KDB again,
	 * see lkdb_main_loop
	 */
	udelay(1000);

	if (argc == 0) {
		kdb_cpu_status();
		return 0;
	}

	if (argc != 1)
		return LKDB_ARGCOUNT;

	diag = lkdbgetularg(argv[1], &cpunum);
	if (diag)
		return diag;

	/*
	 * Validate cpunum
	 */
	if ((cpunum > NR_CPUS)
	 || !cpu_online(cpunum)
	 || !KDB_STATE_CPU(KDB, cpunum))
		return LKDB_BADCPUNUM;

	kdb_new_cpu = cpunum;

	/*
	 * Switch to other cpu
	 */
	return KDB_CMD_CPU;
}

/* The user may not realize that ps/bta with no parameters does not print idle
 * or sleeping system daemon processes, so tell them how many were suppressed.
 */
void
lkdb_ps_suppressed(void)
{
	int idle = 0, daemon = 0;
	unsigned long mask_I = lkdb_task_state_string("I"),
		      mask_M = lkdb_task_state_string("M");
	unsigned long cpu;
	const struct task_struct *p, *g;
	for (cpu = 0; cpu < NR_CPUS; ++cpu) {
		if (!cpu_online(cpu))
			continue;
		p = lkdb_curr_task(cpu);
		if (lkdb_task_state(p, mask_I))
			++idle;
	}
	lkdb_do_each_thread(g, p) {
		if (lkdb_task_state(p, mask_M))
			++daemon;
	} lkdb_while_each_thread(g, p);
	if (idle || daemon) {
		if (idle)
			lkdb_printf("%d idle process%s (state I)%s\n",
				   idle, idle == 1 ? "" : "es",
				   daemon ? " and " : "");
		if (daemon)
			lkdb_printf("%d sleeping system daemon (state M) process%s",
				   daemon, daemon == 1 ? "" : "es");
		lkdb_printf(" suppressed,\nuse 'ps A' to see all.\n");
	}
}

/*
 * kdb_ps
 *
 *	This function implements the 'ps' command which shows
 *	a list of the active processes.
 *
 *	ps [DRSTCZEUIMA]		All processes, optionally filtered by state
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

void
lkdb_ps1(const struct task_struct *p)
{
	struct lkdb_running_process *krp = lkdb_running_process + lkdb_process_cpu(p);
	lkdb_printf("0x%p %8d %8d  %d %4d   %c  0x%p %c%s\n",
		   (void *)p, p->pid, p->parent->pid,
		   lkdb_task_has_cpu(p), lkdb_process_cpu(p),
		   lkdb_task_state_char(p),
		   (void *)(&p->thread),
		   p == lkdb_curr_task(smp_processor_id()) ? '*': ' ',
		   p->comm);
	if (lkdb_task_has_cpu(p)) {
		if (!krp->seqno || !krp->p)
			lkdb_printf("  Error: no saved data for this cpu\n");
		else {
			if (krp->seqno < lkdb_seqno - 1)
				lkdb_printf("  Warning: process state is stale\n");
			if (krp->p != p)
				lkdb_printf("  Error: does not match running process table (0x%p)\n", krp->p);
		}
	}
}

static int
kdb_ps(int argc, const char **argv)
{
	struct task_struct *g, *p;
	unsigned long mask, cpu;

	if (argc == 0)
		lkdb_ps_suppressed();
	lkdb_printf("%-*s      Pid   Parent [*] cpu State %-*s Command\n",
		(int)(2*sizeof(void *))+2, "Task Addr",
		(int)(2*sizeof(void *))+2, "Thread");
	mask = lkdb_task_state_string(argc ? argv[1] : NULL);
	/* Run the active tasks first */
	for (cpu = 0; cpu < NR_CPUS; ++cpu) {
		if (!cpu_online(cpu))
			continue;
		p = lkdb_curr_task(cpu);
		if (lkdb_task_state(p, mask))
			lkdb_ps1(p);
	}
	lkdb_printf("\n");
	/* Now the real tasks */
	lkdb_do_each_thread(g, p) {
		if (lkdb_task_state(p, mask))
			lkdb_ps1(p);
	} lkdb_while_each_thread(g, p);

	return 0;
}

/*
 * kdb_pid
 *
 *	This function implements the 'pid' command which switches
 *	the currently active process.
 *
 *	pid [<pid> | R]
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */


static int
kdb_pid(int argc, const char **argv)
{
	struct task_struct *p;
	unsigned long val;
	int diag;

	if (argc > 1)
		return LKDB_ARGCOUNT;

	if (argc) {
		if (strcmp(argv[1], "R") == 0) {
			p = KDB_RUNNING_PROCESS_ORIGINAL[lkdb_initial_cpu].p;
		} else {
			diag = lkdbgetularg(argv[1], &val);
			if (diag)
				return LKDB_BADINT;

			p = find_task_by_pid_ns((pid_t)val, &init_pid_ns);
			if (!p) {
				lkdb_printf("No task with pid=%d\n", (pid_t)val);
				return 0;
			}
		}

		kdba_set_current_task(p);
	}

	lkdb_printf("KDB current process is %s(pid=%d)\n", lkdb_current_task->comm,
		   lkdb_current_task->pid);

	return 0;
}

/*
 * kdb_ll
 *
 *	This function implements the 'll' command which follows a linked
 *	list and executes an arbitrary command for each element.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_ll(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset = 0;
	kdb_machreg_t va;
	unsigned long linkoffset;
	int nextarg;
	const char *command;

	if (argc != 3) {
		return LKDB_ARGCOUNT;
	}

	nextarg = 1;
	diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
	if (diag)
		return diag;

	diag = lkdbgetularg(argv[2], &linkoffset);
	if (diag)
		return diag;

	/*
	 * Using the starting address as
	 * the first element in the list, and assuming that
	 * the list ends with a null pointer.
	 */

	va = addr;
	if (!(command = lkdb_strdup(argv[3], GFP_KDB))) {
		lkdb_printf("%s: cannot duplicate command\n", __FUNCTION__);
		return 0;
	}
	/* Recursive use of lkdb_parse, do not use argv after this point */
	argv = NULL;

	while (va) {
		char buf[80];

		sprintf(buf, "%s " kdb_machreg_fmt "\n", command, va);
		diag = lkdb_parse(buf);
		if (diag)
			return diag;

		addr = va + linkoffset;
		if (lkdb_getword(&va, addr, sizeof(va)))
			return 0;
	}
	kfree(command);

	return 0;
}

/*
 * kdb_help
 *
 *	This function implements the 'help' and '?' commands.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_help(int argc, const char **argv)
{
	lkdbtab_t *kt;
	int i;

	lkdb_printf("%-15.15s %-20.20s %s\n", "Command", "Usage", "Description");
	lkdb_printf("----------------------------------------------------------\n");
	for(i=0, kt=lkdb_commands; i<kdb_max_commands; i++, kt++) {
		if (kt->cmd_name) {
			if ((argc == 0) ||
			    (argc && !strcmp(argv[1], kt->cmd_name)))
				lkdb_printf("%-15.15s %-20.20s %s\n",
					kt->cmd_name, kt->cmd_usage,
					kt->cmd_help);
		}
	}
	return 0;
}

extern int kdb_wake_up_process(struct task_struct * p);

/*
 * kdb_kill
 *
 *	This function implements the 'kill' commands.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_kill(int argc, const char **argv)
{
	long sig, pid;
	char *endp;
	struct task_struct *p;
	struct siginfo info;

	if (argc!=2)
		return LKDB_ARGCOUNT;

	sig = simple_strtol(argv[1], &endp, 0);
	if (*endp)
		return LKDB_BADINT;
	if (sig >= 0 ) {
		lkdb_printf("Invalid signal parameter.<-signal>\n");
		return 0;
	}
	sig=-sig;

	pid = simple_strtol(argv[2], &endp, 0);
	if (*endp)
		return LKDB_BADINT;
	if (pid <=0 ) {
		lkdb_printf("Process ID must be large than 0.\n");
		return 0;
	}

	/* Find the process. */
	if (!(p = find_task_by_pid_ns(pid, &init_pid_ns))) {
		lkdb_printf("The specified process isn't found.\n");
		return 0;
	}
	p = p->group_leader;
	info.si_signo = sig;
	info.si_errno = 0;
	info.si_code = SI_USER;
	info.si_pid = pid;	/* use same capabilities as process being signalled */
	info.si_uid = 0;	/* kdb has root authority */
	lkdb_send_sig_info(p, &info, lkdb_seqno);
	return 0;
}

struct kdb_tm {
	int tm_sec;	/* seconds */
	int tm_min;	/* minutes */
	int tm_hour;	/* hours */
	int tm_mday;	/* day of the month */
	int tm_mon;	/* month */
	int tm_year;	/* year */
};

static void
kdb_gmtime(struct timespec *tv, struct kdb_tm *tm)
{
	/* This will work from 1970-2099, 2100 is not a leap year */
	static int mon_day[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	memset(tm, 0, sizeof(*tm));
	tm->tm_sec  = tv->tv_sec % (24 * 60 * 60);
	tm->tm_mday = tv->tv_sec / (24 * 60 * 60) + (2 * 365 + 1); /* shift base from 1970 to 1968 */
	tm->tm_min =  tm->tm_sec / 60 % 60;
	tm->tm_hour = tm->tm_sec / 60 / 60;
	tm->tm_sec =  tm->tm_sec % 60;
	tm->tm_year = 68 + 4*(tm->tm_mday / (4*365+1));
	tm->tm_mday %= (4*365+1);
	mon_day[1] = 29;
	while (tm->tm_mday >= mon_day[tm->tm_mon]) {
		tm->tm_mday -= mon_day[tm->tm_mon];
		if (++tm->tm_mon == 12) {
			tm->tm_mon = 0;
			++tm->tm_year;
			mon_day[1] = 28;
		}
	}
	++tm->tm_mday;
}

/*
 * Most of this code has been lifted from kernel/timer.c::sys_sysinfo().
 * I cannot call that code directly from kdb, it has an unconditional
 * cli()/sti() and calls routines that take locks which can stop the debugger.
 */

static void
kdb_sysinfo(struct sysinfo *val)
{
	struct timespec uptime;
	do_posix_clock_monotonic_gettime(&uptime);
	memset(val, 0, sizeof(*val));
	val->uptime = uptime.tv_sec;
	val->loads[0] = avenrun[0];
	val->loads[1] = avenrun[1];
	val->loads[2] = avenrun[2];
	val->procs = nr_threads-1;
	si_meminfo(val);
	_si_swapinfo(val);

	return;
}

/*
 * kdb_summary
 *
 *	This function implements the 'summary' command.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_summary(int argc, const char **argv)
{
	extern struct timespec xtime;
	extern struct timezone sys_tz;
	struct kdb_tm tm;
	struct sysinfo val;

	if (argc)
		return LKDB_ARGCOUNT;

	lkdb_printf("sysname    %s\n", init_uts_ns.name.sysname);
	lkdb_printf("release    %s\n", init_uts_ns.name.release);
	lkdb_printf("version    %s\n", init_uts_ns.name.version);
	lkdb_printf("machine    %s\n", init_uts_ns.name.machine);
	lkdb_printf("nodename   %s\n", init_uts_ns.name.nodename);
	lkdb_printf("domainname %s\n", init_uts_ns.name.domainname);
	lkdb_printf("ccversion  %s\n", __stringify(CCVERSION));

	kdb_gmtime(&xtime, &tm);
	lkdb_printf("date       %04d-%02d-%02d %02d:%02d:%02d tz_minuteswest %d\n",
		1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec,
		sys_tz.tz_minuteswest);

	kdb_sysinfo(&val);
	lkdb_printf("uptime     ");
	if (val.uptime > (24*60*60)) {
		int days = val.uptime / (24*60*60);
		val.uptime %= (24*60*60);
		lkdb_printf("%d day%s ", days, days == 1 ? "" : "s");
	}
	lkdb_printf("%02ld:%02ld\n", val.uptime/(60*60), (val.uptime/60)%60);

	/* lifted from fs/proc/proc_misc.c::loadavg_read_proc() */

#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)
	lkdb_printf("load avg   %ld.%02ld %ld.%02ld %ld.%02ld\n",
		LOAD_INT(val.loads[0]), LOAD_FRAC(val.loads[0]),
		LOAD_INT(val.loads[1]), LOAD_FRAC(val.loads[1]),
		LOAD_INT(val.loads[2]), LOAD_FRAC(val.loads[2]));
	lkdb_printf("\n");
#undef LOAD_INT
#undef LOAD_FRAC

	kdb_meminfo_proc_show();	/* in fs/proc/meminfo.c */

	return 0;
}

/*
 * kdb_per_cpu
 *
 *	This function implements the 'per_cpu' command.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 */

static int
kdb_per_cpu(int argc, const char **argv)
{
	char fmtstr[64];
	lkdb_symtab_t symtab;
	cpumask_t suppress;
	int cpu, diag;
	unsigned long addr, val, bytesperword = 0, whichcpu = ~0UL;

	if (argc < 1 || argc > 3)
		return LKDB_ARGCOUNT;

	cpus_clear(suppress);
	if (!lkdbgetsymval(argv[1], &symtab)) {
		lkdb_printf("%s is not a per_cpu variable\n", argv[1]);
		return LKDB_BADADDR;
	}
	if (argc >=2 && (diag = lkdbgetularg(argv[2], &bytesperword)))
		return diag;
	if (!bytesperword)
		bytesperword = KDB_WORD_SIZE;
	else if (bytesperword > KDB_WORD_SIZE)
		return LKDB_BADWIDTH;
	sprintf(fmtstr, "%%0%dlx ", (int)(2*bytesperword));
	if (argc >= 3) {
		if ((diag = lkdbgetularg(argv[3], &whichcpu)))
			return diag;
		if (!cpu_online(whichcpu)) {
			lkdb_printf("cpu %ld is not online\n", whichcpu);
			return LKDB_BADCPUNUM;
		}
	}

	/* Most architectures use __per_cpu_offset[cpu], some use
	 * __per_cpu_offset(cpu), smp has no __per_cpu_offset.
	 */
#ifdef	__per_cpu_offset
#define KDB_PCU(cpu) __per_cpu_offset(cpu)
#else
#ifdef	CONFIG_SMP
#define KDB_PCU(cpu) __per_cpu_offset[cpu]
#else
#define KDB_PCU(cpu) 0
#endif
#endif

	for_each_online_cpu(cpu) {
		if (whichcpu != ~0UL && whichcpu != cpu)
			continue;
		addr = symtab.sym_start + KDB_PCU(cpu);
		if ((diag = lkdb_getword(&val, addr, bytesperword))) {
			lkdb_printf("%5d " kdb_bfd_vma_fmt0 " - unable to read, diag=%d\n",
				cpu, addr, diag);
			continue;
		}
#ifdef	CONFIG_SMP
		if (!val) {
			cpu_set(cpu, suppress);
			continue;
		}
#endif	/* CONFIG_SMP */
		lkdb_printf("%5d ", cpu);
		kdb_md_line(fmtstr, addr,
			bytesperword == KDB_WORD_SIZE,
			1, bytesperword, 1, 1, 0);
	}
	if (cpus_weight(suppress) == 0)
		return 0;
	lkdb_printf("Zero suppressed cpu(s):");
	for_each_cpu_mask(cpu, suppress) {
		lkdb_printf(" %d", cpu);
		if (cpu == NR_CPUS-1 || next_cpu(cpu, suppress) != cpu + 1)
			continue;
		while (cpu < NR_CPUS && next_cpu(cpu, suppress) == cpu + 1)
			++cpu;
		lkdb_printf("-%d", cpu);
	}
	lkdb_printf("\n");

#undef KDB_PCU

	return 0;
}

/*
 * display help for the use of cmd | grep pattern
 */
static int
kdb_grep_help(int argc, const char **argv)
{
	lkdb_printf ("Usage of  cmd args | grep pattern:\n");
	lkdb_printf ("  Any command's output may be filtered through an ");
	lkdb_printf ("emulated 'pipe'.\n");
	lkdb_printf ("  'grep' is just a key word.\n");
	lkdb_printf
	("  The pattern may include a very limited set of metacharacters:\n");
	lkdb_printf ("   pattern or ^pattern or pattern$ or ^pattern$\n");
	lkdb_printf
	("  And if there are spaces in the pattern, you may quote it:\n");
	lkdb_printf
	("   \"pat tern\" or \"^pat tern\" or \"pat tern$\" or \"^pat tern$\"\n");
	return 0;
}
/*
 * display help for the ps and bt status flag
 */
static int
kdb_ps_help(int argc, const char **argv)
{
	lkdb_printf("The meaning of the State flag in ps command output:\n");
	lkdb_printf("  R RUNNING\n");
	lkdb_printf("  D TASK_UNINTERRUPTIBLE\n");
	lkdb_printf("  S TASK_INTERRUPTIBLE\n");
	lkdb_printf("  T TASK_STOPPED\n");
	lkdb_printf("  C TASK_TRACED\n");
	lkdb_printf("  Z EXIT_ZOMBIE\n");
	lkdb_printf("  E EXIT_DEAD\n");
	lkdb_printf("  U UNRUNNABLE\n");
	lkdb_printf("  M sleeping DAEMON\n");
	lkdb_printf("  I IDLE\n");
	lkdb_printf("  (note that most idles are named 'kworker/NN')\n");
	lkdb_printf("\n");
	lkdb_printf(
	"The above can be specified to ps and bta to select tasks\n");
	lkdb_printf("  A all of above\n");
	lkdb_printf("  default is RDSTCZEU  (not Idle or sleeping Daemon)\n");
	return 0;
}

/*
 * lkdb_register_repeat
 *
 *	This function is used to register a kernel debugger command.
 *
 * Inputs:
 *	cmd	Command name
 *	func	Function to execute the command
 *	usage	A simple usage string showing arguments
 *	help	A simple help string describing command
 *	repeat	Does the command auto repeat on enter?
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, one if a duplicate command.
 * Locking:
 *	none.
 * Remarks:
 *
 */

#define kdb_command_extend 50	/* arbitrary */
int
lkdb_register_repeat(char *cmd,
		    lkdb_func_t func,
		    char *usage,
		    char *help,
		    short minlen,
		    lkdb_repeat_t repeat)
{
	int i;
	lkdbtab_t *kp;

	/*
	 *  Brute force method to determine duplicates
	 */
	for (i=0, kp=lkdb_commands; i<kdb_max_commands; i++, kp++) {
		if (kp->cmd_name && (strcmp(kp->cmd_name, cmd)==0)) {
			lkdb_printf("Duplicate kdb command registered: "
				"%s, func %p help %s\n", cmd, func, help);
			return 1;
		}
	}

	/*
	 * Insert command into first available location in table
	 */
	for (i=0, kp=lkdb_commands; i<kdb_max_commands; i++, kp++) {
		if (kp->cmd_name == NULL) {
			break;
		}
	}

	if (i >= kdb_max_commands) {
		lkdbtab_t *new = kmalloc((kdb_max_commands + kdb_command_extend) * sizeof(*new), GFP_KDB);
		if (!new) {
			lkdb_printf("Could not allocate new kdb_command table\n");
			return 1;
		}
		if (lkdb_commands) {
			memcpy(new, lkdb_commands, kdb_max_commands * sizeof(*new));
			kfree(lkdb_commands);
		}
		memset(new + kdb_max_commands, 0, kdb_command_extend * sizeof(*new));
		lkdb_commands = new;
		kp = lkdb_commands + kdb_max_commands;
		kdb_max_commands += kdb_command_extend;
	}

	kp->cmd_name   = cmd;
	kp->cmd_func   = func;
	kp->cmd_usage  = usage;
	kp->cmd_help   = help;
	kp->cmd_flags  = 0;
	kp->cmd_minlen = minlen;
	kp->cmd_repeat = repeat;

	return 0;
}

/*
 * lkdb_register
 *
 *	Compatibility register function for commands that do not need to
 *	specify a repeat state.  Equivalent to lkdb_register_repeat with
 *	LKDB_REPEAT_NONE.
 *
 * Inputs:
 *	cmd	Command name
 *	func	Function to execute the command
 *	usage	A simple usage string showing arguments
 *	help	A simple help string describing command
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, one if a duplicate command.
 * Locking:
 *	none.
 * Remarks:
 *
 */

int
lkdb_register(char *cmd,
	     lkdb_func_t func,
	     char *usage,
	     char *help,
	     short minlen)
{
	return lkdb_register_repeat(cmd, func, usage, help, minlen, LKDB_REPEAT_NONE);
}

/*
 * lkdb_unregister
 *
 *	This function is used to unregister a kernel debugger command.
 *	It is generally called when a module which implements kdb
 *	commands is unloaded.
 *
 * Inputs:
 *	cmd	Command name
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, one command not registered.
 * Locking:
 *	none.
 * Remarks:
 *
 */

int
lkdb_unregister(char *cmd)
{
	int i;
	lkdbtab_t *kp;

	/*
	 *  find the command.
	 */
	for (i=0, kp=lkdb_commands; i<kdb_max_commands; i++, kp++) {
		if (kp->cmd_name && (strcmp(kp->cmd_name, cmd)==0)) {
			kp->cmd_name = NULL;
			return 0;
		}
	}

	/*
	 * Couldn't find it.
	 */
	return 1;
}

/*
 * kdb_inittab
 *
 *	This function is called by the legacy_kdb_init function to initialize
 *	the kdb command table.   It must be called prior to any other
 *	call to lkdb_register_repeat.
 *
 * Inputs:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *
 */

static void __init
kdb_inittab(void)
{
	int i;
	lkdbtab_t *kp;

	for(i=0, kp=lkdb_commands; i < kdb_max_commands; i++,kp++) {
		kp->cmd_name = NULL;
	}

	lkdb_register_repeat("md", kdb_md, "<vaddr>",   "Display Memory Contents, also mdWcN, e.g. md8c1", 1, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("mdr", kdb_md, "<vaddr> <bytes>", 	"Display Raw Memory", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("mdp", kdb_md, "<paddr> <bytes>", 	"Display Physical Memory", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("mds", kdb_md, "<vaddr>", 	"Display Memory Symbolically", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("mm", kdb_mm, "<vaddr> <contents>",   "Modify Memory Contents", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("id", lkdb_id, "<vaddr>",   "Display Instructions", 1, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("go", kdb_go, "[<vaddr>]", "Continue Execution", 1, LKDB_REPEAT_NONE);
	lkdb_register_repeat("rd", kdb_rd, "",		"Display Registers", 1, LKDB_REPEAT_NONE);
	lkdb_register_repeat("rm", kdb_rm, "<reg> <contents>", "Modify Registers", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("ef", kdb_ef, "<vaddr>",   "Display exception frame", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("bt", lkdb_bt, "[<vaddr>]", "Stack traceback", 1, LKDB_REPEAT_NONE);
	lkdb_register_repeat("btp", lkdb_bt, "<pid>", 	"Display stack for process <pid>", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("bta", lkdb_bt, "[DRSTCZEUIMA]", 	"Display stack all processes", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("btc", lkdb_bt, "", 	"Backtrace current process on each cpu", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("btt", lkdb_bt, "<vaddr>", 	"Backtrace process given its struct task address", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("ll", kdb_ll, "<first-element> <linkoffset> <cmd>", "Execute cmd for each element in linked list", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("env", kdb_env, "", 	"Show environment variables", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("set", lkdb_set, "", 	"Set environment variables", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("help", kdb_help, "", 	"Display Help Message", 1, LKDB_REPEAT_NONE);
	lkdb_register_repeat("?", kdb_help, "",         "Display Help Message", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("cpu", kdb_cpu, "<cpunum>","Switch to new cpu", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("ps", kdb_ps, "[<flags>|A]", "Display active task list", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("pshelp", kdb_ps_help, "",
	"Display help for the ps and bt task State flag", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("pid", kdb_pid, "<pidnum>",	"Switch to another task", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("reboot", kdb_reboot, "",  "Reboot the machine immediately", 0, LKDB_REPEAT_NONE);
#if defined(CONFIG_KDB_KDUMP)
	lkdb_register_repeat("kdump", kdb_kdump, "",    "Calls kdump mode", 0, LKDB_REPEAT_NONE);
#endif
#if defined(CONFIG_MODULES)
	lkdb_register_repeat("lsmod", kdb_lsmod, "",	"List loaded kernel modules", 0, LKDB_REPEAT_NONE);
#endif
#if defined(CONFIG_MAGIC_SYSRQ)
	lkdb_register_repeat("sr", kdb_sr, "<key>",	"Magic SysRq key", 0, LKDB_REPEAT_NONE);
#endif
	lkdb_register_repeat("dmesg", kdb_dmesg, "[lines]",	"Display syslog buffer", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("defcmd", kdb_defcmd, "name \"usage\" \"help\"", "Define a set of commands, down to endefcmd", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("kill", kdb_kill, "<-signal> <pid>", "Send a signal to a process", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("summary", kdb_summary, "", "Summarize the system", 4, LKDB_REPEAT_NONE);
	lkdb_register_repeat("per_cpu", kdb_per_cpu, "", "Display per_cpu variables", 3, LKDB_REPEAT_NONE);
	lkdb_register_repeat("grephelp", kdb_grep_help, "",
		"Display help on | grep", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("print", kdb_debuginfo_print, "<expression>",
		"Type casting, as in lcrash",  0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("px", kdb_debuginfo_print, "<expression>",
	   "Print in hex (type casting) (see 'pxhelp')",  0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("pxhelp", kdb_pxhelp, "",
		"Display help for the px command", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("pd", kdb_debuginfo_print, "<expression>",
		"Print in decimal (type casting)", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("whatis", kdb_debuginfo_print,"<type or symbol>",
	"Display the type, or the address for a symbol", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("sizeof", kdb_debuginfo_print, "<type>",
	"Display the size of a structure, typedef, etc.", 0, LKDB_REPEAT_NONE);
        lkdb_register_repeat("walk", kdb_walk, "",
		"Walk a linked list (see 'walkhelp')", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("walkhelp", kdb_walkhelp, "",
		"Display help for the walk command", 0, LKDB_REPEAT_NONE);
}

/*
 * The user has written to our "file"
 * file: the /proc file
 * buffer: user address of the data he is writing
 * count:  number of bytes in the user's buffer
 */
static int
kdb_write_proc_filename(struct file *file, const char __user *buffer,
		unsigned long count, void *data)
{
	int	ret_count;

	/* our buffer is kdb_debug_info_filename[256] */
	if (count > 256) {
		return 0;
	}
	if (copy_from_user(kdb_debug_info_filename, buffer, count)) {
		return 0;
	}
	ret_count = count; /* actual count */
	/* remove any newline from the end of the file name */
	if (kdb_debug_info_filename[count-1] == '\n') count--;
        kdb_debug_info_filename[count] = '\0';

	return ret_count;
}

/*
 * The user is reading from our "file"
 * page:  the beginning of the user's buffer
 * start: pointer to the user's pointer (tells him where we put the data)
 * off:   offset into the resource to be read
 * count: length of the read
 */
static int
kdb_read_proc_filename(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	/* give him kdb_debug_info_filename[]; */
	return snprintf(page, count, "%s\n", kdb_debug_info_filename);
}

/*
 * kdb_proc_filename
 *
 * create /proc/kdb/debug_info_name
 */
static void
kdb_proc_filename(void)
{
	struct proc_dir_entry *kdb_dir_entry, *kdb_file_entry;

	/* create /proc/kdb */
	kdb_dir_entry = proc_mkdir("kdb", NULL);
	if (!kdb_dir_entry) {
		printk ("kdb could not create /proc/kdb\n");
		return;
	}

	/* read/write by owner (root) only */
	kdb_file_entry = create_proc_entry("debug_info_name",
					S_IRUSR | S_IWUSR, kdb_dir_entry);
	if (!kdb_file_entry) {
		printk ("kdb could not create /proc/kdb/kdb_dir_entry\n");
		return;
	}
	kdb_file_entry->nlink = 1;
	kdb_file_entry->data = (void *)NULL;
	kdb_file_entry->read_proc = kdb_read_proc_filename;
	kdb_file_entry->write_proc = kdb_write_proc_filename;
	return;
}

/*
 * kdb_cmd_init
 *
 *	This function is called by the legacy_kdb_init function to execute any
 *	commands defined in lkdb_cmds.
 *
 * Inputs:
 *	Commands in *lkdb_cmds[];
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *
 */

static void __init
kdb_cmd_init(void)
{
	int i, diag;
	for (i = 0; lkdb_cmds[i]; ++i) {
		if (!defcmd_in_progress)
			if (console_loglevel >= 6 /* KERN_INFO */)
				lkdb_printf("kdb_cmd[%d]: %s", i, lkdb_cmds[i]);
		diag = lkdb_parse(lkdb_cmds[i]);
		if (diag)
			lkdb_printf("kdb command %s failed, kdb diag %d\n",
				lkdb_cmds[i], diag);
	}
	if (defcmd_in_progress) {
		lkdb_printf("Incomplete 'defcmd' set, forcing endefcmd\n");
		lkdb_parse("endefcmd");
	}
}

/*
 * kdb_panic
 *
 *	Invoked via the panic_notifier_list.
 *
 * Inputs:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	Zero.
 * Locking:
 *	None.
 * Remarks:
 *	When this function is called from panic(), the other cpus have already
 *	been stopped.
 *
 */

static int
kdb_panic(struct notifier_block *self, unsigned long command, void *ptr)
{
	LKDB_FLAG_SET(CATASTROPHIC);	/* kernel state is dubious now */
	KDB_ENTER();
	return 0;
}

static struct notifier_block kdb_block = { kdb_panic, NULL, 0 };

#ifdef	CONFIG_SYSCTL
static int proc_do_kdb(ctl_table *table, int write, void __user *buffer,
		size_t *lenp, loff_t *ppos)
{
	if (LKDB_FLAG(NO_CONSOLE) && write) {
		printk(KERN_ERR "kdb has no working console and has switched itself off\n");
		return -EINVAL;
	}
	return proc_dointvec(table, write, buffer, lenp, ppos);
}

static struct ctl_table kdb_kern_table[] = {
	{
		.procname	= "kdb",
		.data		= &kdb_on,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_do_kdb,
	},
	{}
};

static struct ctl_table kdb_root_table[] = {
	{
		.procname	= "kernel",
		.mode		= 0555,
		.child		= kdb_kern_table,
	},
	{}
};
#endif	/* CONFIG_SYSCTL */

static int
kdb_cpu_callback(struct notifier_block *nfb, unsigned long action, void *hcpu)
{
	if (action == CPU_ONLINE && kdb_on) {
		int cpu = (unsigned long)hcpu;
		int ret;

		cpumask_var_t save_cpus_allowed;

		if (!zalloc_cpumask_var(&save_cpus_allowed, GFP_KERNEL))
			return NOTIFY_BAD;

		cpumask_copy(save_cpus_allowed, &current->cpus_allowed);
		set_cpus_allowed_ptr(current, cpumask_of(cpu));
		kdb(LKDB_REASON_CPU_UP, 0, NULL); /* do kdb setup on this cpu */
		set_cpus_allowed_ptr(current, save_cpus_allowed);
		free_cpumask_var(save_cpus_allowed);
	}
	return NOTIFY_OK;
}

static struct notifier_block kdb_cpu_nfb = {
	.notifier_call = kdb_cpu_callback
};

/*
 * legacy_kdb_init
 *
 * 	Initialize the kernel debugger environment.
 *
 * Parameters:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *	None.
 */

void __init
legacy_kdb_init(void)
{
	lkdb_initial_cpu = smp_processor_id();
	/*
	 * This must be called before any calls to lkdb_printf.
	 */
	lkdb_io_init();

	kdb_inittab();		/* Initialize Command Table */
	lkdb_initbptab();	/* Initialize Breakpoint Table */
	lkdb_id_init();		/* Initialize Disassembler */
	kdba_init();		/* Architecture Dependent Initialization */

	/*
	 * Use printk() to get message in log_buf[];
	 */
	printk("kdb version %d.%d%s by Keith Owens, Scott Lurndal. "\
	       "Copyright SGI, All Rights Reserved\n",
		LKDB_MAJOR_VERSION, LKDB_MINOR_VERSION, LKDB_TEST_VERSION);

	kdb_cmd_init();		/* Preset commands from lkdb_cmds */
	lkdb_initial_cpu = -1;	/* Avoid recursion problems */
	kdb(LKDB_REASON_CPU_UP, 0, NULL);	/* do kdb setup on boot cpu */
	lkdb_initial_cpu = smp_processor_id();
	atomic_notifier_chain_register(&panic_notifier_list, &kdb_block);
	register_cpu_notifier(&kdb_cpu_nfb);

#ifdef kdba_setjmp
	kdbjmpbuf = vmalloc(NR_CPUS * sizeof(*kdbjmpbuf));
	if (!kdbjmpbuf)
		printk(KERN_ERR "Cannot allocate kdbjmpbuf, no kdb recovery will be possible\n");
#endif	/* kdba_setjmp */

	lkdb_initial_cpu = -1;
	lkdb_wait_for_cpus_secs = 2*num_online_cpus();
	lkdb_wait_for_cpus_secs = max(lkdb_wait_for_cpus_secs, 10);
}

#ifdef	CONFIG_SYSCTL
static int __init
kdb_late_init(void)
{
	register_sysctl_table(kdb_root_table);
	/* seems that we cannot allocate with kmalloc until now */
        kdb_proc_filename();
	return 0;
}

__initcall(kdb_late_init);
#endif

EXPORT_SYMBOL(lkdb_register);
EXPORT_SYMBOL(lkdb_register_repeat);
EXPORT_SYMBOL(lkdb_unregister);
EXPORT_SYMBOL(lkdb_getarea_size);
EXPORT_SYMBOL(lkdb_putarea_size);
EXPORT_SYMBOL(kdb_getuserarea_size);
EXPORT_SYMBOL(kdb_putuserarea_size);
EXPORT_SYMBOL(lkdbgetularg);
EXPORT_SYMBOL(lkdbgetenv);
EXPORT_SYMBOL(lkdbgetintenv);
EXPORT_SYMBOL(lkdbgetaddrarg);
EXPORT_SYMBOL(kdb);
EXPORT_SYMBOL(kdb_on);
EXPORT_SYMBOL(lkdb_seqno);
EXPORT_SYMBOL(lkdb_initial_cpu);
EXPORT_SYMBOL(lkdbnearsym);
EXPORT_SYMBOL(lkdb_printf);
EXPORT_SYMBOL(lkdb_symbol_print);
EXPORT_SYMBOL(lkdb_running_process);
