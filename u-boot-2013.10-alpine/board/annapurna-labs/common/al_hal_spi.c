/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 or V3 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/**
 * @defgroup group_spi	SPI
 *  @{
 *
 * @file   al_hal_spi.h
 * @brief Header file for the SPI HAL driver
 *
 */

#include "al_hal_spi.h"
#include "al_hal_spi_regs.h"

/* SPI service functions */
/*
 * Check if the SPI device is currently enabled
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 * @return	int
 *		1 if enabled
 *		0 otherwise
 */
static inline int al_spi_is_enabled(struct al_spi_interface *spi_if)
{
	return al_reg_read16(&spi_if->regs->ssienr);
}

/*
 * Enable the SPI device
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 */
static inline void al_spi_enable(struct al_spi_interface *spi_if)
{
	al_reg_write16(&spi_if->regs->ssienr,
				AL_SPI_SSIENR_ENABLE);
}

/*
 * Disable the SPI device
 *
 * Note that disabling the device during ongoing transaction will result with
 * the transaction being terminated (all tx configuration will be lost)
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 */
static inline void al_spi_disable(struct al_spi_interface *spi_if)
{
	al_reg_write16(&spi_if->regs->ssienr,
				AL_SPI_SSIENR_DISABLE);
}

/*
 * waits until reg&mask==val.
 * returns 0 if function succeeds, -ETIME if timer expired.
 */
static int al_spi_wait_on_sr(
	struct al_spi_regs __iomem	*regs,
	uint16_t			mask,
	uint16_t			val,
	uint32_t			timeout)
{
	void __iomem *reg = &regs->sr;
	uint32_t watchdog = 1;
	uint16_t reg_val;

	while (1) {
		reg_val = al_reg_read16(reg);

		if (reg_val & AL_SPI_SR_RFF_FULL) {
			al_err("%s: RX fifo full!\n", __func__);
			return -ETIME;
		}

		if (val == (reg_val & mask))
			break;

		if (watchdog < timeout) {
			al_udelay(1); /* wait fot 1us */
			++watchdog;
		} else {
			al_err("%s: Reading %p encountered timeout\n",
					__func__, reg);
			al_err("%s: val=%08X ,mask=%08X ,timeout=%08X\n"
					, __func__, val, mask, timeout);
			return -ETIME;
		}
	}

	return 0;
}

static inline int al_spi_wait_for_rx_fifo_not_empty(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_RFNE_MASK,
		AL_SPI_SR_RFNE_NOT_EMPTY,
		timeout);
}

static inline int al_spi_wait_for_tx_fifo_not_full(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_TFNF_MASK,
		AL_SPI_SR_TFNF_NOT_FULL,
		timeout);
}

static inline int al_spi_wait_for_tx_fifo_empty(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_TFE_MASK,
		AL_SPI_SR_TFE_EMPTY,
		timeout);
}

static inline int al_spi_wait_for_sr_idle(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_BUSY_MASK,
		AL_SPI_SR_BUSY_IDLE,
		timeout);
}

static void al_spi_tmod_set_aux(struct al_spi_interface *spi_if, unsigned int tmod)
{
	uint16_t ctrl0_reg_val;

	ctrl0_reg_val = al_reg_read16(&spi_if->regs->ctrlr0);
	ctrl0_reg_val = (ctrl0_reg_val & ~AL_SPI_CTRLR0_TMOD_MASK) |
			(tmod & AL_SPI_CTRLR0_TMOD_MASK);
	al_reg_write16(&spi_if->regs->ctrlr0, ctrl0_reg_val);
}

/*
 * Set the baud rate of the SPI device
 *
 * The baud rate determines the SPI device's clk frequency, according to this
 * formula: spi_clk = input_clk / baud_rate
 * Note that the baud rate must be even, so if the divisor is odd, we round it
 * up to an even number (round up, so the spi frequency would be lower than
 * requested - rather than higher)
 * SPI device must be disabled when changing the baud rate
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 * @param [in]	spi_freq
 *		The device frequency we'd like to have, measured in Hz
 */
static void al_spi_set_baudr(struct al_spi_interface *spi_if, uint32_t spi_freq)
{
	uint32_t clk_divider;

	clk_divider = spi_if->input_clk_freq % spi_freq == 0 ?
		spi_if->input_clk_freq / spi_freq :
		(spi_if->input_clk_freq / spi_freq) + 1;
	/* if the clk_divider is odd, even it up */
	if ((clk_divider & 0x1) != 0x0)
		clk_divider += 0x1;

	al_reg_write16(&spi_if->regs->baudr, clk_divider);
}

