/* Func_80c0e38 and Func_80c0e70  --  0x080c0e38 / 0x080c0e70,
 * asm/rom_b5000/rom_bffb8_a_c_c.s
 *
 * BLOCKER CLASS: literal pool PLACEMENT.
 * Status: 18 lines against the ROM's 19. The first SEVEN instructions are
 * exact and the whole body is exact; the one missing instruction is a `b` that
 * exists only to jump over a pool.
 *
 * WHAT THEY DO
 * A matched pair of screen fades: set BLDCNT to 0x2044, then step BLDALPHA
 * eight times, one frame apart. 0x80c0e38 counts the blend DOWN from 0x1010,
 * 0x80c0e70 counts it UP from 0x1000.
 *
 * THE ROM KEEPS ITS POOL INSIDE THE FUNCTION:
 *
 *      mov r5, #1
 *      b   .Lc0e58        <-- jump over the pool
 *      .word 0x2044
 *      .word 0x1010
 *      .pool
 *      .Lc0e58: sub r3, r6, r5 ...
 *
 * gcc puts the pool after the function, so there is nothing to jump over and no
 * branch. Every other instruction, including the register assignment, matches.
 *
 * THREE THINGS WERE NEEDED TO GET THERE and should be kept in any retry:
 *
 *   1. 0x2044 THROUGH AN int LOCAL. Stored straight to `REG_BLDCNT` -- a
 *      `vu16` -- gcc pools it as a HALFWORD (`ldrh`); the ROM loads a word.
 *      Same narrow-constant behaviour as Func_80173ac.
 *   2. BOTH REGISTER ADDRESSES IN POINTER LOCALS, assigned in the ROM's order:
 *      BLDCNT first, then the constant, then BLDALPHA. Without them gcc creates
 *      the pseudos in the other order and r6/r7 come out swapped.
 *   3. The step written `k - i` and `i + k` respectively -- the ROM's
 *      `sub r3, r6, r5` and `add r3, r5, r6` have the operands that way round.
 *
 * This is the same blocker as src/non_matching/ovl_7ec19c/200816c.c and
 * src/non_matching/rom_15000/rom_1c154.c: nothing tried in either moved gcc's
 * pool placement, and adding or removing code around the function did not
 * either. Two more functions on it now, and this pair is the cleanest example
 * because everything else is settled.
 */

#include "gba/io.h"

extern void WaitFrames(int n);

void Func_80c0e38(void)
{
    int i;
    int k;
    int c;
    vu16 *dst;
    vu16 *cnt;

    cnt = &REG_BLDCNT;
    c = 0x2044;
    dst = &REG_BLDALPHA;
    *cnt = c;
    k = 0x1010;
    i = 1;
    do {
        *dst = k - i;
        WaitFrames(1);
        i += 2;
    } while (i <= 0x10);
}

void Func_80c0e70(void)
{
    int i;
    int k;
    int c;
    vu16 *dst;
    vu16 *cnt;

    cnt = &REG_BLDCNT;
    c = 0x2044;
    dst = &REG_BLDALPHA;
    *cnt = c;
    k = 0x1000;
    i = 1;
    do {
        *dst = i + k;
        WaitFrames(1);
        i += 2;
    } while (i <= 0x10);
}
