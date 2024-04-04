/*
 * kref.h - library routines for handling generic reference counted objects
 *
 * Copyright (C) 2004 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2004 IBM Corp.
 *
 * based on kobject.h which was:
 * Copyright (C) 2002-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (C) 2002-2003 Open Source Development Labs
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef _KREF_H_
#define _KREF_H_

#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>

struct kref {
	atomic_t refcount;
};

void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
int kref_put(struct kref *kref, void (*release) (struct kref *kref));
int kref_sub(struct kref *kref, unsigned int count,
	     void (*release) (struct kref *kref));

/**
 * kref_put_spinlock_irqsave - decrement refcount for object.
 * @kref: object.
 * @release: pointer to the function that will clean up the object when the
 *	     last reference to the object is released.
 *	     This pointer is required, and it is not acceptable to pass kfree
 *	     in as this function.
 * @lock: lock to take in release case
 *
 * Behaves identical to kref_put with one exception.  If the reference count
 * drops to zero, the lock will be taken atomically wrt dropping the reference
 * count.  The release function has to call spin_unlock() without _irqrestore.
 */
static inline int kref_put_spinlock_irqsave(struct kref *kref,
		void (*release)(struct kref *kref),
		spinlock_t *lock)
{
	unsigned long flags;

	WARN_ON(release == NULL);
	if (atomic_add_unless(&kref->refcount, -1, 1))
		return 0;
	spin_lock_irqsave(lock, flags);
	if (atomic_dec_and_test(&kref->refcount)) {
		release(kref);
		local_irq_restore(flags);
		return 1;
	}
	spin_unlock_irqrestore(lock, flags);
	return 0;
}

/**
 * kref_get_unless_zero - Increment refcount for object unless it is zero.
 * @kref: object.
 *
 * Return non-zero if the increment succeeded. Otherwise return 0.
 *
 * This function is intended to simplify locking around refcounting for
 * objects that can be looked up from a lookup structure, and which are
 * removed from that lookup structure in the object destructor.
 * Operations on such objects require at least a read lock around
 * lookup + kref_get, and a write lock around kref_put + remove from lookup
 * structure. Furthermore, RCU implementations become extremely tricky.
 * With a lookup followed by a kref_get_unless_zero *with return value check*
 * locking in the kref_put path can be deferred to the actual removal from
 * the lookup structure and RCU lookups become trivial.
 */
static inline int __must_check kref_get_unless_zero(struct kref *kref)
{
	return atomic_add_unless(&kref->refcount, 1, 0);
}
#endif /* _KREF_H_ */