/* SPI API functions */

void al_spi_init(struct al_spi_interface *spi_if,
		void __iomem *spi_regs_base,
		uint32_t input_clk_freq)
{
	al_dbg("%s: Entered..", __func__);
	/* Initialize the internal database */
	spi_if->regs = (struct al_spi_regs __iomem *)spi_regs_base;
	spi_if->input_clk_freq = input_clk_freq;
	spi_if->curr_bus_owner = AL_SPI_CS_NONE;

	/* HW initialization */
	al_spi_disable(spi_if);

	/*
	 * Define the basic properties of our SPI device:
	 *   TMOD	the transfer mode of the device (4 modes, according to
	 *		the ssi protocol)
	 *   SCPOL	clock polarity, determines whether the inactive state
	 *		of the serial clock is high or low
	 *   SCPH	clock phase, determines whether the serial transfer
	 *		begins with the falling edge of the slave select signal,
	 *		or the first edge of the serial clock
	 *   FRF	defines clock phase available directly after reset:
	 *		0- SPI serial data captured on the first clock edge
	 *		1- SPI serial data captured on the second clock edge
	 *   DFS	data frame size
	 */
	al_reg_write16(&spi_if->regs->ctrlr0,
			AL_SPI_CTRLR0_TMOD_EEPROM
			| AL_SPI_CTRLR0_SCPOL_LOW
			| AL_SPI_CTRLR0_SCPH_SS
			| AL_SPI_CTRLR0_FRF_MOTOROLA
			| AL_SPI_CTRLR0_DFS_8_BITS);
}

int al_spi_claim_bus(struct al_spi_interface *spi_if,
			uint32_t freq,
			enum al_spi_phase_t phase,
			enum al_spi_polarity_t polarity,
			uint32_t cs)
{
	int ret = 0;
	uint16_t ctrl0_reg_val;

	if (spi_if->curr_bus_owner != AL_SPI_CS_NONE) {
		al_err("%s: bus is not free.\n", __func__);
		return -EINVAL;
	}

	/* Disable the device, in order to configure the necessary registers */
	al_spi_disable(spi_if);

	/* Config CTRLR0 according to input parameters */
	ctrl0_reg_val = al_reg_read16(&spi_if->regs->ctrlr0);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPOL_MASK;
	ctrl0_reg_val |= (polarity<<AL_SPI_CTRLR0_SCPOL_SHIFT);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPH_MASK;
	ctrl0_reg_val |= (phase<<AL_SPI_CTRLR0_SCPH_SHIFT);

	al_reg_write16(&spi_if->regs->ctrlr0, ctrl0_reg_val);

	/* Config BAUD RATE according to input parameters */
	al_spi_set_baudr(spi_if, freq);

	spi_if->curr_bus_owner = SPI_CS_INDEX_TO_VAL(cs);

	return ret;
}

void al_spi_release_bus(struct al_spi_interface *spi_if, uint32_t cs_index)
{
	if (spi_if->curr_bus_owner != SPI_CS_INDEX_TO_VAL(cs_index)) {
		al_err("%s: Failed. Input slave isn't the current bus holder\n",
			__func__);
		al_err("\tCurrent bus holder = 0x%04X, slave CS = 0x%04X\n",
			spi_if->curr_bus_owner, SPI_CS_INDEX_TO_VAL(cs_index));
		return;
	}

	al_spi_disable(spi_if);
	spi_if->curr_bus_owner = AL_SPI_CS_NONE;
}

void al_spi_tmod_set(struct al_spi_interface *spi_if, enum al_spi_tmod_t tmod)
{
	unsigned int tmod_val;

	switch(tmod) {
	case AL_SPI_TMOD_TRANS_RECV:
		tmod_val = AL_SPI_CTRLR0_TMOD_TRANS_RECV;
		break;
	case AL_SPI_TMOD_TRANS:
		tmod_val = AL_SPI_CTRLR0_TMOD_TRANS;
		break;
	case AL_SPI_TMOD_RECV:
		tmod_val = AL_SPI_CTRLR0_TMOD_RECV;
		break;
	case AL_SPI_TMOD_EEPROM:
		tmod_val = AL_SPI_CTRLR0_TMOD_EEPROM;
		break;
	default:
		al_err("%s: received an invalid tmod value: %x\n", __func__, tmod);
		return;
	}

	al_spi_tmod_set_aux(spi_if, tmod_val);
}

