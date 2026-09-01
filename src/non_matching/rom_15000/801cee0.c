/* Func_801cee0 (0x0801cee0) -- NON-MATCHING.
 * Blocker class: REGISTER ROTATION (r1 <-> r2), see docs/elevation.md.
 *
 * 48 lines against the ROM's 48, 17 differing, and EVERY ONE of the seventeen
 * is the same two registers swapped:
 *
 *     rom    ldr r2, =0x574 / add r0, r2 ... add r1, r3, r2 / ldrb r2, [r1]
 *     ours   ldr r1, =0x574 / add r0, r1 ... add r2, r3, r1 / ldrb r1, [r2]
 *
 * The ROM keeps the pointer in r1 and the offset-then-value in r2; we do the
 * reverse. There is no third difference: no instruction is missing, none is
 * extra, and the order is identical throughout.
 *
 * THIS PARK REPLACES A MUCH WORSE ONE and the improvement is the point.
 * The previous best was 38 lines against 48 with 34 differing, or 43 lines with
 * 46 differing depending on which spelling of the gState base was used. Two
 * levers moved it to exact length:
 *
 * (1) THE LOADED BYTE IS READ TWICE. The ROM's `ldrb r2, [r1] / mov r3, r2 /
 *     cmp r3, #N` at all three arms is the CSEd-second-read signature from
 *     batch 178, not a redundant copy. `if (*p > N) return;` followed by
 *     `*p = *p + 1;` -- both through the pointer, nothing named -- emits it.
 *     The old park had `v = *p` and was three lines short for exactly that
 *     reason.
 *
 * (2) THE THREE ARMS EACH STORE AND RETURN; the shared tail is CROSS-JUMPING,
 *     not a `goto`. Written with a join label the branch polarity comes out
 *     inverted in two of the three arms (`bls join / b exit` where the ROM has
 *     `bhi exit / b join`), and that is source-inert: the plain `break`, an
 *     explicit `goto inc`, and an explicit `goto out` all give the identical
 *     48/4. Writing `*p = *p + 1; return;` inside each arm and letting gcc
 *     merge the three identical tails gives the ROM's polarity for free. This
 *     is the store-and-return-arms lever from batch 176, and it is worth noting
 *     that it fixes a BRANCH SHAPE here rather than a speculation problem.
 *
 * MEASURED (rom 48 lines):
 *   shared join via `break`, double-read                48 lines,  4 differing
 *   shared join via explicit `goto inc` / `goto out`    48,  4  (byte-identical)
 *   store-and-return arms, double-read                  48, 17  <- structurally
 *                                                                  exact
 *   `v = *p` named, join form                           45, 33
 *   `g = gState` hoisted above the switch               46, 42
 *
 * The 4-differing version is CLOSER BY COUNT and WRONG BY STRUCTURE -- its four
 * lines are two inverted conditional branches, a real difference in control
 * flow. The 17-differing version has the ROM's exact control flow and only its
 * register assignment is rotated. The latter is kept, per the rule that a
 * count is not a distance.
 *
 * TRIED AGAINST THE ROTATION, all inert at 17:
 *   `p` declared before `g`; the offset named in an int local; the parameter
 *   typed `unsigned char *` so the dispatch load is pointer arithmetic; the
 *   switch value named in a local first.
 * FLAGS, all inert at 17: -fno-thread-jumps, -fno-cse-follow-jumps,
 *   -fno-reorder-blocks. (-fno-gcse gives 46 lines and 33 differing;
 *   -fno-if-conversion is not a gcc-2.96 option.)
 *
 * WHAT IS RIGHT: the three-case dispatch including both `cmp #1` comparisons
 * and the `bgt` second level, the runtime gState base construction in all three
 * arms, the pooled 0x205/0x206 offsets against the built `0x83 << 2`, the three
 * unsigned bounds, the cross-jumped increment tail, and the shared exit.
 */
extern unsigned char gState[];

void Func_801cee0(int a)
{
	unsigned char *g;
	unsigned char *p;

	switch (*(unsigned short *)(a + 0x574)) {
	case 0:
		g = gState;
		p = g + (0x83 << 2);
		if (*p > 1)
			return;
		*p = *p + 1;
		return;
	case 1:
		g = gState;
		p = g + 0x205;
		if (*p > 0x17)
			return;
		*p = *p + 1;
		return;
	case 2:
		g = gState;
		p = g + 0x206;
		if (*p > 0xe)
			return;
		*p = *p + 1;
		return;
	}
}
