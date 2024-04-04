/*
 *  ahci.c - AHCI SATA support
 *
 *  Maintained by:  Tejun Heo <tj@kernel.org>
 *    		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2004-2005 Red Hat, Inc.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * libata documentation is available via 'make {ps|pdf}docs',
 * as Documentation/DocBook/libata.*
 *
 * AHCI hardware documentation:
 * http://www.intel.com/technology/serialata/pdf/rev1_0.pdf
 * http://www.intel.com/technology/serialata/pdf/rev1_1.pdf
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <linux/gfp.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_cmnd.h>
#include <linux/libata.h>
#include "ahci.h"
#include "ahci_alpine.h"

#include <soc/alpine/al_hal_iofic.h>
#include <soc/alpine/al_hal_iofic_regs.h>

#define al_ahci_iofic_base(base)	((base) + 0x2000)

struct alpine_host_priv {
	/* for msix interrupts */
	unsigned int msix_vecs;
	struct msix_entry msix_entries[];
};

void ahci_port_intr(struct ata_port *ap);

int al_init_msix_interrupts(struct pci_dev *pdev, unsigned int n_ports,
				struct ahci_host_priv *hpriv)
{
	int i, rc;
	void __iomem *iofic_base = al_ahci_iofic_base(hpriv->mmio);
	struct alpine_host_priv *al_data;

	hpriv->plat_data = NULL;

	al_data = kzalloc(
		sizeof(unsigned int) + n_ports * sizeof(struct msix_entry), GFP_KERNEL);

	if (!al_data)
		return -ENOMEM;

	al_data->msix_vecs = n_ports;

	for (i = 0; i < n_ports; i++) {
		/* entries 0-2 are in group A */
		al_data->msix_entries[i].entry = 3 + i;
		al_data->msix_entries[i].vector = 0;
	}

	rc = pci_enable_msix_exact(pdev, al_data->msix_entries, n_ports);

	if (rc) {
		dev_info(&pdev->dev,"failed to enable MSIX, vectors %d rc %d\n",
				n_ports, rc);
		kfree(al_data);
		return -EPERM;
	}

	/* we use only group B */
	al_iofic_config(iofic_base, 1 /*GROUP_B*/,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_AUTO_CLEAR |
			INT_CONTROL_GRP_AUTO_MASK |
			INT_CONTROL_GRP_CLEAR_ON_READ);

	al_iofic_moder_res_config(iofic_base, 1, 15);

        al_iofic_unmask(iofic_base, 1, (1 << n_ports) - 1);

	hpriv->plat_data = al_data;

	return n_ports;
}
EXPORT_SYMBOL(al_init_msix_interrupts);

void alpine_clean_cause(struct ata_port *ap_this)
{
	struct ata_host *host = ap_this->host;
	struct ahci_host_priv *hpriv = host->private_data;
	void __iomem *iofic_base = al_ahci_iofic_base(hpriv->mmio);

	/* clean host cause */
	writel(1 << ap_this->port_no, hpriv->mmio + HOST_IRQ_STAT);

	/* unmask the interrupt in the iofic (auto-masked) */
	al_iofic_unmask(iofic_base, 1, 1 << ap_this->port_no);
}
EXPORT_SYMBOL(alpine_clean_cause);

int al_port_irq(struct ata_host *host, int port)
{
	struct ahci_host_priv *hpriv = host->private_data;
	struct alpine_host_priv *al_priv = hpriv->plat_data;

	return al_priv->msix_entries[port].vector;
}
EXPORT_SYMBOL(al_port_irq);
