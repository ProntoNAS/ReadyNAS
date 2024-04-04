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

#include <al_hal_common.h>
#include <al_hal_reg_utils.h>
#include <al_init_sys_fabric.h>
#include "al_init_ccu_regs.h"
#include "al_init_gicv2_regs.h"
#include "al_init_smmu_regs.h"
#include "al_init_sys_fabric_offsets.h"

/* secure initialization of different units.
 * Must be called from a secure context, to allow non-secure access
 * to the unit
 */

static void al_ccu_security_init(void __iomem *ccu_address)
{
	/*enable non-secure access to CCU*/
	al_reg_write32(ccu_address + AL_CCU_SECURE_ACCESS_OFFSET, 1);
}

static void al_smmu_security_init(void __iomem *smmu_address)
{
	/* convert all secure transactions to non-secure
	 * Addressing RMN: 804
	 */
	al_reg_write32(smmu_address + SMMU_CFG_R0_OFFSET,
			SMMU_CFG_R0_BYPASS | SMMU_CFG_R0_NON_SECURE);
	/* assign all SMMU resources to non-secure translation */
	al_reg_write32(smmu_address + SMMU_CFG_R1_OFFSET,
			SMMU_CFG_R1_NSNUMCBO_MASK | SMMU_CFG_R1_NSNUMSMRGO_MASK
			| SMMU_CFG_R1_NSNUMIRPTO_MASK);
}


static void al_gic_security_init(void __iomem *gic_base)
{
	int irq, irq_num;
	uint32_t temp;

	temp = al_reg_read32(gic_base + GIC_DIST_OFFSET + GIC_DIST_CTR);
	irq_num = ((temp & 0x1f) + 1) * 32;
	/* set non-private interrupts to group 1. 1 bit per interrupt */
	for (irq = 32; irq < irq_num; irq += 32) {
		al_reg_write32(gic_base + GIC_DIST_OFFSET + GIC_DIST_IGROUPR
				+ irq/8
				, 0xffffffff);
	}
}

/* API */

void al_sys_fabric_security_init(void __iomem *nb_base_address)
{
	al_ccu_security_init(nb_base_address + AL_NB_CCU_OFFSET);
	al_smmu_security_init(nb_base_address + AL_NB_SMMU0_OFFSET);
	al_smmu_security_init(nb_base_address + AL_NB_SMMU1_OFFSET);
	al_gic_security_init(
			nb_base_address + AL_NB_GIC_OFFSET(NB_GIC_MAIN));
	al_gic_security_init(
			nb_base_address + AL_NB_GIC_OFFSET(NB_GIC_SECONDARY));
}


