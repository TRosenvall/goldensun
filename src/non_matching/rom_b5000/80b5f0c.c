/* Func_80b5f0c (RunBattleOutroText) -- 0x080b5f0c.
 *
 * STILL PARKED, 49 of 155 stream entries.
 * NOT A TRUE DISTANCE, and this file ALREADY SAID SO -- see the next block.
 * Re-measured in batch 347: ref 153 instructions against our 151, 352 bytes
 * against 348.  WE ARE TWO SHORT, both in the last loop's pre-header.
 * Aligned residue (tools/aligncmp.py): 141 of 155 equal, 15 differing in 9
 * hunks -- re-measured unchanged.
 *
 * ===== BATCH 347 BRIEF A: THE SELECTION CRITERION MIS-FIRED ON THIS PARK ==
 *
 * This park was handed to brief A of batch 347 inside a pool of four selected
 * because "every one of your four park headers is wrong about its own figure --
 * each asserts in prose that its figure IS a true distance."  THAT IS TRUE OF
 * THE OTHER THREE AND FALSE OF THIS ONE.  Line 6 of the header below reads
 * `NOT A TRUE DISTANCE: 348 bytes against 352 and 153 encodings against 155 --
 * TWO INSTRUCTIONS SHORT`, which is exactly right and has been right since
 * batch 335.  The tree-wide sweep that built the pool matched the phrase
 * "TRUE DISTANCE" and counted the NEGATED form as an assertion.
 *
 * That matters beyond this file: the headline figure is "256 parks assert a
 * distance in prose and 94 are out of phase."  Whatever that number is, it is
 * an overcount by however many parks say "NOT a true distance", and those are
 * the parks whose analysis is SOUND -- so the sweep is biased towards handing
 * agents the parks that least need re-deriving.  The grep needs a negation
 * guard before the remaining 94 are allocated.  (Verified on the other three
 * targets of this brief: all three carry the real defect, the sentence
 * "Re-classified in batch 322; the figure IS a distance", two lines after
 * correctly saying the counts differ.  All three were batch-319 recipe
 * backfills; this one was not.)
 *
 * THE VERDICT BELOW ALSO SURVIVES RE-MEASUREMENT, which is worth saying because
 * the brief instructed that these four be treated as untrusted.  The aligned
 * residue is 141/155 to the encoding, the nine hunks are the nine the header
 * describes, and the two missing instructions are the two it names:
 *   ref  ... adds r4, r6, #0 / movs r3, #132 / lsls r3, #1 / adds r2, r6, r3 /
 *        ldr r3, [r2] / ... / bge / adds r0, r2, #0 / adds r2, r4, #0
 *   ours ... movs r4, #132 / lsls r4, #1 / adds r0, r6, r4 / ldr r3, [r0] /
 *        adds r2, r6, #0 / ... / bge          <- the last two copies absent
 * Of the nine hunks, two are pool-relative `ldr` offsets (a consequence of the
 * four-byte shortfall, not a defect), one is a one-instruction reorder of
 * `strb r3, [r4]` in the first clear loop, two are the scheduling swap of the
 * pool load against the size copy before the last call, and four are the
 * pre-header the header already analyses.
 *
 * ===== NEW RECORDED NEGATIVE: BOTH AUTOMATIC AXES ARE CLOSED =============
 *
 * `python3 tools/autosweep.py src/non_matching/rom_b5000/80b5f0c.c` (run on the
 * HOST -- it invokes docker itself and exits 2 inside the container):
 *
 *   FLAG AXIS, unchanged body, eight per-file groups, NOTHING BETTER:
 *     -O1                        147 of 155   aligned 15 -> 141, count -9
 *     -fno-gcse                  145 of 155   aligned 15 ->  79, count -8
 *     -fno-schedule-insns2        88 of 155
 *     -ffixed-r7                 142 of 155   aligned 15 ->  75, count +5
 *     -fno-strict-aliasing        49 of 155   TIES THE BASE
 *     -fno-strength-reduce       108 of 155
 *     -fno-rerun-cse-after-loop   78 of 155
 *     -fno-rerun-loop-opt        121 of 155   aligned 15 ->  49, count -4
 *   ANCHOR AXIS, 35 of 35 statement boundaries, device-free, NOTHING BETTER.
 *     Five anchors tie the base at 49 (anchor00, 04, 05, 13, 17); anchor30 --
 *     after the last loop's body -- reads ndiff 48 with dsize -4, which is
 *     MISALIGNMENT and not an improvement.
 *   autosweep's verdict: "The flag and anchor axes are CLOSED on this park."
 *
 * TWO OF THOSE ROWS ARE CANDIDATE PREREQUISITES RATHER THAN DEAD ENDS, per the
 * standing rule that a tie is not a negative: `-fno-strict-aliasing` and the
 * five tying anchors change nothing ALONE.  They have not been crossed with any
 * source variant, and this park's own history is that its levers compose (112 ->
 * 82 -> 69 -> 52 -> 49 was four levers, none of which worked alone).  Note the
 * anchor axis result is a claim about SINGLE anchors only -- all 31 subsets of
 * five in-loop boundaries were needed to settle one park in batch 346.
 *
 * ===== AND ONE CROSS-PARK NOTE FROM THE SAME BRIEF =======================
 *
 * This park's blocker is "the guard and the loop body must hold the address in
 * two DIFFERENT pseudos, and gcse's cprop at pass 07 commons every spelling
 * before loop.c runs."  Brief A's p2 (Func_8003e58, src/non_matching/rom_c0/
 * 8003e58.c) has the SAME blocker in the same words, and batch 347 spent a
 * sweep refuting the obvious lever for it: `cse.c:5992-5994` gates its copy
 * rewrite on `prev_nonnote_insn (insn)` being the insn that set the source, so
 * a copy placed at a DISTANCE from its source's definition should survive.
 * On that park the distance is BIT-INERT across all 27 partitions x 3 positions.
 * Do not spend this park's round on it either.
 *
 */
