/* Func_80c0228 -- asm/rom_b5000/rom_bffb8_a_a_c.s
 *
 * NON-MATCHING, 21 of 53 encodings  (MEASURED, batch 333).
 *   SIZE AND ENCODING COUNT MATCH (53 both) but a PAD IS ABSORBING a
 *   one-instruction difference -- instructions are ref 50, ours 49 -- so this
 *   positional figure STILL MEASURES MISALIGNMENT.  It stood at thirty-six of
 *   fifty-three; aligned-equal (tools/aligncmp.py) rose from nineteen to 34 of
 *   53 and the aligned residue fell from thirty-five differing in nine hunks to
 *   20 in 10.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c0228.c \
 *     asm/rom_b5000/rom_bffb8_a_a_c_c.s --func Func_80c0228
 *   and then, because the figure above is phase:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_b5000/80c0228.c \
 *     asm/rom_b5000/rom_bffb8_a_a_c_c.s Func_80c0228 -v
 *
 * === BATCH 333: THE LENGTH GAP IS LOCATED, AND A NEW LEVER MOVED IT. ===
 *
 * WHERE THE ONE INSTRUCTION IS.  The reference spends ONE MORE CALLEE-SAVED
 * REGISTER: `push {r5, lr}` and a separate `pop {r5}` before the `pop {r0} /
 * bx r0`.  Note r4 is NOT pushed on either side -- the build is
 * -fcall-used-r4, so the ROM's whole callee-saved budget here is r5 alone.
 * The missing encoding is that `pop {r5}`; ours carries a trailing 2-byte pad
 * instead, which is why the stream lengths agree while the instruction counts
 * do not.  Everything else is a register rename.
 *
 * THE LEVER, AND IT IS NEW HERE: `i = 0;` MUST PRECEDE THE WHOLE ADDRESS
 * COMPUTATION IN SOURCE ORDER, in BOTH blocks.  Written
 *
 *     i = 0;
 *     d = (unsigned short *)(((0xd - r) << 6) + 0x6006000);
 *
 * the parameter `v` lands in r0 and `tile` in r4, exactly as the ROM.  Written
 * the other way round -- which is how this park stood -- they are r1 and r0 and
 * the rename propagates through every instruction of both fill loops.  That one
 * reordering is worth fifteen aligned encodings on its own.
 *
 * AND THE ORDERING REQUIREMENT IS STRICT: `i = 0;` must dominate the ENTIRE
 * address computation.  Putting a named `row = 0xd - r;` ABOVE the `i = 0;`
 * loses the whole gain and returns the park to thirty-five differing.  The
 * counter initialisation has to be the first statement of the block.
 *
 * WHAT REMAINS is one coupled allocation decision, visible as a three-way
 * rename.  The reference keeps `0xd - row` in r2 and the shifted value in r3,
 * then reuses r2 for the walking pointer and takes r5 for the pooled
 * 0x6006000; ours keeps the shift result in r2 and `0xd - row` in r3, so r2
 * falls free early and the pooled base reuses it instead of claiming a sixth
 * name.  The ROM's shift of `0xd - r` by six is THREE-OPERAND (`lsl r3, r2,
 * #6`) and ours is TWO-ADDRESS (`lsl r3, r3, #6`) -- that is the tell, and it
 * is an allocator coincidence rather than a liveness fact, because nothing
 * reads either source after the shift.
 *
 * MEASURED IN BATCH 333, ALL BYTE-IDENTICAL TO EACH OTHER at 20 aligned
 * differing -- a thirty-six-variant clean negative, which is the widest
 * recorded for this park:
 *   six declaration orders (p,v,r,row,tile,i,d; row before r; fully reversed;
 *     tile before r; d before i; i,d early) x three block-1 forms (the
 *     expression inline; a named `row = 0xd - r`; the pool constant written
 *     first) x two block-2 forms (`(row << 6) + 0x6006000` and the reverse)
 *   a named `off = (0xd - r) << 6` in one block and in both
 *   pointer arithmetic `(unsigned short *)0x6006000 + ((0xd - r) << 5)`
 *   `-(r - 0xd)` against a halved base
 * So DECLARATION ORDER IS NOT THE DIMENSION HERE and the two operand orders of
 * the final add are not either.  The park's older note that naming the 0x7 mask
 * "fixed nothing" is confirmed and is the same class of negative.
 *
 * ONE CORRECTION TO THE RECORD BELOW: the claim that "the remaining single line
 * is `mov r2, r3` in the SECOND block -- the ROM copies before shifting there
 * too" IS STALE.  Ours emits that copy (`adds r3, r2, #0 / adds r3, #13`); the
 * second block differs from the reference only in which register holds which
 * value.  The copy-then-modify reading that produced the `r = v / 8;
 * row = r + 0xd;` split is sound and is kept.
 *
 * --- THE PRE-BATCH-333 RECORD FOLLOWS; ITS NUMBERS ARE SUPERSEDED ---
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER AS FIRST STATED: REGISTER ROTATION (allocator preference). thirty-six of fifty-five, one line short.
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
 *   baseline, `row = v / 8 + 0xd` as one expression        fifty-three lines, forty differ
 *   division result named separately, `r = v / 8;
 *     row = r + 0xd;`                                      fifty-four lines, thirty-six differ
 *   + the 0x7 mask named in a local born before 0xf081     fifty-four lines, thirty-six differ
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
        i = 0;
        d = (unsigned short *)(((0xd - r) << 6) + 0x6006000);
        do {
            i++;
            *d = tile;
            d++;
        } while (i != 0x20);
        tile |= 0x80 << 4;
        r = v / 8;
        row = r + 0xd;
        if ((unsigned int)row <= 0x14) {
            i = 0;
            d = (unsigned short *)((row << 6) + 0x6006000);
            do {
                i++;
                *d = tile;
                d++;
            } while (i != 0x20);
        }
    }
}
