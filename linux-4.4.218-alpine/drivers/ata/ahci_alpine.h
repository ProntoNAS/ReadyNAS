#ifndef _AHCI_ALPINE_H
#define _AHCI_ALPINE_H

#ifdef CONFIG_AHCI_ALPINE

extern int al_init_msix_interrupts(struct pci_dev *pdev,
		unsigned int n_ports, struct ahci_host_priv *hpriv);

extern void alpine_clean_cause(struct ata_port *ap_this);

extern int al_port_irq(struct ata_host *host, int port);
#else

static inline int al_init_msix_interrupts(struct pci_dev *pdev,
	unsigned int n_ports, struct ahci_host_priv *hpriv)
{ return 0; }

static inline void alpine_clean_cause(struct ata_port *ap_this)
{};

static inline int al_port_irq(struct ata_host *host, int port)
{ return -EINVAL; };

#endif

#endif /* _AHCI_ALPINE_H */
