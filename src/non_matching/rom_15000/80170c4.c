/* Func_80170c4  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_c_a_a_a.s
 * Best screen: 2 instructions in disagreeing regions, of 24 (rom 24, ours 23).
 *
 * BLOCKER CLASS: a copy elided at a shared exit.
 *
 *      rom   lsl r3, r4, #1 / add r5, r3 ... mov r0, r5
 *      ours  lsl r3, r4, #1 / add r0, r5, r3
 *
 * Both paths return the destination pointer. The ROM advances it in place and
 * copies to r0 at the shared exit; gcc computes the advanced value straight
 * into the return register on the path that needs it.
 *
 * WHAT WAS TRIED
 *   1. Two `return d;` statements (kept below). 2 of 24, ours one shorter.
 *   2. A single `return d;` reached by `goto out;` from the early path, which
 *      is literally the ROM's shared exit. THE LENGTH BECOMES CORRECT, 24
 *      against 24 -- the copy appears -- BUT THE COUNT GETS WORSE, 8 of 24,
 *      because r4 and r5 then swap roles and six further instructions differ.
 *   3. With (2), copying `n` into a local before the destination pointer, to
 *      match the ROM's `mov r4, r2 / mov r5, r0` order. Byte-identical.
 *   4. With (2), both declaration orders for those two locals. Byte-identical.
 *
 * So the shared exit is reachable and the register assignment is not, and the
 * two cannot be had at once from this source. (1) is kept because 2 is a better
 * park than 8, but (2) is the structurally faithful one and is recorded here so
 * the choice is visible rather than looking like an oversight.
 *
 * THE DMA IS NOT THE PROBLEM, which is worth saying because dma.h register
 * binding is a documented blocker class. `DMA3_SET(&buf, d, cnt)` from
 * include/dma.h reproduces the ROM's `stmia r3!, {r0, r1, r2} / sub r3, #0xc`
 * exactly, with the halfword staged at sp+2 by a plain local. This is the first
 * function in the corpus where that header has been used successfully.
 */
#include "dma.h"

void *Func_80170c4(void *dst, int value, int n)
{
    unsigned short buf;
    unsigned char *d;
    int cnt;

    d = (unsigned char *)dst;
    if (n <= 0)
        return d;
    buf = value;
    cnt = 0x81 << 24;
    cnt |= n;
    DMA3_SET(&buf, d, cnt);
    d += n * 2;
    return d;
}
