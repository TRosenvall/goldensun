/* Cluster Func_80e727c extracted from goldensun/asm/rom_c9000/rom_e6638_a_c_c.s by
 * `tools/split_s.py asm/rom_c9000/rom_e6638_a_c_c.s Func_80e727c`.
 *
 * The piece also held Anim_Unsummon, which is parked and large, so the split is a
 * prerequisite: _a keeps Anim_Unsummon, _b is this function. Text only, no data, no
 * new export. No pins, no flags.
 *
 * A palette brightener: over 0x3f entries from 0x5000002, unpack the three five-bit
 * BGR channels, add a per-channel delta, clamp each at 31, repack and store.
 *
 * ================ THE SAME `(u16)` CAST, PLUS A LOOP-INVARIANCE LEVER ================
 *
 * The ROM holds the five-bit mask TWICE: pooled into callee-saved r7 before the loop
 * and shared by the blue and green `and`s, and again as a fresh `mov r0, #0x1f` INSIDE
 * the loop for the red channel. Two different things are needed to get both.
 *
 * ONE: `(u16)` BEFORE THE BLUE AND GREEN MASKS. That makes each an HImode pool entry
 * (`*thumb_movhi_insn`, `pool_range` 64 -- arm.md:4318, :4353), which is what pools the
 * value and drags the loop's base pointer constant into the same mid-function pool with
 * a `b` over it. See the sibling landing for Func_80a2144 and the batch-276 correction
 * in const.sym for the mechanism; this is the same class.
 *
 * TWO: THE RED CHANNEL MASKS INTO ITS OWN DESTINATION, AS TWO STATEMENTS.
 * `r = 0x1f; r &= v;` -- not `r = 0x1f & v;`. With the cast in place but the red mask
 * written as one expression, the SImode constant is loop-invariant, `loop` hoists it
 * out, and gcc pays an extra callee-saved register plus its push and pop -- four
 * instructions too many, which reads as an overshoot rather than as the wrong lever.
 * Masking into the destination makes the mask pseudo the SAME pseudo as the channel, so
 * `set_in_loop` exceeds one and loop-invariant motion can no longer touch it. That is
 * the lever recorded in src/non_matching/rom_f6000/80f6148.c, used here for the first
 * time alongside the cast rather than instead of it.
 *
 * SO THE TWO HALVES PULL IN OPPOSITE DIRECTIONS and the function needs both: the cast
 * pools two of the three masks, and the destination form keeps the third out of the
 * pool AND out of the loop preheader. Neither alone is closer than the park was.
 */
#include "gba/types.h"

void Func_80e727c(int db, int dg, int dr)
{
    unsigned short *p;
    unsigned int v;
    unsigned int t;
    int i;
    int b;
    int g;
    int r;

    p = (unsigned short *)0x5000002;
    i = 0;
    do {
        v = *p;
        t = v << 16;
        b = (u16)(t >> 26) & 0x1f;
        g = (u16)(t >> 21) & 0x1f;
        r = 0x1f;
        r &= v;
        b += db;
        g += dg;
        r += dr;
        if (b > 0x1f)
            b = 0x1f;
        if (g > 0x1f)
            g = 0x1f;
        if (r > 0x1f)
            r = 0x1f;
        i++;
        *p = (b << 10) | (g << 5) | r;
        p++;
    } while (i != 0x3f);
}
