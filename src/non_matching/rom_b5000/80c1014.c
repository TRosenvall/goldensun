/* Func_80c1014 (0x080c1014) -- NON-MATCHING.
 *
 * NON-MATCHING, 23 of 30 encodings  (RE-MEASURED UNCHANGED, batch 342 brief A).
 *   COUNT DIFFERS -- instructions ref 28 / ours 26, so this positional figure
 *   counts PHASE and ours is TWO SHORT.  Size ref 64 bytes, ours 60.
 *   RELOCATIONS differ, but THE SAME TWO SYMBOLS AT A SHIFTED OFFSET
 *   (Func_80b6c08 at 0x10 both sides; Func_80c0f98 at 0x28 against 0x24),
 *   which is a CONSEQUENCE of the two missing instructions, not a blocker.
 *   NOTE: the figure line this park carried before batch 342 printed the
 *   STREAM counts ("ref 30, ours 28") on the line that says COUNT DIFFERS.
 *   Those are stream entries; the instruction counts are 28 / 26.  The batch-334
 *   addendum below had it right and the header line did not.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c1014.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_a.s --func Func_80c1014
 *
 * PIECE: tools/piece.py reports THREE functions in rom_bffb8_c_c_a_a.s --
 * Func_80c0eec (66 of 79), Func_80c0f98 (30 of 61) and this one.  Any member
 * can be cut out alone with tools/split_s.py.
 * PINS: tools/shimcount.py reports 0.
 * Blocker class: WHICH OF FOUR CALL-CROSSING VALUES LOSES LO_REGS.
 *
 * THE TWO MISSING INSTRUCTIONS ARE ONE DECISION AND IT IS NOT A COPY.  The
 * reference's `mov r8, r5` and its per-iteration `mov r2, r8` both exist
 * because the loop's buffer base is in a HIGH register and `ldrsh` needs a low
 * one, so `mov r2, r8` is a RELOAD.  Our output is the same program with two
 * values' roles swapped: we put the parameter in r8 (where `cmp` reads a high
 * register for free) and the base in r7.  r4 is call-used in this build, so
 * only r5, r6 and r7 are low callee-saved for FOUR values that live across the
 * inner call -- one MUST go high, and the only question is which.
 *
 * ===== BATCH 342 BRIEF A: THE SHAPE IS REACHABLE, AND BATCH 334's ARITHMETIC
 * ===== WAS WRONG BECAUSE n_refs IS WEIGHTED BY LOOP DEPTH.
 *
 * TWO NEW FACTS, ONE OF WHICH INVALIDATES THE OLD BOUND.
 *
 * (1) THE ROM'S SHAPE IS REACHABLE.  Under `-ffixed-r7` gcc produces exactly
 * the reference's treatment of the base -- `mov r8, sp` and a per-iteration
 * `mov r3, r8 / ldrsh r0, [r6, r3]` -- at 19 of 30, autosweep's only improving
 * row.  It does NOT land, and the reason is a hard budget rather than a choice:
 * with r7 fixed only r5 and r6 are low, so TWO of the four go high, the second
 * one (`sl`) has to be saved and restored, and ours is then 29 instructions
 * against the reference's 28.  So `-ffixed-r7` cannot close this park at four
 * long-lived values, whatever else is done.  Record that as a bound WITH its
 * reason; the escape would be a body with only three.
 *
 * (2) `n_refs` IS A LOOP-DEPTH-WEIGHTED COUNT.  **flow.c:5114-5116** reads
 *     REG_N_REFS (regno) += (optimize_size ? 1 : pbi->bb->loop_depth + 1);
 * so a reference inside a depth-1 loop counts TWO, not one.  (The same
 * weighting is at :4948 for the auto-increment path and :5556 for a deleted
 * increment; :5114 is the general one and is the line to cite.)  Batch 334
 * counted raw occurrences -- "the base has three references against the
 * parameter's two" -- and concluded the escape needs the parameter at FOUR
 * references and the base at TWO.  With the weighting the real counts are
 *     parameter   1 (prologue set, depth 0) + 2 (loop compare, depth 1) = 3
 *     base        1 (set) + 1 (call argument) + 2 (loop load)           = 4
 * and `allocno_compare` (**global.c:597-619**) scores
 * `floor_log2 (n_refs) * n_refs / live_length * 10000 * size`, so the two
 * priorities are 1*3/L_param = 3/L and 2*4/L_base = 8/L -- a far wider gap
 * than batch 334 thought, because `floor_log2` has already jumped for the base.
 * ONE REFINEMENT IN OUR FAVOUR: :617-619 breaks a tie with `return v1 - v2`,
 * ascending allocno number, and the parameter IS the lower allocno (32 against
 * 33), so A TIE IS A WIN and the requirement is `>=`, not `>`.
 *
 * THAT MODEL WAS THEN CONFIRMED BY THREE PROBES, and the probes are the point
 * because they separate two things that look identical in the source:
 *   - ONE EXTRA IN-LOOP reference to the parameter (`Func_80c0f98(v, arg0 | 1)`)
 *     takes its weighted count 3 -> 5, `floor_log2 * n` 3 -> 10, AND FLIPS THE
 *     ALLOCATION: `mov r7, r0` for the parameter, `mov r8, sp` for the base,
 *     and the reference's `mov r3, r8 / ldrsh` reload.  (32 instructions, so
 *     not a landing -- it is a probe.)
 *   - ONE EXTRA DEPTH-0 reference (a second clause in the guard,
 *     `count > 0 && arg0 >= 0`) takes the weighted count only 3 -> 4,
 *     `floor_log2 * n` 3 -> 8, a TIE against the base's 8 -- and DOES NOT flip,
 *     because the parameter is born one insn earlier so L_param > L_base and
 *     8/L_param < 8/L_base.  The tie-break never gets reached.
 *   - a FIFTH long-lived value crossing the call, with the parameter's
 *     references unchanged, does not flip it either.
 * So the escape is now stated sharply: THE PARAMETER NEEDS ONE MORE REFERENCE
 * INSIDE THE LOOP, worth 2, and it must cost no instruction.  Nothing found so
 * far does; that is the open question and it is a much smaller one than
 * "four references".
 *
 * MEASURED INERT THIS ROUND, all exactly 23 of 30 and byte-identical to this
 * body -- 17 spellings, and note the last six are the shape the reference's
 * `mov r8, r5` actually has, a second name for the buffer address whose COPY
 * SITS BEFORE THE CALL so the first name does not die at it and regmove's
 * optimize_reg_copy_1 cannot coalesce.  gcse/cse collapse them anyway:
 *   the array passed straight to the call, keeping `base` for the loop only
 *   the compare's operands swapped
 *   `v - arg0 != 0` and `(v ^ arg0) != 0` (both fold back to one `cmp`)
 *   the subtraction named
 *   `arg0 = arg0;` ahead of the call (the batch-336 parameter-in-place lever)
 *   q = base before the call, q = buf before the call, the call taking q
 *   base kept live past the loop
 *   q = base + 0
 * MEASURED WORSE: `base` defined inside the guard (26); the loop indexing `buf`
 *   directly (29).
 * AUTOSWEEP (batch 342): flag axis -- `-ffixed-r7` 19 (see (1) above), `-O1`
 *   and `-fno-schedule-insns2` 24, the other five inert at 23; anchor axis --
 *   10 of 10 boundaries, ALL inert or worse.  BOTH AXES ARE CLOSED.
 *
 * NEXT: a free in-loop reference to `arg0`, or a body with three call-crossing
 * values rather than four (which would also make `-ffixed-r7` affordable).
 * Nothing else in the allocator is reachable from here -- the priority order
 * `;; 6 regs to allocate: 38 36 35 37 33 32` in `.18.greg` is identical across
 * every one of the 17 inert spellings.
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
