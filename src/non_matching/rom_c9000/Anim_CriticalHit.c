/* ===================== BATCH 327 (brief A) -- 15 -> 13, BODY CHANGED =========
 * Anim_CriticalHit -- NON-MATCHING, 13 differing encodings of 707 (was 15).
 * PIN-FREE, DEVICE-FREE, NO FLAG GROUP, NO fakematch ROW.
 *     XX ENCODINGS differ in 13 place(s) (ref 707, ours 707)
 *        first at index 78: ref 6812  ours 33bc
 * NO SIZE LINE AND NO RELOCATIONS LINE, so the 13 is a true distance.
 * aligncmp: aligned-equal 702 of 707 = 99.3%, 10 differing/ins/del in 9 hunks
 * (was 701, 12 in 11).
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/Anim_CriticalHit.c asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_CriticalHit
 * (`--whole` additionally reports Anim_Attack and BaseAnim_Attack "missing from
 * candidate" plus a SIZE and RELOCATIONS line.  THAT IS THE UNDONE SPLIT, NOT A
 * DEFECT: the .s holds three functions and this .c defines one.  Read the
 * per-function row.)
 *
 * -------- THE EDIT, AND IT IS ONE STATEMENT PLUS ONE DECLARATION -------------
 *     int msk;                       <- appended to the declaration list
 *     ...
 *     msk = 0xff;                    <- NEW, immediately BEFORE `p = base + K`
 *     p = (Part *)(base + (0xe1 << 7));
 *     do {
 *         ...
 *         p->vx = (Random() & msk) << 10;      -- was & 0xff, three times
 *         p->vy = (Random() & msk) << 10;
 *         p->vz = ((Random() & msk) - 0x7f) << 10;
 * It closes RUN 5 (idx 464/465) EXACTLY and moves nothing else.
 *
 * -------- WHY, AND THE RULE IS REUSABLE ACROSS THE WHOLE BANK ----------------
 * 0xff has three uses in that loop, so it is a loop-invariant MOVABLE and
 * move_movables hoists it with emit_insn_before (..., loop_start) -- AT THE END
 * OF THE PREHEADER, above every ordinary preheader statement.  `p = base +
 * (0xe1 << 7)` is an ordinary statement, so its `add r5,fp` had the LOWER LUID.
 * Both insns are region LEAVES (their consumers are inside the loop, a different
 * scheduling region), so priority = own cost = 1 for both, EVERY
 * rank_for_schedule RUNG TIES, and LUID ALONE DECIDES, LOWEST FIRST -- giving us
 * `add r5,fp` then `mov r7,#0xff`, and the ROM the reverse.
 * > GIVING A LICM-HOISTED LOOP CONSTANT ITS OWN SOURCE STATEMENT AHEAD OF THE
 * > LAST ORDINARY PREHEADER STATEMENT MOVES IT FROM move_movables' INSERTION
 * > POINT TO ITS WRITTEN POSITION.  THAT IS THE ONLY HANDLE ON A PREHEADER TIE,
 * > BECAUSE EVERY PREHEADER INSN WHOSE CONSUMER IS INSIDE THE LOOP IS A REGION
 * > LEAF AND PRIORITY CAN NEVER SEPARATE THEM.
 * > PRECONDITION: THE HOISTED QUANTITY MUST HAVE NO USE OUTSIDE THE LOOP.
 * > (AnimEnd's cause 2 is the same class and fails exactly this test -- its
 * > `b + (0xc9<<3)` is also used before the loop, so naming it folds the two
 * > computations and loses three instructions.  See that park.)
 *
 * -------- FULL DECOMPOSITION OF THE REMAINING 13: FOUR RUNS -----------------
 * Measured with a helper that IMPORTS tools/objcmp.py (never forks it) to print
 * ALL differing indices, not just the first:
 *     differ 13 at: 78 79 80 81 82 83 84 | 169 170 | 245 246 | 364 365
 *
 * RUN 1 (idx 78-84, SEVEN -- this header previously said FOUR).  The gPtrs block:
 *     ref   6812 ldr r2,[r2] | 920e str r2,[sp,#0x38] | 33bc add r3,#0xbc |
 *           466d mov r5,sp   | 681b ldr r3,[r3]       | 3538 add r5,#0x38 |
 *           9504 str r5,[sp,#0x10]
 *     ours  33bc 466d 6812 681b 3538 9504 920e
 *   Roles: ldr r2,[r2] + str r2,[sp,#0x38] = `d[0] = gPtrs[0x2e]` (d is at
 *   sp+0x38); add r3,#0xbc + ldr r3,[r3] = `f1 = gPtrs[0x2f]`; mov r5,sp +
 *   add r5,#0x38 + str r5,[sp,#0x10] = `fp = d`.
 *   *** THE SINGLE STRUCTURAL FACT: THE ROM ISSUES str r2,[sp,#0x38] IMMEDIATELY
 *   AFTER ITS LOAD (slot 2 of 7); WE DEFER IT TO SLOT 7 OF 7.  That store is
 *   insn 1771, a reload-created number, at prio 18 -- THE LOWEST PRIORITY IN THE
 *   WINDOW -- which is exactly why we sink it. ***  The previous reading
 *   ("prio(166)=21 beats prio(157)=20") is ONE DECISION INSIDE A SEVEN-ENCODING
 *   PERMUTATION whose real shape is the deferred store.  AIM AT prio(1771).
 *
 * RUN 2 (idx 169/170) -- `mov r7,#0` against `add r5,fp`, swapped.  THE SAME
 *   CLASS AS THE EDIT ABOVE, at the FIRST particle loop, where r7 holds the
 *   hoisted `0` for `p->y = 0; p->z = 0;` (confirmed from our own asm:
 *   `add r5,r5,fp / mov r7,#0 / .L11: ... str r7,[r5,#4]`).  The lever does NOT
 *   transfer.  MEASURED THIS BATCH, standalone and crossed with the msk edit:
 *     `zero` named, declared top-level, assigned before p        51 / 49  RELOCDIFF
 *     `zero` named, assigned before `i = 0`                      53 / 51  RELOCDIFF
 *     `zero` named, declared BLOCK-SCOPED in the crit block      53 / 51  RELOCDIFF
 *     block-scoped decl, assigned separately before p            51 / 49  RELOCDIFF
 *     `p = base + K` moved before `i = 0`, no new local          16 / 14
 *   Every naming form is a DIFFERENT PROGRAM (relocations differ), not a near
 *   miss.  The asymmetry against the msk edit is real and unexplained: msk is
 *   consumed as an `&` OPERAND, zero is consumed as a STORED VALUE, and only the
 *   operand form survives.  NEXT READER: that asymmetry is the question, and it
 *   is worth 2.
 *
 * RUNS 3 and 4 (idx 245/246, 364/365) -- each a pool-load-against-other-load
 *   ADJACENT SWAP:
 *     245/246  ref 0109 lsl r1,r1,#4 then 4862 ldr r0,[pc,#392]   ours swapped
 *              (the 4862/4863 difference is the pc displacement MOVING WITH it,
 *               not a different operand -- the earlier note is right)
 *     364/365  ref 4d24 ldr r5,[pc,#0x90] then 980d ldr r0,[sp,#0x34]  ours swapped
 *   Run 4 is the batch-310c "stack reload before the lsl, pool load after it"
 *   asymmetry; RUN 3 IS THE SAME ASYMMETRY FAILING AT A SECOND SITE, which makes
 *   it a class of two.  *** AND AnimEnd's POOL-LOAD RULE BOUNDS BOTH: a
 *   pc-relative pool load has no register inputs and an unchanging,
 *   privately-aliased MEM, so it can never acquire a new forward dependent and
 *   its priority is a fixed function of the program.  DO NOT SPEND ANOTHER ROUND
 *   TRYING TO RE-RANK THESE TWO THROUGH THE SCHEDULER. ***
 *
 * -------- FOUR NEW EXACTLY-INERT PREREQUISITES ON THE gPtrs BLOCK -----------
 * All at the base figure, all verified to be GENUINELY DIFFERENT OUTPUT (md5 of
 * the -S asm differs from base and from each other), so none is the "flat row
 * means the edit never happened" trap:
 *     a separate carrier `f0` for d[0], both stored through fp     INERT
 *     `d[0]=..; d[1]=..; fp=d;`  (the previously recorded prereq)  INERT
 *     `d[1] = f1;` instead of `fp[1] = f1;`                        INERT
 *     `f0 = gPtrs[0x2e]; d[0] = f0;` then the base shape           INERT
 *   WORSE: carrier on the FIRST pointer with d[1] direct 23; `fp` formed first
 *   with both stores through it and no `f1` 18.
 * *** AND THE CROSSING IS A CLEAN NEGATIVE: all four crossed with the msk edit
 * still read 13.  So the previous ask -- "give the d[0] chain an equal-length
 * tail" -- IS CONSTRUCTIBLE AND IS NOT RUN 1'S PARTNER.  Run 1's partner is the
 * deferred store (prio 18), not the carrier shape. ***
 *
 * -------- CORRECTION TO THIS PARK'S OWN FAMILY CLAIM ------------------------
 * THE CLAIM BELOW THAT NO `Anim_*` SOURCES HAVE LANDED IS FALSE, and it came
 * from me, not from this park's author.  **148 `Anim_*`/`BaseAnim_*` functions are
 * defined in landed sources** under `src/rom_c9000/` against 52 parked.  The error
 * was a FILENAME check standing in for a DEFINITION check: every landed file in
 * that bank is split-named (`rom_XXXXXX_*.c`), so a scan for `Anim_*.c` finds
 * none of them.  `Anim_Hail` is landed in `Anim_Venus`'s own upstream module and
 * `Anim_Confuse` in `AnimEnd`'s; `tools/upstream_module.py <Func>` prints the
 * landed, parked and still-asm siblings of any function's module.
 *
 * A false NEGATIVE is the expensive direction, because it tells the next reader
 * not to look.  Treat the paragraph below as retracted.
 *
 * This header says "THERE ARE NO LANDED Anim_* OR BaseAnim_* SOURCES ... all 46
 * named animations are parked".  *** THAT IS FALSE.  148 ARE LANDED in
 * src/rom_c9000/ against 52 parked. ***  The claim came from checking FILENAMES
 * (all split-named rom_XXXXXX_*.c) instead of definitions -- a name check
 * standing in for a definition check.  `tools/upstream_module.py
 * Anim_CriticalHit` prints the six landed module-mates of rom_e3958 unprompted.
 * A FALSE NEGATIVE IS WORSE THAN A FALSE POSITIVE, BECAUSE IT TELLS THE NEXT
 * AGENT NOT TO LOOK; this one suppressed the best available evidence for two
 * briefs across two batches.
 * =====================================================================================
 */
