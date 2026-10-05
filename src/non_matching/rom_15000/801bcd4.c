/* Func_801bcd4 -- PARK IMPROVED 48 -> 16, and the CROSS-JUMP BOUND IS REFUTED.
 * Batch 326 brief A.
 *
 *   16 differing encodings of 81.  ref 81 encodings, ours 81 -- EQUAL.
 *   SIZE IDENTICAL (196 bytes).  RELOCATIONS IDENTICAL -- both
 *   `R_ARM_THM_CALL LoadInventoryIcon` are back, so the cross-jump is GONE.
 *   90 instruction lines against the ROM's 90, matching LINE FOR LINE except
 *   the prologue and one register role.  The previous body read 48 of 81 at
 *   80 encodings / 192 bytes with shifted relocations.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801bcd4.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_c.s --func Func_801bcd4
 *
 * ================= THE ONE EDIT: `default: break;` INSTEAD OF `default: return s;`
 *
 * Semantically identical -- no call sits between the two, so both yield `s` --
 * and gcc even reproduces the ROM's own two-label exit (`.L1bd82: ldr r4,[sp,#8]`
 * followed by `.L1bd84: mov r0,r4`, with the out-of-range `bhi` and the two
 * unused jump-table words aimed at the SECOND label, skipping the reload).
 *
 * WHY IT MOVES 32 ENCODINGS, read in the dumps rather than inferred:
 *
 *   `s` is address-taken, so every by-value read of it is a MEM read, and
 *   `.07.gcse` PREs all of them into one pseudo (reg 65 / 62 / 63 depending on
 *   the body).  Each three-argument arm's own eval-time temp must be its
 *   quantity's `first_reg` to survive `canon_reg` (cse.c:2630-2646, which
 *   substitutes `first_reg` unconditionally for any pseudo with a valid
 *   quantity).  `make_regs_eqv`'s promotion test (cse.c:1026-1035) needs
 *
 *       uid_cuid[REGNO_LAST_UID (temp)] > uid_cuid[REGNO_LAST_UID (PRE pseudo)]
 *
 *   ** AND THE PRE PSEUDO'S LAST MENTION WAS IN THE `default:` ARM. **  With
 *   `default: return s;` that arm is laid out AFTER every other arm (insn 159
 *   at `code_label 156`, bb 11, in `.07.gcse`), so NO arm's temp can outlive
 *   it -- not the last arm's, and not a shared function-scope temp spanning all
 *   four three-argument arms, which I measured at 48 FLAT with insn 85's source
 *   still rewritten at `.09.cse2`.  Written `default: break;`, that arm reads
 *   the stack slot through the common exit instead, the PRE pseudo's last
 *   mention moves back into case 9, the eval-time temps stay canonical, insn 78
 *   survives, and r2 is set FIRST in every arm.
 *
 * > THE PARK'S BOUND SAID "THE SUFFIX LENGTH *IS* THE COLLAPSE ... THERE IS NO
 * > SECOND DOOR".  That is right about the mechanism and wrong about the bound:
 * > the door was the DEFAULT ARM, which is not part of the suffix at all.  The
 * > park correctly identified `load_register_parameters` as emitting r2 last
 * > (confirmed: calls.c:1692-1695 loops FORWARD because arm defines no
 * > `LOAD_ARGS_REVERSED`), and correctly identified `canon_reg` as the actor;
 * > what it got wrong was treating the PRE pseudo's lifetime as fixed "by
 * > construction".  IT IS SET BY WHERE THE LAST READ OF `s` IS.
 *
 * ================= WHAT THE REMAINING 16 IS -- ONE PROLOGUE COPY CHAIN
 *
 *   rom   mov r8,r3 / mov r3,#1 / sub sp,#0xc / mov r4,r2 / neg r3,r3 /
 *         mov r7,r0 / mov r5,r1 / str r2,[sp,#8] / mov r6,r4
 *   ours  mov r5,r2 / mov r8,r3 / mov r3,#1 / sub sp,#0xc / mov r4,r5 /
 *         neg r3,r3 / mov r7,r0 / mov r6,r1 / str r5,[sp,#8]
 *
 * NINE INSNS EACH.  The ROM has THREE pseudos for the slot value -- the
 * parameter (allocated to the INCOMING r2, so `(set reg34 r2)` is a noop and
 * vanishes), the PRE pseudo (r4), and a third for the early return (r6, copied
 * from r4).  We have TWO: the parameter lives across the call in r5 (because
 * the early return reads it) and the PRE pseudo is r4.  Our `mov r5,r2` stands
 * where the ROM's `mov r6,r4` does.  The other 7 of the 16 are the consequent
 * r5/r6 swap for the second argument `b`, one per arm plus the early return.
 *
 * ** SO THE 16 IS THE SAME COPY-COLLAPSE FAMILY AGAIN, now in the prologue, and
 * the blocker is cse2's EXTENDED BASIC BLOCK. **  Traced on a
 * `s = slot; s0 = s; ... return s0;` body (scratch_elev/b326/A/dump/bcd4s0):
 * `.02.jump` through `.08.loop` hold `insn 36 (set (reg/i:SI 0 r0) (reg/v:SI
 * 37))` with `s0` = reg 37 intact; at `.09.cse2` it reads `(reg/v:SI 34)` and
 * reg 37 is dead by `.12.life`.  There is NO CODE_LABEL between the function
 * entry and the early return -- the guard's fall-through path has none -- so
 * `cse_end_of_basic_block` (cse.c:6572, `while (p && GET_CODE (p) !=
 * CODE_LABEL)`) puts reg 37's WHOLE live range inside one cse2 block, promotion
 * sub-condition (i) fails, and `canon_reg` rewrites the return to the parameter.
 *
 * MEASURED on top of this body, all EXACTLY INERT at 16 (crossfire, 5 edits):
 *   `s0 = s` + compare s0 + return s0; `s0 = s` + compare s0 + return slot;
 *   `s0 = s` + compare the PARAMETER + return s0; `return -1` in the guard
 *   (legal -- `s` is -1 there); and a block-scoped `int s0 = s;` placed INSIDE
 *   the guard, after the compare.  Spelling does not reach it, exactly as the
 *   family's law predicts.
 * MEASURED AND WORSE: `t = 0;` as a separator reads 80 at 83 insns; a shared
 *   function-scope arg temp `u` with `default: u = s; return u;` reads 65 at 78.
 *
 * NEXT, named: give the early-return pseudo a live range that LEAVES cse2's
 * first extended block, or get a CODE_LABEL between the entry and the early
 * return without paying an instruction.  REFUTED IF either is reachable.
 *
 * SIBLINGS BY SIGNATURE (unverified): any park whose residue is "two copies of
 * one value in the opposite direction" where one of the two is a parameter
 * copy and the other is read after a call.  The diagnostic is cheap -- grep
 * `.09.cse2` for a `(set (reg/i:SI 0 r0) (reg/v:SI N))` whose N differs from
 * `.08.loop`'s.
 */

