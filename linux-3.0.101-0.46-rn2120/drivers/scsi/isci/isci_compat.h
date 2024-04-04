/*
 * isci_compat.h
 *
 * Compability functions for isci on SLES11 SP2
 */
#ifndef _ISCI_COMPAT_H
#define _ISCI_COMPAT_H

#ifndef	BUILD_BUG_ON_NOT_POWER_OF_2
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)
#endif
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(condition)
#endif

#endif /* _ISCI_COMPAT_H */
