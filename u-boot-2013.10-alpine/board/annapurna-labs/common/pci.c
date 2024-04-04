 /*
   * board/annapurna-labs/common/pci.c
   * Thie file contains the PCI driver for the Annapurna Labs
   * architecture.
   * Copyright (C) 2012 Annapurna Labs Ltd.
   *
   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 2 of the License, or
   * (at your option) any later version.
   *
   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, write to the Free Software
   * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   */

#include <common.h>
#include <pci.h>
#include <asm/io.h>

#include "al_globals.h"
#include "al_hal_pcie.h"
#include "al_hal_unit_adapter_regs.h"

/* --->>> Constants definitions <<<--- */
#define SNOOP_EN	AL_FALSE
#define INTR_EN		0
#define EQ_EN		1

#define LINK_UP_WAIT	100	/*in ms*/

#define CONFIG_PCIE_0_MEMORY_BUS	AL_PCIE_0_BASE
#define CONFIG_PCIE_0_MEMORY_PHYS	AL_PCIE_0_BASE
#define CONFIG_PCIE_0_MEMORY_SIZE	AL_PCIE_0_SIZE
#define CONFIG_PCIE_0_ECAM_BUS		AL_PCIE_0_ECAM_BASE
#define CONFIG_PCIE_0_ECAM_PHYS		AL_PCIE_0_ECAM_BASE
#define CONFIG_PCIE_0_ECAM_SIZE		AL_PCIE_0_ECAM_SIZE

#define CONFIG_PCIE_1_MEMORY_BUS	AL_PCIE_1_BASE
#define CONFIG_PCIE_1_MEMORY_PHYS	AL_PCIE_1_BASE
#define CONFIG_PCIE_1_MEMORY_SIZE	AL_PCIE_1_SIZE
#define CONFIG_PCIE_1_ECAM_BUS		AL_PCIE_1_ECAM_BASE
#define CONFIG_PCIE_1_ECAM_PHYS		AL_PCIE_1_ECAM_BASE
#define CONFIG_PCIE_1_ECAM_SIZE		AL_PCIE_1_ECAM_SIZE

#define CONFIG_PCIE_2_MEMORY_BUS	AL_PCIE_2_BASE
#define CONFIG_PCIE_2_MEMORY_PHYS	AL_PCIE_2_BASE
#define CONFIG_PCIE_2_MEMORY_SIZE	AL_PCIE_2_SIZE
#define CONFIG_PCIE_2_ECAM_BUS		AL_PCIE_2_ECAM_BASE
#define CONFIG_PCIE_2_ECAM_PHYS		AL_PCIE_2_ECAM_BASE
#define CONFIG_PCIE_2_ECAM_SIZE		AL_PCIE_2_ECAM_SIZE

#define CONFIG_PCIE_INT_MEMORY_BUS	AL_PCIE_INT_BASE
#define CONFIG_PCIE_INT_MEMORY_PHYS	AL_PCIE_INT_BASE
#define CONFIG_PCIE_INT_MEMORY_SIZE	AL_PCIE_INT_SIZE
#define CONFIG_PCIE_INT_ECAM_BUS	AL_PCIE_INT_ECAM_BASE
#define CONFIG_PCIE_INT_ECAM_PHYS	AL_PCIE_INT_ECAM_BASE
#define CONFIG_PCIE_INT_ECAM_SIZE	AL_PCIE_INT_ECAM_SIZE

/* --->>> MACROS <<<--- */
#define AL_PCI_CONFIG_REG_TO_ADDR(Bus, Dev, Func, Reg)	\
		(((Bus) << 20) | ((Dev) << 15) | ((Func) << 12) | (Reg))

/* --->>> Data Structures <<<--- */

/* The ecam address struct is being used for parsing the ecam address parameter
   out of the hose private_data field (see read/write_config functions) */
struct al_ecam_addr {
	uint32_t ecam_addr;
};

/* The following struct is being used to initialize the pci/e devices */
struct al_pcie_init_struct {
	unsigned int			idx;
	struct pci_controller		*hose;
	struct al_ecam_addr		*ecam;
	pci_addr_t			bus_start;
	phys_addr_t			phys_start;
	pci_size_t			size;
	unsigned long			flags;
	struct al_pcie_port		*pcie_port;
	struct al_pcie_config_params	config_params;
	struct al_pcie_link_params	link_params;
	void				*pcie_reg;
};

