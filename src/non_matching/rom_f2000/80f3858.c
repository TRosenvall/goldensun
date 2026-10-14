/* ===== BATCH 350 BRIEF B -- 2 of 29 RE-DERIVED AND HELD.  BODY UNCHANGED.
 * ===== THREE AXES CLOSED WITH MECHANISM, AND THE PARK'S RELOAD EVIDENCE IS
 * ===== CORRECTED (its CONCLUSION survives; the dump it cites does not show it).
 *
 * NON-MATCHING, 2 differing encodings of 29  (MEASURED THIS BATCH).
 * ref 29 / ours 29, 16-bit instructions 26 / 26, dsize 0, relocations
 * IDENTICAL, no SIZE line, no POOL WORD line, first differing index 9.
 * All three length dimensions in phase, so the 2 IS A TRUE DISTANCE.
 * PINS 0.  DEVICE-FREE.  NO FLAG GROUP.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f2000/80f3858.c \
 *     asm/rom_f2000/rom_f2028_c_c_c_a_a.s --func Func_80f3858
 *
 * SPLIT SHAPE (tools/piece.py, verified this batch): the reference .s holds ONE
 * function and it is the only parked member, so NO SPLIT is needed and none is
 * in the manifest.  Install path when it lands:
 * src/rom_f2000/rom_f2028_c_c_c_a_a.c.
 *
 * THE RESIDUE, for the record -- one adjacent swap at indices 9/10:
 *   ref   ... add r1,#1 / mov r2,#0    / strb r5,[r3] / add r3,r4,r1 / strb r2,[r3]
 *   ours  ... add r1,#1 / strb r5,[r3] / mov r2,#0    / add r3,r4,r1 / strb r2,[r3]
 *
 * ===========================================================================
 * 1.  THE NEGATIVE-LIST RE-TEST: 36 SPELLINGS, 9 DISTINCT PROGRAMS (75% FLAT)
 * ===========================================================================
 * tools/sweep_variants.py with object fingerprinting, two rounds
 * (scratch_elev/b350/B/v3 and v3b; generators in that directory).
 *
 * ROUND 1, zero-BETWEEN (this body): 20 rows, 5 DISTINCT PROGRAMS.
 *   BIT-IDENTICAL TO THE BASE -- these 16 rows measured NOTHING:
 *     `register int zero`; `zero` declared before `p`; `zero` in an inner
 *     scope; the three offsets as `(0x80<<3)/(0x80<<5)/(0xe0<<5)`;
 *     `zero` as `char` / `unsigned char` / `short` / `unsigned int` / `long`
 *     (the park only ever crossed TYPE with zero-BEFORE -- TYPE x zero-BETWEEN
 *     was an uncrossed dimension and it is a NON-AXIS, all five are one
 *     program); a dead `zero = 0;` AFTER the second store; the three
 *     statements as one comma expression; an early-`return` guard instead of
 *     the `if`; a one-trip `while (p != 0) { ...; break; }`.
 *   DISTINCT:
 *     `zero = 0;` before the first store AND between  7, first index 14
 *     `int zero = 0;` at the declaration + between    8, dsize +4
 *     the SECOND store's address named                13, first index 7
 *     the same named outside the `if`                 25, first index 0
 *
 * ROUND 2, zero-BEFORE: 16 rows, and 11 ARE BIT-IDENTICAL TO EACH OTHER.
 *   ONE PROGRAM at 7, first index 14:  the plain zero-before control;
 *     `zero` as `short` / `unsigned short` / `unsigned int` / `long`;
 *     the first store's address as `p[0x3000+1]`, `(p+0x3000)[1]`,
 *     `*(p + 0x3001)` and `p[(0x600<<3)+1]`; `unsigned char *p`;
 *     the FIRST store's address named; arg `a` named.
 *   DISTINCT:  both store addresses named 9; arg `b` named 6 (reproduces the
 *     park's `bac`/`b` row); a third store 24 RELOCDIFF (wrong program);
 *     `zero = 0` above `p = ...` 8 and one instruction long.
 *
 * >> THE ZERO-BEFORE FAMILY IS AN ABSORBING STATE.  Eleven of sixteen rows are
 * >> one program, and the five that are not include the only two the park had
 * >> already found.  METHOD.md's rule applies: when variants go flat in bulk,
 * >> stop looking for a lever in that family.  The park's whole "INERT at 7"
 * >> list is ONE PROGRAM, so its length was never evidence of coverage. <<
 *
 * ===========================================================================
 * 2.  THREE AXES CLOSED, EACH WITH A MECHANISM
 * ===========================================================================
 * (a) THE STATEMENT-FORM AXIS IS CLOSED.  This is batch 349's `Field_Halt`
 *     escape -- vary the KIND of construct, not the spelling -- and it does
 *     not apply here.  Comma-sequencing the three statements, an early-`return`
 *     guard and a one-trip `while` are ALL BIT-IDENTICAL to the base: `jump.c`
 *     normalises the three CFGs and the comma's sequence point does not change
 *     expand order.
 *
 * (b) ROUTE 3 (add a definition so the live range splits) COLLAPSES INTO
 *     zero-BEFORE.  `zero = 0;` before the first store AND between them reads
 *     7 of 29 at first index 14 -- the park's own zero-BEFORE figure and
 *     shape.  Both definitions are the same constant, so cse keeps the EARLIER
 *     one and the program simply IS zero-before.  And a dead `zero = 0;` AFTER
 *     the live one is bit-identical to the base.
 *       >> ROUTE 3 NEEDS THE TWO DEFINITIONS TO CARRY DIFFERENT VALUES.  It
 *       >> paid on Func_80b6d30 because there the second definition was
 *       >> `j = ret` against a first `j = 0`; a DUPLICATE definition of one
 *       >> constant is not a second definition once cse has run.  Worth
 *       >> carrying: METHOD.md states route 3 as "the compiler reacts to the
 *       >> existence of a second definition, nothing else", and that is too
 *       >> strong -- it reacts to a second definition that SURVIVES cse. <<
 *
 * (c) TYPE x zero-BETWEEN is a non-axis (five widths, one program).
 *
 * ===========================================================================
 * 3.  THE PARK'S RELOAD DIAGNOSIS: CONCLUSION RIGHT, EVIDENCE WRONG
 * ===========================================================================
 * The park says the zero-BEFORE argument block breaks because
 * `allocate_reload_reg` (`reload1.c:4995-5064`) hands the `0x400` temp r3
 * instead of r2.  THE CONCLUSION IS CORRECT.  The dump it points at does not
 * show it, and a reader who checks there will conclude the park is wrong.
 *
 * `.18.greg` prints the SAME choices for the three constant adds in BOTH
 * bodies -- `Spilling for insn 46/50/54.  Using reg 3` three times each.  Read
 * there, the two bodies are indistinguishable.  The divergence is in
 * `.19.flow2`, in the reload-GENERATED insns:
 *
 *   insns 110/111, the 0x400 = 0x80<<3 temp feeding insn 46:
 *     zero-BETWEEN (fig 2):  (set (reg:SI 2 r2) (const_int 128)) / ashift 3
 *     zero-BEFORE  (fig 7):  (set (reg:SI 3 r3) (const_int 128)) / ashift 3
 *   insns 112/113 (0x1000) and 114/115 (0x1c00): r3 in BOTH bodies.
 *
 * EXACTLY ONE RELOAD REGISTER DIFFERS, and it is the 0x400 temp's.  With two
 * registers sched2 has two independent chains and interleaves them into the
 * reference's `mov r3 / lsl r3 / mov r2 / add r1 / mov r3 / lsl r2 / lsl r3 /
 * add r0 / add r2`; with one register the three sequences are a chain and no
 * scheduling can interleave them.  The park's reading of the CONSEQUENCE is
 * exact; only its citation is not.
 *   >> This is METHOD.md's "find_reg's printed choice is nearly
 *   >> information-free" (`reload1.c:3611-3623`) in its worst form: here it
 *   >> prints `reg 3` for the very reload that comes out r2.  READ `.19.flow2`,
 *   >> NOT `.18.greg`'s "Using reg" LINES. <<
 * The one greg print that DOES differ is the FIRST STORE's address reload:
 *   zero-BETWEEN: `Spilling for insn 25.  Using reg 2`
 *   zero-BEFORE:  `Spilling for insn 28.  Using reg 1`
 * so if a handle exists it is on which register is made spillable for
 * `p + 0x3001`, not on anything in the argument block.  Measured: eight
 * spellings of that address are one program with the zero-before control.
 *
 * AND NO ALLOCATION-PRIORITY LEVER CAN REACH THIS RESIDUE.  `.17.lreg` is
 * IDENTICAL in the two bodies across the whole argument block: pseudos 45, 47
 * and 49 are `p+0x400`, `p+0x1000` and `p+0x1c00`, expanded in source order
 * a,b,c and copied to r0/r1/r2 at insns 56/58/60, with dispositions
 * `45 in 0  47 in 1  49 in 2` on both sides.  `.18.greg` has only "2 regs to
 * allocate: 33 32", so every argument pseudo is local-alloc's and is already
 * in its final hard register before reload.
 *   >> THERE IS NO PSEUDO TO RE-RANK, so METHOD.md's routes 1-5 (delete a
 *   >> reused local, move a definition earlier or later, split a range, cross
 *   >> a block boundary, lengthen a range to prune a preference) ALL have
 *   >> nothing to act on here.  The only free variable is a reload register
 *   >> choice, and the source space above does not reach it. <<
 *
 * ===========================================================================
 * 4.  THE SCHEDULER RUNGS, RE-READ FROM SOURCE THIS BATCH
 * ===========================================================================
 * `rank_for_schedule` (`haifa-sched.c:4040-4116`) in full, against insn 28
 * (`strb r5,[r3]`) and insn 31 (`r2 = 0`):
 *   :4042-4044  priority -- TIES.
 *   :4047-4049  INSN_REG_WEIGHT -- gated `!reload_completed`, and sched2 runs
 *               AFTER reload, so THIS RUNG IS UNREACHABLE IN sched2 AT ALL.
 *   :4052-4068  the three interblock rungs -- gated
 *               `INSN_BB (tmp) != INSN_BB (tmp2)`; both insns are in block 1.
 *   :4069-4094  class vs the last-scheduled insn -- both class 3, because the
 *               rung assigns class 3 whenever `insn_cost (...) == 1` and a
 *               Thumb mov is `core_cycles single`, latency 1.
 *   :4097-4110  dependent count -- TIES AT 2 EACH.  28's are insn 38
 *               (`r3=r4+r1`, ANTI on r3) and insn 41 (memory); 31's are insn
 *               41 (true, r2) and insn 110 (`r2=0x80`, OUTPUT on r2).  Both
 *               counts are forced by the reference's own text.
 *   :4113-4116  INSN_LUID, lowest first -- DECIDES.
 * So the figure needs LUID(`mov r2,#0`) < LUID(`strb r5`), i.e. zero-BEFORE,
 * which costs the 0x400 temp's reload register.  The park's rung walk is
 * CONFIRMED; note it had skipped the reg-weight and interblock rungs, which is
 * harmless here because both are structurally unreachable -- but say so, since
 * batch 329 was burned by a park that omitted a rung it needed.
 *
 *   >> PARK HOLDS AT 2 of 29.  THE FLOOR FOR zero-BETWEEN IS 2 AND FOR
 *   >> zero-BEFORE IS 6, and both are now backed by bit-identity sets rather
 *   >> than by "nothing we tried worked".  WHAT WOULD RETIRE IT: a
 *   >> count-neutral way to change which register `allocate_reload_reg` hands
 *   >> insn 46's constant under zero-BEFORE.  DO NOT re-screen placement,
 *   >> type, offset spelling, statement form or a duplicate zero definition --
 *   >> all five are measured bit-identical sets. <<
 *
 * -- scratch_elev/b350/B
 * ===========================================================================
 */
