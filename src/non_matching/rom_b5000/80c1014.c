/* Func_80c1014 (0x080c1014) -- NON-MATCHING.
 *
 * NON-MATCHING, 23 of 30 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 30, ours 28) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 64 bytes, ours 60.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c1014.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_a.s --func Func_80c1014
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: REGISTER-ROLE SWAP plus one uncreatable copy.
 *
 * 31 lines against the ROM's 33. The bodies agree instruction for instruction;
 * the two missing lines and every register difference come from one thing the
 * ROM does and gcc will not:
 *
 *     rom    mov r5, sp / mov r7, r0 ... (guard) ... mov r8, r5
 *            then `mov r2, r8 / ldrsh r0, [r6, r2]` each iteration
 *     ours   mov r7, sp / mov r8, r0 ... no copy ...
 *            then `ldrsh r0, [r6, r7]` each iteration
 *
 * The ROM materialises the buffer address in r5, copies it into r8 INSIDE the
 * guard, and reloads it into a scratch register every iteration. We keep it in
 * one callee-saved register and index straight off that, which is a shorter and
 * obviously better allocation -- and two instructions the ROM has.
 *
 * MEASURED, all 31 lines and 25 differing, byte-identical to each other:
 *   one `base` local passed to the call and used in the loop
 *   a SECOND local `p = base;` assigned inside the guard -- the shape the
 *     ROM's `mov r8, r5` reads as; gcc coalesces it straight back
 *   the array passed directly to the call and `p = buf;` inside the guard,
 *     so the two materialisations have no common variable at all
 *
 * The third is the interesting negative: even with no shared name between the
 * call argument and the loop base, gcc still produces ONE address and never the
 * ROM's copy. This is the same wall as src/non_matching/ovl_77dd1c/2009498.c
 * from the previous round -- the C has no way to demand a particular
 * callee-saved register, or a redundant copy into one.
 *
 * WHAT IS RIGHT: the offset-first operand order on the load. The ROM's
 * `ldrsh r0, [r6, r2]` puts the OFFSET register first, which
 * `*(short *)(off + (int)p)` produces and `p[i]` does not -- the rule recorded
 * for register-offset loads, applied per access.
 *
 * NEXT: nothing source-level.
 
 * ===== BATCH 334 BRIEF B: THE COPY IS NOT THE BLOCKER.  THE BLOCKER IS THE
 * ===== ALLOCNO PRIORITY ORDER, AND IT IS NOW QUANTIFIED.
 *
 * Figure re-derived unchanged: twenty-three of thirty stream encodings,
 * instructions ref twenty-eight against ours twenty-six.  Body unchanged.
 *
 * THE TWO MISSING INSTRUCTIONS ARE ONE DECISION, AND IT IS NOT A COPY.
 * The reference's `mov r8, r5` and its per-iteration `mov r2, r8` both exist
 * because the loop's buffer base is in a HIGH register and `ldrsh` needs a low
 * one -- so `mov r2, r8` is a RELOAD, not a source-level copy.  Our output is
 * the same program with the roles of two values swapped: we put the parameter
 * in r8 (where a `cmp` can read it for free) and the base in r7.  The park's
 * reading, and batch 333's correction of it to gcse copy propagation, are both
 * about keeping `p = base` alive; neither would produce the reference even if
 * it worked, because the reference's two instructions come from WHICH VALUE
 * TAKES THE HIGH REGISTER.
 *
 * READ IT STRAIGHT OFF `.18.greg`, which prints the order for you:
 *     ;; 6 regs to allocate: 38 36 35 37 33 32
 *     ;; Register dispositions: 32 in 8  33 in 7  35 in 0  36 in 5  37 in 6
 * That list IS `allocno_order` after its sort, lowest priority LAST.  reg 32
 * is the parameter (set from r0 at insn 4) and reg 33 is the base.  The
 * parameter is last, so it is the one that fails its preferred class and
 * retries with `reg_alternate_class` (**global.c:561-566**), and that retry is
 * what hands out r8.  To reach the reference, reg 33 must be last instead.
 *
 * WHY IT CANNOT BE, ARITHMETICALLY.  `allocno_compare` (**global.c**, the
 * `pri1`/`pri2` expression) is
 * `floor_log2 (n_refs) * n_refs / live_length`, descending.  The base has
 * three references (its set, the call argument, the loop load) against the
 * parameter's two, and the parameter is born at the FIRST insn of the function
 * so its `live_length` is strictly the larger.  Ordering the base after the
 * parameter therefore needs `3 / L_base < 2 / L_param`, i.e. the base's live
 * length more than half again the parameter's -- impossible when the base is
 * born later.  Cutting the base to two references does not help either: with
 * equal reference counts the shorter life wins, and that is still the base.
 *
 * SO THE ESCAPE IS EXPLICIT AND NARROW: the compared value needs FOUR
 * references and the base TWO.  `floor_log2` jumps at four, so 2/L_base is
 * then below 8/L_param for any plausible pair of lengths.  That is the shape
 * to go looking for, and it is a statement about reference COUNTS, not about
 * spellings of a copy.
 *
 * MEASURED INERT THIS ROUND, all twenty-six instructions and twenty-one
 * aligned-equal of thirty, i.e. byte-identical to the installed body -- and
 * note these vary the dimension the park and batch 333 both aimed at:
 *   - the parameter copied into a local inside the guard
 *   - a second pointer assigned inside the guard
 *   - both of the above together
 *   - the parameter copied into a local BEFORE the call
 *   - the array passed to the call directly, with the base named in the guard
 * MEASURED WORSE: reusing `count` as the loop counter (nineteen aligned-equal).
 * `.18.greg` confirms every one of these collapses back to the six allocnos
 * above, which is why they are all one cell of the search.
 *
 * ALSO WORTH KNOWING: r4 is unavailable across the loop's call in this build,
 * so only r5, r6 and r7 are low callee-saved registers for four values that
 * live across it.  One of the four MUST go high; the question is only which,
 * and that is the priority order above.
 */
extern int Func_80b6c08(int a, short *buf);
extern void Func_80c0f98(int v, int b);

void Func_80c1014(int arg0)
{
    short buf[14];
    short *base;
    int count;
    int n;
    int off;
    int v;

    base = buf;
    count = Func_80b6c08(3, base);
    if (count > 0) {
        off = 0;
        n = count;
        do {
            v = *(short *)(off + (int)base);
            if (v != arg0)
                Func_80c0f98(v, 1);
            n--;
            off += 2;
        } while (n != 0);
    }
}
