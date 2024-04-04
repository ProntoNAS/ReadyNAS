/*
 * Etron USB HOST XHCI Controller
 *
 * Copyright (C) 2014 Etron Technology, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <pci.h>
#include <usb.h>

#include "xhci.h"

const char hcd_name[] = "xhci_hcd-141104";

#define DBG_LEVEL_NOISE		0
#define DBG_LEVEL_TRACE		0
#define DBG_LEVEL_INFO		1
#define DBG_LEVEL_ERROR		1

#define xhci_debug(cond, fmt, args...) \
	debug_cond(cond, "%s: "fmt, hcd_name, ##args)

#ifdef CONFIG_USB_MAX_CONTROLLER_COUNT
#define XHCI_HOST_CONTROLLERS	CONFIG_USB_MAX_CONTROLLER_COUNT
#else
#define XHCI_HOST_CONTROLLERS	1
#endif

struct xhci_hcd {
	pci_dev_t pdev;
	u16 vendor;
	u16 device;

	struct xhci_hccr *hccr;
	struct xhci_hcor *hcor;
	u32 hcc_params1;
};
static struct xhci_hcd etron_hcd[XHCI_HOST_CONTROLLERS];

static struct pci_device_id xhci_pci_ids[] = {
	{0x1b6f, 0x7023},
	{0x1b6f, 0x7052},
	{0, 0}
};

static struct xhci_hcd *get_resource(int index)
{
	struct xhci_hcd *hcd;
	pci_dev_t pdev;
	u16 reg;

	xhci_debug(DBG_LEVEL_TRACE, "=> %s\n", __func__);

	if (index >= XHCI_HOST_CONTROLLERS) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Invalid parameter index %d\n", index);
		return NULL;
	}

	pdev = pci_find_devices(xhci_pci_ids, index);
	if (pdev < 0) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: PCI device not found pdev %d\n", pdev);
		return NULL;
	}

	/* Enable Bus Mastering and memory region */
	pci_read_config_word(pdev, PCI_COMMAND, &reg);
	reg |= (PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config_word(pdev, PCI_COMMAND, reg);

	/* Check if mem accesses and Bus Mastering are enabled. */
	pci_read_config_word(pdev, PCI_COMMAND, &reg);
	if (!(reg & PCI_COMMAND_MASTER) ||
		!(reg & PCI_COMMAND_MEMORY)) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Failed enabling mem access or bus mastering:\n");
		xhci_debug(DBG_LEVEL_ERROR, "** command = 0x%04x\n", reg);
		return NULL;
	}

	hcd = &etron_hcd[index];
	memset(hcd, 0, sizeof(struct xhci_hcd));
	hcd->pdev = pdev;
	hcd->hccr = (struct xhci_hccr *)pci_map_bar(hcd->pdev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	hcd->hcor = (struct xhci_hcor *)((uint32_t) hcd->hccr + HC_LENGTH(xhci_readl(&hcd->hccr->cr_capbase)));

	xhci_debug(DBG_LEVEL_TRACE, "** hccr 0x%x and hcor 0x%x hc_length %d\n",
			(uint32_t)hcd->hccr, (uint32_t)hcd->hcor,
			(uint32_t)HC_LENGTH(xhci_readl(&hcd->hccr->cr_capbase)));

	pci_read_config_word(hcd->pdev, PCI_VENDOR_ID, &hcd->vendor);
	pci_read_config_word(hcd->pdev, PCI_DEVICE_ID, &hcd->device);
	pci_read_config_dword(hcd->pdev, 0x58, &hcd->hcc_params1);
	hcd->hcc_params1 &= 0xffff;

	if ((0xffff == hcd->vendor) ||
		(0xffff == hcd->device) ||
		(0 == hcd->vendor) ||
		(0 == hcd->device)) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Invalid PCI Information:\n");
		xhci_debug(DBG_LEVEL_ERROR, "** vendor = 0x%04x\n", hcd->vendor);
		xhci_debug(DBG_LEVEL_ERROR, "** device = 0x%04x\n", hcd->device);
		return NULL;
	}

	if ((0xffffffff == xhci_readl(&hcd->hccr->cr_capbase)) ||
		(0xffffffff == xhci_readl(&hcd->hccr->cr_rtsoff)) ||
		(0 == xhci_readl(&hcd->hccr->cr_capbase)) ||
		(0 == xhci_readl(&hcd->hccr->cr_rtsoff))) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Invalid xHCI Registers:\n");
		xhci_debug(DBG_LEVEL_ERROR, "** cr_capbase = 0x%08x\n", xhci_readl(&hcd->hccr->cr_capbase));
		xhci_debug(DBG_LEVEL_ERROR, "** cr_rtsoff  = 0x%08x\n", xhci_readl(&hcd->hccr->cr_rtsoff));
		return NULL;
	}

	if ((HC_VERSION(xhci_readl(&hcd->hccr->cr_capbase)) < 0x95) ||
		(0 == HCS_MAX_SLOTS(xhci_readl(&hcd->hccr->cr_hcsparams1))) ||
		(0 == HCS_MAX_INTRS(xhci_readl(&hcd->hccr->cr_hcsparams1))) ||
		(0 == HCS_MAX_PORTS(xhci_readl(&hcd->hccr->cr_hcsparams1)))) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Invalid xHCI Registers:\n");
		xhci_debug(DBG_LEVEL_ERROR, "** cr_capbase    = 0x%08x\n", xhci_readl(&hcd->hccr->cr_capbase));
		xhci_debug(DBG_LEVEL_ERROR, "** cr_hcsparams1 = 0x%08x\n", xhci_readl(&hcd->hccr->cr_hcsparams1));
		return NULL;
	}

	xhci_debug(DBG_LEVEL_TRACE, "** xHCI Capability Registers:\n");
	xhci_debug(DBG_LEVEL_TRACE, "** cr_capbase    = 0x%08x\n", xhci_readl(&hcd->hccr->cr_capbase));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_hcsparams1 = 0x%08x\n", xhci_readl(&hcd->hccr->cr_hcsparams1));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_hcsparams2 = 0x%08x\n", xhci_readl(&hcd->hccr->cr_hcsparams2));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_hcsparams3 = 0x%08x\n", xhci_readl(&hcd->hccr->cr_hcsparams3));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_hccparams  = 0x%08x\n", xhci_readl(&hcd->hccr->cr_hccparams));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_dboff      = 0x%08x\n", xhci_readl(&hcd->hccr->cr_dboff));
	xhci_debug(DBG_LEVEL_TRACE, "** cr_rtsoff     = 0x%08x\n", xhci_readl(&hcd->hccr->cr_rtsoff));

	return hcd;
}

/*
 * Create the appropriate control structures to manage
 * a new xHCI host controller.
 */
int xhci_hcd_init(int index, struct xhci_hccr **ret_hccr, struct xhci_hcor **ret_hcor)
{
	struct xhci_hcd *hcd;

	xhci_debug(DBG_LEVEL_TRACE, "=> %s\n", __func__);

	hcd = get_resource(index);
	if (NULL == hcd) {
		xhci_debug(DBG_LEVEL_ERROR, "** Error: Host controller not found\n");
		return -ENODEV;
	}

	*ret_hccr = hcd->hccr;
	*ret_hcor = hcd->hcor;

	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the xHCI host controller.
 */
void xhci_hcd_stop(int index)
{
	xhci_debug(DBG_LEVEL_TRACE, "=> %s\n", __func__);
}
