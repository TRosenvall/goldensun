/* OvlFunc_924_200a648  --  0x0200a648  [asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a.s]
 * OvlFunc_924_200adcc  --  0x0200adcc  [asm/overlays/rom_7ac2d8/ovl_2dcc_a.s]
 *
 * NOT MATCHING. 9 of 24, LENGTH EXACT. This file parks BOTH functions: they are
 * the same routine with three different addresses and a loop bound of 5 rather
 * than 6, and both screen identically at every step below. Solving either
 * solves both.
 *
 * A palette rotation: if a frame counter is a multiple of 8, save the first
 * halfword aside and shift the next seven down by one slot.
 *
 * AN UNSIGNED COUNTER BLOCKS GCC'S LOOP REVERSAL, and that is the finding worth
 * keeping. Written with a signed `int` counter, gcc reverses the loop -- it
 * emits `mov r2, #6` and counts DOWN with `sub r2, #1` -- where the ROM counts
 * UP from zero and compares against 6:
 *
 *     rom    mov r0, #0 ... add r0, #1 / cmp r0, #6 / bls
 *     int    mov r2, #6 ... sub r2, #1
 *     uint   mov r2, #0 ... add r2, #1 / cmp r2, #6 / bls     <- matches
 *
 * Changing only the counter's type from `int` to `unsigned int` restores the
 * ROM's direction. Reversal is legal for the signed counter because gcc can
 * prove the trip count and the counter is dead afterwards; with the unsigned
 * type it declines. NOTHING ELSE REACHED IT -- measured, all identical at 10:
 *
 *     do/while with `i <= 6`, `for (i = 0; i <= 6; i++)`, a `while` loop,
 *     `while (++i <= 6)`, and `i < 7` instead of `i <= 6`
 *
 * and four flags, every one inert:
 *
 *     -fno-strength-reduce, -fno-unroll-loops, -fno-rerun-loop-opt,
 *     -fno-expensive-optimizations
 *
 * That is worth stating plainly: five source spellings and four flags all left
 * the reversal in place, and a one-word type change removed it. The lever here
 * is the TYPE, not the loop's shape.
 *
 * WHAT REMAINS IS A REGISTER-ROLE ROTATION, nine instructions of it, and no
 * instruction is wrong -- only which register each value occupies:
 *
 *     rom    ldr r3, =SAVE / ldrh r2, [r1] / strh r2, [r3] / ldr r2, =SRC / mov r0, #0
 *     ours   ldr r2, =SAVE / ldrh r3, [r1] / ldr r0, =SRC  / strh r3, [r2] / mov r2, #0
 *
 * The ROM keeps the source pointer in r2 and the counter in r0; gcc does the
 * reverse. Everything in the loop body follows from that one choice.
 *
 * This is the same class as src/non_matching/ovl_793768/2008ef4.c, parked in
 * this batch: an allocation-ORDER question rather than a placement one. The pin
 * does not apply -- it decides where a value is WRITTEN, not which register the
 * allocator parks a long-lived value in, and that boundary is recorded in
 * src/non_matching/rom_c0/rom_64b8.c.
 *
 * NEXT: find what makes gcc prefer r2 for the pointer over the counter. The
 * order in which the two are first assigned is the obvious candidate and is
 * already the ROM's order in the source below, so it is not simply that.
 */

extern int iwram_3001e40;

void OvlFunc_924_200a648(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) != 0)
        return;
    d = (volatile unsigned short *)0x5000050;
    *(volatile unsigned short *)0x500005e = *d;
    s = (volatile unsigned short *)0x5000052;
    i = 0;
    do {
        *d = *s;
        i++;
        s++;
        d++;
    } while (i <= 6);
}
