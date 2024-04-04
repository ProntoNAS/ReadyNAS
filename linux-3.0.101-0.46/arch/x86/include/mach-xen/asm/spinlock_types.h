#ifndef _ASM_X86_SPINLOCK_TYPES_H
#define _ASM_X86_SPINLOCK_TYPES_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

#include <asm/types.h>

typedef union {
	unsigned int slock;
	struct {
#ifdef CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING
/*
 * On Xen we support CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING levels of
 * interrupt re-enabling per IRQ-safe lock. Hence we can have
 * (CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING + 1) times as many outstanding
 * tickets. Thus the cut-off for using byte register pairs must be at
 * a sufficiently smaller number of CPUs.
 */
#if (CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING + 1) * CONFIG_NR_CPUS < 256
# define TICKET_SHIFT 8
		u8 cur, seq;
#else
# define TICKET_SHIFT 16
		u16 cur, seq;
#endif
#if CONFIG_NR_CPUS <= 256
		u8 owner;
#else
		u16 owner;
#endif
#else /* ndef CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING */
/*
 * This differs from the pre-2.6.24 spinlock by always using xchgb
 * rather than decb to take the lock; this allows it to use a
 * zero-initialized lock structure.  It also maintains a 1-byte
 * contention counter, so that we can implement
 * __byte_spin_is_contended.
 */
		u8 lock;
#if CONFIG_NR_CPUS < 256
		u8 spinners;
#else
# error NR_CPUS >= 256 not implemented
#endif
#endif /* def CONFIG_XEN_SPINLOCK_ACQUIRE_NESTING */
	};
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED	{ 0 }

typedef struct {
	unsigned int lock;
} arch_rwlock_t;

#define __ARCH_RW_LOCK_UNLOCKED		{ RW_LOCK_BIAS }

#endif /* _ASM_X86_SPINLOCK_TYPES_H */
