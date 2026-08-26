/* Func_80c0e38 and Func_80c0e70  --  0x080c0e38 / 0x080c0e70, cut from the tail
 * of goldensun/asm/rom_b5000/rom_bffb8_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/rom_b5000/rom_bffb8_a_c_c.o in goldensun/stage1.ld.
 *
 * A matched pair of screen fades: set BLDCNT to 0x2044, then step BLDALPHA
 * eight times, one frame apart. 0x80c0e38 counts the blend DOWN from 0x1010,
 * 0x80c0e70 counts it UP from 0x1000.
 *
 * PARKED IN BATCH 74 UNDER "literal pool PLACEMENT" -- 18 lines against 19,
 * with the missing instruction a `b` that exists only to jump over the ROM's
 * mid-function pool. Batch 79 turned that class from a dead end into a lever,
 * and this pair is the first thing it opened.
 *
 * WHAT MOVES THE POOL. gcc dumps a minipool at the last BARRIER within reach of
 * its first entry. arm.md gives a HImode pool reference a pool_range of 32-60
 * bytes against SImode's 1020, so a narrow reference cannot wait for the
 * barrier at the end of the function and dump_table manufactures a jump over an
 * early pool -- which is exactly the ROM's shape here.
 *
 * The park's own advice was the thing standing in the way. It said:
 *
 *     1. 0x2044 THROUGH AN int LOCAL. Stored straight to `REG_BLDCNT` -- a
 *        `vu16` -- gcc pools it as a HALFWORD (`ldrh`); the ROM loads a word.
 *
 * That is backwards. `ldrh rD, .L` and `ldr rD, =v` are the same instruction in
 * Thumb-1 (there is no pc-relative halfword load), so the ROM's spelling says
 * nothing about the mode -- and the int local made 0x2044 SImode, which pushed
 * the pool past the end of the function and deleted the `b`. Storing the
 * literal straight to the `vu16` keeps it narrow and the pool lands where the
 * ROM has it.
 *
 * The mode is legible in the POOL ORDER, which is what settled it. gcc sorts
 * pool entries by address-plus-pool_range, so narrow constants sort ahead of
 * symbol addresses:
 *
 *     rom      0x2044  0x1010  0x04000050  0x04000052
 *     int c    0x1010  0x04000050  0x2044  0x04000052   <- 0x2044 is wide
 *     u16 c    (same as int c -- the local is what widens it, not its type)
 *     literal  0x2044  0x1010  0x04000050  0x04000052   <- matches
 *
 * Two of the park's three findings survive and are still load-bearing:
 *   - both register addresses go in pointer locals assigned in the ROM's order,
 *     or gcc creates the pseudos the other way round and r6/r7 swap;
 *   - the step is `k - i` in the first and `i + k` in the second, matching the
 *     ROM's `sub r3, r6, r5` and `add r3, r5, r6`.
 */

#include "gba/io.h"

extern void WaitFrames(int n);

void Func_80c0e38(void)
{
    int i;
    vu16 *dst;
    vu16 *cnt;

    cnt = &REG_BLDCNT;
    dst = &REG_BLDALPHA;
    *cnt = 0x2044;
    i = 1;
    do {
        *dst = 0x1010 - i;
        WaitFrames(1);
        i += 2;
    } while (i <= 0x10);
}

void Func_80c0e70(void)
{
    int i;
    vu16 *dst;
    vu16 *cnt;

    cnt = &REG_BLDCNT;
    dst = &REG_BLDALPHA;
    *cnt = 0x2044;
    i = 1;
    do {
        *dst = i + 0x1000;
        WaitFrames(1);
        i += 2;
    } while (i <= 0x10);
}
