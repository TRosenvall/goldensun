/* Func_80c0228 -- asm/rom_b5000/rom_bffb8_a_a_c.s
 *
 * BLOCKER: REGISTER ROTATION (allocator preference). 36 of 55, one line short.
 *
 * Every structural feature reproduces: the unsigned range guard, both signed
 * divisions by 8 (the ROM recomputes rather than reusing, and so does ours),
 * both 0x20-iteration fill loops, the `orr` of 0x80<<4 between them, the
 * second range guard, and both address computations. What differs is WHICH
 * REGISTER holds what, throughout:
 *
 *     rom    v->r0   tile->r4  base->r5  counter->r1   push {r5, r14}
 *     ours   v->r1   tile->r0  base->r4  counter->r3   push {r14}
 *
 * The ROM spends one MORE callee-saved register than gcc does. This function
 * has no calls, so r0-r3 are free for the whole body and gcc's allocation is
 * the cheaper one. Same asymmetry as src/non_matching/rom_f4000/80f4100.c and
 * src/non_matching/rom_15000/8029274.c: to match, gcc must be made to WANT a
 * register it has no reason to take, and the body is already minimal.
 *
 * MEASURED:
 *   baseline, `row = v / 8 + 0xd` as one expression        53 lines, 40 differ
 *   division result named separately, `r = v / 8;
 *     row = r + 0xd;`                                      54 lines, 36 differ
 *   + the 0x7 mask named in a local born before 0xf081     54 lines, 36 differ
 *
 * The middle step is a real gain and is kept. It was read off two ROM
 * instructions -- `mov r2, r3 / add r2, #0xd`, a COPY before the add, where
 * ours had an in-place `add r3, #0xd`. A copy-then-modify in the ROM is a tell
 * that two named values exist where we wrote one expression; that is worth
 * carrying to the next function of this shape.
 *
 * The mask naming is a clean negative: it fixed nothing. The same lever moved
 * Func_8029274 by two lines, so it is site-dependent, not general.
 *
 * The remaining single line is `mov r2, r3` in the SECOND block -- the ROM
 * copies before shifting there too (`lsl r3, r2, #6` three-operand, ours
 * two-operand in place). Naming did not produce it a second time.
 */
extern int iwram_3001ef8;

void Func_80c0228(void)
{
    int *p;
    int v;
    int r;
    int row;
    int tile;
    int i;
    unsigned short *d;

    p = (int *)iwram_3001ef8;
    v = p[0];
    if ((unsigned int)v <= 0x4f) {
        tile = (v & 7) + 0xf081;
        r = v / 8;
        d = (unsigned short *)(((0xd - r) << 6) + 0x6006000);
        i = 0;
        do {
            i++;
            *d = tile;
            d++;
        } while (i != 0x20);
        tile |= 0x80 << 4;
        r = v / 8;
        row = r + 0xd;
        if ((unsigned int)row <= 0x14) {
            d = (unsigned short *)((row << 6) + 0x6006000);
            i = 0;
            do {
                i++;
                *d = tile;
                d++;
            } while (i != 0x20);
        }
    }
}
