/* Func_8020b64 -- PARK STANDS at 50 of 57, but the BLOCKER IS REDUCED FROM FOUR
 * SITES TO ONE, and two of the three legs are solved.  Batch 326 brief A.
 *
 *   50 differing encodings of 57.  ref 57 encodings / 116 bytes / 56 insns,
 *   ours 55 / 112 / 55.  Re-measured; identical to the batch-324/325 figure.
 *   The whole 50 is ONE MISSING INSTRUCTION plus its misalignment.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8020b64.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s --func Func_8020b64
 *
 * =========== CORRECTION: THE SWAP'S SECOND GATE IS ADJACENCY, AND IT BREAKS
 *
 * docs/elevation.md and the headers below treat `cse_insn`'s transform as
 * governed by the liveness test in `make_regs_eqv`.  It has TWO MORE gates,
 * both structural, and both defeated by ONE ORDINARY STATEMENT:
 *
 *   cse.c:5980   && NEXT_INSN (PREV_INSN (insn)) == insn
 *   cse.c:5992   rtx prev = prev_nonnote_insn (insn);
 *   cse.c:5993-4 if (prev != 0 && GET_CODE (prev) == INSN
 *                    && GET_CODE (PATTERN (prev)) == SET
 *                    && SET_DEST (PATTERN (prev)) == SET_SRC (sets[0].rtl))
 *
 * ** A STATEMENT PLACED BETWEEN TWO MEMBERS OF THE COPY CHAIN STOPS THE SWAP
 * OUTRIGHT, IN cse AND IN cse2, WHATEVER THE LIVENESS. **  The fourteen flat
 * bodies varied names, types, read order and role; none moved a statement INTO
 * the chain.  Note also that `prev_nonnote_insn` skips NOTEs but NOT
 * CODE_LABELs, so a chain member that is the first insn of a block is already
 * immune -- which is why only the IN-LOOP chain ever needed fixing.
 *
 * Chain length after the loop's `ldrb`, by pass (insn counts in brackets; count the reg-reg SImode copies between the
 *   `(zero_extend (mem:QI ...))` and the loop-closing compare in each -da dump):
 *
 *   body                  .02.jump .03.cse .09.cse2 .12.life .13.combine .19.flow2
 *   this park body           3        2        2        2         2          2  [55]
 *   batch 325's block `x`    4        3        3(dead)  2         2          2  [55]
 *   A: `x=*src; n++; c=x;`   4        3        3        3         2          2  [55]
 *   E: store between         4        3        3        3         2          2  [55]
 *   H3 / K: see below        4        3        3        3         3          2  [55]
 *
 * ** FOUR bodies now carry the chain past cse2 and TWO carry it past combine.
 * No body before batch 326 got past cse2 at all. **
 *
 * =========== THE THREE LEGS, EACH WITH ITS OWN MEASURED SINK
 *
 * Name the ROM's three insns p1 (`ldrb r3,[r1]`), p2 (`mov r2,r3`), p3
 * (`mov r3,r2`).  The ROM's liveness is: p1 ONE use (p2's copy), p2 TWO uses
 * (the `strb` at the top of the NEXT iteration, and p3's copy), p3 ONE use (the
 * loop-closing `cmp`).  p1 dies before p3 is born, so they SHARE r3; p2 is r2.
 *
 * leg 1 -- the cse/cse2 swap.  SOLVED: put a statement between p1's set and
 *   p2's set.  (`x = *src;` always emits load-then-copy adjacently, so the
 *   separator has to sit between the NAMED copies, which needs three names.)
 *
 * leg 2 -- combine folding p1 forward into p2.  SOLVED, and the lever is
 *   specifically a MEMORY WRITE, not any statement: with `n++` as the separator
 *   (body A) combine moves the `mem:QI` load into p2; with `buf[n] = c` as the
 *   separator (body E) it does not.  Both bodies are otherwise identical.
 *
 * leg 3 -- combine folding p3 into the loop-closing `cbranchsi4`.  OPEN.  p3's
 *   only use is that branch and it dies there, so combine substitutes p2 and
 *   deletes the copy.  PROOF: body E's `.13.combine` branch reads
 *   `(ne (reg/v:SI 34) 0)` where this park body's reads `(reg/v:SI 35)` -- the
 *   branch's operand IS the collapsed copy.
 *
 * MEASURED on body E, all EXACTLY INERT at 52 (so leg 3 is a USE COUNT, not a
 * mode or a spelling): `t` as `unsigned char`, `unsigned int`, `short`,
 * `unsigned short`; and `while (t)` for `while (t != 0)`.
 *
 * =========== WHY THE TWO BODIES THAT DO PASS COMBINE STILL CANNOT LAND
 *
 * H3 (`src++; x=*src; buf[n]=c; c=x; t=c; n++;`) and K (`src++; x=*src;
 * buf[n]=c; t=x; n++; c=t;`) both reach `.15.regmove` with THREE insns and lose
 * one at `.19.flow2`.  Both reverse the chain so that p3, not p2, is the
 * loop-carried value.  p3 is then live across the back edge while p1 is live at
 * the load that precedes the store, so p1 and p3 OVERLAP and cannot share a
 * register; the allocator gives p1 and p2 the same one instead, the copy becomes
 * `(set (reg:SI 3 r3) (reg:SI 3 r3))` and the noop-move pass deletes it.
 * Confirmed in `.19.flow2`: `insn 49 ... NOTE_INSN_DELETED`.
 *
 * > SO BODY E'S STRUCTURE IS THE ONLY ONE WITH THE ROM'S LIVENESS, and its
 * > single remaining blocker is leg 3.  Body E reads 52 at 55 insns, worse than
 * > this park's 50, so the 50 stays as the shipped figure and 52 is recorded as
 * > a figure ABOUT the blocker.
 *
 * REFUTED IF someone gives p3 a second real use, or puts p3's set and the
 * loop-closing branch in different basic blocks, without costing an
 * instruction.  Both were looked for and neither was found; `for (;;)` with an
 * `if (c == 0) break;` costs TWO instructions (53 insns, 52 differing), and an
 * explicit cursor pointer costs two more (53 insns at 108 bytes) -- which
 * independently re-confirms CORRECTION 1 below.
 *
 * SIBLINGS BY SIGNATURE (unverified, for whoever picks this class up):
 * grep the parks for a loop whose body holds TWO OR MORE reg-reg copies of one
 * loaded byte/halfword -- i.e. a `.02.jump` chain of length >= 3 between a
 * `(zero_extend (mem:QI ...))` and the loop-closing compare.  The three-legged
 * test above applies verbatim to any of them, and legs 1 and 2 are cheap.
 */

