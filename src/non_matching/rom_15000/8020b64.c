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