int al_spi_read(struct al_spi_interface *spi_if,
		uint8_t const *cmd,
		uint8_t cmd_len,
		uint8_t *din,
		uint32_t bytelen,
		uint32_t cs,
		uint32_t timeout)
{
	int ret = 0;
	int cmd_idx = 0;
	int rx_avail;
	/*
	 * The DW SPI core requires 'dummy' data to be inserted into the TX
	 * FIFO, in order to perform the read procedure. The data will be
	 * ignored during the read command, as its only purpose is enabling
	 * the interface to perform read transaction.
	 */
	int dummy_tx = bytelen;

	al_dbg("%s: Entered with params:\n\t", __func__);
	al_dbg("cmd[0]=0x%02X | cmd_len=0x%02X | bytelen=0x%08X\n",
			cmd[0], cmd_len, bytelen);

	al_spi_disable(spi_if);

	/* Set SPI mode */
	al_spi_tmod_set_aux(spi_if, AL_SPI_CTRLR0_TMOD_TRANS_RECV);

	/* -- Command Phase -- */

	/*
	 * Enable the SPI interface, and disable the SER:
	 * once the FIFO is empty, the transfer is terminated - therefore we
	 * must make sure that the fifo isn't empty during the transfer
	 */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_enable(spi_if);

	for (cmd_idx = 0 ; cmd_idx < cmd_len ; ++cmd_idx) {
		/* Check that the transmit FIFO isn't full per each byte tx */
		ret = al_spi_wait_for_tx_fifo_not_full(spi_if, timeout);
		if (ret != 0) {
			al_spi_disable(spi_if);
			return ret;
		}
		/* FIFO not full -> write command byte*/
		al_reg_write16(&spi_if->regs->dr[0], cmd[cmd_idx]);
	}

	/* Fill the TX FIFO as much as possible before selecting the slave */
	while (dummy_tx) {
		if (((al_reg_read16(&spi_if->regs->sr)&AL_SPI_SR_TFNF_MASK)
			== AL_SPI_SR_TFNF_FULL))
			break;

		al_reg_write16(&spi_if->regs->dr[0], 0);
		dummy_tx--;
	}

	/* -- Read Data Phase -- */

	al_reg_write16(&spi_if->regs->ser, SPI_CS_INDEX_TO_VAL(cs));

	while (bytelen > 0) {
		if (dummy_tx && ((al_reg_read16(&spi_if->regs->sr) &
			AL_SPI_SR_TFNF_MASK) == AL_SPI_SR_TFNF_NOT_FULL)) {
			al_reg_write16(&spi_if->regs->dr[0], 0);
			dummy_tx--;
		}

		rx_avail = al_reg_read16(&spi_if->regs->rxflr);

		while (rx_avail) {
			*din = al_reg_read16(&spi_if->regs->dr[0]);

			if (!cmd_len) {
				din++;
				bytelen--;
			} else if (cmd_len) {
				cmd_len--;
			}

			rx_avail--;
		}
	}

	/* Wait until SPI transaction is done */
	ret = al_spi_wait_for_sr_idle(spi_if, timeout);

	/* Disable the SPI interface */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_disable(spi_if);

	return ret;
}