/* Func_801bcd4 -- PARK STANDS.  Batch 325 brief B: figure re-derived, and the
 * park's PASS ATTRIBUTION REFUTED by its own dumps.
 *
 *   48 differing encodings of 81.  ref 81 encodings / 196 bytes, ours 80 / 192.
 *   Re-measured in batch 325 brief B; identical to the batch-324 figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801bcd4.c asm/rom_15000/rom_1aeec_a_a_c_c.s --func Func_801bcd4
 *
 * INDEPENDENT PROOF OF THE MERGE, from objcmp's relocation list: the reference
 * carries TWO `R_ARM_THM_CALL LoadInventoryIcon` (0x74 and 0x80); we carry ONE
 * (0x7a).  The cross-jump is not an inference.
 *
 * =============== REFUTED: "cse2 propagates 65 into insn 84, insn 78 is left a
 * dead copy that delete_trivially_dead_insns removes.  THE SURVIVOR IS INSN 84."
 *
 * `.09.cse2` actually holds BOTH insns (scratch_elev/b325/B/dump/bcd4):
 *
 *   (insn 78 (set (reg:SI 48) (reg:SI 65)))        <- STILL PRESENT at cse2
 *   (insn 80 (set (reg:SI 0 r0) (reg/v:SI 33)))
 *   (insn 82 (set (reg:SI 1 r1) (const_int 58)))
 *   (insn 84 (set (reg:SI 2 r2) (reg:SI 65)))      <- its SRC was (reg 48)
 *
 * So this is NOT the cse.c:5959 swap -- that needs a PSEUDO dest (insn 84's dest
 * is hard reg r2) and would have rewritten insn 78's DEST, which it did not.  It
 * is plain `canon_reg` substituting the quantity's `first_reg` into insn 84's
 * SET_SRC.  `delete_trivially_dead_insns` is again only a janitor.
 *
 * =============== AND `.07.gcse` IS PRE, NOT "COMMONING" -- IT INSERTS INSNS
 *
 *   (insn 225 (set (reg:SI 65) (reg:SI 41)))   inserted after the 1st read of s
 *   (insn 226 (set (reg:SI 65) (reg:SI 43)))   inserted after the 2nd read of s
 *
 * reg 65 is a gcse PRE representative for the address-taken local `s`
 * (`(mem/f:SI (plus sfp -4))`), and ALL FIVE three-argument arms were rewritten
 * to read it (insns 78, 92, 127, 141, 155).  ** THE ROM DID THE SAME PRE **:
 * every ROM arm is `mov r2,r4`, a register, not `ldr r2,[sp,#8]`.  So gcse is
 * NOT the divergence, and the park's "we lose it in .09.cse2" is right about the
 * pass and wrong about the transform.
 *
 * =============== THE DIVERGENCE, AND IT IS STRUCTURAL
 *
 * The arm's eval-time temp (reg 48) is BORN AND DEAD INSIDE THE SWITCH ARM'S OWN
 * BASIC BLOCK: set at insn 78, read at insn 84, with `code_label 75` immediately
 * before and `jump_insn 87` / `barrier 88` immediately after.  `make_regs_eqv`'s
 * promotion test (cse.c:1413-1433) therefore fails on BOTH sub-conditions:
 *
 *   (i)  `LAST_UID(48) > cse_basic_block_end` is false AND
 *        `FIRST_UID(48) < cse_basic_block_start` is false -- its whole span is
 *        inside one block, and a cse block ends at every CODE_LABEL
 *        (`cse_end_of_basic_block`: `while (p && GET_CODE (p) != CODE_LABEL)`);
 *   (ii) `LAST_UID(48) > LAST_UID(65)` is false -- reg 65's last mention is in
 *        the LAST switch arm (insn 155), long after insn 84.
 *
 * > A SWITCH ARM'S ARGUMENT TEMP CAN NEVER BE ITS QUANTITY'S CANONICAL
 * > REGISTER, so canon_reg must rewrite the argument move to the PRE pseudo and
 * > the eval-time copy must die.  Sub-condition (i) fails because a switch arm
 * > is always its own cse block; (ii) because the PRE pseudo outlives every
 * > individual arm by construction.
 *
 * =============== THEREFORE BRIEF-325 ITEM 3 IS CLOSED: THE SUFFIX *IS* THE
 * COLLAPSE
 *
 * With insn 78 dead, the three argument moves are exactly
 * `load_register_parameters`' r0, r1, r2 in ASCENDING hard-register order, so the
 * last one is always `mov r2,r4` -- always identical between the two arms,
 * always the second match, so `jump.c:675`'s threshold of 2 is always met.  Our
 * asm: `.L8: mov r0,r6 / mov r1,#58 / b .L15` + `.L9: mov r0,r6 / mov r1,#42` +
 * `.L15: mov r2,r4 / bl`.  The ROM gets a ONE-insn suffix only because its r2 is
 * set by a SURVIVING eval-time insn at an earlier LUID.  There is no second door:
 * you cannot shorten the suffix without keeping that insn alive.
 *
 * REFUTED IF a body makes the third argument's eval-time value a NON-COPY (a
 * real computation, which nothing collapses) while still reaching r2 from a
 * register rather than the stack slot -- or if gcse can be made not to PRE the
 * five reads of `s` while the ROM's `mov r2,r4` is still produced.  Those look
 * mutually exclusive, which is the content of the bound.
 */

