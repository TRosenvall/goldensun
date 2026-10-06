/* OvlFunc_932_20086a0  --  NOT MATCHING
 *
 * NON-MATCHING, 10 of 27 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 27, ours 25) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 60 bytes, ours 56.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b9cb4/20086a0.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_a.s --func OvlFunc_932_20086a0
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c.s
 * Best screen: 3 instructions in disagreeing regions, of 26 (rom 26, ours 24).
 *
 * BLOCKER CLASS: mask narrowing on a halfword store.
 *
 * The ROM narrows the value explicitly before storing it, even though `strh`
 * truncates on its own:
 *
 *      rom   lsl r3, r5, #0x10 / lsr r3, #0x10 / strh r3, [r6, #0x0]
 *      ours  strh r5, [r6, #0x0]
 *
 * gcc knows the store discards the top bits and drops the pair.  Every attempt
 * to keep them was folded away:
 *
 *  1. `m = (unsigned short)s; *p = m;` through an int temp.  Byte-identical.
 *  2. Declaring `s` as `unsigned short` so the OR itself narrows.  Also
 *     byte-identical -- gcc sinks the narrowing to the store and then removes
 *     it there.
 *
 * Same class as the other mask-narrowing parks: the redundancy is provable, so
 * the optimiser removes it no matter where the source puts it.
 *
 * TWO SPELLINGS THAT DID WORK and are kept below:
 *   - `x = 0x64; x *= r;` makes the CONSTANT the destination of the multiply,
 *     matching `mov r3, #0x64 / mul r3, r0`.  See docs/elevation.md; this is
 *     the same lever as the AND/ORR case and it applies to MUL too.
 *   - `t = 0xfdff; t &= v;` likewise.
 * The local data label is reached the established way,
 * `extern unsigned char L5238[] __asm__(".L5238");`.
 *
 * ================= BATCH 331 BRIEF A =================
 *
 * THE RESIDUE IS TWO INSTRUCTIONS, IN ONE PLACE.  The figure claimed at the top
 * of this file is a MISALIGNMENT count, as the header already warns, and nobody
 * had decomposed it.  tools/aligncmp.py -v resolves it into three hunks
 * carrying five entries, and only ONE of the three is real:
 *     ref index 4    ldr r3, [pc, #40]   vs ours ldr r3, [pc, #36]
 *     ref index 11   ldr r2, [pc, #28]   vs ours ldr r2, [pc, #24]
 *     ref indices 19 to 21   lsl r3, r5, #16 / lsr r3, #16 / strh r3, [r6]
 *     ours indices 19 to 20                                  strh r5, [r6]
 * The two pool-load hunks are a CONSEQUENCE of the length difference, exactly
 * as the header's re-classification says.  So the true gap is the narrowing
 * pair at the end and nothing else: twenty-two of the ROM's twenty-four
 * instructions are exact.  Read the INSTRUCTION COUNT line, not the stream
 * line, when deciding whether this park is close.
 *
 * WHAT THE FUNCTION IS, which identifies every constant in it.  `0x80 << 19` is
 * 0x04000000, the GBA display-control register; include/gba/io.h:352 declares
 * REG_DISPCNT as an lvalue through a volatile unsigned-halfword pointer at that
 * address.  `0xfdff` is the complement of 0x0200 and `0x80 << 2` IS 0x0200,
 * which include/gba/io.h:517 names DISPCNT_BG1_ON.  So the body turns BG1 on
 * when a scaled random draw clears the threshold halfword at .L5238.  Worth
 * keeping for pass-4 naming whatever happens to the match.
 *
 * THE PARK'S DIAGNOSIS SURVIVED, and the spelling dimension is now closed hard.
 * The header had two negatives; nine more were measured and EVERY ONE is
 * byte-identical to the body above, with three of them producing
 * character-identical assembly:
 *   a volatile unsigned-halfword pointer, cast store        inert
 *   the same, plain store (this IS the REG_DISPCNT shape)   inert
 *   the same, masking store                                 inert
 *   casting the masked value at the store                   inert
 *   masking with 0xffff and no cast                         inert
 *   an unsigned left-then-right shift pair at the store      inert
 *   declaring the value a signed short, plain store          inert
 *   volatile pointer crossed with the signed short           inert
 *   declaring the value an unsigned short, plain store       inert
 * The volatile row is the newsworthy one: the real source almost certainly
 * writes through REG_DISPCNT, and a VOLATILE halfword destination does not
 * preserve the narrowing either.
 *
 * WHY THE WHOLE SPELLING DIMENSION IS CLOSED.  For a halfword destination the
 * conversion never becomes an insn at all -- expand converts the right-hand
 * side to the destination's mode with a SUBREG, so there is no zero-extend
 * instruction for any later pass to keep.  Combine is not the actor here, which
 * means blocking combine cannot help, and that matters because the obvious next
 * idea is the cross-block trick: flow.c:4476 only creates a LOG_LINK when
 * `BLOCK_NUM (y) == blocknum`, so a set in another block is already outside
 * combine's reach.  That lever is unavailable for a second reason too -- in the
 * ROM the shift pair and the store are all three in the SAME block, so combine
 * COULD have folded them there if a narrowing insn had existed.  It did not
 * fold them, so whatever the ROM emitted was not a foldable
 * narrowing-at-a-store in the first place.
 *
 * NEXT: stop spelling the conversion.  The remaining question is what makes a
 * zero-extension exist as an INSN before a halfword store, which is a question
 * about why the value is needed thirty-two bits wide, not about how the store
 * is written.
 */
extern unsigned char L5238[] __asm__(".L5238");
extern int __Random(void);

void OvlFunc_932_20086a0(void)
{
    unsigned short *p;
    int v;
    int t;
    int s;
    int r;
    unsigned int x;
    int lim;
    int m;

    p = (unsigned short *)(0x80 << 19);
    v = *p;
    t = 0xfdff;
    t &= v;
    s = (short)(t << 16 >> 16);
    r = __Random();
    x = 0x64;
    x *= r;
    lim = *(unsigned short *)L5238;
    x >>= 16;
    if (x >= (unsigned int)lim) {
        m = 0x80 << 2;
        s |= m;
    }
    *p = (unsigned short)s;
}