/* Func_8020b64 -- PARK STANDS.  Batch 325 brief B: figure re-derived, and the
 * family's MECHANISM CORRECTED against the compiler source.
 *
 *   50 differing encodings of 57.  ref 57 encodings / 116 bytes, ours 55 / 112.
 *   Re-measured in batch 325 brief B; identical to the batch-324 figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8020b64.c asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s --func Func_8020b64
 *
 * ================= CORRECTION 1: IT IS NOT delete_trivially_dead_insns ========
 *
 * The header below, and docs/elevation.md's COPY-COLLAPSE section, attribute the
 * collapse to `delete_trivially_dead_insns` inside the cse pass block and state
 * it is "not cse_insn's canonicalisation".  BOTH HALVES ARE WRONG.
 *
 * `delete_trivially_dead_insns` (cse.c:7245) deletes an insn only when
 * `counts[REGNO (SET_DEST)] == 0` -- a whole-function use count.  It is a
 * JANITOR; it chooses no direction.  In THIS function's production build it is
 * not even the collector: the dead copy is removed by `life_analysis` at
 * `.12.life` (traced pass-by-pass, scratch_elev/b325/B/NOTES.md).
 *
 * The DECISION is a specific named transform in `cse_insn`:
 *
 *   cse.c:5959  "Special handling for (set REG0 REG1) where REG0 is the
 *                'cheapest', cheaper than REG1. ... change this insn to
 *                (set REG1 REG0) and replace REG1 with REG0 in the previous
 *                insn that computed their value.  Then REG1 will become a
 *                dead store"
 *
 *   cse.c:5999   validate_change (prev, &SET_DEST (PATTERN (prev)), dest, 1);
 *   cse.c:6000   validate_change (insn, &SET_DEST (sets[0].rtl),    src,  1);
 *   cse.c:6001   validate_change (insn, &SET_SRC  (sets[0].rtl),    dest, 1);
 *
 * That is what rewrites the LOAD'S DESTINATION -- which no propagation could do,
 * and which the old header correctly observed without naming.
 *
 * ================= CORRECTION 2: THE GATE IS A LIVENESS TEST =================
 *
 * Two conditions must hold for the swap:
 *
 *  GATE 1 (cse.c:5986)  src_ent->first_reg == REGNO (SET_DEST).  `first_reg` is
 *    maintained by `make_regs_eqv (new, old)` (cse.c:1397), which for two
 *    pseudos promotes `new` ONLY IF
 *
 *      (uid_cuid[REGNO_LAST_UID (new)]  > cse_basic_block_end
 *       || uid_cuid[REGNO_FIRST_UID (new)] < cse_basic_block_start)
 *      && uid_cuid[REGNO_LAST_UID (new)] > uid_cuid[REGNO_LAST_UID (firstr)]
 *
 *    i.e. (i) `new` must live OUTSIDE the current cse extended basic block and
 *    (ii) its last mention must come after `firstr`'s.  A cse block ends at
 *    EVERY CODE_LABEL (`cse_end_of_basic_block`'s
 *    `while (p && GET_CODE (p) != CODE_LABEL)`), so a loop body is always its
 *    own block.
 *
 *  GATE 2 (cse.c:5992)  `prev_nonnote_insn (insn)` must BE the insn whose
 *    SET_DEST is REG1.
 *
 * ** NEITHER GATE IS A NAME OR A TYPE.  Both are liveness/adjacency facts. **
 * That is exactly why fourteen whole bodies measured flat: they varied names,
 * types, read order and statement order, and none varied which pseudo's live
 * range leaves the block.
 *
 * ================= WHAT THAT BUYS: .03.cse REACHES THE ROM'S THREE ==========
 *
 * A BLOCK-SCOPED `unsigned char` intermediate inside the loop body:
 *
 *      while (t != 0) {
 *              unsigned char x;
 *              buf[n] = c;  src++;
 *              x = *src;  c = x;  t = c;
 *              n++;
 *      }
 *
 * makes `.03.cse` keep THREE insns -- the ROM's shape, which no previously
 * measured body has produced:
 *
 *   (insn 44 (set (reg:SI 41)   (zero_extend:SI (mem:QI (reg/v:SI 33) 0))))
 *   (insn 49 (set (reg/v:SI 35) (reg:SI 41)))
 *   (insn 52 (set (reg/v:SI 34) (reg/v:SI 35)))
 *
 * `x`'s pseudo is born and dead inside the loop body's cse block, so gate 1(i)
 * fails, it is never promoted, and the swap does not fire.
 *
 * WHY THE PARK'S "three names ... 50 flat" MISSED IT: that row used a
 * FUNCTION-SCOPE `int x`.  Both halves break it, and I reproduced both --
 * `int x` against `unsigned char c` inserts a TRUNCATION, so `c = x` expands as
 * `(set (reg) (lshiftrt (reg) 24))` and the cse.c:5959 transform does not apply
 * to it at all; and a function-scope name is not bb-local, so even as a clean
 * copy its promotion test passes.  ** THE TYPE AND THE SCOPE ARE BOTH
 * LOAD-BEARING, and the sweep varied the type at fixed scope. **
 *
 * ================= WHY IT STILL DOES NOT LAND: FOUR SITES, NOT ONE ==========
 *
 * Loop-chain length per pass (scratch_elev/b325/B/dump/v4norm, v4nocse2):
 *
 *   pass          production    -fno-rerun-cse-after-loop
 *   .02.jump          4              4
 *   .03.cse           3              3
 *   .09.cse2          3 (1 DEAD)     3
 *   .12.life          2              3
 *   .13.combine       2              2
 *
 * `.09.cse2` reapplies the SAME cse.c:5959 swap (it runs with after_loop = 1,
 * toplev.c:3095, which drops cse_end_of_basic_block's NOTE_INSN_LOOP_END break
 * and widens its blocks), and `life_analysis` collects.  With cse2 off, the three
 * survive to `.12.life` and then COMBINE merges the load into the copy through a
 * LOG_LINK -- a second, independent sink.
 *
 * BOUND, with its evidence: twelve bodies measured here, every one reaching
 * `.12.life` at 2 and reading 50 of 57 at 112 bytes / 55 insns:
 *   fn-scope `int x`; fn-scope + `n++` between load and copy; block `unsigned
 *   char x`; block `unsigned int x`; block + `n++` between; TWO and THREE
 *   block-scoped intermediates; one, two and three block-scoped intermediates
 *   placed AFTER the loop-carried variable (these collapse to 2 inside cse
 *   alone); and `x = *src; c = x; t = x;` giving x two uses.
 * `.03.cse`'s floor is 3 for one, two or three intermediates alike.
 * INSTRUMENTS: -fno-rerun-cse-after-loop reads 56 on the block-scoped body and
 *   57 on the park body, both still 55 insns -- it moves the flip without
 *   restoring the instruction.
 *
 * The structural reason: cse's own collapse always leaves the BB-LOCAL name
 * owning the load and the BB-CROSSING name owning the next copy -- precisely the
 * pair whose promotion test passes, so cse2 swaps it.
 *
 * REFUTED IF someone finds a spelling where the copy surviving `.03.cse` has a
 * destination dead inside the loop body's cse block while still being the value
 * the next iteration reads, or gives the load's destination a second REAL use
 * (so combine has no LOG_LINK) that is not itself collapsed.
 */

