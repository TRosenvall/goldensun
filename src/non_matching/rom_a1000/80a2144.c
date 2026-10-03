/* Func_80a2144 (LoadMenuPalette) -- NON-MATCHING.
 *
 * NON-MATCHING, 31 of 49 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 49, ours 48) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   SIZE ref 108 bytes, ours 104.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a2144.c \
 *     asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_c.s --func Func_80a2144
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: a POOLED 0x1f mask, with the same value NOT pooled beside it.
 * 46 lines against the ROM's 47, 21 differing.
 *
 * THE DMA ELISION THAT BLOCKED THIS IS FIXED. This function used to fail at
 * line 11 on two DMA3 transfers sharing a source, where gcc elided the second
 * `ldr r0`. include/dma.h now lists "r0" in DMA3_SET's clobbers, which forces
 * the reload; the whole corpus was rebuilt and all 96 overlays still compare,
 * and Func_80b0840 -- the first function parked on that class -- is elevated.
 * The first difference here has moved from line 11 to line 18.
 *
 * WHAT REMAINS. The ROM masks two colour channels with 0x1f and pools ONE of
 * them:
 *
 *     rom    ldr r3, =0x1f / mov r0, #0x1f / and r1, r3 / and r0, r2
 *     ours   mov r3, #0x1f / mov r1, r3    / and r0, r3 / and r1, r2
 *
 * Same constant, same function, one pooled and one immediate -- which is
 * exactly the internal control const.sym's header describes for _CONST_2. That
 * makes 0x1f a candidate for an entry, but the bar is a MEASURED set of literal
 * spellings and only one has been tried here, so nothing was added.
 *
 * Also unresolved: r0 and r1 are exchanged, and the clamp branches are `ble`
 * where the ROM has `bls`. Declaring the three channel values `unsigned int`
 * moves 24 differing to 21 and is kept below, but does not reach the branch --
 * the comparison is against a constant that fits either way, so gcc picks the
 * signed form regardless of the operand type.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80a2144(int bank)
{
    unsigned short *pal;
    unsigned short c;
    unsigned int r, g, b;
    unsigned int t;

    pal = (unsigned short *)((bank << 5) + (0xa0 << 19));
    DMA3_SET((void *)0x50001e0, pal, 0x80000010);
    DMA3_SET((void *)0x50001e0, pal, 0x84000008);
    c = pal[4];
    t = (unsigned int)c << 16;
    b = t >> 26;
    g = (t >> 21) & 0x1f;
    r = 0x1f & c;
    b += 9;
    if (b > 0x1f)
        b = 0x1f;
    g += 9;
    if (g > 0x1f)
        g = 0x1f;
    r += 9;
    if (r > 0x1f)
        r = 0x1f;
    pal[4] = (b << 10) | (g << 5) | r;
}
