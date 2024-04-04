#ifndef __XEN_BARRIER_H__
#define __XEN_BARRIER_H__

#include <asm/system.h>

#define xen_mb()  mb()
#define xen_rmb() rmb()
#define xen_wmb() wmb()

#endif /* __XEN_BARRIER_H__ */
