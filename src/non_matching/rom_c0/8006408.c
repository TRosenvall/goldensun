/* Func_8006408 -- 0x08006408 -- asm/rom_c0/rom_5cf8_a_a_c_a.s
 *
 * NON-MATCHING, 7 of 34 encodings  (MEASURED, batch 332).
 *   Stream lengths now AGREE: ref 34, ours 34, instructions 27 and 27.
 *   So this figure IS a distance.  The aligned residue is SIX instructions
 *   in FOUR hunks: the 0x81 constant in the load-delay stall (shared with the
 *   sibling park, same cause), the state store one slot early against the
 *   flag-byte store, and the two pool loads at the tail in the opposite order.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c0/8006408.c \
 *     asm/rom_c0/rom_5cf8_a_a_c_a.s --func Func_8006408
 *
 * Pins: 0.  Devices: none.  Production flags.
 *
 * Refuses if a state word is non-zero, otherwise sets two control bytes, clears
 * a halfword, stores its argument into the state word and clears a flag byte --
 * all under IME-off -- and returns 0.
 *
 * ===================================================================
 * BATCH 332: TWO STANDING CLAIMS IN THIS HEADER ARE REFUTED.
 * ===================================================================
 *
 * The superseded claim was thirty-one of thirty-four on streams of unequal
 * length.  It was blamed on "a saved parameter plus register rotation" and
 * called two lines long.  Both of its stated causes were wrong.
 *
 * (1) THE PARAMETER SAVE IS REACHABLE, AND IT IS THE SAME MECHANISM AS THE
 * SIBLING Func_80063bc IN THIS SAME PIECE -- read that park for the full
 * derivation.  In short: allocno_compare (global.c:598-621) orders allocnos by
 * floor_log2(n_refs)*n_refs/live_length, find_reg's pass 0 cannot hand out a
 * register nobody has used yet (global.c:1013) and pass 1 ignores the
 * preference (global.c:1023), so r0 goes to whichever allocno is allocated
 * FIRST rather than to the argument that arrives in it.  Moving the state
 * store to the front of the critical section shortens the argument's live
 * range, lifts its priority above the control-byte pointer's, and the
 * argument keeps r0 with no copy -- exactly as the reference does.  The second
 * scheduling pass then puts the store back near the end, which is why the old
 * header's "the ROM keeps its argument in r0 for the whole body" reads as
 * unreachable from the output alone.
 *
 * (2) THE POOLED ZERO IS NOT EVIDENCE OF A SYMBOL.  The old header recorded
 * the reference's PC-relative load of a zero word as proof that "that operand
 * was a SYMBOL whose value is zero", on the ground that gcc never pools a
 * value it can build with an eight-bit move, and left the namespace question
 * open.  gcc-2.96 POOLS THIS ZERO BY ITSELF.  This body makes it do so with no
 * symbol named: the destination is a byte-wide global, so the constant is
 * wanted in a NARROW mode, the narrow-mode move has no immediate alternative
 * and the value goes through force_const_mem.  The old body hid this because
 * there the already-zero state register was commoned into the store and no
 * constant was materialised at all.  Stream lengths agree at thirty-four
 * against thirty-four with the pool word present and unrelocated, so the
 * owner-facing question the old header opened does not need an answer.
 *
 * MEASURED.  All one hundred and twenty orderings of the five
 * critical-section statements, crossed with the control-byte pointer named or
 * not, the state pointer named or not, and the zero spelled four ways:
 *   - old body: thirty-one, on unequal lengths
 *   - the ordering alone, zero spelled as a plain assignment: nine
 *   - the ordering with the zero written through its own named pointer, the
 *     two statements adjacent: this body's claim
 * Only ONE of the hundred and twenty orderings is in this basin; the next best
 * is eleven, and splitting the pointer's assignment away from its store loses
 * ten.  Naming the control-byte pointer or the state pointer is INERT here --
 * which is consistent with the old header's own finding that naming a fixed
 * address names nothing, since what moved the figure was live_length, not
 * naming.
 *
 * WHAT IS LEFT.  The 0x81 constant in the load-delay stall is the sibling's
 * residue and has the same cause and the same refutation attached to it there
 * (disabling the second scheduling pass is refuted, not untested).  The other
 * two hunks are both ordering inside the tail and are the open question here.
 *
 * NOTE FOR WHOEVER CLOSES THIS.  Func_80063bc, Func_8006408 and Func_8006384
 * are ALL in this one .s, so none of them can be converted alone: one .c is
 * one TU.  Func_8006384 additionally emits a relocation the reference does not
 * -- it reaches the serial control register through an extern object where the
 * reference has a bare literal address -- and that must be fixed with an
 * address cast before the piece can pass.
 */

typedef volatile unsigned short vu16;

extern int ewram_20023ac;
extern unsigned char ewram_2002220[];
extern unsigned short ewram_2002238;
extern unsigned char ewram_20023a4;

int Func_8006408(int v)
{
    unsigned char *p;
    unsigned char *q;
    vu16 *ime;
    int cur;
    int save;

    cur = ewram_20023ac;
    p = ewram_2002220;
    if (cur != 0)
        return -1;
    ime = (vu16 *)0x4000208;
    save = *ime;
    *ime = (unsigned int)ime;   /* 0x0208: bit 0 clear, so interrupts off */
    p[1] = 0x81;
    ewram_20023ac = v;
    ewram_2002238 = cur;
    p[0] = 1;
    q = &ewram_20023a4;
    *q = 0;
    *ime = save;
    return 0;
}
