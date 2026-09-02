/* Func_8020b64 (0x08020b64) -- NON-MATCHING.
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
