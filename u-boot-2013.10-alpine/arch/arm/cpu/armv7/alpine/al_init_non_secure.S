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
#include <linux/linkage.h>

ENTRY(al_enter_non_secure)
	@ WA for ARM_ERRATA_763126
	@ Addressing RMN: 1845, 1846
	mrc	p15, 0, r10, c1, c0, 1		@ read aux control register
	orr	r10, r10, #(1 << 31)		@ set Snoop-delayed exclusive handling
	mcr	p15, 0, r10, c1, c0, 1		@ write aux control register

   @ enabling NEON
   MRC p15, 0, r1, c1, c0, 2  @ Read Coprocessor Access Control Register
   ORR r1, #(0xff << 20)      @ enable cp10 and cp 11
   MCR p15, 0, r1, c1, c0, 2  @ Write Coprocessor Access Control Register

   @setting NSACLR
   MRC p15, 0, r1, c1, c1, 2  @ Read Non-Secure Access Control Register data
   ORR r1, #(0x3 << 10)       @ allow ns access to cp10/11 (neon)
   ORR r1, #(0x3 << 17)       @ allow ns access to SMP bit & L2ERR
   MCR p15, 0, r1, c1, c1, 2  @ Write Non-Secure Access Control Register data

   @ global neon + vfp enable
   MOV r1, #0x40000000
  .word 0xeee81a10     @ manual encoding : VMSR FPEXC, r1

   @ see this assembler bug:
   @ http://lists.gnu.org/archive/html/bug-binutils/2012-03/msg00096.html
   mov r1, #0x31                @allow non-secure to change A and F in CPSR,
                                @ become non-secure
   MCR p15, 0, r1, c1, c1, 0    @Write Secure Configuration Register data
   dsb
   isb
   mov pc,lr
ENDPROC(al_enter_non_secure)