/* --->>> Static parameters <<<--- */
static void al_pcie_config_device(struct pci_controller *hose,
				pci_dev_t dev,
				struct pci_config_table *entry);


static struct pci_controller	al_pcie_int_hose;
static struct al_ecam_addr	al_int_ecam_addr = {
	.ecam_addr	= CONFIG_PCIE_INT_ECAM_PHYS
};

#ifdef CONFIG_AL_PCIE_0
static struct pci_controller	al_pcie_0_hose;
static struct al_ecam_addr	al_dev0_ecam_addr = {
	.ecam_addr	= CONFIG_PCIE_0_ECAM_PHYS
};
static struct al_pcie_port	pcie_0_port;
#endif
#ifdef CONFIG_AL_PCIE_1
static struct pci_controller	al_pcie_1_hose;
static struct al_ecam_addr	al_dev1_ecam_addr = {
	.ecam_addr	= CONFIG_PCIE_1_ECAM_PHYS
};
static struct al_pcie_port	pcie_1_port;
#endif
#ifdef CONFIG_AL_PCIE_2
static struct pci_controller	al_pcie_2_hose;
static struct al_ecam_addr	al_dev2_ecam_addr = {
	.ecam_addr	= CONFIG_PCIE_2_ECAM_PHYS
};
static struct al_pcie_port	pcie_2_port;
#endif

#if (defined(CONFIG_AL_PCIE_0) || defined(CONFIG_AL_PCIE_1) || defined(CONFIG_AL_PCIE_2))
static struct al_pcie_gen3_lane_eq_params gen3_eq_params[] = {
	{
		.downstream_port_transmitter_preset = 5,
		.downstream_port_receiver_preset_hint = 0,
		.upstream_port_transmitter_preset = 5,
		.upstream_port_receiver_preset_hint = 0,
	},
	{
		.downstream_port_transmitter_preset = 5,
		.downstream_port_receiver_preset_hint = 0,
		.upstream_port_transmitter_preset = 5,
		.upstream_port_receiver_preset_hint = 0,
	},
	{
		.downstream_port_transmitter_preset = 5,
		.downstream_port_receiver_preset_hint = 0,
		.upstream_port_transmitter_preset = 5,
		.upstream_port_receiver_preset_hint = 0,
	},
	{
		.downstream_port_transmitter_preset = 5,
		.downstream_port_receiver_preset_hint = 0,
		.upstream_port_transmitter_preset = 5,
		.upstream_port_receiver_preset_hint = 0,
	},
};

static struct al_pcie_gen3_params gen3_params_set = {
		 .perform_eq = AL_TRUE,
		 .interrupt_enable_on_link_eq_request = AL_FALSE,
		 .eq_params = gen3_eq_params, /* array of lanes params */
		 .eq_params_elements = ARRAY_SIZE(gen3_eq_params), /* number of elements in the eq_params array */
		 .eq_disable = AL_FALSE, /* disables the equalization feature */
		 .eq_phase2_3_disable = AL_FALSE, /* Equalization Phase 2 and Phase 3 */
					/* Disable (RC mode only) */
		 .local_lf = 6, /* Full Swing (FS) Value for Gen3 Transmit Equalization */
				/* Value Range: 12 through 63 (decimal).*/
		 .local_fs = 27, /* Low Frequency (LF) Value for Gen3 Transmit Equalization */
	};
#endif

static struct al_pcie_ep_params ep_params  = {
	.cap_d1_d3hot_dis = AL_TRUE,
	.cap_flr_dis = AL_TRUE,
	.cap_aspm_dis = AL_TRUE,
	.relaxed_pcie_ordering = AL_TRUE,
	.bar_params_valid = AL_FALSE,
};

