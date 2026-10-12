/* Func_80a7440 (0x080a7440) -- MATCHES, 0 of 25 encodings.  (batch 338, brief A)
 *
 *   56 bytes, 25 encodings and 2 relocations identical.  Pin-free and
 *   device-free: `tools/shimcount.py` prints no rows on this body, and the
 *   `do { } while (0);` is a C no-op that emits no instruction -- it is not
 *   fakematch-class and carries no pass-3 depin debt (batch 337 precedent,
 *   Func_801fd34).
 *
 *   The park figure it replaces was 4 of 25 and that figure was a TRUE
 *   DISTANCE: stream 25/25 with INSTRUCTION COUNTS 21/21 and clean
 *   relocations.  Re-derived with objcmp at the head of batch 338.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a7380_a_c_a_a_b.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_a_b.s --func Func_80a7440
 *
 * Split shape: asm/rom_a1000/rom_a7380_a_c_a_a.s held TWO functions
 * (tools/piece.py), this one and Func_80a7478 at 178 of 243.
 *   tools/datacheck.py asm/rom_a1000/rom_a7380_a_c_a_a.s   -- no rows, no data
 *   tools/split_s.py ... Func_80a7440 --dry-run
 *     -> asm/rom_a1000/rom_a7380_a_c_a_a_b.s  (1 function, 30 lines)   <- this
 *     -> asm/rom_a1000/rom_a7380_a_c_a_a_c.s  (1 function, 253 lines)  Func_80a7478
 *     -> removes rom_a7380_a_c_a_a.s, rewrites stage1.ld
 * No exports needed.  Func_80a7478's park must be repointed at
 * rom_a7380_a_c_a_a_c.s.
 *
 * Clears the menu block's halfword at +0x174, asks Func_80a77a4 for a
 * selection, and returns either that selection's minus one or the byte at
 * +0x21a.  The reference epilogue `pop {r5} / pop {r1} / bx r1` confirms the
 * `int` return from the bytes.
 *
 * ============================================================
 * HOW THE LAST FOUR CLOSED, AND THE PARK'S "NEXT" SECTION IS REFUTED.
 *
 * The park (rewritten in 332, re-measured in 335) had the residue diagnosed
 * EXACTLY right and reproduces to the letter: four encodings of pure
 * post-reload ORDER at stream positions two through five, registers and
 * selection identical, and the contest is
 *
 *     ref   ldr r3,=sym | ldr r5,[r3]  | mov r2,#0   | mov r1,#0xba | lsl r1,#1
 *     ours  ldr r3,=sym | mov r1,#0xba | ldr r5,[r3] | lsl r1,#1    | mov r2,#0
 *
 * Its priority arithmetic is also right, and I re-derived it from the source
 * rather than inheriting it.  With P = priority of the `strh`:
 *     prio(ldr r5,[r3]) = 2 (load, arm.md:260) + 1 (the `add r3,r5,r1`) + P
 *     prio(mov r1,#0xba) = 1 (to the lsl) + 1 (lsl to the add) + P
 * Both 3 + P, so rank_for_schedule's priority rung (haifa-sched.c:4040-4043)
 * ties.  arm_adjust_cost (arm.c:2415-2450) does NOT discount the dependence to
 * one cycle: its pool/stack/frame discount at arm.c:2443-2449 requires `dep` to
 * be a STORE (`GET_CODE (SET_DEST (d_pat)) == MEM`) and the pool load's SET_DEST
 * is a register, so the cost stays 2.  rank_for_schedule then reaches the
 * last-scheduled-insn CLASS rung (haifa-sched.c:4069-4094), the dereference is
 * data dependent on the just-scheduled pool load (class 1), the offset mov is
 * independent (class 3), highest class wins, and the mov takes the slot.  Every
 * step of that is correct.
 *
 * THE PARK'S VERDICT IS WHAT WAS WRONG.  It closed with "the only handle left is
 * to raise the dereference's scheduling priority above the offset mov's by
 * exactly one, or to drop the offset mov's by one ... nothing
 * semantics-preserving reaches it", and treated the four as a bound.  You do not
 * have to WIN the ranking.  You can EMPTY THE READY LIST, and one statement
 * does it:
 *
 *     s = iwram_3001f2c;
 *     do { } while (0);        <-- here
 *
 * MECHANISM, read out of the compiler rather than recalled.  `do { } while (0)`
 * is a C no-op but it is not an RTL no-op: expand_start_loop/expand_end_loop
 * leave NOTE_INSN_LOOP_BEG / _CONT / _END in the stream, and they survive to
 * sched2.  Confirmed in the dumps: `.00.rtl` gains those three notes and
 * `.19.flow2` block 0 is otherwise INSN-FOR-INSN IDENTICAL to the parked body
 * (same sets in the same order, same hard registers), and the basic-block count
 * is 3 on both sides -- so this is NOT a block split and nothing before sched2
 * changes.  The fence is `sched_analyze_insn`'s loop-note barrier:
 *
 *   haifa-sched.c:3709-3712  "If there is a {LOOP,EHREGION}_{BEG,END} note in
 *                             the middle of a basic block, then we must be sure
 *                             that no instructions are scheduled across it."
 *   haifa-sched.c:3726-3731  LOOP_BEG / LOOP_END set schedule_barrier_found
 *   haifa-sched.c:3735-3736  the notes are hung on the FOLLOWING insn, so the
 *                             fence lands immediately before it
 *   haifa-sched.c:3741-3752  REG_DEP_ANTI on every register's last uses, plus a
 *                             real dependence on every last set and every last
 *                             clobber
 *   haifa-sched.c:3754-3756  reg_pending_sets_all = 1 and flush_pending_lists,
 *                             so memory is fenced too
 *
 * So `mov r1,#0xba` acquires an anti-dependence on the insns that precede the
 * note and cannot be hoisted past the dereference.  At that cycle the
 * dereference is the ONLY ready insn, the class rung is never consulted, and
 * the remaining three positions fall into source order, which is the
 * reference's order.  This is the same reach the park's own 335 note was
 * reaching for from the other side -- it tried to lengthen the dereference's
 * dependence chain with alias-set-0 unions (four crossings, all inert, and its
 * explanation of why is correct) when the move was to shorten the opponent's
 * reachability instead.
 *
 * CORRECTION TO THE SHARED BRIEF, worth carrying: batch 336's device table and
 * batch 337's amendment both cite haifa-sched.c:3744-3757 for the anchor.  Those
 * lines are the right lines but they are inside `if (loop_notes)`
 * (haifa-sched.c:3714) -- i.e. they are reached by a LOOP NOTE, which is why
 * `do { } while (0)` reaches them at all, and a bare `__asm__ volatile ("")`
 * does NOT go through this path (it is handled as a volatile insn in
 * sched_analyze_insn's own body).  The anchor and the barrier are two different
 * mechanisms that happen to agree on reach, not one mechanism with two
 * spellings.  Stating it that way is what makes the placement rule predictable:
 * the anchor fences IMMEDIATELY BEFORE THE NEXT INSN, so place it after the
 * statement whose insn must come first.
 *
 * TWO PLACEMENTS REACH ZERO, both verified byte-identical, both device-free:
 *   after `s = iwram_3001f2c;`  (anchor03)  <-- shipped, the fence is exactly
 *                                              at the contested boundary
 *   after `out = 0;`            (anchor04)  identical result
 * The eight other top-level boundaries measure 4 (anchor02, anchor05, ...) or
 * worse; full table in scratch_elev/b338/A/sweep_80a7440.txt.
 *
 * AUTOSWEPT (tools/autosweep.py, batch 338) -- recorded negatives, do not
 * re-run: flag axis all eight groups on the parked body, -O1 23, SCHED2_CFLAGS
 * 3 (BETTER but see below), the other six inert at 4.  SCHED2_CFLAGS is still
 * correctly declined: the park's 332 note that with the scheduler off the front
 * four are exact and the TAIL goes wrong is reproduced, so that group trades one
 * residue for another, and the anchor needs no flag at all.
 *
 * The park's full inert/worse table (twenty-odd spellings from 332 and 335) is
 * preserved in reports/batch-338.md; none of it is invalidated -- every row was
 * a measurement of the ranking, and the ranking is not what moved.
 */
extern unsigned char *iwram_3001f2c;
extern int Func_80a77a4(int a);

int Func_80a7440(void)
{
	unsigned char *s;
	int out;
	int sel;

	s = iwram_3001f2c;
	do { } while (0);
	out = 0;
	*(unsigned short *)(s + (0xba << 1)) = out;
	sel = Func_80a77a4(0);
	if (sel == -1)
		out = sel;
	else
		out = s[0x21a];
	return out;
}
