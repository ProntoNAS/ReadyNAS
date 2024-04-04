/*******************************************************************************
  Copyright (C) 2013 Annapurna Labs International Ltd.

This software file is triple licensed: you can use it either under the terms of
Commercial, the GPL, or the BSD license, at your option.

a) If you received this File from Annapurna Labs and you have entered into a
   commercial elicense agreement (a "Commercial License") with Annapurna Labs,
   the File is licensed to you under the terms of the applicable Commercial
   License.

Alternatively,

b) This file is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   this library; if not, write to the Free Software Foundation, Inc., 51
   Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Alternatively,

c) Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/*
 * Initializing per-CPU security might be done without stack or mmu.
 * So we need to implement it in assembly
 */
#include <linux/linkage.h>
#include "al_init_sys_fabric_offsets.h"
#include "al_init_gicv2_regs.h"

/* r0 (param) - pointer to northbridge */
ENTRY(al_sys_fabric_percpu_security_init)
	/* set private interrupts to group 0 */
	ldr r1, =0xffffffff
	ldr r2, =(AL_NB_GIC_OFFSET(0) + GIC_DIST_OFFSET + GIC_DIST_IGROUPR)
	ldr r3, =(AL_NB_GIC_OFFSET(1) + GIC_DIST_OFFSET + GIC_DIST_IGROUPR)
	str r1, [r0, r2]
	str r1, [r0, r3]
	/* accept all priorities (accept non-secure priorities) */
	ldr r1, =0xff
	ldr r2, =(AL_NB_GIC_OFFSET(0) + GIC_CPU_OFFSET + GIC_CPU_PRIMASK)
	ldr r3, =(AL_NB_GIC_OFFSET(1) + GIC_CPU_OFFSET + GIC_CPU_PRIMASK)
	str r1, [r0, r2]
	str r1, [r0, r3]
	mov pc, lr
ENDPROC(al_sys_fabric_percpu_security_init)
