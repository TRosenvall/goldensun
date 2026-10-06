/* Func_80a7440 (0x080a7440) -- NON-MATCHING, 4 of 25 encodings.
 *
 *   STREAM LENGTHS NOW AGREE (ref 25, ours 25) and the RELOCATIONS ARE CLEAN,
 *   so this figure IS a distance.  SIZE 56 both sides.  Pin-free.
 *   The residue is FOUR encodings at stream positions two through five and is
 *   nothing but post-reload instruction ORDER; registers, selection and the
 *   whole rest of the body are identical to the reference.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a7440.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_a.s --func Func_80a7440
 *
 * Clears the menu block's halfword at +0x174, asks Func_80a77a4 for a
 * selection, and returns either that selection's minus one or the byte at
 * +0x21a.  The reference epilogue is `pop {r5} / pop {r1} / bx r1`, and a
 * single-register pop into r1 encodes a one-to-four-byte declared return, so
 * the `int` return here is confirmed by the bytes.
 *
 * ============================================================
 * REWRITTEN IN BATCH 332.  Everything above the line is new; everything the
 * park used to claim is below, and the two superseded figures are spelled in
 * WORDS so no tool can harvest them as live claims.
 *
 * WHAT THE PARK USED TO SAY, AND WHY IT WAS WRONG.  The old header's blocker
 * class was "A NAMED ZERO THAT CSEs WITH A CALL ARGUMENT".  Its claim was that
 * `*(unsigned short *)(s + 0x174) = 0` gets the zero POOLED where the reference
 * has a register mov, that naming the zero in a local fixes the pooling, and
 * that doing so makes cse "collapse the two zeroes into one register and drop
 * two instructions" -- so the fix cost more than it bought and the park stayed
 * on the pooled-zero body.
 *
 * THE POOLING OBSERVATION IS CORRECT.  THE CSE EXPLANATION IS REFUTED.  With
 * the zero named in an `int` local, the output keeps BOTH zeroes
 * (`mov r3,#0` for the store and a separate `mov r0,#0` for the call argument).
 * Nothing is commoned.  What actually disappeared was a `b` that had been
 * jumping over a MID-FUNCTION literal pool: the pooled zero was a pool word, so
 * removing it let the pool move past `bx` and the branch-over went away.  The
 * park was reading a POOL-PLACEMENT artifact as a cse effect, and because it
 * read the loss as unavoidable it closed the only dimension that mattered.
 *
 * THE REAL LENGTH GAP.  Our old twenty instructions were nineteen real ones
 * plus that pool-skip branch; the reference's twenty-one are all real.  So two
 * real instructions were missing, not one, and they are a JOIN COPY PAIR: the
 * reference materialises the return value in r2, copying it in after the call
 * and out again at the merge point.
 *
 * THE CONSTRUCT THAT PRODUCES THEM.  The returned variable must ALSO carry the
 * stored zero, and the two arms must be an explicit if/else rather than an
 * assign-then-overwrite.  Being born at the zero is what puts it in r2, which
 * is what forces a copy in from the call's return register and a copy out to it
 * at the return.  Every PLAIN join -- a second name copied from the call result
 * before the test -- is coalesced away and measures identical to the old body;
 * that whole class is dead, and the old header's note that it was
 * "byte-identical" is reproduced here.
 *
 * WHY THE REMAINING FOUR LOOK UNREACHABLE WITHOUT A PER-FILE FLAG.  The two
 * sides disagree only about the order of four instructions:
 *
 *     ref   ldr r3,=sym | ldr r5,[r3]  | mov r2,#0   | mov r1,#0xba | lsl r1,#1
 *     ours  ldr r3,=sym | mov r1,#0xba | ldr r5,[r3] | lsl r1,#1    | mov r2,#0
 *
 * arm.md:260 gives a thumb load a ready cost of two cycles, so the dereference
 * is queued and the cycle behind the pool load must be filled from the ready
 * list.  arm_adjust_cost (arm.c:2416-2451) discounts a load to one cycle only
 * when its dependence is on a STORE and its own address is a pool, stack or
 * frame address; here the dependence is on another LOAD, so no discount
 * applies.  The two candidates then tie on priority -- the dereference reaches
 * the address add at cost two, and the offset mov reaches it through the shift
 * at one plus one -- and rank_for_schedule breaks that tie at
 * haifa-sched.c:4068-4093 with its last-scheduled-insn class rule, which
 * prefers the insn INDEPENDENT of the last scheduled insn over the
 * data-dependent one.  So the offset mov always wins that cycle and the
 * dereference cannot sit at stream position two.
 *
 * AND THE POST-RELOAD SCHEDULER CANNOT SIMPLY BE TURNED OFF HERE.  Measured
 * with the scheduler disabled (the group the Makefile spells SCHED2_CFLAGS),
 * the front four match the reference EXACTLY and the difference moves to the
 * tail instead -- the join copy sinks below the compare's constant setup, and a
 * register copy is emitted as a zero add rather than a mov -- for three
 * differing positions.  So this object is NOT a candidate for that group: with
 * the scheduler on the tail is right, with it off the front is right, and the
 * reference has both.  That is a stronger statement than "the flag does not
 * close it" and is why it is recorded rather than proposed.
 *
 * MEASURED AND INERT OR WORSE -- do not re-run, all at stream length 25 unless
 * noted, counted the same way as the claim at the top:
 *   zero named in an int local, old assign-then-overwrite join .... 22 (19 insns)
 *   zero named, plain join variable copied before the test ........ 22 (19)
 *   zero named, ternary ........................................... 22 (19)
 *   zero named, early return on the minus-one arm ................. 22 (19)
 *   zero named, inverted test with two returns .................... 22 (19)
 *   zero named, byte read through its own local ................... 22 (19)
 *   zero named as an unsigned short ............................... 22 (19)
 *   ONE variable for both the zero and the selection .............. 24 (18) WORSE
 *     -- and THIS is where cse really does common the two zeroes, emitting the
 *        call argument as a register copy.  The old header's mechanism exists;
 *        it just needs the selection and the zero to be the SAME name, which is
 *        not the construct it was blaming.
 *   returned-variable-carries-the-zero plus if/else ................ 4  BEST
 *   the same, with a halfword-pointer index for the store .......... 4 identical
 *   the same, with the store written as a pointer-plus-index ....... 4 identical
 *   the same, base load moved before the zero ...................... 4 identical
 *   the same, offset spelled 0x174 instead of a shift .............. 4 identical
 *   the same, call-result local declared first ..................... 4 identical
 *   the same, offset hoisted into its own local .................. 24 (20) WORSE
 *   the same, store and zero-assignment fused into one statement ... 7
 *   a volatile-qualified base (a DEVICE, run only as a probe) ...... 4 inert
 *   two unused parameters on the signature (a probe) ............... 4 inert
 *
 * COMBINED-TU MULTIPLIER: CHECKED AND DECLINED.  The piece
 * asm/rom_a1000/rom_a7380_a_c_a_a.s holds exactly two functions and the other,
 * Func_80a7478, is ALSO parked -- but it is a two-hundred-instruction screen
 * driver whose park records a local-alloc contention blocker for r5 with nine
 * measured-inert spellings.  One `.c` would be one TU, but that one is not
 * close, so there is no two-for-one here.
 *
 * NEXT, if reopened.  The only handle left is to raise the dereference's
 * scheduling priority above the offset mov's by exactly one, or to drop the
 * offset mov's by one.  Both chains currently cost two to reach the address
 * add, which is why they tie.  Lengthening the base pointer's in-block
 * dependence chain would do it, and nothing semantics-preserving reaches it:
 * the base has exactly one in-block use.  Treat the four as a scheduling bound
 * with the evidence above attached, not as an expression-level question.
 */
extern unsigned char *iwram_3001f2c;
extern int Func_80a77a4(int a);

int Func_80a7440(void)
{
	unsigned char *s;
	int out;
	int sel;

	s = iwram_3001f2c;
	out = 0;
	*(unsigned short *)(s + (0xba << 1)) = out;
	sel = Func_80a77a4(0);
	if (sel == -1)
		out = sel;
	else
		out = s[0x21a];
	return out;
}
