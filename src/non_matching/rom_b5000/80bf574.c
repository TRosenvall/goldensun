/* Func_80bf574 (0x080bf574) -- NON-MATCHING.
 *
 * NON-MATCHING, **3 differing encodings of 24** (MEASURED batch 323 brief B,
 * RE-DERIVED batch 326 brief F: ref 24 / ours 24, 24 instructions each, --whole
 * agrees, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent).
 * Was parked at 15 of 24.  PIN-FREE, and the memory profile MATCHES the
 * reference (ldr=1 ldrb=1 strb=2 -- no MEM flag from tools/crossfire.py).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80bf574.c asm/rom_b5000/rom_bbb0c_a_c_c_a_c_a.s --func Func_80bf574
 *
 * --whole reports the SAME 3 (the .s holds this function alone, so there is no
 * section-tail or sibling effect hiding in the --func figure).
 * tools/datacheck.py on the reference is silent: no data section.
 *
 * FIRST, TWO CORRECTIONS TO THE OLD PARK.
 *
 * 1. Its "24 lines against the ROM's 25 -- ONE SHORT -- with 16 differing" was
 *    reporting objcmp's 15, and 15 was MISALIGNMENT, not distance.  The ROM is
 *    23 instructions plus one pool word; the park's body was 22 instructions
 *    plus an alignment half-word plus the pool word.  Both come to 24
 *    encodings, so the COUNT looked equal while the instruction streams were
 *    one apart, and ten of the fifteen "differences" were identical text at a
 *    shifted index.  Only ONE cause was ever present.
 *
 * 2. Its verdict -- "gcc will not produce the pair, because it is right not to
 *    ... There is no source spelling for 'please compute this redundantly'" --
 *    is REFUTED.  There is one, and it is the ordinary one: name the
 *    decremented byte.  `unsigned char v = *p + 0xff; *p = v;` makes the
 *    comparison need a real 8-bit value, so combine can no longer collapse the
 *    zero-extension into the lsl-only form it used for `(x << 24) != 0`, and
 *    the ROM's `lsl`+`lsr` pair appears in place.  The redundancy is not
 *    redundant once the value is also stored.  15 -> 3 on the first candidate.
 *
 * WHAT THE RESIDUE OF 3 IS: A ROTATION OF THREE INSTRUCTIONS, NOT A DISTANCE.
 *
 *     rom    adds r3, #0xff / strb r3, [r1] / lsls r3, #24 / lsrs r3, #24 / cmp
 *     ours   adds r3, #0xff / lsls r3, #24  / lsrs r3, #24 / strb r3, [r1] / cmp
 *
 * Everything else -- all 21 other encodings, the pool word and both
 * relocations -- is exact.  The ROM stores the RAW sum and then zero-extends in
 * place; we zero-extend first and store the extended value.
 *
 * WHY, FROM `.23.sched2`.  sched2 has NO freedom here.  Block 1's dependence
 * table is a single chain -- 33(add) -> 36(lsl) -> 37(lsr) -> 43(store) ->
 * 45(cmp+branch) -- because `v` is a QImode pseudo and the store reads the QI
 * subreg of the SImode register the extension writes.  The ROM needs the store
 * to be a LEAF off the add, parallel to the extension.  So this is an RTL-shape
 * question, not a scheduling one, and no `-fsched` lever can reach it.
 *
 * THE OTHER HORN, AND WHY IT IS A WRONG PROGRAM RATHER THAN A BETTER BODY.
 *
 * Making the store independent of the extension IS possible -- store an `int`
 * and truncate in a separate statement:
 *
 *     t = *p + 0xff;  *p = t;  t = (unsigned char)t;  if (t != 0) ...
 *
 * and that reproduces the ROM's `add / strb / lsl / lsr / cmp / bne` EXACTLY,
 * indices 9-14 and 17-22 all green.  It reads 9, and it is NOT an improvement:
 * crossfire flags it COUNT MEM.  It is 23 instructions against 24 and it has
 * LOST the pool word -- with `u` displaced to r1 the offset 0x146 stays live in
 * r0, and reload_cse_move2add (reload1.c:8840) derives 0x147 from it as
 * `adds r0, #1` instead of loading the ROM's `.word 0x147`.  One fewer `ldr`
 * than the reference is a wrong program, so the 9 is a figure ABOUT this
 * blocker and not a candidate.  Six crossed edits on that body were inert or
 * worse (best 9, next 10).
 *
 * So the two horns are mutually exclusive as measured: the spelling that gets
 * the ORDER right loses the allocation, and the spelling that gets the
 * ALLOCATION right cannot separate the store from the extension.  3 is the
 * better and memory-faithful one.
 *
 * MEASURED, ALL WORSE (tools/crossfire.py, depth 2, base = this body):
 *   final store through `p = u + 0x147`                             8
 *   `*p = *p + 0xff; v = *p;` (re-read after the store)        19, MEM
 *       -- gcc emits a fresh `ldrb` reload, not a truncation; this is the
 *          park's own observation reproduced, and the MEM flag now says why it
 *          can never be right: the reference has one `ldrb`, this has two.
 *   `*p = v = *p + 0xff;` (chained assignment)                      4
 *   `int t` + separate truncation, six crossings            9-18, mostly MEM
 * EXACTLY INERT (candidate prerequisites, all 3): declaration order with `v`
 *   first, `if (v)` for `if (v != 0)`, `if (!*p)` for `if (*p == 0)`,
 *   `*p - 1` for `*p + 0xff`, and writing the companion offset as
 *   `(0xa3 << 1) + 1` instead of `0x147`.
 *
 * ===== BATCH 326 BRIEF F: THE TWO HORNS ARE ONE CAUSAL CHAIN, NAMED =====
 *
 * Figure re-derived with tools/objcmp.py: **3 differing encodings of 24**,
 * ref 24 / ours 24, 24 instructions each, and --whole agrees (3 of 24, first at
 * index 10).  SIZE, INSTRUCTION COUNT and RELOCATIONS all silent -- no pad is
 * absorbing anything.
 *
 * The park said the two horns are "mutually exclusive as measured".  They are
 * mutually exclusive BY A FIVE-STEP CHAIN, and the int-temp horn's 9 is not a
 * spelling problem:
 *
 *   1. The ROM's RTL needs a SEPARATE truncation insn, which only an `int` temp
 *      gives.  With the QImode variable `*p = v` depends on the extension --
 *      `(set (mem:QI p) (subreg:QI v))` where v IS the extended value -- so the
 *      store can never be a leaf off the add.
 *   2. In the int-temp shape the compare's `zero_extend (mem:QI (reg 33))` DIES
 *      at the branch (`REG_DEAD (reg:SI 37)` on jump_insn 24) and block 1 emits
 *      a SECOND `zero_extend (mem:QI (reg 33))`.  reload_cse deletes the
 *      duplicate load later so `ldrb` stays at 1, but **`p`'s REG_N_REFS is 4,
 *      not 3**: `.17.lreg` says `Register 33 used 4 times across 6 insns`
 *      against the v-body's `3 times across 8 insns`.
 *   3. allocno_compare (global.c:597-620) therefore ranks `p` at
 *      floor_log2(4)*4/6 = 13333 instead of floor_log2(3)*3/8 = 3750, and
 *      `.18.greg`'s order line proves the move: v-body
 *      `;; 5 regs to allocate: 42 48 36 33 32`, int-temp
 *      `;; 4 regs to allocate: 34 33 46 32` -- `p` goes from 4th to 2nd.
 *   4. `u` is allocated LAST in both bodies and `REG_ALLOC_ORDER` on ARM is
 *      `{3, 2, 1, 0, ...}` (arm.h:989, DESCENDING), with global.c's `find_reg`
 *      (:1024-1048) taking the FIRST non-conflicting register in that order.
 *      So `u in 0` in the matching body is pure ELIMINATION.
 *   5. `u` in r1 leaves the 0x146 offset live in r0, and `reload_cse_move2add`
 *      (reload1.c:8891) rewrites `(set (reg r0) (const_int 0x147))` into
 *      `adds r0, #1`, deleting the ROM's pooled `.word 0x147`.  THAT is the
 *      COUNT+MEM flag: 23 encodings against 24 with `ldr` 0 against 1.
 *
 * So the 9 is ONE cause with three symptoms and the visible symptom (the
 * missing pool word) is three steps downstream of the fixable one.
 *
 * ===== WHAT ELIMINATION ACTUALLY TURNS ON -- READ OFF THE CONFLICT LINES =====
 *
 * The earlier next-step said "find an int-temp body with THREE MUTUALLY
 * CONFLICTING allocnos ahead of `u`".  **That framing is wrong and this is the
 * correction.**  `;; N conflicts:` prints allocno conflicts first (numbers >= 32)
 * and then `hard_reg_conflicts` (numbers < 32) -- global.c:1894-1901 -- and the
 * hard-register bits are what decide it:
 *
 *   v-body (3 of 24, u in r0):   42->r3  48->r2  36->r2  33->r1  32->r0
 *     36 conflicts: 32 33 36 37 **3** 13      <- hard r3
 *     33 conflicts: 32 33 36 37 41 42 43 **3** 13
 *     32 conflicts: 32 33 36 37 41 42 43 **3** 13
 *   int-temp (9 of 24, u in r1): 34->r3  41->r2  33->r2  32->r1
 *     34 conflicts: 32 33 34 41 13            <- NO hard r3, and `34 preferences: 3`
 *     33 conflicts: 32 33 34 13               <- NO hard r3
 *     32 conflicts: 32 33 34 37 **3** 13
 *
 * The hard-r3 bit comes from local-alloc having already parked the block-local
 * pseudos in r3 (`.17.lreg`: `;; Register 37 in 3.`, `;; Register 41 in 3.`,
 * `;; Register 43 in 3.`), so every allocno whose range crosses one of them
 * inherits a conflict with r3.  In the v-body BOTH long-lived conflictors of
 * `u` carry that bit, are pushed off r3 onto r2 and r1, and `u` then has THREE
 * independent blocks (its own hard r3, 36 on r2, 33 on r1) and lands in r0.
 * In every int-temp body at most ONE of `p`/`t` carries the bit, so the other
 * takes r3 -- the register `u`'s own hard conflict already blocks -- the two
 * blocks collapse onto one register, and r1 is free.
 *
 * **The requirement is therefore: BOTH long-lived conflictors of `u` must carry
 * the hard-r3 conflict**, i.e. both `p` and the value carrier must be live
 * across the block-0 double-read temp that local-alloc puts in r3.  That is a
 * statement about LIVE RANGES, not about allocno count, and it is the next
 * thing to engineer.
 *
 * MEASURED ON THE INT-TEMP HORN: **17 bodies, `u` is the LAST allocno in every
 * one and lands in r1 in every one.**  Eight inherited (v00_base, v04_sepv,
 * w04_readviaU, w12_bbyte, w13_t2, x01_intb, x04_intb_t2, x07_intb_q) and nine
 * new ones built to span block 0 with a SINGLE reused `int t` (t = *p; if (t ==
 * 0); t = t + 0xff; *p = t; t = (unsigned char)t): plain, `t += 0xff`, `t - 1`,
 * `t & 0xff`, `(t << 24) >> 24`, `if (!t)`/`if (t)`, and three declaration
 * orders, plus `unsigned int t`.  Figures: 9 for all but `t & 0xff` (**15**),
 * `(t<<24)>>24` (**10**) and `t - 1` (**10**).  All 23 instructions, all with
 * `ldrb 1 strb 2` and no `ldr` -- i.e. all have lost the pool word.
 *
 * Earlier, also all exactly 9 with an identical COUNT+MEM profile: declaration
 * order x3, `int`/`unsigned int`/separate carrier/second carrier, the pointer
 * eliminated (`u[0xa3<<1]`, `*(u + (0xa3<<1))`), read-via-u / store-via-u split,
 * second pointer `q = p`, the named byte in three types, the byte reused as the
 * carrier, `(0xa3<<1)+1` for 0x147, `u[0x147]`, `if (t)`, `p = 0` after the
 * store.  An index variable reads 22; the final address named early reads 17.
 *
 * ===== A FAMILY NOBODY HAD CHECKED, NOW CLOSED, WITH ITS EVIDENCE =====
 *
 * `mov r4, sl`-style "replace a constant with a register that already holds it"
 * exists in this compiler and runs AFTER reload, so it was worth asking whether
 * the ROM's shape comes from there rather than from source.  It cannot, and the
 * same arithmetic shuts both doors:
 *
 *   * `reload_cse_simplify_set` (**reload1.c:8046-8056**) takes an equivalent
 *     register when `this_cost < old_cost`, or on a tie because "If equal costs,
 *     prefer registers over anything else".
 *   * `reload_cse_simplify_operands` (**reload1.c:8219-8224**) -- optional
 *     reloading -- gates a CONST_INT operand on
 *     `rtx_cost (operand, SET) > rtx_cost (reg, SET)`.
 *
 * and on Thumb `arm_rtx_costs` returns **0** for any `const_int` below 256 with
 * `outer == SET` (**arm.c:2078-2081**), `REGISTER_MOVE_COST` is **4** whenever
 * HI_REGS is involved and 2 otherwise (**arm.h:1280-1285**), and
 * `rtx_cost ((reg), SET)` is **1** = `! CHEAP_REG` (**cse.c:805**, CHEAP_REG at
 * **cse.c:505-507**; `CHEAP_REGNO` is false for r10, which is neither fp/sp/ap
 * nor `FIXED_REGNO_P`).  `4 < 0` and `4 == 0` are both false, and `0 > 1` is
 * false.  **BOUND: on Thumb no post-reload pass can replace a `const_int` below
 * 256 with a register.**  (Checked on the sibling park 80b6d30, same bank, where
 * the identical question arises at its index 23.)
 *
 * STILL RIGHT, inherited from the old park and re-confirmed: the `mov`+`lsl`
 * construction of the even 0xa3<<1 offset, the pooled 0x147 companion offset,
 * the CSEd double read at the top (`ldrb r2 / mov r3, r2 / cmp r3, #0` -- the
 * batch-178 lever, and naming the byte AFTER the store is what collapses it),
 * the `add r3, #0xff` decrement rather than `(*p)--`, the inverted `bne` guard
 * that distinguishes this sibling from its eleven mates, and the shared
 * `mov r0, #0` exit.
 *

 * ===== BATCH 327 BRIEF H: FOUR MORE int-temp BODIES, AND THE ONE UNTRIED
 * ===== LEVER ON THAT HORN
 *
 * Figure re-derived: **3 differing encodings of 24**, ref 24 / ours 24, first at
 * index 10, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent.  BODY UNCHANGED.
 *
 * Treating the rejected rows as candidates, four int-temp bodies the park's 17
 * do not include were built -- the cast moved into the TEST so the carrier has
 * `REG_N_SETS == 1` rather than 2:
 *   `int v; v = *p + 0xff; *p = v; if ((unsigned char)v != 0) ...`   23 insns
 *   the same with `unsigned int v`                                   23 insns
 *   the same with `u[0x147] = v`                                     23 insns
 *   single read `v = *p; if (v == 0); v += 0xff; *p = v; ...`        22 insns
 * The first three read "19 differ" and **objcmp's own note says the 19 is
 * MISALIGNMENT, not distance**: size and encoding count match while the
 * instruction count is 23 against 24, so a pad is absorbing the difference.
 * They are three more members of the park's 9-of-24 family with the 0x147 pool
 * word gone, which independently confirms step 5 of the five-step chain.
 * `REG_N_SETS` on the carrier is therefore NOT the lever.
 *
 * THE ONE UNTRIED LEVER ON THE int-temp HORN, with its line.
 * `reload_cse_move2add`'s rewrite is gated on `reg_set_luid[regno] >
 * last_label_luid` (**reload1.c:8871**), and `last_label_luid` is reset at every
 * CODE_LABEL (**:8855-8857**).  So a CODE_LABEL between the insn that puts 0x146
 * into r0 and the insn that puts 0x147 there would save the pool word and leave
 * the int-temp horn at 24 instructions.  The int-temp body has no label on that
 * path and manufacturing one costs a branch -- but it is a different lever from
 * the allocation one the park names, and nobody has tried it.
 * Why the reference is immune at all: there the two offsets live in DIFFERENT
 * registers -- 0x146 is built in **r3** (`mov r3,#0xa3 / lsl r3,#1`) and r3 is
 * then REUSED for the byte, while 0x147 is pooled into **r1** -- so move2add has
 * nothing to derive from.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf574(void)
{
	unsigned char *u;
	unsigned char *p;
	unsigned char v;

	u = _GetUnit();
	p = u + (0xa3 << 1);
	if (*p == 0)
		goto fail;
	v = *p + 0xff;
	*p = v;
	if (v != 0)
		goto fail;
	*(u + 0x147) = v;
	return 1;
fail:
	return 0;
}