int al_spi_write(struct al_spi_interface *spi_if,
		uint8_t const *cmd,
		uint8_t cmd_len,
		uint8_t const *dout,
		uint32_t bytelen,
		uint32_t cs,
		uint32_t timeout)
{
	int ret = 0;
	int ser_set = 0, cmd_idx = 0;

	al_dbg("%s: Entered with params:\n\t", __func__);
	al_dbg("cmd[0]=0x%02X | bytelen=0x%08X | cs = 0x%04X\n",
			cmd[0], bytelen, cs);

	al_spi_disable(spi_if);

	/* Set SPI mode */
	al_spi_tmod_set_aux(spi_if, AL_SPI_CTRLR0_TMOD_TRANS);

	/* -- Command Phase -- */

	/*
	 * Enable the SPI interface, and disable the SER:
	 * once the FIFO is empty, the transfer is terminated - therefore we
	 * must make sure that the fifo isn't empty during the transfer
	 */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_enable(spi_if);

	for (cmd_idx = 0 ; cmd_idx < cmd_len ; ++cmd_idx) {
		/* Check that the transmit FIFO isn't full per each byte tx */
		ret = al_spi_wait_for_tx_fifo_not_full(spi_if, timeout);
		if (ret != 0) {
			al_spi_disable(spi_if);
			return ret;
		}
		/* FIFO not full -> write command byte*/
		al_reg_write16(&spi_if->regs->dr[0], cmd[cmd_idx]);
	}

	/* -- Write Data Phase -- */

	while (bytelen > 0) {
		/* If the fifo is full, set the Slave Enable Register */
		if (ser_set == 0 &&
			((al_reg_read16(&spi_if->regs->sr)&AL_SPI_SR_TFNF_MASK)
				== AL_SPI_SR_TFNF_FULL)) {
			al_reg_write16(&spi_if->regs->ser,
					SPI_CS_INDEX_TO_VAL(cs));
			ser_set = 1;
		}

		/* Check that the transmit FIFO isn't full per each byte tx */
		ret = al_spi_wait_for_tx_fifo_not_full(spi_if, timeout);
		if (ret != 0) {
			al_spi_disable(spi_if);
			return ret;
		}
		/* FIFO not full -> write Data Register */
		al_reg_write16(&spi_if->regs->dr[0], *dout);

		dout++;
		bytelen--;
	}

	/*
	 * If the ser register wasn't set during data writing phase -
	 *  set it, so the data transfer will begin
	 */
	if (ser_set == 0)
		al_reg_write16(&spi_if->regs->ser, SPI_CS_INDEX_TO_VAL(cs));

	/* Wait until SPI transaction is done */
	ret = al_spi_wait_for_tx_fifo_empty(spi_if, timeout);
	if (ret) {
		al_err("%s: al_spi_wait_for_tx_fifo_empty failed!\n", __func__);
		return ret;
	}

	ret = al_spi_wait_for_sr_idle(spi_if, timeout);
	if (ret) {
		al_err("%s: al_spi_wait_for_sr_idle failed!\n", __func__);
		return ret;
	}

	/* Disable the SPI interface */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_disable(spi_if);


	return ret;
}

unsigned int al_spi_raw_intr_get(
		struct al_spi_interface *spi_if)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;

	return al_reg_read16(&regs->risr);
}

unsigned int al_spi_masked_intr_get(
		struct al_spi_interface *spi_if)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;

	return al_reg_read16(&regs->isr);
}

void al_spi_intr_enable(
		struct al_spi_interface *spi_if,
		unsigned int enable_val)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;
	uint16_t curr_val;

	curr_val = al_reg_read16(&regs->imr);

	al_reg_write16(&regs->imr, curr_val | enable_val);
}

void al_spi_intr_disable(
		struct al_spi_interface *spi_if,
		unsigned int disable_val)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;
	uint16_t curr_val;

	curr_val = al_reg_read16(&regs->imr);

	al_reg_write16(&regs->imr, curr_val & ~disable_val);
}

void al_spi_intr_clear(
		struct al_spi_interface *spi_if,
		unsigned int clear_val)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;

	al_assert(spi_if);

	/* SW can't clear 'tx empty' and 'rx full' interrupts */
	al_assert((clear_val & AL_SPI_INTR_TXEIM) == 0);
	al_assert((clear_val & AL_SPI_INTR_RXFIM) == 0);

	if (clear_val & AL_SPI_INTR_TXOIM)
		al_reg_read16(&regs->txoicr);
	if (clear_val & AL_SPI_INTR_RXUIM)
		al_reg_read16(&regs->rxuicr);
	if (clear_val & AL_SPI_INTR_RXOIM)
		al_reg_read16(&regs->rxoicr);
	if (clear_val & AL_SPI_INTR_MSTIM)
		al_reg_read16(&regs->msticr);
}

void al_spi_cs_override_set(
		void __iomem		*spi_regs_base)
{
	struct al_spi_regs *regs = (struct al_spi_regs __iomem *)spi_regs_base;

	al_reg_write16(&regs->ssi_ovr,
			AL_SPI_SSI_OVR_CS_0
			| AL_SPI_SSI_OVR_CS_1
			| AL_SPI_SSI_OVR_CS_2
			| AL_SPI_SSI_OVR_CS_3);
}

/** @} end of SPI group */