/* Func_80b5f0c (RunBattleOutroText) -- 0x080b5f0c, the ONLY function in
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a_a_c.s (datacheck: no data;
 * `grep -ci func_start` = 1).  Whole-file conversion when it closes.
 *
 * NON-MATCHING: 49 encodings of 155 differ (objcmp).
 * NOT A TRUE DISTANCE: 348 bytes against 352 and 153 encodings against 155 --
 * TWO INSTRUCTIONS SHORT, both in the last loop's pre-header, so everything
 * after that point is counted as differing whether or not it really is.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b5f0c.c \
 *     asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a_a_c.s --whole
 *
 * WHAT IS ALREADY RIGHT.  The whole first two thirds is instruction-for-
 * instruction identical apart from two scratch-register choices; the last loop's
 * BODY is byte-identical.  Four levers got it from 112/155 to 49/155:
 *
 * 1. `int size;` NAMED, AND USED AT EVERY SITE INCLUDING INSIDE THE LOOP.
 *    112 -> 82, and it is the one that matters.  The reference builds the
 *    allocation size in r5 and copies it (`mov r5, #0xaa / lsl r5, #1 /
 *    mov r0, r5`), which is the named-local tell.  But writing the literal
 *    `0xaa << 1` at the two IN-LOOP sites while naming only the allocation is
 *    WORSE than naming nothing: calls.c:855 precomputes any call argument whose
 *    `rtx_cost (value, SET) > 2` into a pseudo, and thumb's CONST_INT cost for a
 *    shiftable constant is COSTS_N_INSNS (2) = 6, so BOTH in-loop uses become
 *    pseudos, cse2 commons them, and loop.c hoists the result into a fifth
 *    callee-saved register -- which forces `p + 0x12a` onto the stack
 *    (`sub sp, #0x14` instead of `#0x10`, plus a str/ldr pair per iteration).
 *    Writing `size` at all five sites lets local-alloc give the pseudo r5 for
 *    the allocation and REMATERIALISE `mov rN, #0xaa / lsl rN, #1` at the later
 *    sites, exactly as the ROM does, and the spill disappears.
 *    This is a counter-case worth recording: naming a constant can REMOVE a
 *    spill rather than add a register, and the mechanism is argument precompute,
 *    not CSE.  `-fno-rerun-cse-after-loop` does NOT help here (121 of 155), and
 *    separate named locals per in-loop site do not either (82 of 155).
 *
 * 2. `k = buf[i] + 0x48; s[k] = i - 0x80;` -- THE NAMED BYTE INDEX.  82 -> 69.
 *    It is what keeps base and index in separate registers
 *    (`strb r2, [r4, r3]`); folded inline gcc emits `add r3, r9 / add r3, #72 /
 *    strb r2, [r3]`.  Copied straight from the landed sibling
 *    src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_c_c_b.c (Func_80b6378), which needs the
 *    same idiom for the same store.
 *
 * 3. `int *cnt` NAMED FOR THE COUNT ADDRESS in the last loop, plus its own index
 *    local `j`.  69 -> 52.  Inline, gcc recomputes `p + 0x108` after the guard.
 *
 * 4. A SEPARATE COUNTER `m` FOR THE LAST LOOP.  52 -> 49.  Sharing `i` with the
 *    two earlier loops pins the counter to the callee-saved r5; the reference
 *    runs that loop entirely in r0-r4 (there is no call in it), which only
 *    happens when the counter is its own pseudo.
 *
 * 5. `int fill = 0xff;` for the 8-byte clear loop.  50 -> 49 (one scheduling
 *    slot: the reference materialises 0xff BEFORE the counter).
 *
 * THE BLOCKER, and it is arithmetic rather than cosmetic.  The reference's last
 * loop pre-header is
 *      mov r4, r6 / mov r3, #0x84 / lsl r3, #1 / add r2, r6, r3 /
 *      ldr r3, [r2] / mov r1, #0 / add r4, #8 / cmp r1, r3 / bge exit /
 *      mov r0, r2 / mov r2, r4
 * -- eleven instructions, the last two being COPIES of the count address and the
 * element pointer into the registers the loop body uses (r0 and r2).  We emit
 * the same nine instructions but compute straight into r0 and r2, so the copies
 * are absent.  Those copies are loop.c's work: `mov r2, r4` is the
 * strength-reduced element pointer's initial value emitted by emit_iv_add_mult,
 * and `mov r0, r2` is the hoisted `p + 0x108` after cse2 replaced the
 * recomputation with a copy from the value the ENTRY GUARD already had.  For
 * both to exist the guard and the loop body must hold the address in two
 * DIFFERENT pseudos, and no source spelling tried reaches that state:
 *
 *   for (m = 0; m < *cnt; m++)                                   153 (49)
 *   while (m < *cnt) { ... m++; }                                153 (49)
 *   m = 0; if (m < *cnt) do { ... } while (m < *cnt);            153 (49)
 *   the pointers assigned inside the guard                       153 (50)
 *   `e = p; e += 8;` instead of `e = p + 8;`                     153 (52)
 *   `q = e;` / `c2 = cnt;` explicit copies into loop locals      153 (49)
 *   `e[m * 4 + 2]` (an index, not a walk)                        153 (49)
 *   a `struct E *e` with `e[m].b`                                151 (70)
 *   one `struct T *t` carrying both `t->n` and `t->e[m].b`       153 (53)
 *   the count address inline instead of named `cnt`              153 (80)
 * Every one of those is exactly two instructions short in the same place, which
 * is the argument that the residue is a pass artefact and not a spelling.
 *
 * ALSO STILL DIFFERING (inside the 49, downstream of the shortfall): the
 * reference uses r4 as the low scratch for `mov r4, r11` / `mov r4, r9` /
 * `mov r4, r8` / `mov r4, r6` where we get r0 or r1 -- allocate_reload_reg's
 * rotation, per docs/elevation.md's "Reload registers rotate".
 *
 * MEASURED INERT: the CopyFn typedef returning `int` rather than `void` (49
 * either way -- the callee-return-type lever does not reach this file);
 * assigning `copy = Func_8001af8;` once outside the loop instead of per call
 * site (52); `0x154` flat instead of `0xaa << 1`.
 *
 * NO SHIMS in this draft: no pin, no barrier, no volatile, no .equ, no .sym.
 *
 * The indirect-call idiom (a local function pointer, from
 * src/rom_c9000/rom_e0524.c) reproduces both `ldr r3, =Func_8001af8 /
 * bl _call_via_r3` sites first time, and `int` with no return statement is right
 * for the `pop {r1}` epilogue.
 *
 * ===== BATCH 335 BRIEF B: FIGURE RE-DERIVED, DIRECTION STATED, AND SEVENTY-ONE
 * ===== NEW CELLS ACROSS FOUR DIMENSIONS -- EVERY ONE STILL TWO SHORT.
 *
 * Figure re-derived unchanged: forty-nine of one hundred and fifty-five stream
 * encodings, ours one hundred and fifty-three, first at index twelve, three
 * hundred and fifty-two bytes against three hundred and forty-eight.  BODY
 * UNCHANGED.  DIRECTION: the REFERENCE IS LONGER, by two, so we are MISSING two.
 *
 * tools/aligncmp.py puts the whole residue in one place and it is small:
 * ALIGNED-EQUAL ONE HUNDRED AND FORTY-ONE OF ONE HUNDRED AND FIFTY-FIVE, fifteen
 * differing in nine hunks.  Of those nine, two are pool-relative `ldr` offsets,
 * one is a one-instruction reorder in the first clear loop, two are a scheduling
 * swap of the pool load against the size copy before the last call, and the rest
 * are the park's named pre-header.  The loop BODY is byte-identical.
 *
 * FOUR CROSSED SWEEPS, ALL STILL ONE HUNDRED AND FIFTY-THREE INSTRUCTIONS:
 *
 *  1. GUARD-READ against LATCH-READ, varied INDEPENDENTLY (the park varied them
 *     together, which is the diagonal of the square), crossed with walk/index
 *     access: eight cells.  All flat.  gcse's cprop is global
 *     (`gcse.c:3902`), so the entry test's and the latch test's count reads are
 *     one expression however they are spelled.
 *  2. ELEMENT ACCESS against COUNT ADDRESS, five by seven, thirty-five cells.
 *     Access as a byte walk, a byte index, a struct-array index, a struct walk
 *     and an explicit copy pointer; address as named, inline, TWO DIFFERENT
 *     ASSOCIATION ORDERS of the same quantity, an `int *` against `long *`
 *     ALIAS-SET SPLIT, two named pointers, and two named pointers at different
 *     associations.  ALL THIRTY-FIVE FLAT.  Both of the brief's standing levers
 *     reach the right pass and neither splits THIS expression, and the reason is
 *     worth recording as a bound with its evidence: the quantity is a `plus` of
 *     a pseudo and a constant, NOT a MEM, so `gcse.c:1546-1551`'s refusal to
 *     merge MEMs in different alias sets never applies to it.
 *  3. TWO-ROLE LOCAL REUSE, twelve cells (element pointer `e` or `u`, byte index
 *     `j` or `k`, counter `m`, `i` or `n`).  This dimension came from the
 *     bank-mate src/rom_b5000/rom_b8228_c_a_c_c_a_c_c.c, whose header records
 *     that a two-role local MIS-PRICES THE ALLOCNO PRIORITIES.  FIVE CELLS REACH
 *     ONE HUNDRED AND FIFTY-FIVE INSTRUCTIONS -- the reference's length -- AND
 *     ALL FIVE ARE THE BATCH-334 TRAP.  Reusing `k` for the byte index costs an
 *     extra register copy at the `s[k] = i - 0x80` store, in the FIRST loop,
 *     while the pre-header stays two short; the best of them reads ALIGNED-EQUAL
 *     ONE HUNDRED AND TWENTY-SIX against this body's ONE HUNDRED AND FORTY-ONE.
 *     A worse body with a matching count.  DO NOT CHASE THE COUNT HERE.
 *  4. EXHAUSTIVE STATEMENT ORDER: all six permutations of the three pre-loop
 *     assignments, crossed with do/while against `for` and with `e = p + 8`
 *     against `e = p; e += 8;` -- twenty-four cells, every one of them not merely
 *     flat but BIT-IDENTICAL (the same one hundred and forty-one aligned and the
 *     same nine hunks).  gcc canonicalises that block completely.  The bank-mate
 *     header's "permute the source statements exhaustively" rule does NOT reach
 *     this block, and that is a useful limit on it.
 *
 * WHAT THE REGISTER ASSIGNMENT ADDS.  We place the 0x108 constant in r4 and let
 * the count-address pseudo land directly in r0, the loop's own register; the
 * reference places the constant in r3, IMMEDIATELY REUSES r3 FOR THE LOADED
 * COUNT, puts the address in r2 and the element pointer in r4, and only then
 * shuffles both into r0 and r2.  By the bank-mate's rule -- a constant in r4 in
 * a call-free prologue is a conflict-count readout -- our pre-guard block holds
 * one live value too many: five low registers against the reference's four,
 * because OUR count-address pseudo IS the loop's pseudo and so is live across
 * the whole loop instead of dying at a copy.
 *
 * SO THE PARK'S VERDICT SURVIVES, and it is now the better-evidenced half of
 * this file.  The residue needs the guard and the loop to hold each address in
 * two DIFFERENT pseudos, which requires loop.c -- pass 08 -- to hoist an in-loop
 * computation into its own invariant pseudo, and gcse at pass 07 has already
 * commoned every in-loop spelling of that computation with the guard's before
 * loop.c runs.  The brief's standing lever for exactly this is stated for an
 * IN-LOOP `symbol_ref`, which gcse keeps as a separate pseudo; here the base is
 * an allocator result rather than a `symbol_ref`, so the one mechanism known to
 * defeat gcse's commoning is unavailable to this function.
 *
 * NEXT: disagree with `gcse.c:3902`'s globality or find a non-`symbol_ref` way
 * to keep an in-loop address computation alive into pass 08.  Do NOT re-sweep
 * loop shape, statement order, association order or alias-set typing -- that is
 * seventy-one measured cells now, in four shapes.
 */
