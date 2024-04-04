/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This software file is triple licensed: you can use it either under the terms of
Commercial, the GPL, or the BSD license, at your option.

a) If you received this File from Annapurna Labs and you have entered into a
   commercial license agreement (a "Commercial License") with Annapurna Labs,
   the File is licensed to you under the terms of the applicable Commercial
   License.

Alternatively,

b) This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
   MA 02110-1301 USA

Alternatively,

c) Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    *	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *	Redistributions in binary form must reproduce the above copyright
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
#include <config.h>
#include <linux/linkage.h>

/**
 * Initialize l2 cache
 *
 * @param	r0
 *		L2 cache ECC enable
 */
ENTRY(al_l2cache_init)

	/*
	 * Addressing RMN: 2096
	 *
	 * RMN description:
	 * L2 SRAM does not go out of RSCRST, if not using redundancy setting
	 * flow.
	 * Software flow:
	 * Write the value 0x1 to the register in address 0xfd8e0000.
	 * Wait for a "ready" indication by polling the register in address
	 * 0xfd8e0080 for the value 0x2.
	 */
	ldr r1, =0xfd8e0000
	mov r2, #1
	str r2, [r1]
	ldr r1, =0xfd8e0080
1:	ldr r2, [r1]
	cmp r2, #2
	bne 1b

	/* set L2ACTLR */
	MRC p15, 1, r1, c15, c0, 0
	ORR r1, r1, #(1<<3) /* disable push evict to external */
	ORR r1, r1, #(1<<8) /* disable DVM/cache maint. broadcast */
	/* ARM ERRATA 798870 WA (no problem using it other CPUs)
	 * Addressing RMN: 1732, 1843
	 */
	ORR r1, r1, #(1<<7) /* enable hazard detect timeout. */

	MCR p15, 1, r1, c15, c0, 0
	/* Set L2CTR - data & tag DRAM latency */
	MRC p15, 1, r1, c9, c0, 2
	  /* data RAM latency: 0x2 - 3 cycles */
	bic r1, r1, #0x7
	orr r1, r1, #0x2
	  /* tag RAM latency: 0x2 - 3 cycles */
	bic r1, r1, #(0x7 << 6)
	orr r1, r1, #(0x2 << 6)
	/* ECC enable, if r0 != 0 */
	cmp r0, #0
	orrne r1, r1, #(0x1 << 21)
	MCR p15, 1, r1, c9, c0, 2
	isb
	mov pc, lr
ENDPROC(al_l2cache_init)