/* Func_8020b64 -- PARK STANDS.  Re-derived and refined in batch 324 brief F.
 *
 *   50 differing encodings of 57.  ref 57 encodings / 116 bytes, ours 55 / 112.
 *
 * THE ARITHMETIC FIRST, because it reframes the whole figure.  The reference is
 * 56 instructions plus a 2-byte `.short 0x0000` pad (114 is not word-aligned);
 * ours is 55 instructions and needs no pad.  **So the entire 50 is ONE MISSING
 * INSTRUCTION and the misalignment it causes.**  Do not read 50 as 50 problems.
 *
 * DECOMPOSITION -- three runs, and run A is NOT downstream of run B:
 *   A  ref idx 1-5    the prologue chain.  ref `ldrb r2,[r1]` / `adds r3,r2,#0`
 *                     BEFORE `sub sp,#20`; ours loads into r3 and copies after
 *                     `movs r4,#0`.  Two sub-causes: the load's DESTINATION and
 *                     the copy's sched2 position.
 *   B  ref idx 13-19  the loop.  The ROM has THREE copies
 *                     (`ldrb r3,[r1]` / `adds r2,r3,#0` / `adds r3,r2,#0`);
 *                     we have two.  THIS IS THE +1 INSTRUCTION.
 *   C  ref idx 20-55  pure misalignment downstream of B.
 *
 * THE MECHANISM, read in the dumps.  `.02.jump` holds the ROM's three-insn chain
 * verbatim -- insn 43 loads into pseudo 40, insn 45 is `c = x`, insn 48 is
 * `t = c`.  By `.03.cse` insn 45 is GONE and insn 43's DESTINATION HAS BEEN
 * REWRITTEN to 34, so the temp pseudo is folded into `c` inside the cse pass
 * block (`toplev.c:2908-2933`: reg_scan + thread_jumps + cse_main +
 * jump_optimize + delete_trivially_dead_insns).  `cse.c` itself never reads
 * REG_DEAD -- grep gives no hits -- so the rewrite is that block's DEAD-COPY
 * COLLAPSE, not `cse_insn`'s canonicalisation.  By `.17.lreg` only two pseudos
 * exist, so the allocator can never produce the ROM's third copy.
 * ** THE PARK'S ORIGINAL DIAGNOSIS IS REPRODUCED, not refuted. **
 *
 * NEW, AND THE PARK DID NOT HAVE IT: `.09.cse2` FLIPS THE CHAIN.  Through
 * `.03.cse`/`.07.gcse`/`.08.loop` the load writes 34 (`c`) and the copy is
 * `35 = 34` -- the ROM's direction.  At `.09.cse2` the load writes 35 (`t`) and
 * the copy is `34 = 35`.  That flip decides run A's `ldrb r3` against the ROM's
 * `ldrb r2`, and it is a SECOND, INDEPENDENT CAUSE -- which is why the park's
 * `--no-rerun-cse` row measured WORSE rather than inert: it moves the flip and
 * loses other things.
 *
 * MEASURED, TEN WHOLE BODIES, via tools/crossfire.py's own scorer.  Reference
 * memory profile is ldrb=2 strb=7 and EVERY row matched it (no MEM flag), so
 * none of these is a figure bought by doing less work than the ROM:
 *   three names `x int; x=*src; c=x; t=c`  50 flat
 *   three names, x unsigned char           50 flat
 *   four names x,y                         50 flat
 *   a volatile read (INSTRUMENT, a device) 50 flat
 *   n++ moved before the load              50 flat
 *   t = c & 0xff                           50 flat
 *   roles swapped, t=*src; c=t             50 flat
 *   x uchar + c int                        50 flat
 * WORSE: `int c` rather than unsigned char reads 52 at 53 insns (2 short).
 *
 * SO THE REMAINING CAUSE IS NAMED AND IS NOT REACHABLE BY NAMING, BY TYPE, BY
 * READ COUNT OR BY STATEMENT ORDER -- ten bodies, all exactly flat.  See the
 * cross-target note in docs/elevation.md: a two-insn copy chain is collapsed
 * inside the cse pass block, and WHICH OF THE TWO INSNS SURVIVES fixes both the
 * register and the position, because the survivor keeps its LUID and sched2's
 * last rung is LUID.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8020b64.c asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s --func Func_8020b64
 */