/* Func_80b5f0c (RunBattleOutroText) -- 0x080b5f0c, only function in
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a_a_c.s (no data, 1 func_start).
 * Template: src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_a_b.c (Func_80b5e14, its
 * opening counterpart) and src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_c_c_b.c
 * (Func_80b6378, the `k = buf[i] + 0x48; p[k] = i - 0x80;` idiom).
 * `pop {r1}` epilogue -> declared int with no return statement.
 * Indirect call idiom from src/rom_c9000/rom_e0524.c: a local function pointer.
 */
typedef int (*CopyFn)(void *dst, void *src, int len);

extern unsigned int iwram_3001e74;
extern void *Func_8004970(int size);
extern void free(void *p);
extern unsigned char *_GetUnit(int id);
extern int Func_80b6a60(unsigned short *buf);
extern int Func_8001af8(void *dst, void *src, int len);
extern int Func_80063bc(void *buf, int size);
extern void Func_8006458(void);
extern void WaitFrames(int n);
extern void *_Func_8077330(int side);

int Func_80b5f0c(void)
{
	unsigned short buf[8];
	unsigned char *p;
	unsigned char *s;
	unsigned char *u;
	unsigned char *e;
	CopyFn copy;
	int i;
	int n;
	int size;
	int k;
	int *cnt;
	int j;
	int m;
	int fill;

	size = 0xaa << 1;
	p = Func_8004970(size);
	s = *(unsigned char **)&iwram_3001e74;
	fill = 0xff;
	for (i = 7; i >= 0; i--)
		s[i + 0x48] = fill;
	n = Func_80b6a60(buf);
	for (i = 0; i < n; i++) {
		u = _GetUnit(buf[i]);
		copy = Func_8001af8;
		copy(p, u, size);
		p[0x95 << 1] = 2;
		k = buf[i] + 0x48;
		s[k] = i - 0x80;
		if (Func_80063bc(p, size) == -1)
			break;
		Func_8006458();
		WaitFrames(2);
	}
	while (i <= 2) {
		p[0x95 << 1] = 0;
		if (Func_80063bc(p, size) == -1)
			break;
		Func_8006458();
		WaitFrames(2);
		i++;
	}
	size = 0xa0 << 1;
	free(p);
	p = Func_8004970(size);
	copy = Func_8001af8;
	copy(p, _Func_8077330(0), size);
	cnt = (int *)(p + (0x84 << 1));
	e = p + 8;
	for (m = 0; m < *cnt; m++) {
		j = e[2] + 0x48;
		e[2] = s[j];
		e += 4;
	}
	if (Func_80063bc(p, size) != -1) {
		Func_8006458();
		WaitFrames(1);
		WaitFrames(2);
	}
	free(p);
}
