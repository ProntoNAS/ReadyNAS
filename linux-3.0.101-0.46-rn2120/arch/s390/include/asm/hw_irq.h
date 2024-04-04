#ifndef _HW_IRQ_H
#define _HW_IRQ_H

#include <linux/msi.h>
#include <linux/pci.h>

static inline struct msi_desc *irq_get_msi_desc(unsigned int irq)
{
	return __irq_get_msi_desc(irq);
}

/* Must be called with msi map lock held */
static inline int irq_set_msi_desc(unsigned int irq, struct msi_desc *msi)
{
	if (!msi)
		return -EINVAL;

	msi->irq = irq;
	return 0;
}

/* TODO backport
static inline struct msi_desc *irq_data_get_msi(struct irq_data *d)
{
	return NULL;
}

static inline int irq_has_action(unsigned int irq)
{
	return 0;
}
*/
#endif
