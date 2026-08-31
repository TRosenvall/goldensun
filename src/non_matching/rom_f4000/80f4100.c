/* Func_80f4100 -- asm/rom_f4000/rom_f4008_c_c.s
 *
 * BLOCKER: REGISTER RESIDENCY (allocator pressure), not spelling.
 * 39 of 54, LENGTH EXACT.
 *
 * WORTH TWO FUNCTIONS. Func_80f6038 (asm/rom_f6000/rom_f6008_c_a.s) is
 * instruction-for-instruction identical to this one -- same 54 lines, same
 * registers, only the local label addresses differ. Whatever lands this lands
 * both. Check tools/dupfuncs.py for further copies before spending screens.
 *
 * WHAT IT IS. A 15-bit BGR palette scale over a halfword run:
 *
 *     for (n = count; n; n--)
 *         *dst++ = per-channel ((chan * scale) >> 16), recombined
 *
 * with the three channel masks 0x1f, 0xf8<<2 (0x3e0) and 0xf8<<7 (0x7c00).
 * Returns 0. No calls, one loop, one guard.
 *
 * THE PROGRESSION -- four spellings, monotone, and it says where the wall is:
 *
 *   masks as named locals, separate r/g/b temps          63 lines, 60 differ
 *   masks as named locals, expression folded             53 lines, 51 differ
 *   + counter copied after the mask setup (ROM's shape)  53 lines, 47 differ
 *   masks as REPEATED LITERALS, no mask locals           54 lines, 39 differ
 *
 * The last step is the informative one: DROPPING the named locals fixed the
 * line count. The ROM's three mask registers are gcc CSE-ing three repeated
 * literals into registers, NOT four named locals being allocated. Naming them
 * adds a local gcc then has to place, and the extra placement is what pushed
 * the length to 53/63. Do not re-try named masks.
 *
 * Also measured, and NOT the cause: making the masks and `v` unsigned so the
 * shift is `lsr` rather than `asr` changed NOTHING (51 differ before and
 * after). gcc already proves the value non-negative here. Recorded because
 * "the ROM uses lsr so make it unsigned" is a plausible-looking fix that
 * buys nothing on this function.
 *
 * WHAT REMAINS, stated precisely. The whole register assignment is rotated by
 * one and every later difference inherits it:
 *
 *     rom   src->r7  dst->r6  scale->r5   (three callee-saved)
 *     ours  src->r6  dst->r5  scale->r0   (two callee-saved + one caller-saved)
 *
 * and the constant setup follows from it -- the ROM builds the 0x1f mask first
 * and gives it r8 (`mov r1,#0x1f / mov r8,r1`), while gcc builds a 0xf8 mask
 * first because r8 is not yet contended.
 *
 * gcc is not being wrong here. This function contains NO CALLS, so r0-r3 are
 * free for the whole body and using r0 for `scale` is the cheaper choice. The
 * ROM spends a callee-saved register where it does not have to. That asymmetry
 * is the blocker: to match, gcc must be made to want one MORE long-lived
 * register than it currently needs, and no spelling of this body creates that
 * pressure -- the pressure is what decides the allocation, and the body is
 * already minimal.
 *
 * NOT TRIED, and the honest next directions:
 *   - a fifth value genuinely live across the loop, if one can be found that
 *     the ROM also computes (do not invent a use that is not in the asm)
 *   - whether the ROM's shape falls out of a DIFFERENT loop form (a `while`
 *     over a moving end pointer rather than a counter), which would change
 *     what is live rather than merely how it is spelled
 *
 * The 39 is not a near-miss to be nudged; the length is right and the
 * allocation is wrong, so treat it as one decision to flip, not 39 to fix.
 */
int Func_80f4100(unsigned short *src, unsigned short *dst, int scale, int count)
{
    int n;
    int v;

    if (count > 0) {
        n = count;
        do {
            v = *src;
            v = ((((v & 0x1f) * scale) >> 16) & 0x1f)
              | ((((v & (0xf8 << 2)) * scale) >> 16) & (0xf8 << 2))
              | ((((v & (0xf8 << 7)) * scale) >> 16) & (0xf8 << 7));
            n--;
            *dst = v;
            src++;
            dst++;
        } while (n != 0);
    }
    return 0;
}
