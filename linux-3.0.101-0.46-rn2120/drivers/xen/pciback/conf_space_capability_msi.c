/*
 * PCI Backend -- Configuration overlay for MSI capability
 */
#include "pciback.h"
#include "conf_space.h"
#include "conf_space_capability.h"
#include <xen/interface/io/pciif.h>

int pciback_enable_msi(struct pciback_device *pdev,
		struct pci_dev *dev, struct xen_pci_op *op)
{
	int status = pci_enable_msi(dev);

	if (status) {
		if (printk_ratelimit())
			pr_err("error enabling MSI for guest %u status %d\n",
			       pdev->xdev->otherend_id, status);
		op->value = 0;
		return XEN_PCI_ERR_op_failed;
	}

	op->value = dev->irq;
	return 0;
}

int pciback_disable_msi(struct pciback_device *pdev,
		struct pci_dev *dev, struct xen_pci_op *op)
{
	pci_disable_msi(dev);

	op->value = dev->irq;
	return 0;
}

int pciback_enable_msix(struct pciback_device *pdev,
		struct pci_dev *dev, struct xen_pci_op *op)
{
	int i, result;
	struct msix_entry *entries;

	if (op->value > SH_INFO_MAX_VEC)
		return -EINVAL;

	entries = kmalloc(op->value * sizeof(*entries), GFP_KERNEL);
	if (entries == NULL)
		return -ENOMEM;

	for (i = 0; i < op->value; i++) {
		entries[i].entry = op->msix_entries[i].entry;
		entries[i].vector = op->msix_entries[i].vector;
	}

	result = pci_enable_msix(dev, entries, op->value);

	for (i = 0; i < op->value; i++) {
		op->msix_entries[i].entry = entries[i].entry;
		op->msix_entries[i].vector = entries[i].vector;
	}

	kfree(entries);

	op->value = result;

	return result > 0 ? 0 : result;
}

int pciback_disable_msix(struct pciback_device *pdev,
		struct pci_dev *dev, struct xen_pci_op *op)
{

	pci_disable_msix(dev);

	op->value = dev->irq;
	return 0;
}