/* Main data structure for external PCIE devices */
static struct al_pcie_init_struct init_struct_array[] = {
#ifdef CONFIG_AL_PCIE_0
	{
		.idx			= 0,
		.hose			= &al_pcie_0_hose,
		.ecam			= &al_dev0_ecam_addr,
		.bus_start		= CONFIG_PCIE_0_MEMORY_BUS,
		.phys_start		= CONFIG_PCIE_0_MEMORY_PHYS,
		.size			= CONFIG_PCIE_0_MEMORY_SIZE,
		.flags			= PCI_REGION_MEM,
		.pcie_port	= &pcie_0_port,

		.config_params.function_mode = AL_PCIE_FUNCTION_MODE_RC,
		/*.config_params.link_params = &this->link_params,*/
		.config_params.enable_axi_snoop = SNOOP_EN,
		.config_params.enable_ram_parity_int = AL_FALSE,
		.config_params.enable_axi_parity_int = AL_FALSE,
		.config_params.lat_rply_timers = NULL,
		.config_params.gen2_params = NULL,
		.config_params.gen3_params = &gen3_params_set,
		.config_params.tl_credits = NULL,
		.config_params.ep_params = NULL,
		.config_params.features = NULL,
		.config_params.ep_iov_params = NULL,
		.config_params.fast_link_mode = AL_FALSE,
		.config_params.enable_axi_slave_err_resp = AL_FALSE,

		.link_params.enable_reversal = AL_FALSE,

		.pcie_reg	= (void *)(AL_SB_PCIE_BASE(0)),
	},
#endif
#ifdef CONFIG_AL_PCIE_1
	{
		.idx			= 1,
		.hose			= &al_pcie_1_hose,
		.ecam			= &al_dev1_ecam_addr,
		.bus_start		= CONFIG_PCIE_1_MEMORY_BUS,
		.phys_start		= CONFIG_PCIE_1_MEMORY_PHYS,
		.size			= CONFIG_PCIE_1_MEMORY_SIZE,
		.flags			= PCI_REGION_MEM,
		.pcie_port	= &pcie_1_port,

		.config_params.function_mode = AL_PCIE_FUNCTION_MODE_RC,
		/*.config_params.link_params = &this->link_params,*/
		.config_params.enable_axi_snoop = SNOOP_EN,
		.config_params.enable_ram_parity_int = AL_FALSE,
		.config_params.enable_axi_parity_int = AL_FALSE,
		.config_params.lat_rply_timers = NULL,
		.config_params.gen2_params = NULL,
		.config_params.gen3_params = &gen3_params_set,
		.config_params.tl_credits = NULL,
		.config_params.ep_params = NULL,
		.config_params.features = NULL,
		.config_params.ep_iov_params = NULL,
		.config_params.fast_link_mode = AL_FALSE,
		.config_params.enable_axi_slave_err_resp = AL_FALSE,

		.link_params.enable_reversal = AL_FALSE,

		.pcie_reg	= (void *)(AL_SB_PCIE_BASE(1)),
	},
#endif
#ifdef CONFIG_AL_PCIE_2
	{
		.idx			= 2,
		.hose			= &al_pcie_2_hose,
		.ecam			= &al_dev2_ecam_addr,
		.bus_start		= CONFIG_PCIE_2_MEMORY_BUS,
		.phys_start		= CONFIG_PCIE_2_MEMORY_PHYS,
		.size			= CONFIG_PCIE_2_MEMORY_SIZE,
		.flags			= PCI_REGION_MEM,
		.pcie_port	= &pcie_2_port,

		.config_params.function_mode = AL_PCIE_FUNCTION_MODE_RC,
		/*.config_params.link_params = &this->link_params,*/
		.config_params.enable_axi_snoop = SNOOP_EN,
		.config_params.enable_ram_parity_int = AL_FALSE,
		.config_params.enable_axi_parity_int = AL_FALSE,
		.config_params.lat_rply_timers = NULL,
		.config_params.gen2_params = NULL,
		.config_params.gen3_params = &gen3_params_set,
		.config_params.tl_credits = NULL,
		.config_params.ep_params = NULL,
		.config_params.features = NULL,
		.config_params.ep_iov_params = NULL,
		.config_params.fast_link_mode = AL_FALSE,
		.config_params.enable_axi_slave_err_resp = AL_FALSE,

		.link_params.enable_reversal = AL_FALSE,

		.pcie_reg	= (void *)(AL_SB_PCIE_BASE(2)),
	},
#endif
	{ 0 }
};