/* Func_801bcd4 -- PARK STANDS.  Re-derived in batch 324 brief F, and the park's
 * diagnosis is CORRECT -- now with a number attached to it.
 *
 *   48 differing encodings of 81.  ref 81 encodings / 196 bytes, ours 80 / 192.
 *   BOTH STREAMS HOLD 70 INSTRUCTIONS -- equal.  The gap is one `.short 0x0000`
 *   pad plus where the pool words land.
 *
 * DECOMPOSITION -- four runs, only run 1 and run 3 are causes:
 *   1  ref idx 3-11    the prologue copy chain: the same two copies in the
 *                      OPPOSITE DIRECTION, with the r5/r6 roles swapped.
 *                      9 differing, NO length change.
 *   2  ref idx 23,29-35  the nine jump-table words and one pool offset -- pure
 *                      CONSEQUENCE of run 3's shift.
 *   3  ref idx 40,44-72  the two `LoadInventoryIcon` arms are CROSS-JUMPED in
 *                      ours and not in the ROM: -2 insns, paid back +2 in the
 *                      switch-exit and epilogue.
 *   4  ref idx 79-80   the trailing pad, downstream of run 3.
 *
 * THE PARK'S LAYOUT WORRY DOES NOT APPLY.  The ROM's case BODIES sit at 0x5c
 * (1/6), 0x6e (2), 0x7a (7), 0x86 (4), 0x98 (8), 0xa4 (9), 0xb0 (default) --
 * exactly the park's source order, and ours matches.  The landed sibling
 * src/rom_15000/rom_1aeec_a_a_a_a_b.c, from the same source file, records why:
 * `expand_case` sorts the TESTS by value, `emit_case_nodes` lays the BODIES out
 * in SOURCE order.
 *
 * THE CROSS-JUMP THRESHOLD, READ EXACTLY.  `jump.c:660` calls
 * `find_cross_jump (insn, JUMP_LABEL (insn), 1, ...)` for a simplejump against
 * the code before the label; then for every OTHER jump to the same label on
 * `jump_chain`, `jump.c:675` calls it with **minimum 2**.  Inside
 * `find_cross_jump` (`jump.c:1427`) each matching insn before the jump does
 * `--minimum` (line 1602) and the merge fires on `minimum <= 0` (1607).  So two
 * arms that both end in `b .Lexit` need TWO matching instructions before it:
 *
 *   ROM   adds r2,r4,#0 / adds r0,r5,#0 / movs r1,#0x3a / bl / b
 *         `bl` matches (2->1), `movs r1,#imm` DIFFERS  =>  minimum 1, NO MERGE
 *   ours  adds r0,r6,#0 / movs r1,#0x3a / adds r2,r4,#0 / bl / b
 *         `bl` matches (2->1), `adds r2,r4,#0` matches (1->0)  =>  MERGE
 *
 * ** The ROM sets the COMMON argument FIRST and the DIFFERING one LAST, so its
 * common suffix is ONE insn -- exactly one short of the threshold. **
 * Cross-jumping is unconditional at -O1 and above (`toplev.c:3515` is the only
 * JUMP_CROSS_JUMP site), so NO FLAG REACHES IT.
 *
 * WHY THE ROM SETS r2 FIRST -- the real mechanism, and not the park's guess.
 * `.02.jump` for the case-2 arm reads insn 78 `(set (reg 48) (mem/f:SI
 * (addressof:SI (reg/v:SI 40))))`, then insn 80 `r0 = b`, insn 82
 * `r1 = 58`, insn 84 `r2 = 48`.  `s` IS ADDRESS-TAKEN, so reading it for the
 * third argument is a MEM read and `store_one_arg` emits it at EVALUATION time,
 * i.e. BEFORE the three argument moves `load_register_parameters` emits.  The
 * ROM allocated pseudo 48 to r2, so insn 84 became `(set r2 r2)` and jump2's
 * noop-move pass deleted it, leaving insn 78 as `adds r2,r4,#0` at its own early
 * LUID.  The same thing is already visible in the 5-argument arms, WHICH MATCH:
 * `add r2,sp,#8 / add r3,sp,#4` precede `mov r0 / mov r1` because `&s` and `&t`
 * are evaluation-time computations too.
 *
 * WE LOSE IT IN `.09.cse2`: `.07.gcse` commons the load so insn 78 becomes
 * `(set (reg 48) (reg 65))`, cse2 propagates 65 into insn 84, and insn 78 is
 * left a dead copy that `delete_trivially_dead_insns` removes.  THE SURVIVOR IS
 * INSN 84 -- hence r2 LAST, hence a two-insn common suffix, hence the merge.
 * This is the SAME COPY-COLLAPSE FAMILY as Func_8020b64, in a different pass.
 *
 * MEASURED, SEVEN WHOLE BODIES (reference memory profile ldr=3 str=4):
 *   park base                                          48
 *   arm-local `int u = s;` for the 3rd argument        48 flat, BYTE-IDENTICAL
 *   ROM's prologue chain `s = slot; s0 = s; return s0` 48 flat, BYTE-IDENTICAL
 *   both together                                      48 flat, BYTE-IDENTICAL
 * Three further instruments read 42 and ARE REJECTED AS FIGURES THAT LIE: they
 * are 83 instructions against the reference's 81 with ldr=7 against the ROM's 3.
 * Recorded as a figure about the blocker only, never as a candidate.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801bcd4.c asm/rom_15000/rom_1aeec_a_a_c_c.s --func Func_801bcd4
 */

