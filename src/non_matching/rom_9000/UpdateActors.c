/* UpdateActors  --  0x0800cacc  --  TRIAGE PARK, NO CANDIDATE (batch 313, brief B)
 *
 * NON-MATCHING, NO FIGURE CLAIMED.  No candidate was written, so there is no
 * objcmp number and none is implied anywhere below.  This park exists to record
 * four measurements that change how this function should be briefed, and to
 * retire a triage premise that put it in the wrong population.
 *
 * Verify (once a candidate exists) with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/UpdateActors.c \
 *     asm/rom_9000/rom_ca6c_a_c_c.s --func UpdateActors
 *
 * ===================== SHAPE, SPLIT, FRAME, VENEER =======================
 * 683 instructions, 64 labels, 110 branch instructions -- so ~6 instructions
 * per basic block.  This is the BRANCH-DENSE population of band-800plus.md s1,
 * which wants the ordinary 500-instruction lever set; none of that document's
 * straight-line constant-reuse material applies.
 *
 * SPLIT SHAPE: NONE.  `grep -c thumb_func_start` on the reference is 1.
 * tools/datacheck.py is SILENT on the reference.
 *
 * FRAME, ALL FOUR GREPS -- and the fourth corrects the brief's table:
 *   g1  `sub sp, #0x30` / `add sp, #0x30`  -- 48 bytes, under the 508 cap
 *   g2  `(add|sub) sp, rN`                 -- ZERO, no register-built frame
 *   g3  `mov rX, sp` ZERO; `add rX, sp, #K` ONE (`add r1, sp, #0x24`)
 *       -> exactly ONE stack aggregate, and it is the `add rX,sp,#K` idiom,
 *          not `mov rX,sp`.  Count both greps or this one is invisible.
 *   g4  `str rX, [sp]` 2  AND  `ldr rX, [sp]` 3  -- *** PAIRED ***
 * The brief's table lists `sp0 2`, which reads as outgoing argument space for a
 * 5-or-more-argument call.  IT IS NOT.  Both stores are paired with loads (and
 * there is a third load), so sp+0 is a SPILL SLOT.  Pairing stores against
 * loads is the discriminator, and on this function it flips the reading.
 *
 * VENEER -- CONFIRMED AT THE DOCUMENTED FIGURE, scoped and DOT-ANCHORED
 * (`^[[:space:]]*\.call_via`), which is the only way to count it:
 *     r7 x16   r8 x6   r10 x6   r4 x2   r3 x1   =  31 SITES, FIVE REGISTERS
 * The same range separately carries 11 `bl _call_via_rN` (r3 x8, r11 x2, r2 x1)
 * which are gcc's OWN output for an indirect call and must NOT be added in.
 * This reproduces docs/elevation.md's corrected count exactly, against the
 * batch-309 recon's 11-sites-over-3-registers figure, which was a `bl` count.
 *
 * ============== THE TRIAGE PREMISE THAT PUT THIS IN BRIEF B IS FALSE ======
 * The brief selected this function as one of the four highest wide-constant
 * reuse fractions in the tree, read off 98 high-register mentions, and placed
 * it in the "MIXED" population that the pin pass is supposed to be the open
 * question for.  IT IS NOT A CONSTANT-REUSE FUNCTION AT ALL.
 *
 * Partitioning its 58 `mov rlo,rhigh` copies by the ROOT of the value in the
 * high register (the instrument is described in the batch report and reproduces
 * docs/elevation.md's own hand-computed table for 20095dc / 2009410 / 20088ec /
 * 200b4c8 exactly):
 *
 *     root class                      count
 *     MEM (a plain load)                 24
 *     COMPUTED from a load               17
 *     COMPUTED from a load + a pool K    13
 *     arg/prologue                        4
 *     ------------------------------------
 *     PURE-CONSTANT ROOTED                0
 *
 * ZERO of 58 copies root at a constant.  r8/r9/r11 hold three COORDINATE
 * DELTAS -- `sub r0,r3,r1 / asr r0,#16 / mov r8,r0` and the two like it, built
 * from actor position fields at [r6,#0x38]/[0x3c]/[0x40] -- which are then
 * squared and summed for a distance test and reused for the rest of the body.
 * r7/r8/r10 separately hold VENEER CALLEE POINTERS.  So the 98 high-register
 * mentions measure high-register PRESSURE from long-lived computed scalars and
 * held function pointers, which is a different blocker with different levers.
 *
 * CONSEQUENCES, STATED SO THE NEXT BRIEF DOES NOT REPEAT IT:
 *   * THE PIN PASS HAS NO DOMAIN HERE.  There is nothing constant-rooted to
 *     pin.  Do not spend a round on it, selective or blanket.
 *   * The pooled multiset agrees and is the cheap confirmation: only 10
 *     distinct pool entries, of which FIFTEEN OF THE ~27 WORDS ARE CALLEE
 *     ADDRESSES (Func_8000888 x7, Func_80008ac x6, Func_8000948 x2).  The
 *     genuinely numeric entries are 0xffff x5, 0xffffff, 0xfffff000,
 *     0xfffc0000 -- eight words.  There is no wide-constant reuse to recover.
 *   * The live lever is LEVER 1 IN ITS POINTER/COUNTER FORM plus ordinary
 *     live-range work on the three deltas, i.e. the 500-instruction set.
 *   * `.L131c0` is read twice as an address and defined nowhere in the file:
 *     it is a GLOBAL VARIABLE, not a label, and needs
 *     `extern unsigned char L131c0[] __asm__(".L131c0");`.  `Data_8013624` is
 *     a second data symbol in the same range.
 *
 * ==================== WHY NO CANDIDATE WAS ATTEMPTED =====================
 * docs/elevation.md's veneer queue already says this function "is reachable but
 * expensive, and should not be a one-pass job" and recommends landing a
 * single-register main-ROM veneer function first to bank the technique.  That
 * advice is CONFIRMED by the counts above, and the brief's framing of this
 * target as one that merely "returned to the available pool after a retraction"
 * understates it: it is the LARGEST veneer job in the tree (31 of the 134
 * known sites), it needs FIVE helper variants, and two of the five binding
 * registers are HIGH (r8, r10, covering 12 of the 31 sites) and so need the
 * enclosing-block `register ... __asm__("rN")` form.  On top of that, r8 is
 * ALSO one of the three delta registers in a disjoint range, so the veneer
 * work and the live-range work collide in the same register.
 *
 * The retraction that returned this function to the pool is sound -- the
 * veneer reproduces byte-exactly and this park does not dispute it -- but the
 * retraction removed a WALL, not the COST.  Brief B spent its budget on depth
 * where depth was reachable, per its own instruction that depth on two beats
 * shallowness on four.
 *
 * WHAT TO BRIEF: take a 1-site or 2-site main-ROM veneer function first (nine
 * and nine of them respectively).  Then take this one with the veneer treated
 * as the WHOLE job for a round, and the delta live ranges as a separate round.
 */