/* Func_8020b64 (0x08020b64) -- NON-MATCHING.
 *
 * NON-MATCHING, 50 of 57 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 57, ours 55) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 116 bytes, ours 112.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8020b64.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s --func Func_8020b64
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: A COPY CHAIN COLLAPSED BY THE FIRST cse PASS.
 *
 * SIX instructions in disagreeing regions of 61, with EVERY REGISTER IN THE
 * ROM'S PLACE. The previous park recorded 47 of 61 and called it a register
 * rotation; both halves of that were wrong, and both corrections matter.
 *
 * Builds a small display string: copy a null-terminated source into a stack
 * buffer, append two control bytes, pad with 0x5f out to offset 7, append two
 * more and a terminator, then hand it to the text layer.
 *
 * CORRECTION 1 -- THE ROTATION WAS THE PARK'S OWN LOCALS. It carried `base`,
 * `p` and `count` as named locals. Deleting all three and writing plain
 * `buf[n]` indexing with a natural `while (n < 7)` pad loop lets gcc's strength
 * reduction create the cursor itself, and lets `n`'s final value (`mov r4, #7`)
 * and the trip count (`sub r4, r3, r4`) fall out on their own. The registers
 * then land exactly where the ROM has them -- c in r2, t in r3, n in r4, the
 * cursor in r5, the argument in r6, the buffer in r0.
 *
 * This is the recorded "a local that only holds an ADDRESS can cost the
 * ordering -- delete it" lever, and it moved a FOUR-register rotation. The
 * notebook's line that "nothing has moved a rotation of more than two
 * registers" is now false and should be struck.
 *
 * CORRECTION 2 -- "47 of 61" WAS A POSITIONAL ARTIFACT. The two streams differ
 * in length, so a positional comparison counts every instruction after the
 * first insertion as differing. The aligned count is 6.
 *
 * > ANY PARK QUOTING A POSITIONAL COUNT ON A FUNCTION WITH A LENGTH MISMATCH IS
 * > OVERSTATING ITS DISTANCE. The parked set should be re-ranked on aligned
 * > counts before anything else is written off.
 *
 * WHAT REMAINS, and it is one instruction:
 *
 *     rom    ldrb r2, [r1] / mov r3, r2   ... mov r3, r2 / add r5, #0x1
 *     ours   ldrb r3, [r1] / mov r2, r3   ... add r5, #0x1
 *
 * The mechanism is pinned down from RTL dumps rather than inferred. The
 * expander output contains the ROM's three-instruction chain verbatim --
 * load-destination, then `c`, then `t` -- and it survives the jump pass intact.
 * THE FIRST cse PASS COLLAPSES IT TO TWO, folding the load's destination pseudo
 * into `c`; a later pass flips which of the two owns the load. By regmove only
 * two pseudos remain, so the allocator never sees three and cannot produce the
 * ROM's redundant copy.
 *
 * So this is pseudo-creation and copy-collapse in cse_main, NOT allocation, and
 * the read-count lever cannot reach it: a genuine second textual read is
 * byte-identical to the single read here, because cse folds both to the same
 * two insns.
 *
 * The scheduling half -- `add r5, #1` landing in the load-use slot -- is
 * downstream of the same count: the ROM has three instructions to fill that
 * slot and we have two. --no-sched2 is worse, so it is not independent.
 *
 * MEASURED, thirty spellings, aligned counts (rom 61 lines):
 *   the previous park, with base/p/count locals            32
 *   `unsigned char c` alone, no `t`                        11
 *   `unsigned char c; int t; c = *src; t = c;`              6  <- kept
 *   two textual reads `c = *src; t = *src;`                 6  (identical)
 *   three names in the chain                                6  (identical)
 *   `t = c & 0xff`, `unsigned int t`, `uchar t`             6  each
 *   an explicit cursor plus array-indexed tail              6
 *   assignment-in-condition `while ((c = *src) != 0)`      11
 *   an un-rotated `goto test;` loop                        22
 *   a store placed between load and copy as an alias barrier 10
 * FLAGS, all inert at 6: -fno-strict-aliasing,
 *   -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *   -fno-expensive-optimizations, -fno-thread-jumps.
 * Worse: --no-rerun-cse (16), -fno-gcse (30), -fno-strength-reduce (27).
 */
void Func_801e858(unsigned char *dest, int b, int c, int d);

void Func_8020b64(int a, unsigned char *src)
{
	unsigned char buf[0x14];
	unsigned char c;
	int t;
	int n;

	n = 0;
	c = *src;
	t = c;
	while (t != 0) {
		buf[n] = c;
		src++;
		c = *src;
		t = c;
		n++;
	}
	buf[n] = 8;
	n++;
	buf[n] = 2;
	n++;
	while (n < 7) {
		buf[n] = 0x5f;
		n++;
	}
	buf[n] = 8;
	n++;
	buf[n] = 0xf;
	n++;
	buf[n] = 0;
	Func_801e858(buf, a, 0, -2);
}
