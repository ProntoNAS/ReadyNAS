/*
 */

#include <common.h>

#ifdef CONFIG_DISPLAY_CPUINFO
/* Print CPU information */
int print_cpuinfo(void)
{
	puts("Annapurna Labs\n");

	/* TBD: Add printf of major/minor rev info, stepping, etc. */
	return 0;
}
#endif	/* CONFIG_DISPLAY_CPUINFO */
