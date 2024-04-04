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
 * @defgroup group_spi SPI
 * @ingroup group_pbs
 *  @{
 *
 * @file   al_hal_spi.h
 * @brief Header file for the SPI HAL driver
 *
 */

#ifndef _AL_HAL_SPI_H_
#define _AL_HAL_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

#include "al_hal_common.h"


struct al_spi_interface {
	struct al_spi_regs __iomem *regs;
	uint32_t input_clk_freq;
	/* bus owner will be saved as value, not index (e.g. owner#2 = 0x4) */
	uint16_t curr_bus_owner;
};

/*- Following enum values should NOT be changed, as they are used internally -*/

/*
 * Serial Clock Phase, used to select the relationship of the serial clock with
 * the slave select signal.
 * When SCPH = 0, data is captured on the first edge of the serial clock.
 * When SCPH = 1, the serial clock starts toggling one cycle after the slave
 * select line is activated, and data is captured on the second edge of the
 * serial clock.
 */
enum al_spi_phase_t {
	AL_SPI_PHASE_SLAVE_SELECT	= 0,
	AL_SPI_PHASE_CLOCK		= 1
};

/*
 * Serial Clock Polarity, used to select the polarity of the inactive serial
 * clock, which is held inactive when the DW_apb_ssi master is not actively
 * transferring data on the serial bus.
 */
enum al_spi_polarity_t {
	AL_SPI_POLARITY_INACTIVE_LOW	= 0,
	AL_SPI_POLARITY_INACTIVE_HIGH	= 1
};

enum al_spi_tmod_t {
	AL_SPI_TMOD_TRANS_RECV	= 0,
	AL_SPI_TMOD_TRANS	= 1,
	AL_SPI_TMOD_RECV	= 2,
	AL_SPI_TMOD_EEPROM	= 3
};

enum al_spi_intr_t {
	AL_SPI_INTR_TXEIM = (1 << 0),	/* transmit FIFO empty */
	AL_SPI_INTR_TXOIM = (1 << 1),	/* transmit FIFO overflow */
	AL_SPI_INTR_RXUIM = (1 << 2),	/* receive FIFO underflow */
	AL_SPI_INTR_RXOIM = (1 << 3),	/* receive FIFO overflow */
	AL_SPI_INTR_RXFIM = (1 << 4),	/* receive FIFO full */
	AL_SPI_INTR_MSTIM = (1 << 5),	/* multi-master contention */
};

/**
 * Initialize the SPI controller
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 * @param	input_clk_freq
 *		The SPI controller reference clock frequency, measured in Hz.
 *		will be used in order to set the SPI device clock rate
 *		(using the baudr register)
 */
void al_spi_init(
		struct al_spi_interface	*spi_if,
		void __iomem		*spi_regs_base,
		uint32_t		input_clk_freq);


/**
 * Claiming the bus for a specific slave
 *
 * This API function will config the polarity and phase bits of the SPI device
 * according to the claiming slave's configuration.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	freq
 *		The frequency of the current bus claimer, measured in Hz
 * @param	phase
 *		Determines whether the serial transfer begins with the falling
 *		edge of the slave select signal, or the first edge of the serial
 *		clock
 * @param	polarity
 *		Determines whether the inactive state of the serial clock
 *		is high or low
 * @param	cs
 *		The bus claimer's chip select index
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_claim_bus(
		struct al_spi_interface	*spi_if,
		uint32_t		freq,
		enum al_spi_phase_t	phase,
		enum al_spi_polarity_t	polarity,
		uint32_t		cs);

/**
 * Disable the SPI device
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cs_index
 *		The CS index who is supposed to be the current bus claimer
 */
void al_spi_release_bus(
		struct al_spi_interface	*spi_if,
		uint32_t		cs_index);


/**
 * Perform an SPI read transaction
 *
 * Note that the transaction is separated to two phases:
 *	1) Command phase: configuring the parameters for the spi transaction,
 *	then writing the command bytes to the transmit fifo
 *	2) Read data phase: reading the actual data bytes from the receive fifo
 * Note that once you begin writing the command bytes to the fifo, the
 * SSI Enable Register (SSIENR) should NOT be disabled, nor should the Slave
 * Enable Register (SER) - deasserting either one of these registers will
 * terminate the transaction.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cmd
 *		The buffer that holds the command data:
 *		command = {opcode,addrMSB,addr,addrLSB}
 * @param	cmd_len
 *		The length of the command
 * @param	din
 *		The buffer that accepts the incoming data
 * @param	bytelen
 *		The length of the read transaction, measured in bytes
 * @param	cs
 *		The bus claimer's chip select index
 * @param	timeout
 *		how much time should the HAL wait before declaring a timeout,
 *		measured in usec
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_read(
		struct al_spi_interface	*spi_if,
		uint8_t const		*cmd,
		uint8_t			cmd_len,
		uint8_t			*din,
		uint32_t		bytelen,
		uint32_t		cs,
		uint32_t		timeout);


/**
 * Perform an SPI write transaction
 *
 * Note that the transaction is separated to two phases:
 *	1) Command phase: configuring the parameters for the spi transaction,
 *	then writing the command bytes to the transmit fifo
 *	2) Write data phase: writing the actual data bytes to the transmit fifo
 * Note that once you begin writing the command bytes to the fifo, the
 * SSI Enable Register (SSIENR) should NOT be disabled, nor should the Slave
 * Enable Register (SER) - deasserting either one of these registers will
 * terminate the transaction.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cmd
 *		The buffer that holds the command data:
 *		command = {opcode,addrMSB,addr,addrLSB}
 * @param	cmd_len
 *		The length of the command
 * @param	dout
 *		The buffer that holds the outgoing data
 * @param	bytelen
 *		The length of the write data, measured in bytes
 * @param	cs
 *		The slave we are writing into, use al_spi_regs definitions
 * @param	timeout
 *		how much time should the HAL wait before declaring a timeout,
 *		measured in usec
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_write(
		struct al_spi_interface	*spi_if,
		uint8_t const		*cmd,
		uint8_t			cmd_len,
		uint8_t const		*dout,
		uint32_t		bytelen,
		uint32_t		cs,
		uint32_t		timeout);

/**
 * Get the RAW interrupt state
 *
 * Raw interrupt state is the value of the Interrupt State regardless of the
 * interrupt mask.
 *
 * Use the SPI interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 *
 * @returns	RAW interrupt register value
 */
unsigned int al_spi_raw_intr_get(
		struct al_spi_interface	*spi_if);

/**
 * Get the MASKED interrupt state
 *
 * Masked interrupt state is the value of the Interrupt State considering the
 * interrupt mask: if the interrupt's corresponding mask bit is set, it will
 * be masked.
 *
 * Use the SPI interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	spi_if
 *		The spi instance we are working with
 *
 * @returns	Interrupt register value
 */
unsigned int al_spi_masked_intr_get(
		struct al_spi_interface	*spi_if);

/**
 * Enables an interrupt
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_spi_intr_enable(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	enable_val
 *		Value holding all the interrupts to enable
 */
void al_spi_intr_enable(
		struct al_spi_interface	*spi_if,
		unsigned int		enable_val);

/**
 * Disables an interrupt
 *
 * When an interrupt is disabled, the corresponding interrupt will not
 * trigger its interrupt handling function even if it has been set
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_spi_intr_disable(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	disable_val
 *		Value holding all the interrupts to disable
 */
void al_spi_intr_disable(
		struct al_spi_interface	*spi_if,
		unsigned int		disable_val);

/**
 * Clear SPI interrupts
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_spi_intr_clear(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	clear_val
 *		Value holding all the interrupts to clear
 */
void al_spi_intr_clear(
		struct al_spi_interface	*spi_if,
		unsigned int		clear_val);

/**
 * Set SPI transfer mode
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	tmod
 *		The transfer mode to set
 */
void al_spi_tmod_set(
		struct al_spi_interface	*spi_if,
		enum al_spi_tmod_t	tmod);

/**
 * Set spi chip select override
 *
 * Set HW CS control Override, so that SER will control the CS mechanism
 * Using this will prevent the deassertion of CS upon empty TX FIFO
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 *		(passing regs base address enables to invoke this API prior to
 *		initialization)
 */
void al_spi_cs_override_set(
		void __iomem		*spi_regs_base);

#ifdef __cplusplus
}
#endif

#endif

/** @} end of SPI group */