/* Func_801bcd4 (0x0801bcd4) -- NON-MATCHING.
 *
 * NON-MATCHING, 48 of 81 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 81, ours 80) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 196 bytes, ours 192.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801bcd4.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_c.s --func Func_801bcd4
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: CROSS-JUMPING WE CANNOT SUPPRESS, over a register rotation.
 *
 * 91 lines against the ROM's 90 -- one LONG -- with 54 differing.
 *
 * A sprite-slot loader: allocate a slot if the caller passed -1, give up if the
 * allocator is exhausted, then dispatch on a kind code through a nine-entry
 * jump table and call one of five loaders, four of which take the slot by value
 * and two by address.
 *
 * WHAT CAME OUT RIGHT WITHOUT A LEVER: the nine-entry jump table from a plain
 * switch on `kind` with cases 1,2,4,6,7,8,9 and 3/5 falling to default; the
 * `sub r0, r7, #1 / cmp r0, #8 / bhi` bias-and-bound; the three-word stack
 * frame with the slot at sp+8, the out-parameter at sp+4 and the fifth argument
 * at sp+0; and the reload of the slot from sp+8 after every call, which is
 * gcc's own consequence of taking its address.
 *
 * THE ONE REAL FIX, and it is worth keeping: COMPARE THE COPY, NOT THE
 * PARAMETER. The ROM's guard is `cmp r4, r3` where r4 is `s` and r3 is -1, and
 * it saves `mov r6, r4` before branching so the early return has the old value.
 * Written `if (slot == -1)` gcc compares the parameter, never makes the copy,
 * and returns from the parameter's own register: 83 differing. Written
 * `s = slot; if (s == -1) { ...; return slot; }` -- compare the copy, return the
 * parameter -- it drops to 54. Two names for one value are only distinct to gcc
 * when each is READ in a different place.
 *
 * THE BLOCKER. gcc cross-jumps the two `LoadInventoryIcon` arms and the ROM
 * does not:
 *
 *     rom    mov r2, r4 / mov r0, r5 / mov r1, #0x3a / bl LoadInventoryIcon
 *            mov r2, r4 / mov r0, r5 / mov r1, #0x2a / bl LoadInventoryIcon
 *
 *     ours   mov r0, r6 / mov r1, #0x3a / b L11
 *            mov r0, r6 / mov r1, #0x2a / L11: mov r2, r4 / bl LoadInventoryIcon
 *
 * The cause is the ARGUMENT ORDER. The ROM sets r2, then r0, then r1, so the
 * only common suffix the two arms share is the `bl` itself and jump.c leaves
 * them alone. We set r0, r1, r2, which makes `mov r2, r4 / bl` a two-instruction
 * common suffix -- long enough to merge. So the cross-jump is a SYMPTOM of the
 * argument order, not an independent problem, and suppressing it means
 * reproducing the ROM's r2-first evaluation.
 *
 * That is the same knob as the recorded return-type lever, and here it does not
 * turn: declaring all five loaders `int`, and declaring only LoadInventoryIcon
 * `int`, are both byte-identical to the `void` version at 54 differing. The
 * lever moves r0 relative to the OTHER argument registers when the callee
 * returns a value; it does not let r2 be chosen first.
 *
 * MEASURED (rom 90 lines):
 *   `if (slot == -1)`, all callees void            91 lines, 83 differing
 *   `if (s == -1)`, return slot                    91, 54  <- kept
 *   as above, all five callees declared int        91, 54  (byte-identical)
 *   as above, only LoadInventoryIcon int           91, 54  (byte-identical)
 *
 * Under the 54 there is also a plain r5/r6 rotation -- the ROM keeps the
 * second argument in r5 and we keep it in r6 -- which is the recorded
 * allocation-order class and would still be there if the cross-jump were fixed.
 *
 * NEXT: the question worth one screen is whether the third argument can be made
 * to evaluate first by giving it a side effect or an address-taken form that
 * gcc must sequence early. Everything spelling-level has been tried.
 */
extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *p, int *q, int e);
extern void LoadInventoryIcon(int a, int b, int c);
extern void LoadMoveIcon(int a, int b, int *p, int *q, int e);
extern void LoadStatusIcon(int a, int b, int c);
extern void LoadUIBanner(int a, int b, int c);

int Func_801bcd4(int kind, int b, int slot, int d)
{
	int s;
	int t;

	s = slot;
	if (s == -1) {
		s = AllocSpriteSlot();
		if (s == 0x60)
			return slot;
	}
	switch (kind) {
	case 1:
	case 6:
		LoadOldUIIcon(b, d, &s, &t, 1);
		break;
	case 2:
		LoadInventoryIcon(b, 0x3a, s);
		break;
	case 7:
		LoadInventoryIcon(b, 0x2a, s);
		break;
	case 4:
		LoadMoveIcon(b, d, &s, &t, 1);
		break;
	case 8:
		LoadStatusIcon(b, 0, s);
		break;
	case 9:
		LoadUIBanner(b, 0, s);
		break;
	default:
		break;
	}
	return s;
}
