/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

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
#ifndef __mvCpuCntrs_h__
#define __mvCpuCntrs_h__

#include "mvTypes.h"
#include "mvOs.h"

#include "mvCpuCntrsIf.h"

#ifdef CONFIG_PLAT_ARMADA
#include "pj4/mvPJ4Cntrs.h"

#define MV_CPU_CNTRS_NUM            MRVL_PJ4B_MAX_COUNTERS
#define MV_CPU_CNTRS_OPS_NUM        MRVL_PJ4B_EVT_UNUSED

#else /**/

#include "feroceon/mvFeroceonCntrs.h"

#define MV_CPU_CNTRS_NUM            MV_FEROCEON_CNTRS_NUM
#define MV_CPU_CNTRS_OPS_NUM        MV_FEROCEON_CNTRS_OPS_NUM

#endif /* CONFIG_PLAT_ARMADA */

typedef enum {
	MV_CPU_CNTRS_INVALID = 0,
	MV_CPU_CNTRS_CYCLES,
	MV_CPU_CNTRS_ICACHE_READ_MISS,
	MV_CPU_CNTRS_DCACHE_ACCESS,
	MV_CPU_CNTRS_DCACHE_READ_MISS,
	MV_CPU_CNTRS_DCACHE_READ_HIT,
	MV_CPU_CNTRS_DCACHE_WRITE_MISS,
	MV_CPU_CNTRS_DCACHE_WRITE_HIT,
	MV_CPU_CNTRS_DTLB_MISS,
	MV_CPU_CNTRS_TLB_MISS,
	MV_CPU_CNTRS_ITLB_MISS,
	MV_CPU_CNTRS_INSTRUCTIONS,
	MV_CPU_CNTRS_SINGLE_ISSUE,
	MV_CPU_CNTRS_MMU_READ_LATENCY,
	MV_CPU_CNTRS_MMU_READ_BEAT,
	MV_CPU_CNTRS_BRANCH_RETIRED,
	MV_CPU_CNTRS_BRANCH_TAKEN,
	MV_CPU_CNTRS_BRANCH_PREDICT_MISS,
	MV_CPU_CNTRS_BRANCH_PREDICT_HIT,
	MV_CPU_CNTRS_WB_FULL_CYCLES,
	MV_CPU_CNTRS_WB_WRITE_LATENCY,
	MV_CPU_CNTRS_WB_WRITE_BEAT,
	MV_CPU_CNTRS_ICACHE_READ_LATENCY,
	MV_CPU_CNTRS_ICACHE_READ_BEAT,
	MV_CPU_CNTRS_DCACHE_READ_LATENCY,
	MV_CPU_CNTRS_DCACHE_READ_BEAT,
	MV_CPU_CNTRS_DCACHE_WRITE_LATENCY,
	MV_CPU_CNTRS_DCACHE_WRITE_BEAT,
	MV_CPU_CNTRS_LDM_STM_HOLD,
	MV_CPU_CNTRS_IS_HOLD,
	MV_CPU_CNTRS_DATA_WRITE_ACCESS,
	MV_CPU_CNTRS_DATA_READ_ACCESS,
	MV_CPU_CNTRS_BIU_SIMULT_ACCESS,
	MV_CPU_CNTRS_BIU_ANY_ACCESS,

} MV_CPU_CNTRS_OPS;

/* Functions should be implemented for each CPU */
int					mvCpuCntrsMap(int counter, MV_CPU_CNTRS_OPS op);
void				mvCpuCntrsInit(void);
int					mvCpuCntrsStart(int idx, int event);

void				mvCpuCntrsInitialize(void);
MV_STATUS			mvCpuCntrsProgram(int counter, MV_CPU_CNTRS_OPS op, char *name, MV_U32 overhead);

#endif /* __mvCpuCntrs_h__ */
