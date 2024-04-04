/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

#define MV_ASMLANGUAGE
#include "mvOsAsm.h"
#include <config.h>
#include <version.h>
#include "mvBoardEnvSpec.h"
#include "mvCtrlEnvSpec.h"
#include "mvAhbToMbusRegs.h"
#include "ddr2_3/mvDramIfRegs.h"
#include "mvCtrlEnvAsm.h"
#include "switchingServices.h"

#define SYNC1  nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop

        .align  2
        .global hsuRebootBootRomAsm
        .type    hsuRebootBootRomAsm,function

hsuRebootBootRomAsm:

/*
    Bring cpu back to ground zero:
     Disable caches.
     Drain caches.
     Set cpu address decoding windows to default.
     move v=r (virtual = real)  code to vr area 0x00000000 and exeute there.
     Turn off mmu.
     Jump to bootrom address = 0xffff0000
*/

/* disable interrupts */
        mrs     r2, cpsr
        orr     r3, r2, #0xc0
        msr     cpsr_c, r3
        SYNC1

/* fix cpu adw's (address decoding windows) to default */

        ldr r0, =0XF1020080 /* change base to D0000000 */
        ldr r1, =0XD0000000
        str r1, [r0, #0]

        ldr r0, =0XD0020000
        ldr r1, =0X07FFE841
        str r1, [r0, #0]

        ldr r0, =0XD0020004
        ldr r1, =0X80000000
        str r1, [r0, #0]

        ldr r0, =0XD0020008
        ldr r1, =0X80000000
        str r1, [r0, #0]

        ldr r0, =0XD002000C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020010
        ldr r1, =0X07FFE841
        str r1, [r0, #0]

        ldr r0, =0XD0020014
        ldr r1, =0X88000000
        str r1, [r0, #0]

        ldr r0, =0XD0020018
        ldr r1, =0X88000000
        str r1, [r0, #0]

        ldr r0, =0XD002001C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020020
        ldr r1, =0X07FFE841
        str r1, [r0, #0]

        ldr r0, =0XD0020024
        ldr r1, =0X90000000
        str r1, [r0, #0]

        ldr r0, =0XD0020028
        ldr r1, =0X90000000
        str r1, [r0, #0]

        ldr r0, =0XD002002C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020030
        ldr r1, =0X07FFE841
        str r1, [r0, #0]

        ldr r0, =0XD0020034
        ldr r1, =0X98000000
        str r1, [r0, #0]

        ldr r0, =0XD0020038
        ldr r1, =0X98000000
        str r1, [r0, #0]

        ldr r0, =0XD002003C
        ldr r1, =0X00000000
        str r1, [r0, #0]


        ldr r0, =0XD0020040
        ldr r1, =0X07FFE881
        str r1, [r0, #0]

        ldr r0, =0XD0020044
        ldr r1, =0XA0000000
        str r1, [r0, #0]

        ldr r0, =0XD0020048
        ldr r1, =0XA0000000
        str r1, [r0, #0]

        ldr r0, =0XD002004C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020050
        ldr r1, =0X07FFE880
        str r1, [r0, #0]

        ldr r0, =0XD0020054
        ldr r1, =0XA8000000
        str r1, [r0, #0]

        ldr r0, =0XD0020058
        ldr r1, =0XA8000000
        str r1, [r0, #0]

        ldr r0, =0XD002005C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020060
        ldr r1, =0X07FFE880
        str r1, [r0, #0]

        ldr r0, =0XD0020064
        ldr r1, =0XB0000000
        str r1, [r0, #0]

        ldr r0, =0XD0020068
        ldr r1, =0XB0000000
        str r1, [r0, #0]

        ldr r0, =0XD002006C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020070
        ldr r1, =0X07FFE880
        str r1, [r0, #0]

        ldr r0, =0XD0020074
        ldr r1, =0XB8000000
        str r1, [r0, #0]

        ldr r0, =0XD0020078
        ldr r1, =0XB0000000
        str r1, [r0, #0]

        ldr r0, =0XD002007C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020090
        ldr r1, =0X00000191
        str r1, [r0, #0]

        ldr r0, =0XD0020094
        ldr r1, =0XC8010000
        str r1, [r0, #0]

        ldr r0, =0XD0020098
        ldr r1, =0X07FF2F11
        str r1, [r0, #0]

        ldr r0, =0XD002009C
        ldr r1, =0XD8000000
        str r1, [r0, #0]


        ldr r0, =0XD00200A0
        ldr r1, =0X07FF3E11
        str r1, [r0, #0]

        ldr r0, =0XD00200A4
        ldr r1, =0XE0000000
        str r1, [r0, #0]

        ldr r0, =0XD00200A8
        ldr r1, =0X07FF3D11
        str r1, [r0, #0]

        ldr r0, =0XD00200AC
        ldr r1, =0XE8000000
        str r1, [r0, #0]

        ldr r0, =0XD00200B0
        ldr r1, =0X07FF3B11
        str r1, [r0, #0]

        ldr r0, =0XD00200B4
        ldr r1, =0XF0000000
        str r1, [r0, #0]

        ldr r0, =0XD00200B8
        ldr r1, =0X07FF1D11
        str r1, [r0, #0]

        ldr r0, =0XD00200BC
        ldr r1, =0XF8000000
        str r1, [r0, #0]

        ldr r0, =0XD00200C0
        ldr r1, =0X03FF1E11
        str r1, [r0, #0]

        ldr r0, =0XD00200C4
        ldr r1, =0XD4000000
        str r1, [r0, #0]

        ldr r0, =0XD00200C8
        ldr r1, =0X0000E040
        str r1, [r0, #0]

        ldr r0, =0XD00200CC
        ldr r1, =0XD0300000
        str r1, [r0, #0]

        ldr r0, =0XD00200D0
        ldr r1, =0X0000E040
        str r1, [r0, #0]

        ldr r0, =0XD00200D4
        ldr r1, =0XD0310000
        str r1, [r0, #0]

        ldr r0, =0XD00200D8
        ldr r1, =0X0000E040
        str r1, [r0, #0]

        ldr r0, =0XD00200DC
        ldr r1, =0XD0320000
        str r1, [r0, #0]

        ldr r0, =0XD00200E0
        ldr r1, =0X007F00C0
        str r1, [r0, #0]

        ldr r0, =0XD00200E4
        ldr r1, =0XD3800000
        str r1, [r0, #0]

        ldr r0, =0XD00200E8
        ldr r1, =0X07FF0E01
        str r1, [r0, #0]

        ldr r0, =0XD00200EC
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD00200F0
        ldr r1, =0XF8000000
        str r1, [r0, #0]

/*
   set  BOOTROM_ROUTINE_ERR_REG:
        0xFFFFFFFF : indicates  after powerup for bootrom
        0X00001000 : indicates  "ddr3 already init" for ddr3Init
        0X00002000 : indicates  warm-restart for uboot
*/

        ldr r0, =0XD00182D0
        ldr r1, =0XFFFF3000
        str r1, [r0, #0]

/* stop the timer completely!! linux/vxWorks leaves a periodic timer running. */

        ldr r0, =0XD0020300
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020310
        ldr r1, =0X00000000
        str r1, [r0, #0]

        SYNC1

/* fix mpps. */

        ldr r0, =0XD0018000
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018004
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018008
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD001800C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018010
        ldr r1, =0X11110000
        str r1, [r0, #0]

        ldr r0, =0XD0018014
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018018
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD001801C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018020
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018024
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0018028
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD001802C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        SYNC1

        /* fix misc regs  for bootrom and uboot defaults */

        ldr r0, =0XD007241C /* eth sdma config  */
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0020200 /* coherency fabric */
        ldr r1, =0X00000102
        str r1, [r0, #0]

        ldr r0, =0XD0008100 /* l2 control register */
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072460
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072500
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072504
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072580
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072584
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072588
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD007258C
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072590
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072594
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD0072598
        ldr r1, =0X00000000
        str r1, [r0, #0]

        ldr r0, =0XD007259C
        ldr r1, =0X00000000
        str r1, [r0, #0]

     /* fix NETA_RXQ_CONFIG_REG's */

        ldr r0, =0XD0071400
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0071404
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0071408
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD007140C
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0071410
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0071414
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0071418
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD007141C
        ldr r1, =0X00000040
        str r1, [r0, #0]

        ldr r0, =0XD0018220 /* fix POWER_MNG_CTRL_REG */
        ldr r1, =0XFFFFFFFF
        str r1, [r0, #0]

        ldr r0, =0XD0010604 /* fix SPI baud rate */
        ldr r1, =0X0000021D
        str r1, [r0, #0]

        mov     r1, #0
	mcr	p15, 0, r1, c1, c0, 1 /* fix aux_ctrl reg */
//        isb

        ldr     r3, =0xffff0000      @ get bootrom address
        ldr     r4, =0xc52078        @ c1 value from xdb

/* move v=r code to address 0x00000000 (v=r) */

        ldr     r2,=0x0
        ldr     r1,vr_code
        str     r1, [r2, #0]

        ldr     r2,=0x4
        ldr     r1,vr_code+4
        str     r1, [r2, #0]
        mov     pc, #0x00000000              @ jump to zero and execute the
                                             @ code there
vr_code:
        mcr     p15, 0, r4, c1, c0, 0        @ disable the mmu
        mov     pc, r3                       @ jump to bootrom

.globl mvSetSecondarySram
mvSetSecondarySram:
        /* Configure CPU0-3 Data Lockdown */
        mov     r2, #4
        ldr     r0, _l2_data_lockdown
        mov     r1, #0xffffff00
1:
        str     r1, [r0, #0x00]
        str     r1, [r0, #0x04]
        add     r0, r0, #8
        subs    r2, r2, #1
        bne     1b
        str     r1, [r0, #0x84]  // io bridge

        /*
          l2 cache has 32 ways of 64k each = 2MB total.
          We configure 75% to sram = ways 8-31 (1.5MB)
        */

        ldr     r0, _l2_block_alloc
        ldr     r1, _sram_base
        orr     r1, r1, #0x0008
        mov     r2, #23
1:
        str     r1, [r0]
        subs    r2, r2, #1
        add     r1, r1, #0x10000
        add     r1, r1, #0x1
        bne     1b

        /* Open the SRAM window */
        ldr     r0, _sram_win_control
        ldr     r1, _sram_base  // assuming _sram_base = 0x50000000
        orr     r1, r1, #0x0700 // 512k at 0x50000000
        orr     r1, r1, #0x0001
        str     r1, [r0, #0x04]

        ldr     r0, _sram_win_control
        add     r0, r0, #4
        ldr     r1, _sram_base
        add     r1, r1, #0x80000
        orr     r1, r1, #0x0700 // 512k at 0x40800000
        orr     r1, r1, #0x0001
        str     r1, [r0, #0x04]

        ldr     r0, _sram_win_control
        add     r0, r0, #8
        ldr     r1, _sram_base
        add     r1, r1, #0x100000
        orr     r1, r1, #0x0700 // 512k at 0x40100000
        orr     r1, r1, #0x0001
        str     r1, [r0, #0x04]

        bx      lr

_l2_block_alloc:
        .word 0xf100878c
_l2_data_lockdown:
        .word 0xf1008900
_sram_win_control:
         .word 0xf1020240
_sram_base:
         .word CONFIG_FREERTOS_SRAM_ADDR // freertos text base