/* Define memory allocation for PCIe units */
static struct pci_config_table al_pci_config_table[] = {
	{
		.vendor		= PCI_ANY_ID,
		.device		= PCI_ANY_ID,
		.class		= PCI_ANY_ID,
		.bus		= PCI_ANY_ID,
		.dev		= PCI_ANY_ID,
		.func		= PCI_ANY_ID,
		.config_device	= al_pcie_config_device,
	}
};

/* --->>> Static functions <<<--- */

/* The following is very similar to U-Boot's pci_hose_config_device function,
 * except it passes mem variable by pointer instead of by value,
 * so the outer datastructures can be updated according to the function's
 * operations
 */
static int al_pci_hose_config_device(struct pci_controller *hose,
			   pci_dev_t dev,
			   unsigned long io,
			   pci_addr_t *mem,
			   unsigned long command)
{
	unsigned int bar_response, old_command;
	pci_addr_t bar_value;
	pci_size_t bar_size;
	unsigned char pin;
	int bar, found_mem64;

	debug ("PCI Config: I/O=0x%lx, Memory=0x%llx, Command=0x%lx\n",
		io, (u64)*mem, command);

	pci_hose_write_config_dword (hose, dev, PCI_COMMAND, 0);

	for (bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4) {
		pci_hose_write_config_dword (hose, dev, bar, 0xffffffff);
		pci_hose_read_config_dword (hose, dev, bar, &bar_response);

		if (!bar_response)
			continue;

		found_mem64 = 0;

		/* Check the BAR type and set our address mask */
		if (bar_response & PCI_BASE_ADDRESS_SPACE) {
			bar_size = ~(bar_response & PCI_BASE_ADDRESS_IO_MASK) + 1;
			/* round up region base address to a multiple of size */
			io = ((io - 1) | (bar_size - 1)) + 1;
			bar_value = io;
			/* compute new region base address */
			io = io + bar_size;
		} else {
			if ((bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
				PCI_BASE_ADDRESS_MEM_TYPE_64) {
				bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
				found_mem64 = 1;
			} else {
				bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
			}

			/* round up region base address to multiple of size */
			*mem = ((*mem - 1) | (bar_size - 1)) + 1;
			bar_value = *mem;
			/* compute new region base address */
			*mem = *mem + bar_size;
		}

		/* Write it out and update our limit */
		pci_hose_write_config_dword (hose, dev, bar, (u32)bar_value);

		if (found_mem64) {
			bar += 4;
#ifdef CONFIG_SYS_PCI_64BIT
			pci_hose_write_config_dword(hose, dev, bar, (u32)(bar_value>>32));
#else
			pci_hose_write_config_dword (hose, dev, bar, 0x00000000);
#endif
		}
	}

	/* Configure Cache Line Size Register */
	pci_hose_write_config_byte (hose, dev, PCI_CACHE_LINE_SIZE, 0x08);

	/* Configure Latency Timer */
	pci_hose_write_config_byte (hose, dev, PCI_LATENCY_TIMER, 0x80);

	/* Disable interrupt line, if device says it wants to use interrupts */
	pci_hose_read_config_byte (hose, dev, PCI_INTERRUPT_PIN, &pin);
	if (pin != 0) {
		pci_hose_write_config_byte (hose, dev, PCI_INTERRUPT_LINE, 0xff);
	}

	pci_hose_read_config_dword (hose, dev, PCI_COMMAND, &old_command);
	pci_hose_write_config_dword (hose, dev, PCI_COMMAND,
				     (old_command & 0xffff0000) | command);

	return 0;
}

/* The pci config function expects hose->regions[0] to
 * be configured as the pci region.
 */
static void al_pcie_config_device(struct pci_controller *hose,
				pci_dev_t dev,
				struct pci_config_table *entry)
{
	pci_addr_t *pcie_device_start =
			(pci_addr_t *)(&hose->regions[0].bus_lower);

	al_pci_hose_config_device(hose, dev,
				0,			/* IO Address*/
				pcie_device_start,	/* MEM Address */
				entry->priv[2]);	/* Command*/
}

/* The following function maps a given logical bus number into a physical bus
 * number.
 * Our current architecture supports only 1 bus per device. Bus number 0 is
 * reserved to the internal PCI.
 */
static int al_pcie_bus_map(
		struct pci_controller	*hose,
		int			bus_in,
		int			*bus_out)
{
	if (bus_in == 0)
		*bus_out = 0;
	else if ((bus_in - hose->first_busno) == 0)
		*bus_out = 0;
	else {
		al_err("%s: Error: single bus per controller is supported.\n"
				"recevied bus: %d, controller first bus: %d\n",
				__func__, bus_in, hose->first_busno);
		return -EINVAL;
	}

	return 0;
}

/* The following are read/write functions for the PCI config space.
 * These functions are being used as a base for 8/16 bit commands
 * as well, using U-Boot's coversion functions.
 */
static int al_pci_read_config32(struct pci_controller *hose, pci_dev_t dev,
			int offset, uint32_t *val)
{
	int bus, device, function;
	pci_dev_t anpa_dev;
	uint32_t ecam_addr;
	int err = 0;

	/* Hide bus>0, dev>0 */
	if (PCI_BUS(dev) && PCI_DEV(dev)) {
		*val = 0;
		return 0;
	}

	/* Parse the pci_dev_t variable */
	err = al_pcie_bus_map(hose, PCI_BUS(dev), &bus);
	if (err)
		return err;
	device = PCI_DEV(dev);
	function = PCI_FUNC(dev);
	/* Get the ECAM address out of the hose */
	ecam_addr = ((struct al_ecam_addr *) hose->priv_data)->ecam_addr;
	/* Reconfigure the device according to our specification */
	anpa_dev = AL_PCI_CONFIG_REG_TO_ADDR(bus, device, function, offset);
	*val = readl(ecam_addr | anpa_dev);

	debug("%s(%d.%d.%d, %08x) --> %08x\n", __func__, bus, device, function, offset, *val);

	return 0;
}

static int al_pci_write_config32(struct pci_controller *hose, pci_dev_t dev,
			int offset, uint32_t val)
{
	int bus, device, function;
	pci_dev_t anpa_dev;
	uint32_t ecam_addr;
	int err = 0;

	/* Hide bus>0, dev>0 */
	if (PCI_BUS(dev) && PCI_DEV(dev))
		return 0;

	/* Parse the pci_dev_t variable */
	err = al_pcie_bus_map(hose, PCI_BUS(dev), &bus);
	if (err)
		return err;
	device = PCI_DEV(dev);
	function = PCI_FUNC(dev);
	/* Get the ECAM address out of the hose */
	ecam_addr = ((struct al_ecam_addr *) hose->priv_data)->ecam_addr;
	/* Reconfigure the device according to our specification */
	anpa_dev = AL_PCI_CONFIG_REG_TO_ADDR(bus, device, function, offset);

	debug("%s(%d.%d.%d, %08x) <-- %08x\n", __func__, bus, device, function, offset, val);

	writel(val, (ecam_addr | anpa_dev));

	return 0;
}

/* Initializes the given pci controller (hose)
 *
 * Returns the next available bus number
 */
static void al_pci_init_hose(
		struct pci_controller	*hose,
		struct al_ecam_addr	*ecam,
		pci_addr_t		bus_start,
		phys_addr_t		phys_start,
		pci_size_t		size,
		unsigned long		flags,
		unsigned int		first_busno)
{
	hose->config_table = al_pci_config_table;

	hose->first_busno = first_busno;
	hose->last_busno = 0xFF;

	/* Memory space */
	pci_set_region(hose->regions + 0,
		bus_start,
		phys_start,
		size,
		flags);

	hose->regions[0].bus_lower = phys_start;

	hose->region_count = 1;

	/* Define the read/write config address functions */
	pci_set_ops(hose,
		pci_hose_read_config_byte_via_dword,
		pci_hose_read_config_word_via_dword,
		al_pci_read_config32,
		pci_hose_write_config_byte_via_dword,
		pci_hose_write_config_word_via_dword,
		al_pci_write_config32);

	hose->priv_data = ecam;
	debug("Registered PCIe controller. ECAM address:%0x\n",
			ecam->ecam_addr);

	/* Register the hose in the generic pci database */
	pci_register_hose(hose);
}


/* --->>> API functions <<<--- */

static unsigned int first_busno = 0;
static unsigned int last_busno = 0;

void pci_init_board_external(void)
{
	int err = 0, i;

	struct al_pcie_ib_hcrd_os_ob_reads_config ib_hcrd_os_ob_reads_config = {
		.nof_outstanding_ob_reads	= 16,
		.nof_cpl_hdr			= 81,
		.nof_np_hdr			= 8,
		.nof_p_hdr			= 8,
	};

	/* Initialize the PCIE external devices according to configuration */
	for (i = 0 ; i < (ARRAY_SIZE(init_struct_array) - 1) ; ++i) {
		struct al_pcie_port *pcie_port;
		struct al_pcie_link_status link_status;
		int idx;

		BUG_ON(init_struct_array[i].idx > AL_SB_PCIE_NUM);

		pcie_port = init_struct_array[i].pcie_port;
		idx = init_struct_array[i].idx;

		if (!al_globals.pcie_cfg[idx].present)
			continue;

#ifndef CONFIG_EP_INIT_PRINT
		if (al_globals.pcie_cfg[idx].ep)
			continue;
#else
		if (!al_globals.pcie_cfg[idx].ep)
			continue;
#endif

		init_struct_array[i].link_params.max_speed =
			al_globals.pcie_cfg[idx].max_speed;

port_retry:
		err = al_pcie_handle_init(pcie_port,
			init_struct_array[i].pcie_reg,
			(void __iomem *)AL_PBS_REGFILE_BASE,
			idx);
		if (err) {
			printf("%s: PCIE_%d initialization failed\n",
					__func__,
					idx);
			continue;
		}

		err = al_pcie_port_func_mode_config(
			pcie_port,
			al_globals.pcie_cfg[idx].ep ?
			AL_PCIE_FUNCTION_MODE_EP :
			AL_PCIE_FUNCTION_MODE_RC);
		if (err) {
			printf("%s: al_pcie_port_func_mode_config(%d) failed\n",
					__func__,
					idx);
			goto port_disable;
		}

		err = al_pcie_port_max_lanes_set(
			pcie_port,
			al_globals.pcie_cfg[idx].num_lanes);
		if (err) {
			printf("%s: al_pcie_port_max_lanes_set(%d) failed\n",
					__func__,
					idx);
			goto port_disable;
		}

		/*
		 * Inbound header credits and outstanding outbound reads
		 * configuration
		 */
		if ((al_globals.dev_id != AL_CHIP_DEV_ID_ALPINE) ||
			(al_globals.rev_id > 0)) {
			if (al_globals.pcie_cfg[idx].ep)
				al_pcie_port_ib_hcrd_os_ob_reads_config(
					pcie_port,
					&ib_hcrd_os_ob_reads_config);
		}

		/* Release PCIe controller reset */
		al_pcie_port_enable(pcie_port);

		/* Wait more than 2000 clock cycles */
		al_udelay(1000);

		if (al_globals.pcie_cfg[idx].ep) {
			init_struct_array[i].config_params.function_mode =
				AL_PCIE_FUNCTION_MODE_EP;
			init_struct_array[i].config_params.ep_params =
				&ep_params;
		}

		init_struct_array[i].config_params.link_params =
			&init_struct_array[i].link_params;
		err = al_pcie_port_config(pcie_port,
					&init_struct_array[i].config_params);
		if (err) {
			printf("%s: PCIE_%d configuration failed\n",
					__func__,
					idx);
			goto port_disable;
		}

		if (al_globals.pcie_cfg[idx].ep)
			al_pcie_app_req_retry_set(pcie_port, AL_TRUE);

		err = al_pcie_link_start(pcie_port);
		if (err) {
			printf("%s: PCIE_%d link start failed\n",
					__func__,
					idx);
			goto port_disable;
		}

		if (al_globals.pcie_cfg[idx].ep) {
			printf("PCIE_%d: EP ready\n", idx);
			continue;
		}

		/* Notice that encountered a card that expects a reset here */

		err = al_pcie_link_up_wait(pcie_port, LINK_UP_WAIT);

		/*
		 * if no link, try less lanes
		 * if single lane, skip this port
		 */
		if (err) {
			al_pcie_link_stop(pcie_port);

			if (al_globals.pcie_cfg[idx].num_lanes > 1) {
				al_pcie_port_disable(pcie_port);
				al_globals.pcie_cfg[idx].num_lanes /= 2;
				goto port_retry;
			} else if (init_struct_array[i].link_params.max_speed != AL_PCIE_LINK_SPEED_GEN1) {
				init_struct_array[i].link_params.max_speed = AL_PCIE_LINK_SPEED_GEN1;
				goto port_retry;
			}

			printf("%s: PCIE_%d no link found\n",
					__func__,
					idx);
			goto port_disable;
		}

		err = al_pcie_link_status(pcie_port, &link_status);
		if (err) {
			printf("%s: PCIE_%d failed to get link status\n",
					__func__,
					idx);
			goto port_disable;
		}
		/* print PCIe link width and speed */
		printf("PCIE_%d: Link up. Speed %s Width x%d\n", idx,
				link_status.speed == AL_PCIE_LINK_SPEED_GEN1 ? "2.5GT/s" :
				link_status.speed == AL_PCIE_LINK_SPEED_GEN2 ? "5GT/s" :
				link_status.speed == AL_PCIE_LINK_SPEED_GEN3 ? "8GT/s" : "Unknown",
				link_status.lanes);

		al_globals.pcie_cfg[init_struct_array[i].idx].present = 1;
		al_globals.pcie_any_link_up = AL_TRUE;

		/* support only one bus (#1) */
		al_pcie_target_bus_set(pcie_port,
			   0 /*target_bus*/,
			   0xfe /*mask_target_bus*/);

		al_pci_init_hose(init_struct_array[i].hose,
				 init_struct_array[i].ecam,
				 init_struct_array[i].bus_start,
				 init_struct_array[i].phys_start,
				 init_struct_array[i].size,
				 init_struct_array[i].flags,
				 first_busno);
		last_busno = pci_hose_scan(init_struct_array[i].hose);
		init_struct_array[i].hose->last_busno = last_busno;
		first_busno = last_busno + 1;

		continue;

port_disable:
		al_globals.pcie_cfg[init_struct_array[i].idx].present = 0;
		al_pcie_port_disable(pcie_port);
		al_pcie_port_memory_shutdown_set(pcie_port, AL_TRUE);
	}

}

void pci_init_board()
{
	debug("pci_init_board: Entered...\n");

	/* Initialize the internal PCI device's data structure */
	al_pci_init_hose(&al_pcie_int_hose,
					&al_int_ecam_addr,
					CONFIG_PCIE_INT_MEMORY_BUS,
					CONFIG_PCIE_INT_MEMORY_PHYS,
					CONFIG_PCIE_INT_MEMORY_SIZE,
					PCI_REGION_MEM,
					first_busno);

	last_busno = pci_hose_scan(&al_pcie_int_hose);
	al_pcie_int_hose.last_busno = last_busno;
	first_busno = last_busno + 1;

#ifndef CONFIG_CMD_AL_PCI_EXT
	pci_init_board_external();
#endif

	debug("pci_init_board: Finished\n");
}

/* Avoid skipping printing of dev 0, func 0 */
int pci_print_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 1;
}

/* Enable internal PCI adapter snooping */
#ifdef CONFIG_SYS_HW_CACHE_COHERENCY
void pci_internal_snoop_enable(pci_dev_t devno)
{
	uint32_t dword;

	/* enable snoop */
	pci_read_config_dword(devno, AL_ADAPTER_SMCC, &dword);
	dword |= 0x3;
	pci_write_config_dword(devno, AL_ADAPTER_SMCC, dword);

	/* enable awcache, arcache */
	pci_read_config_dword(devno, AL_ADAPTER_GENERIC_CONTROL_11, &dword);
	dword |= 0x3ff;
	pci_write_config_dword(devno, AL_ADAPTER_GENERIC_CONTROL_11, dword);
}
#endif