/* Anim_CriticalHit -- 0x080e40a4, 672 ROM instructions (707 encodings).
 *
 * NON-MATCHING, 15 of 707 encodings differ.   [batch 310c: was 19]
 *
 * MEASUREMENT -- THIS COUNT IS A TRUE DISTANCE.  SIZE IS EXACT (1612 bytes both
 * sides, objcmp prints no SIZE line) and the instruction COUNT IS EXACT
 * (707 / 707), so the 15 ranks directly.  First differing index 78.
 * aligncmp ranks within that, SEPARATELY:
 *
 *     aligned-equal 701 of 707 = 99.2%,  12 differing/ins/del in 11 hunks
 *     (was 697 of 707 = 98.6%, 16 differing in 12 hunks)
 *
 * RELOCATIONS: 84 rows both sides, AND THEY NOW MATCH EXACTLY -- objcmp prints
 * no RELOCATIONS line at all.  The two `_call_via_r5` / `_call_via_r6` rows at
 * 0x336 and 0x340 that were the whole recorded relocation discrepancy are
 * closed; see BATCH 310C below.
 *
 * Verify with (the delivered park body, runnable as written):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_CriticalHit.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_CriticalHit
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_CriticalHit.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s Anim_CriticalHit -v
 * Installed path is src/non_matching/rom_c9000/Anim_CriticalHit.c; substitute it
 * for the scratch path once this body replaces that file.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * register pin, no barrier, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * BATCH 310C -- THE SHARED BLOCKER IS CLOSED, AND IT CLOSED ON BOTH FUNCTIONS
 * FROM ONE CHANGE
 * ================================================================
 *
 * THE PAIR HYPOTHESIS HELD.  The recorded blocker here was explicitly "the same
 * blocker as Anim_Djinni's, in the same shape, on a DIFFERENT function".  One
 * three-token change, written once and applied verbatim to both, moved both:
 *
 *     int clen = 0x80 << 7;          <-- a new local, INITIALISED AT ITS
 *                                        DECLARATION
 *     ...
 *     cl(ctx, clen);
 *     cl((void *)0x6004000, clen);
 *
 *     Anim_Djinni       26 -> 16 of 738, relocations now EXACT
 *     Anim_CriticalHit  19 -> 15 of 707, relocations now EXACT
 *
 * > AN INT CARRIER INITIALISED AT ITS DECLARATION MAKES ITS PSEUDO LIVE FROM
 * > FUNCTION ENTRY, WHICH IS HOW YOU MAKE A QUANTITY LOSE A HARD REGISTER.
 * > allocno_compare's priority is log2(n_refs) * freq / LIVE_LENGTH, so a range
 * > starting at entry is the LONGEST range and the LOWEST priority.  The pseudo
 * > is allocated last, loses, and because its REG_EQUIV is a constant, reload
 * > REMATERIALISES it at each reference rather than spilling it.  That is the
 * > ROM's form.  The recorded reading -- "the reference's pseudo FAILS to get a
 * > hard register and reload rematerialises it" -- was RIGHT; what was missing
 * > was that the source handle is the INITIALISER POSITION, not the spelling of
 * > the constant.
 *
 * WHY THE FOUR RECORDED SPELLINGS READ INERT.  They were all short-range forms
 * -- `0x4000`, `0x80 << 7`, `n << 7` off a block-scoped local, two
 * separately-scoped locals.  A body assignment or a block-scoped initialiser
 * keeps the range SHORT, which RAISES the constant's priority: the exact
 * opposite of what is wanted.  Measured on Anim_Djinni in this batch: `int
 * clen;` declared with `clen = 0x80 << 7;` assigned in the epilogue is 26 with
 * the veneers still r6; the same assignment moved earlier (before either
 * `StopTask`) is also 26.  Only the declaration initialiser reaches it.
 * DECLARATION RANK IS FREE (before `cl`, after `cl`, first in the whole list --
 * all identical) and so is the SPELLING (`0x4000` identical).
 *
 * HUNK A IS NOW GONE.  Our clear-call block is byte-exact against the
 * reference, both calls, including the asymmetric argument-0 schedule: the ROM
 * puts a STACK RELOAD as argument 0 BEFORE the `lsl r1,#7` and a POOL LOAD as
 * argument 0 AFTER it, this function has one of each, and we match both.  That
 * asymmetry is the discriminator for Anim_Djinni's last two encodings, where
 * argument 0 is a pool load and we still hoist it.
 *
 * ================================================================
 * BATCH 325H -- HUNK 1 IS NOT A TIE-BREAK, IT IS A PRIORITY DIFFERENCE, AND
 * THE NUMBER IS READ OFF .23.sched2
 * ================================================================
 *
 * Reproduced exactly: 15 of 707, ref 707 / ours 707, no SIZE line, no
 * RELOCATIONS line, first differing index 78.  aligncmp: 701 of 707 aligned,
 * 12 differing/ins/del in 11 hunks.  Everything the park records about the
 * SHAPE of the residue reproduced; what follows corrects its MECHANISM.
 *
 * SCHED2 DID THE REORDERING AND THE CHOICE WAS DECIDED ON RUNG ONE.  Compiled
 * with `-da -fsched-verbose=6`, the eleven instructions of hunk 1 are basic
 * block 5 (`-- basic block 5 from 148 to 251 -- after reload`) and the whole
 * hunk turns on ONE decision:
 *
 *     ;;   Ready list (t =  4):    157  1917  166
 *     ;;           --> scheduling insn <<<166>>> on unit core
 *
 * with the block's dependence table giving
 *
 *     ;;      155     5   ...  prio 21  ...  : 251 157          <- d[0]'s address
 *     ;;      157   173   ...  prio 20  ...  : 251 195 194 1920 1771
 *     ;;      166     5   ...  prio 21  ...  : 251 168          <- f1's address
 *     ;;      168   173   ...  prio 20  ...  : 251 195 182 174 1920
 *     ;;     1917   173   ...  prio 20  ...  : 251 245 ... 171
 *
 * `rank_for_schedule` (haifa-sched.c) compares INSN_PRIORITY first and returns
 * on any difference, so `166` at 21 beats `157` at 20 before the CLASS rung,
 * the dependent-count rung or INSN_LUID is ever consulted.  The reference
 * schedules `157` there, so:
 *
 * > HUNK 1 IS NOT REACHABLE BY STATEMENT ORDER OR BY ANY OTHER LUID LEVER.
 * > It needs prio(157) >= 21, or prio(166) <= 20.  LUID already favours us
 * > (157 < 166); the priority overrides it.
 *
 * WHERE THE 21 COMES FROM, so the next reader can aim at it.  Priority is the
 * longest path to the block end.  `d[0] = gPtrs[0x2e];` has a TWO-insn tail
 * (load `157`, store `1771` at prio 18, cost 2 -> 20).  `f1 = gPtrs[0x2f];`
 * has a THREE-insn tail (address `166`, load `168`, store `174` at prio 18,
 * cost 2 -> 20, +1 -> 21) because the `f1` INTERMEDIATE puts an extra rung
 * under it.  The park's lever (3) -- naming `f1` -- bought the last
 * instruction and the exact size, and it is ALSO what gives `166` the extra
 * point of priority that loses the schedule.  Those are the two halves to
 * cross: a form that keeps `f1`'s instruction count and gives the `d[0]`
 * chain an equal-length tail.
 *
 * MEASURED THIS BATCH on the gPtrs block, all with relocations and size
 * checked, base 15:
 *     `d[1] = gPtrs[0x2f]; fp = d;` (no `fp[1]`)      15  EXACTLY INERT
 *                                                        -- candidate prereq
 *     `f1` read before `d[0]`                        533  +4 bytes, RELOCDIFF
 *     `fp = d;` between the two reads                619  +4 bytes, RELOCDIFF
 *     `fp = d;` first, both stores through `fp`      619  +4 bytes, RELOCDIFF
 *     `f1` reused as the carrier for `d[0]` too      531  +4 bytes, RELOCDIFF
 *     `f1` read, `fp = d`, then `d[0]`                18
 * The four large rows are not near-misses: each is a different program (one
 * more pool word and a relocation shift), so they rule the SHAPE out, not just
 * the figure.
 *
 * ONE CORRECTION TO THE BRIEF THAT SENT ME HERE.  The brief said this bank has
 * "155 landed sources ... full of named families, many already landed", and to
 * port levers from a landed `Anim_*` sibling.  THERE ARE NO LANDED `Anim_*` OR
 * `BaseAnim_*` SOURCES: all 155 `.c` files in `src/rom_c9000/` are split-named
 * `rom_XXXXXX_*.c`, and all 46 named animations (41 `Anim_*`, 5 `BaseAnim_*`)
 * are parked.  The reusable structure in this family is the PAIR relationship
 * this park and Anim_Djinni already record -- one three-token change moved both
 * in batch 310c -- not a landed-sibling port.
 *
 * ================================================================
 * BATCH 325H -- THE .18.greg RELOAD TRIAGE, AND A FAMILY-WIDE COUNT
 * ================================================================
 * `Using reg N for reload M` in `.18.greg` is `find_reg`'s decision
 * (reload1.c:1664, inside find_reg at :1588), find_reg reads `REG_ALLOC_ORDER`
 * explicitly (:1645-1662) with `inv_reg_alloc_order` breaking equal-`spill_cost`
 * ties, and `choose_reload_regs_init` (:5129) leaves one bit -- so one reload on
 * an insn means no freedom.  ONE `Using reg` per block => REG_ALLOC_ORDER blamed
 * CORRECTLY, act on `spill_cost` (the live set).  TWO OR MORE, or inheritance
 * => a round-robin cursor reading applies.  NONE => an ALLOCNO question.
 *
 * This function: 162 blocks -- 75 with no reload, 75 with one, 12 with two,
 * ZERO `Reusing reg`.  HUNK 1's insns (155, 157, 166, 168) have NO RELOAD AT
 * ALL, so they are not a reload question in any form; the answer above is a
 * sched2 PRIORITY and not a register.  The two hunk-1 blocks that do reload
 * (insns 171 and 174) carry exactly one each, both `Using reg 2` -- case 1, so
 * if either is ever suspected, change the LIVE SET at that insn, not a spelling.
 *
 * FAMILY COUNT, because it is reusable and this bank is where it matters:
 * TWELVE `Anim_*` parks cite `REG_ALLOC_ORDER` and every one is in rom_c9000.
 * Compiled all twelve plus `d5c48_Curse` with `-da` and classified `.18.greg`:
 * 2+-reload insns are 0% to 10% of blocks (median ~2%), and there is **ZERO
 * reload inheritance in any of the fifteen functions measured in this bank** --
 * not one `Reusing reg` line in ~2,100 `Spilling for insn` blocks.
 * `Anim_Ice` and `d5c48_Curse` have NO multi-reload insn in the whole function,
 * so their citations can only be case 1 or case 3; `Anim_Venus` has 2 of 87
 * blocks and `Anim_Mars` 1 of 84.  Per-function figures are in
 * scratch_elev/b325/H/FINDINGS.md section 5.
 *
 * > IN THIS BANK THE CURSOR READING IS THE HYPOTHESIS OF LAST RESORT, NOT THE
 * > FIRST.  But run the triage on the SPECIFIC INSN behind the rotation, not on
 * > the function: Anim_Djinni's hunk 1 sits on one of the rare 2+ insns.
 * The counts are measured on OUR candidate bodies, not the reference's TU.
 *
 * ================================================================
 * THE REMAINING 15 -- ELEVEN HUNKS, EVERY ONE A SINGLE-SLOT TRANSPOSITION
 * ================================================================
 *
 * Twelve differing positions in eleven hunks, and every hunk is ONE instruction
 * moved by ONE slot.  Same instructions, same registers, same roles, same
 * immediates:
 *
 *   ref[78:80]    `ldr r2,[r2] / str r2,[sp,#0x38]` two slots early in ours
 *                 -- the gPtrs block, the same class as Anim_Djinni's hunk 1,
 *                 and 4 encodings here against 12 there.
 *   ref[170]      `add r5, fp` one slot late in ours
 *   ref[246]      `ldr r0,[pc,#392]` one slot late in ours (the 4862/4863
 *                 encoding difference is the displacement moving with it, not
 *                 a different operand)
 *   ref[365]      `ldr r0,[sp,#0x34]` one slot late in ours
 *   ref[465]      `add r5, fp` one slot late in ours -- the SAME instruction
 *                 as ref[170] at a second site, which makes it a class of two
 *                 rather than two accidents, and the first thing to look at
 *                 next.
 *
 * `add r5, fp` is `base + <something>` with base in r11; both sites are loop
 * preheaders.  The pair appearing identically at two disjoint sites is the
 * recorded "a register repeating across disjoint loops suggests a partition" --
 * and the recorded caution applies: it is NOT proof, the direction is not
 * fixed, and a partition must be applied WHOLE before anything is concluded.
 *
 * ================================================================
 * SPLIT SHAPE -- AND A CORRECTION TO THE BRIEF THAT SENT ME HERE
 * ================================================================
 * This is NOT a whole-file conversion.  asm/rom_c9000/rom_e3958_c_c_c_c_a.s
 * holds THREE functions -- Anim_Attack, BaseAnim_Attack, Anim_CriticalHit -- so
 * a split IS required.  What is true, and it is the part that matters, is that
 * the split needs ZERO `.global` exports: `tools/datacheck.py` prints nothing
 * for this stem because the file HAS NO DATA SECTION AT ALL, and
 * `tools/split_s.py ... --dry-run` does not refuse.
 *
 * THE ORDER IS NOT SYMMETRIC AND THE DRY RUNS PROVE IT:
 *
 *   split for BaseAnim_Attack  -> _a.s Anim_Attack        (46 lines)
 *                                 _b.s BaseAnim_Attack   (711 lines)
 *                                 _c.s Anim_CriticalHit  (723 lines)
 *   split for Anim_CriticalHit -> _a.s Anim_Attack + BaseAnim_Attack (757)
 *                                 _b.s Anim_CriticalHit  (723 lines)
 *
 * So CUTTING FOR BaseAnim_Attack FIRST is the good order: it leaves this
 * function ALONE in _c.s, and elevating it afterwards is then a pure file
 * rename with no further split.  Cutting for this function first buries
 * BaseAnim_Attack in a two-function _a.s that would have to be split again.
 * Whoever lands either one should land both in the same sitting, off the
 * Attack-first cut.  `split_s.py` was run ONLY with `--dry-run`.
 *
 * ONE LINKER-SCRIPT HAZARD, FLAGGED BECAUSE IT IS EASY TO MISS: stage1.ld names
 * this object TWICE -- line 1919 `(.text)` and line 1988 `(.rodata)` -- even
 * though the .s HAS no `.rodata` section.  The `.rodata` entry is contributing
 * nothing today, but a split must still leave the script consistent; do not
 * assume the absence of a data section means the absence of a data line.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * barriers, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE ROM'S COUNTERS PARTITION THE LOOPS, AND THE
 * PARTITION IS READABLE OFF THE REGISTERS -- 71.7% -> 89.4% IN ONE EDIT
 * ================================================================
 *
 * This function has six loops: four that run 0x40 times (the crit seeding
 * loop, the inner draw loop of the first frame loop, the reseed loop, the inner
 * draw loop of the second frame loop) and two that do not (the two 0x20-frame
 * loops, plus a 7-iteration fade loop at the end).
 *
 * I gave them three counters on the obvious reading -- `i` for the standalone
 * 0x40 loops and the fade loop, `k` for the two inner loops, `frame` for the
 * two frame loops -- and was EIGHT INSTRUCTIONS SHORT.  The reference uses
 * TWO, and says so plainly: EVERY inner/0x40 counter is `sl` (r10) and EVERY
 * frame-ish counter is r9, the fade loop included.  Repartitioning to
 *
 *     i      -> all FOUR 0x40-iteration loops
 *     frame  -> both frame loops AND the 7-iteration fade loop
 *
 * moved aligncmp 71.7% -> 89.4%, the differing/ins/del count 242 -> 90, and the
 * length from 8 short to 2 long.  `k` disappeared entirely.
 *
 * THE MECHANISM, WHICH IS WHY THE HIGH REGISTERS ARE THE TELL.  gcc-2.96's
 * `allocno_compare` ranks by roughly log2(n_refs) * freq / live_length, so a
 * LONGER live range means LOWER priority.  Unifying a counter across four
 * disjoint loops gives one pseudo whose live_length is the SUM of four ranges
 * -- the recorded "one variable per region" complement read the other way --
 * which drops it below the walkers and masks and lands it in r9/r10.  Thumb-1
 * cannot use r8-r11 as an ALU or `cmp` operand, so the ROM then PAYS for that
 * placement: `movs r1,#1 / add sl,r1 / mov r2,sl / cmp r2,#0x40` where a low
 * register needs only `adds r7,#1 / cmp r7,#0x40`.
 *
 * > A COUNTER IN A HIGH REGISTER IS NOT AN ACCIDENT AND IT IS NOT FREE.  When
 * > the reference spends two extra instructions per loop to keep a counter in
 * > r9/r10, that is the allocator telling you the counter is shared across MORE
 * > loops than you have written.  Our stream being SHORT is the signature.
 * > Read which register each loop's counter uses FIRST and let the repeats
 * > define the partition; do not infer it from what the loops mean.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (1) THE ORACLE: src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c (Anim_Fireball,
 *     matching) is the same animation family and supplied, verbatim in shape:
 *     `pp = g; base = *pp++; ctx = *pp;`, `cam = *(void **)((char *)g - 0x6c)`,
 *     `extern void *gPtrs[]` with `gPtrs[0x2e]`/`gPtrs[0x2f]`, the `DrawFn *fp`
 *     indirection over a two-element local array, `(int *)*_GetBattleActor(...)`
 *     with `extern int *_GetBattleActor(int)`,
 *     `MatrixSetLook(cam, (char *)cam + 0xc)`, the explicit
 *     `(unsigned)(frame - K) <= N` range test, and `Data_edeXX[w - 1]` with
 *     `w * 2` last.  First candidate: 8 short, 71.7% aligned.
 *
 * (2) THE COUNTER REPARTITION above, and in the SAME edit `cam2` deleted as a
 *     declared local.  71.7% -> 89.4%.  The `cam + 0xc` spill slot (sp+0x08)
 *     sits BELOW the gcse-created `&va` pointer (sp+0x0c), and a DECLARED local
 *     cannot rank below a gcse pseudo -- expand_decl runs first, so declared
 *     locals always get the LOWER pseudo numbers and therefore the HIGHER
 *     slots.  Writing `(char *)cam + 0xc` inline inside the frame loop makes it
 *     a loop.c hoist, created after gcse, and the two slots swap into place.
 *
 *     > THE SPILL-SLOT ORDER DATES THE PASS THAT CREATED THE PSEUDO, not just
 *     > the declaration order: parms, then declared locals in declaration
 *     > order, then cse/gcse pseudos, then loop.c's.  A slot BELOW a pseudo you
 *     > know gcse made cannot belong to anything you declared.
 *
 * (3) THE Anim_Fireball `f1` INTERMEDIATE, WORTH THE LAST INSTRUCTION AND THE
 *     EXACT SIZE: 709 -> 707, 91.2% -> 92.8%.  `d[0] = gPtrs[0x2e]; fp = d;
 *     fp[1] = gPtrs[0x2f];` makes gcc spill `fp` and RELOAD it to perform the
 *     `[fp,#4]` store, one instruction the ROM does not spend.  Naming the
 *     second pointer first --
 *
 *         d[0] = (DrawFn)gPtrs[0x2e];
 *         f1   = (DrawFn)gPtrs[0x2f];
 *         fp   = d;
 *         fp[1] = f1;
 *
 *     -- finishes the gPtrs chain BEFORE the `&d` address is formed, so the
 *     address is still live in the register when the store happens.  Moving
 *     `fp = d;` earlier instead measured WORSE (90.7%): the reload came back.
 *
 * (4) THE FUNCTION POINTER MUST BE ASSIGNED AFTER THE CALL THAT FEEDS IT:
 *     43 of 707, 92.8% -> 95.5%, and it fixed the ENTIRE prologue register
 *     rotation -- the first differing index jumped from 21 to 78.  The palette
 *     copy is `bl _call_via_r3` in the reference: r3 is CALL-CLOBBERED, so the
 *     pointer cannot be live across `GetFile`.  Written as
 *
 *         cp = Func_8001af8;
 *         cp(dst, GetFile(FILE_8e), 0x80);
 *
 *     the pseudo spans the GetFile call, needs a call-saved register, takes r5,
 *     and pushes `slot` to r6 and `crit` to r7 for the whole prologue -- a
 *     one-position rotation across sixty instructions, from one statement.
 *     Hoisting the call out first:
 *
 *         void *f = GetFile(FILE_8e);
 *         cp = Func_8001af8;
 *         cp(dst, f, 0x80);
 *
 *     puts the definition after the call, r3 suffices, and the rotation goes.
 *
 *     > A `_call_via_rN` VENEER NAMES THE REGISTER CLASS OF THE POINTER.  A LOW
 *     > veneer register (r0-r3, call-clobbered here) proves the pointer does
 *     > NOT cross a call, which fixes where its assignment can stand.  Read the
 *     > veneer register before writing the statement, not after.
 *
 * (5) TWO WALKING POINTERS, PARTITIONED THE WAY THE REGISTERS SAY:
 *     43 -> 23, 95.5% -> 98.4%.  Same lever as Anim_Djinni in this batch and
 *     the same reasoning: the reference's crit seeding loop and reseed loop
 *     BOTH walk in r5 while the first frame loop's inner walker is r6, so the
 *     two standalone loops share one variable (disjoint ranges, one pseudo --
 *     register inherited) and the inner loop needs its own.  I had it the other
 *     way round.  Nothing about what the loops DO suggests this grouping; only
 *     the repeated register does.
 *
 * (6) `i = 0;` BEFORE THE `base + K` WALKER at all three remaining preheaders
 *     (Anim_Vine's recorded "assign the base + K pointer LAST"): 23 -> 19,
 *     98.4% -> 98.6%.  The reference materialises the counter, the mask and the
 *     zero and only then emits `add r5, fp`.
 *
 * (7) `w = w >> 3; w = w + 2;` as TWO STATEMENTS, not `w = (w >> 3) + 2;`, for
 *     the second frame loop's sprite size, plus `frame = 0;` before the fade
 *     loop's `base + K` pointer: 89.4% -> 91.2% together.  The reference emits
 *     the two destructive in-place forms `asrs r6, r6, #3` then `adds r6, #2`;
 *     the single expression computes the shift into a temp and adds out of
 *     place.  One C statement per emitted instruction is what reproduces an
 *     in-place chain on a variable that is its own input.
 *
 * ================================================================
 * MECHANISMS READ OFF THE REFERENCE
 * ================================================================
 *
 *   - `slot` IS ASSIGNED TWICE, and the second assignment is load-bearing
 *     (first candidate 699 -> 701 instructions).  The reference forms
 *     `base + 0x7828` into r5 at the top, spends r5 on the `&d` address in the
 *     `fp` block, and then RE-FORMS it -- `ldr r5,=0x7828 / add r5,fp` -- for
 *     the LoadVFXFile sequence, where it survives six calls in a callee-saved
 *     register.  One variable, two live ranges, written as two assignments;
 *     inside the four loops the address is written out at every use instead,
 *     because there the reference re-computes it every time.  This is the
 *     recorded "the unit is the REGION" rule with the region boundary visible.
 *
 *   - `crit` IS A VARIABLE, NOT A RE-TEST: `movs r6,#1 / cmp r3,#0xc7 / bgt /
 *     movs r6,#0` and a `cmp r6,#1` three hundred instructions later.  Written
 *     as `crit = 1; if (f0 <= 0xc7) crit = 0;` -- the `bgt` skipping the store
 *     is what that spells, not `crit = (f0 > 0xc7)`.
 *
 *   - `shake = 0x40 - va.x` COMPILES THE 0x40 OUT OF THE LOOP COUNTER.  The
 *     reference emits `mov r0, sl / subs r0, r0, r3` with sl still holding `i`
 *     at its exit value: cse's `record_jump_equiv` knows i == 0x40 after
 *     `while (i != 0x40)`.  Same mechanism as the literal `0` inside
 *     `if (t == 0)` on Anim_Djinni, and the same rule -- write the LITERAL, not
 *     the variable; the variable would be a different program.  The fade
 *     loop's `strh` of 0x20 is the same thing off `frame`.
 *
 *   - `&va` IS NOT A DECLARED POINTER.  The reference computes `sp+0x64` into
 *     sp+0x0c inside the crit branch AND in the empty else arm -- that is
 *     gcse PRE inserting the address on the edge where it is not available,
 *     because `&va` is passed to GetBattleActorPos2 on both sides of the join.
 *     Writing `&va` and `va.x` textually produces both copies.  Note the
 *     reference then reads `va.x` THROUGH the pointer (`ldr r3,[r2]`) but
 *     `va.y` DIRECTLY (`ldr r2,[sp,#0x68]`): cse rewrites the offset-0 access
 *     because that MEM is literally `(mem (reg))`, and leaves the offset-4 one
 *     alone.  Both forms fall out of the same plain source.
 *
 *   - THE FOUR-WAY SPRITE CHAIN IS AN if / else-if CHAIN, not a switch: a run
 *     of `cmp rN,#1 / bgt`, `cmp rN,#3 / bgt`, `cmp rN,#5 / bgt`, `cmp rN,#7 /
 *     bgt` with the first three tails cross-jumped to one shared call site.
 *
 *   - `_Actor_SetAnimSpeed`'s actor sits in r8, so EVERY field access costs a
 *     `mov rN, r8` first.  The five saves are written in the order
 *     0x24, 0x28, 0x2c, 0x48, 0x34 but their SPILL SLOTS descend
 *     0x24, 0x20, 0x1c, 0x18, 0x14 -- so the DECLARATION order is
 *     0x24, 0x28, 0x2c, 0x34, 0x48 and the ASSIGNMENT order is not the same
 *     list.  Separating the two is the Anim_Djinni rule again.
 *
 *   - `((short *)q)`-style halfword traffic: `shake` lives in a spill slot and
 *     is stored to `iwram_3001ad0[2]` as a short, which is why the reference
 *     forms `add r1, sp, #0x30` before the `ldrh` -- Thumb has no sp-relative
 *     `ldrh`, so reload must materialise the address.  Nothing is owed there.
 *
 *   - `0xa0 << 19`, `0xef << 7`, `0xe1 << 7`, `0xe1 << 6`, `0x90 << 3`,
 *     `0xc8 << 4`, `0x80 << 7` and `0xc9 << 3` written as shifts, for the
 *     thumb `movs`+`lsls` constant splitter.
 *
 *   - THE 0x1f80 / 0x1f81 WRITES TO REG_BG1CNT ARE ALREADY RIGHT AND MUST NOT
 *     BE "FIXED".  We emit `ldrh r3, .LC` where the reference prints
 *     `ldr r3, .LC @ 0x1f80`; gas assembles the two identically, which is the
 *     recorded invisible-carrier case.  Chasing it once cost eight encodings on
 *     another function.  No `int` carrier is used here.
 *
 * ================================================================
 * MEASURED AND INERT / WORSE
 * ================================================================
 *   - naming `mask` for BOTH seeding loops (0xffff then 0xff, one variable):
 *     MUCH WORSE -- 709 instructions, size +4, aligncmp 86.6%.  Naming it for
 *     the second loop ONLY: byte-identical to not naming it at all.  So unlike
 *     Anim_Fireball, this function wants its masks left as literals; the
 *     constants reach their callee-saved registers through cse without help,
 *     and the extra live range only costs.
 *   - `fp = d;` moved above `d[0] = ...`: 90.7%, worse than lever (3)'s 92.8%.
 *
 * ================================================================
 * RULED OUT, with what was measured:
 *   - NOT a mis-read program.  Size and count exact; all 84 relocations present
 *     in the reference's order with only two veneer registers differing; I read
 *     the immediates in every differing hunk and not one constant, shift
 *     amount, structure offset or branch condition differs.
 *   - NOT sched1: it does not run in this build.
 *   - NOT declaration order: every spill offset in the frame is exact, both the
 *     five address-taken aggregates (0x64, 0x58, 0x4c, 0x40, 0x38) and the
 *     twelve slots from 0x34 down to 0x08.
 *   - NOT a FILE-STRUCTURE refusal: the split needs no exports and both dry
 *     runs are clean.
 *   - NOT the pin class: shimcount is zero and no pin was tried.
 *
 * NEXT MOVE: hunk A is now a two-function pattern with a named mechanism, so it
 * is worth attacking as a class rather than per function -- find the source
 * shape that makes a constant pseudo LOSE a register while a nearby pointer
 * keeps one, and two parks close at once.  The four transpositions are worth
 * less than that and should wait.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*ClearFn)(void *dst, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern short iwram_3001ad0[];
extern char *iwram_3001e74;
extern Part gBuffer[];
extern unsigned char ewram_2013840[];
extern unsigned short Data_ede5c[];

extern void _Func_80c0df4(int a, int b, int c);
extern void WaitFrames(unsigned int n);
extern void InitRenderTilemapBG1(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);
extern int *_GetBattleActor(int id);
extern int Random(void);
extern void _Actor_SetAnimSpeed(int *actor, int speed);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _PlaySound(int id);
extern void InitMatrixStack(void);
extern void MatrixRoll(int a);
extern void MatrixPitch(int a);
extern void MatrixYaw(int a);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *p, int a, int b);
extern void Func_80008d4(void *dst, int len);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern int _Func_80b8530(int id);
extern void _Func_80c0700(unsigned short a, int b);
extern void Func_80cdd14(void);
extern void gfree(int tag);

void Anim_CriticalHit(void *context)
{
    vec3_t va;
    vec3_t vb;
    vec3_t vc;
    vec3_t ve;
    DrawFn d[2];
    void *ctx;
    int shake;
    void *base2;
    void *cam;
    int s24;
    int s28;
    int s2c;
    int s34;
    int s48;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    State **slot;
    int *actor;
    int *actorB;
    int crit;
    int frame;
    int i;
    int h;
    Part *p;
    Part *q;
    CopyFn cp;
    int clen = 0x80 << 7;
    ClearFn cl;
    int msk;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    crit = 1;
    if (((State *)context)->f0 <= 0xc7) {
        crit = 0;
    }
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    _Func_80c0df4(((State *)context)->f8, ((State *)context)->fc, 0x82);
    WaitFrames(1);
    InitRenderTilemapBG1();
    REG_BG1CNT = 0x1f80;
    if ((*slot)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    }
    d[0] = (DrawFn)gPtrs[0x2e];
    f1 = (DrawFn)gPtrs[0x2f];
    fp = d;
    fp[1] = f1;
    slot = (State **)(base + 0x7828);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_49, base, 1, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_4a, gBuffer, 1, 1);
    if ((*slot)->f8 > 7) {
        void *f = GetFile(FILE_8e);
        cp = Func_8001af8;
        cp((volatile u16 *)(0xa0 << 19), f, 0x80);
    }
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_76, base2, 0, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    if (crit == 1) {
        actor = (int *)*_GetBattleActor((*slot)->f8);
        i = 0;
        p = (Part *)(base + (0xe1 << 7));
        do {
            p->x = (Random() & 0x3f) + 0x10;
            p->y = 0;
            p->z = 0;
            p->vx = Random() & 0xffff;
            p->vy = Random() & 0xffff;
            p->vz = Random() & 0xffff;
            i++;
            p++;
        } while (i != 0x40);
        _Actor_SetAnimSpeed(actor, 0);
        s24 = actor[9];
        s28 = actor[10];
        s2c = actor[11];
        s48 = actor[18];
        s34 = actor[13];
        actor[9] = 0;
        actor[10] = 0;
        actor[11] = 0;
        actor[13] = 0;
        actor[18] = 0;
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        shake = 0x40 - va.x;
        iwram_3001ad0[2] = shake;
        iwram_3001ad0[3] = 0x50;
        *(int *)(base + 0x77b4) = 0x18;
        *(int *)(base + 0x77b8) = 0;
        StartTask(Func_80cd4b4, 0xc8 << 4);
        _PlaySound(0xd4);
        frame = 0;
        do {
            _Func_80c0df4((*(State **)(base + 0x7828))->f8,
                          (*(State **)(base + 0x7828))->fc, 0x82);
            i = 0;
            q = (Part *)(base + (0xe1 << 7));
            do {
                if (q->x >= 0 && frame >= i / 4) {
                    int w = (i & 1) + 5;
                    InitMatrixStack();
                    MatrixRoll(q->vz);
                    MatrixPitch(q->vx);
                    MatrixYaw(q->vy);
                    Func_80e3944((vec3_t *)q, &vc);
                    vc.x = vc.x + 0x40;
                    vc.y = vc.y + va.y + 0x18;
                    if (vc.z < -0x3c) {
                        vc.z = -0x3c;
                    }
                    if (vc.z > 0x3c) {
                        vc.z = 0x3c;
                    }
                    vc.z = vc.z + 0x3c;
                    d[0](ctx, (char *)base2 + Data_ede5c[w - 1], vc.x - w,
                         vc.y - w, w * 2, w * 2);
                    q->x = q->x - 4;
                }
                i++;
                q++;
            } while (i != 0x40);
            *(int *)(base + 0x7824) = 1;
            WaitFrames(1);
            frame++;
        } while (frame != 0x20);
        StopTask(Func_80cd4b4);
        _Actor_SetAnimSpeed(actor, 0x10);
        actor[9] = s24;
        actor[10] = s28;
        actor[11] = s2c;
        actor[13] = s34;
        actor[18] = s48;
    }
    cl = Func_80008d4;
    cl(ctx, clen);
    cl((void *)0x6004000, clen);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    REG_BG1CNT = 0x1f81;
    GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &vb);
    if ((*(State **)(base + 0x7828))->f4 == 0) {
        shake = 0x20 - vb.x;
    } else {
        shake = 0x60 - vb.x;
    }
    if (shake > 0) {
        shake = 0;
    }
    if (shake < -0x80) {
        shake = -0x80;
    }
    vb.x = vb.x + shake;
    iwram_3001ad0[3] = 0x50;
    iwram_3001ad0[2] = shake;
    actorB = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    h = _Func_80b8530((*(State **)(base + 0x7828))->ids[0]) / 2;
    i = 0;
    msk = 0xff;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = actorB[2];
        p->y = actorB[3] + h;
        p->z = actorB[4];
        p->vx = (Random() & msk) << 10;
        p->vy = (Random() & msk) << 10;
        p->vz = ((Random() & msk) - 0x7f) << 10;
        if (p->x > 0) {
            p->vx = -p->vx;
        }
        p->t = i + 0x10;
        i++;
        p++;
    } while (i != 0x40);
    frame = 0;
    do {
        if (frame == 5) {
            _Func_80bd7dc(0x86);
        }
        if (frame == 4) {
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 0);
        }
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        va.y = va.y + 0x10;
        if (frame <= 1) {
            d[0](ctx, base, 0, 0, 0x78, 0x78);
        } else if (frame <= 3) {
            d[0](ctx, base + (0xe1 << 6), 0, 0, 0x78, 0x78);
        } else if (frame <= 5) {
            d[0](ctx, gBuffer, 0, 0, 0x78, 0x78);
        } else if (frame <= 7) {
            d[0](ctx, ewram_2013840, 0, 0, 0x78, 0x78);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        if ((unsigned)(frame - 4) <= 0x1b) {
            i = 0;
            do {
                int m = i / 2;
                Part *r = (Part *)(base + m * 28 + (0xe1 << 7));
                int w = r->t;
                if (w > 0) {
                    Func_80e3944((vec3_t *)r, &ve);
                    ve.x = ve.x + shake;
                    w = w >> 3;
                    w = w + 2;
                    ve.y = ve.y + 0x10;
                    fp[m & 1](ctx, (char *)base2 + Data_ede5c[w - 1],
                              ve.x - w, ve.y - w, w * 2, w * 2);
                    Func_80e38b8(r, 0x3c, -0x400);
                    r->t = r->t - 1;
                }
                i++;
            } while (i != 0x40);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x20);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    iwram_3001ad0[3] = 0x20;
    {
        unsigned short *t;
        frame = 0;
        t = (unsigned short *)(iwram_3001e74 + (0xc9 << 3));
        do {
            _Func_80c0700(*t, 6 - frame);
            WaitFrames(1);
            frame++;
        } while (frame != 7);
    }
    Func_80cdd14();
}
