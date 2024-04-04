/*
 */

#include <config.h>
#include <version.h>
#include <linux/linkage.h>

	.align	5

/* void cycle_delay(unsigned int num_cycles) */
ENTRY(cycle_delay)
1:
	cmp	r0, #0
	beq	2f
	sub	r0, #1
	b	1b
2:
	mov		pc, lr
ENDPROC(cycle_delay)

#ifdef CONFIG_SYS_HW_CACHE_COHERENCY
ENTRY(cp15_turn_on_smp_bit)
   mrc p15, 0, r0, c1, c0, 1
   orr r0, r0, #(0x1 << 6)     @SMP bit
   mcr p15, 0, r0, c1, c0, 1
   mov pc, lr
ENDPROC(cp15_turn_on_smp_bit)

ENTRY(cp15_turn_off_smp_bit)
   mrc p15, 0, r0, c1, c0, 1
   bic r0, r0, #6     @SMP bit
   mcr p15, 0, r0, c1, c0, 1
   mov pc, lr
ENDPROC(cp15_turn_off_smp_bit)

ENTRY(v7_outer_cache_flush_all)
   mov r0, #2
   mov r1, #1
   mov r2, #0
   b dcache_invalidate_all
ENDPROC(v7_outer_cache_flush_all)

ENTRY(v7_outer_cache_inval_all)
   mov r0, #2
   mov r1, #0
   mov r2, #0
   b dcache_invalidate_all
ENDPROC(v7_outer_cache_inval_all)

ENTRY(v7_outer_cache_flush_range)
   sub r1, r0
   b l2cache_flush_range
ENDPROC(v7_outer_cache_flush_range)

.global __dcache_invalidate_all
.func __dcache_invalidate_all
__dcache_invalidate_all:
    @ dcache invalidetion - with no memory accesses
    @ this function is not to be called from C!!
    @ r0 - cache level (1 or 2 - l1 or l2 respectively)
    @ r1 - flush (!0) or invalidate (0)
    @ r2 - disable cache if !0
    @ r3-r7 used by function, not backed-up or restored!!
    @
    @ MMU + cache invalidate
    @ Invalidate Data cache
    @ to make the code general purpose, we calculate the
    @ cache size first and loop through each set + way
    @ fixed assumptions:
    @    Associativity = 2 for l1, 16 for l2
    @    Cache line size = 64 byte
    cmp r2, #0
    beq 1f
    @disable cache if necessary
    mrc p15, 0, r2, c1, c0, 0	@ get CR
    bic r2, #(1 << 2)			@disable d-cache
    mcr p15, 0, r2, c1, c0, 0	@ set CR
    isb
    dsb
1:
    mov r7, r0					@r7 - level requested
    bic r7, #1					@(0 for L1, 2 for L2)
    mov r5, r1					@r5 - flush if !0
    mov r6, #0					@r6 - cache level
cache_level_loop:
    cmp r6, #0
    @r4 - associativity level
    moveq r4, #2
    movne r4, #16
    mcr p15, 2, r6, c0, c0, 0
    mrc p15, 1, r0, c0, c0, 0   @ Read Cache Size ID
    ldr r3, =0xfffe000
    and r0, r3, r0
    mov r0, r0, LSR #13
    add r0, r0, #1              @r0 - number of sets
    mov r1, #0                  @ r1 = way counter way_loop
1:                              @way loop
    mov r3, #0                  @ r3 = set counter set_loop
2:                              @set loop
    cmp r4, #2
    moveq r2, r1, LSL #31         @ if 2-way associative
    movne r2, r1, LSL #28         @ if 16-way associative
    orr r2, r3, LSL #6          @ 64B line. r2 = set/way cache operation format
    orr r2, r6					@cache level
    cmp r5, #0
    mcreq p15, 0, r2, c7, c6, 2    @ Invalidate line described by r2
    mcrne p15, 0, r2, c7, c14, 2   @ clean & Invalidate line described by r2
    add r3, r3, #1              @ Increment set counter
    cmp r0, r3                  @ Last set reached yet?
    bne 2b                      @ if not, iterate set_loop
    add r1, r1, #1              @ else, next
    cmp r1, r4                  @ last way reached yet?
    bne 1b                      @ if not, iterate way_loop
    isb
    dsb
    @if original request was for l2 - go back and do l2.
    cmp r6, r7
    movne r6, r7
    bne cache_level_loop
    mov pc, lr
.endfunc

.global dcache_invalidate_all
.func dcache_invalidate_all
dcache_invalidate_all:
    @ dcache-invalidation - interface function
    @ this function may be called from C, but it will add memory accesses to
    @ the stack.
    @ r0-r2 - arguments, See __dcache_invalidate_all for details
    push {r4-r7,lr}
    bl __dcache_invalidate_all
    pop {r4-r7,pc}
.endfunc

.global icache_invalidate_all
.func icache_invalidate_all
icache_invalidate_all:
    @ Invalidate Instruction cache
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 0
    isb
    dsb
    mov pc,lr
.endfunc

.global tlb_invalidate_all
.func tlb_invalidate_all
tlb_invalidate_all:
    @ Invalidate TLB
    mcr p15, 0, r1, c8, c7, 0
    isb
    dsb
    mov pc,lr
.endfunc

.global l2cache_flush_range
.func l2cache_flush_range
l2cache_flush_range:
    mov r3, r0
    add r1, r1, r0
1:  cmp r3, r1
    bgt 2f          @last address reached
    mcr p15, 0, r3, c7, c14, 1
    add r3, #64     @next cache line
    b 1b
2:  isb
    dsb
    mov pc, lr
.endfunc
#endif