/* Func_80f3858 (BeginPaletteFade) -- NON-MATCHING.
 *
 * NON-MATCHING, 2 differing encodings of 29  (MEASURED, batch 329 brief J;
 * was 8 of 29).  Counts now AGREE (ref 29, ours 29) and objcmp prints no SIZE
 * and no POOL WORD line, so the inherited 4-byte size excess and the surplus
 * pool word are both GONE.  First differing index 9.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_f2000/80f3858.c asm/rom_f2000/rom_f2028_c_c_c_a_a.s --func Func_80f3858
 *
 * ----- everything below to the next ===== heading is the INHERITED park text,
 * ----- kept verbatim; its figure line, its MISALIGNMENT reading and its recipe
 * ----- are all superseded by the two lines above.
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: CONSTANT DERIVATION, and it is the MIRROR IMAGE of
 * src/non_matching/ovl_7ac2d8/200adcc.c.
 *
 * 28 lines against the ROM's 28, 7 differing, all in the two byte stores.
 *
 *     rom   ldr r1, =0x3001 / add r3, r4, r1 / add r1, #1  ... add r3, r4, r1
 *     ours  ldr r2, =0x3001 / add r3, r4, r2 ... ldr r3, =0x3002 / add r2, r4, r3
 *
 * The ROM keeps 0x3001 in a register and DERIVES 0x3002 by adding one.  gcc
 * takes a second pool entry instead.
 *
 * Worth pairing with 200adcc, where the same class runs the other way: there
 * gcc derived 0x50000c4 as 0x50000ce - 0xa and the ROM took the second pool
 * entry.  So gcc-2.96 is not uniformly more or less willing to derive one
 * near constant from another than the ROM's compiler was; the two disagree in
 * both directions, on the same kind of address pair, a few hundred bytes
 * apart in the same game.  That rules out a single flag as the explanation
 * for either, and it is why neither park proposes one.
 *
 * Tried:
 *   - `p[0x3001]` and `p[0x3002]` as plain subscripts:  7 differing
 *   - a named `off` variable incremented between the stores, which is the
 *     "name the OFFSET, not the base" lever and is exactly the ROM's shape:
 *     20 differing and a stream two lines SHORT.  gcc constant-folds `off++`
 *     before it ever reaches register allocation, so the lever cannot express
 *     a runtime increment of a compile-time offset.  This is the useful
 *     negative: that lever moves which VALUE is named, never whether the
 *     addition survives to runtime.
 *   - --no-rerun-cse (7, unchanged), --O1 (18), --no-sched2 (18).
 *
 * The rest of the function -- the three scaled base additions feeding
 * Func_80f2ebc -- is byte-exact, including the interleaved lsl/add pairs.
  *
 * BATCH 205 -- WRITING THE DERIVATION EXPLICITLY IS MUCH WORSE, not better.
 *
 * This park calls itself the mirror image of ovl_7ac2d8/200adcc.c: there the
 * ROM loads two pool constants and gcc derives one from the other, here the
 * ROM derives (`add r1, #1`) and gcc loads two. The natural move is therefore
 * to write the derivation into the source, and docs/elevation.md records that
 * deriving is reachable when the first value is genuinely CONSUMED before the
 * increment -- which it is here, `add r3, r4, r1` precedes `add r1, #1`.
 *
 * It does not work. Both spellings measured:
 *
 *     off = 0x3001; p[off] = frames; off += 1; p[off] = 0;   26 lines, 20 differing
 *     the same with `off++`                                  26 lines, 20 differing
 *
 * against 28 lines and 7 differing for the two plain literal indices kept
 * below. The function comes out TWO INSTRUCTIONS SHORT, so gcc is not merely
 * ordering things differently -- naming the offset lets it fold both stores
 * through one address computation and drop work the ROM does.
 *
 * So the mirror framing does not carry a lever with it. The reachable-derivation
 * rule is about a value that must survive as a VARIABLE across its uses; here
 * naming it gives gcc a strength-reduction opportunity it takes, and the ROM's
 * `add r1, #1` is a consequence of how it held the offset rather than something
 * the source asked for.
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        p[0x3002] = 0;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
 * ===== TWO THINGS THE PARK ASSERTED, BOTH REFUTED. =====
 *
 * (1) "COUNT DIFFERS (ref 29, ours 30) -- so this positional figure measures
 * MISALIGNMENT, not distance."  It did not.  objcmp on the park body reports
 * `POOL WORD COUNT ref 3, ours 4 (instructions AGREE at 26)` and says in terms
 * that the instruction streams are the same length and the difference is pool
 * CONTENT.  The 8 was a distance all along.  This is another sighting of the
 * pool/padding trap INSIDE a standing park header; read objcmp unfiltered.
 *
 * (2) "BATCH 205 -- WRITING THE DERIVATION EXPLICITLY IS MUCH WORSE."  True, and
 * beside the point: the derivation did not need writing.  The park's blocker was
 *
 *   rom   ldr r1,=0x3001 / add r3,r4,r1 / add r1,#1 / mov r2,#0
 *         / strb r5,[r3] / add r3,r4,r1 / strb r2,[r3]
 *   park  ldr r2,=0x3001 / add r3,r4,r2 / strb r5,[r3] / ldr r3,=0x3002
 *         / add r2,r4,r3 / mov r3,#0 / strb r3,[r2]
 *
 * and it is now exact as far as index 8 -- `add r1,#0x1` included -- with the
 * two offsets still written as the plain literals `p[0x3001]` and `p[0x3002]`.
 * gcc DERIVES the second constant from the first on its own.  The one change is
 * a named `int zero` whose assignment sits BETWEEN the two stores.
 *
 * So the park's "mirror image of ovl_7ac2d8/200adcc.c" framing led the search
 * at the constant, and the constant was never the free variable.  Whether gcc
 * derives 0x3002 from 0x3001 or takes a second pool entry is decided by what
 * else is live across the first store, not by how the offsets are spelled.
 * Same lever, same batch, same shape as OvlFunc_common1_15b8 (6 -> 0).
 *
 * ===== THE RESIDUE AT 2 IS ONE ADJACENT SWAP, AND IT IS A TIE-BREAK. =====
 *
 *   rom   ... add r1,#0x1 / mov r2,#0x0 / strb r5,[r3,#0x0] ...
 *   ours  ... add r1,#0x1 / strb r5,[r3,#0x0] / mov r2,#0x0 ...
 *
 * Both insns sit at scheduling priority 0 -- arm_adjust_cost returns 0 for
 * anti/output deps (arm.c), and a store whose only dependent is an anti dep has
 * priority 0 -- so sched2 falls through to INSN_LUID, i.e. source order.  The
 * ROM's order therefore needs `zero = 0;` BEFORE the first store.
 *
 * AND THAT IS THE TENSION, measured both ways:
 *   zero = 0 BEFORE the first store  -> store block EXACT (indices 0..13 all
 *       match, `mov r2,#0` included) but the Func_80f2ebc argument block breaks:
 *       7 of 29, first index 14.  The ROM interleaves two shift chains
 *       (r3 for 0x1000, r2 for 0x400); with `zero` live into that region gcc
 *       serialises all three through one temp.
 *   zero = 0 BETWEEN the stores      -> argument block EXACT, store block one
 *       swap short: 2 of 29.  THIS BODY.
 * Naming the three argument pointers in the ROM's computation order (b, a, c)
 * recovers one of the seven: 6 of 29.  Still worse than 2.
 *
 * MEASURED, zero's PLACEMENT is the whole lever and its TYPE is irrelevant:
 *   int/char/unsigned char zero, assigned inside the if before the stores: all
 *   7 of 29, first index 14, identical.
 *   `int zero = 0;` at the declaration, or assigned before `p = ...`, or
 *   between `p = ...` and the if: all 7 of 29 but first index 6 -- i.e. the
 *   derivation is NOT recovered.  It has to be inside the if.
 *   Declaration order (zero before p, or zero in an inner scope) and
 *   `register int zero`: all 2 of 29, byte-identical to this body.
 *   --no-rerun-cse 2 (no change); --no-sched2 13 of 29.
 * MEASURED WORSE: pointer increment (`q = p + 0x3001; *q = frames; q++;`)
 *   27 lines, 23 differ; `off = 0x3001; ... off++;` with a named zero 26 lines,
 *   20 differ (the park's own result, reproduced); the two stores swapped in
 *   source 12 of 29.
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;
    int zero;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        zero = 0;
        p[0x3002] = zero;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
  * ===== BATCH 336: 2 OF 29 CONFIRMED, AND THE DIAGNOSIS IS NOW SPECIFIC. =====
 *
 * Re-measured, not inherited: 2 of 29 in STREAM terms (ref 29, ours 29), first
 * differing index 9, no SIZE and no POOL WORD line.  Pin-free.  Body unchanged.
 * The residue is the adjacent pair at 9/10, exactly as read above.
 *
 * (1) WHICH RUNG DECIDES -- the scheduler's own dump, not an inference.
 * `-fsched-verbose=4`, block 1, insn 28 = `[r3]=r5`, insn 31 = `r2=0x0`:
 *
 *   ;;	Ready list (t =  4):    31  28
 *   ;;		--> scheduling insn <<<28>>> on unit core
 *
 * The section above attributes this to "both insns sit at scheduling priority 0
 * ... so sched2 falls through to INSN_LUID".  THE PRIORITY CLAIM IS WRONG AND
 * THE LUID CONCLUSION IS RIGHT FOR A DIFFERENT REASON.  Walking the dependence
 * graph (rank_for_schedule, haifa-sched.c:4040-4114):
 *   insn 28 (`strb r5,[r3]`) has TWO dependents -- insn 38 (`r3=r4+r1`, ANTI on
 *     r3) and insn 41 (`strb r2,[r3]`, memory).
 *   insn 31 (`r2=0x0`) also has TWO -- insn 41 (true, r2) and insn 110
 *     (`r2=0x80`, the first argument temp; an OUTPUT dependence on r2).
 * So rung 5 (depend_count) TIES AT TWO EACH and the fall-through to rung 6,
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)`, is what decides.  Both counts are
 * forced by the reference's own text -- the second address is recomputed into
 * r3 in the ROM too, and arg c lands in r2 -- so THE ONLY WAY TO WIN IS A LOWER
 * LUID FOR THE ZERO, i.e. the original source assigned it BEFORE the first
 * store.  That is now an inference about the original, not just a probe result.
 *
 * (2) WITH ZERO-BEFORE THE STORE BLOCK IS EXACT, as recorded above (7 of 29,
 * first index 14, indices 0..13 byte-identical with `movs r2,#0` at 9).
 *
 * (3) AND THE ARGUMENT BLOCK'S BREAK IS A RELOAD REGISTER CHOICE, NOT A
 * SCHEDULE.  `-fno-schedule-insns2` on both bodies shows it directly:
 *
 *   body                        0x400 temp  0x1000  0x1c00  post-sched
 *   zero BETWEEN (this body)        r2        r3      r3     the ROM's
 *                                                            interleave, exact
 *   zero BEFORE (7 of 29)           r3        r3      r3     serialised
 *
 * With two registers sched2 has two independent chains and interleaves them
 * into the reference's `mov r3 / lsl r3 / mov r2 / add r1 / mov r3 / lsl r2 /
 * lsl r3 / add r0 / add r2`.  With one register the three sequences are a chain
 * and NO scheduling can interleave them.  The register is chosen by
 * allocate_reload_reg (reload1.c:4995-5064), which ROUND-ROBINS A FUNCTION-WIDE
 * CURSOR: `i = last_spill_reg;` at :5003, advanced and stored at :4937, reset
 * to -1 only once per reload pass at :821.  So the temp's register depends on
 * how many reload registers were allocated EARLIER IN THE FUNCTION, and on
 * nothing local to the argument block.
 *
 * CORRECTION TO THE SECTION ABOVE: it says the argument block breaks because
 * "with `zero` live into that region gcc serialises all three through one
 * temp".  `zero` IS NOT LIVE INTO THAT REGION -- in both bodies it dies at
 * `strb r2,[r3]`, several insns before the first temp.  The observation
 * (serialisation) is sound; the cause is the reload cursor.
 *
 * (4) EVERYTHING SWEPT TO MOVE THAT CURSOR, zero-before throughout:
 * INERT at 7 (first index 14, identical): `char`, `unsigned char`, `short`,
 *   `unsigned int`, `long` for `zero`; `zero` in an inner scope; a local copy
 *   of `frames`; `unsigned char *p`; `&p[0x400]`-style arguments; and the three
 *   offsets written `(0x80<<3)`, `(0x80<<5)`, `(0xe0<<5)` instead of literals,
 *   which is EXACTLY inert at every naming -- the constant spelling is not a
 *   lever here at all.
 * NAMED ARGUMENT POINTERS, all six assignment orders x both offset spellings:
 *   bac 6, abc 7, bca 18, cba 19, acb 21, cab 21.
 * PARTIAL NAMING, every one- and two-element subset in every order (an
 *   uncrossed dimension before this batch): b 6, ba 6, a 7, ab 7, bc 18,
 *   cb 19, ac 21, c 21, ca 21.
 * WORSE: the three arguments computed before the stores 21/21/22; named store
 *   pointers (`q = p + 0x3001; *q = ...`) 23; `zero = 0` before `p = ...` 8
 *   AND ONE INSTRUCTION LONG.
 *
 * SO 6 IS THE FLOOR FOR ZERO-BEFORE AND THIS BODY'S 2 IS THE FLOOR FOR
 * ZERO-BETWEEN.  NEXT: the only live direction is shifting
 * allocate_reload_reg's cursor so the 0x400 temp lands in r2 under zero-before
 * -- which needs the count of reload-register allocations before the argument
 * block to change by one, with the instruction count unchanged.  Nothing in the
 * source space above does that.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 29, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: ALL 5 PLACEMENTS WERE BIT-IDENTICAL TO THE BASE,
 *   so THIS AXIS MEASURED NOTHING.  gcc canonicalises every one of these
 *   spellings away before the back end.  That is NOT a negative and must
 *   not be read as one -- the anchor lever is UNTESTED here.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;
    int zero;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        zero = 0;
        p[0x3002] = zero;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
