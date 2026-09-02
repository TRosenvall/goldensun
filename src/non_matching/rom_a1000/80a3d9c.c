/* Func_80a3d9c (0x080a3d9c) -- NON-MATCHING.
 * Blocker class: COMBINE FOLDS A NARROWING MASK IT CAN PROVE REDUNDANT.
 *
 * 28 lines against the ROM's 32, 25 differing -- an improvement on this park's
 * previous 27, and the whole prologue emission order now reproduces exactly.
 * More usefully, THE MECHANISM IS NOW NAMED rather than guessed at.
 *
 * Scans a unit's fifteen item slots for one whose low nine bits match, and
 * returns its stack count (bits 11..15) plus one, or zero.
 *
 * BLOCKER A -- three of the four missing instructions.
 *
 *     rom    mov r3, #0xf8 / lsl r3, #0x8 / and r3, r2 / lsr r5, r3, #0xb
 *     ours   lsr r3, r2, #0xb
 *
 * gcc-2.96's combine performs a three-insn combination of the constant build,
 * the AND and the shift. Because `v` is single-set from an `ldrh`,
 * `nonzero_bits(v) <= 0xffff`, so simplify_shift_const rewrites the pair to
 * `(and (lshiftrt v 11) 0x1f)` and then drops the AND as redundant -- one
 * `lsr`. If the width were NOT known, the rewrite would leave an AND with 0x1f,
 * which has no Thumb insn and no valid two-insn split, so combine would reject
 * the whole combination and emit exactly the ROM's four lines.
 *
 * So the function turns entirely on making gcc not know the loaded halfword is
 * sixteen bits wide, and nothing in the source does that. This is the same wall
 * the tree already records for Func_80788c4 and Func_801f730 -- "gcc knows the
 * loaded value's range and folds the narrowing operation" -- in its
 * mask-before-shift flavour, and this function will close by itself if that
 * fold is ever defeated.
 *
 * ONLY TWO STATES ARE REACHABLE FROM SOURCE, and both were mapped:
 *   a `for`/`while` loop lets loop.c hoist the constant build into the
 *     preheader; the AND then survives (different basic block, no LOG_LINK) but
 *     the hoisted build costs r7 and a wider push -- 31 lines.
 *   a `do`/`while` or goto loop keeps the build beside the AND, combine sees
 *     all three and folds -- 28 lines. The ROM is neither: it has the AND
 *     inside the loop with no extra push.
 *
 * BLOCKER B -- the fourth missing instruction is the `mov r3, r2` copy before
 * the compare. A GENUINE SECOND READ IS BYTE-IDENTICAL TO `t = v;` here: CSE
 * folds it away and no copy appears. That is a measured NEGATIVE for the
 * read-count lever on this shape, and worth having, because the same lever
 * unparked six other functions in this batch.
 *
 * WHAT WAS RULED OUT, all measured: a second live definition of the loaded
 * value; splitting 0xf800 so it arrives as two instructions (CSE reconstitutes
 * it before combine); `(v >> 11) & 0x1f` (hoists `mov r7, #0x1f` instead);
 * union and struct bitfield extraction (gives `lsl #0x10 / lsr #0x1b`, or an
 * `ldrb`); and the flags -fno-gcse, -fno-rerun-cse-after-loop,
 * -fno-cse-follow-jumps, -fno-cse-skip-blocks, -O1, --no-sched2.
 *
 * TWO FINDINGS WORTH CARRYING OUT OF THIS, both new:
 *
 *   `u16 v; u32 t = v;` PRODUCES THE COPY WHILE KEEPING `ldrh`. This
 *   contradicts the rule recorded in src/non_matching/rom_a1000/80a3ddc.c that
 *   a HImode local always forces `ldrsh rD, [rB, rI]` -- the `ldrsh` appears
 *   only when the u16 value is used DIRECTLY. Widening it into a u32 local on
 *   the next statement keeps the zero-extending load and still emits the copy.
 *   It lands in the wrong place here, but it is the first spelling in this
 *   family to get both at once.
 *
 *   THE PROLOGUE ORDER IS A PURE STATEMENT-ORDER LEVER. Naming the lookup's
 *   result and assigning the counter and the accumulator BEFORE computing the
 *   walking pointer moves `add r0, #0xd8` after the two `mov #0`s and
 *   reproduces the ROM's prologue exactly. That is what took this park from 27
 *   differing to 25.
 *
 * MEASURED, best of thirty spellings (rom 32 lines): 28 lines, 25 differing.
 * The full table lives in the batch report; the shape of it is that every
 * 31-line variant is the hoisted-constant state and every 28-line variant is
 * the folded state, with nothing in between.
 */
#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

s32 Func_80a3d9c(s32 unitId, s32 itemId)
{
	u8 *rec;
	u16 *p;
	u32 mask;
	s32 i;
	u32 v;
	u32 t;
	s32 r;

	rec = _GetUnit(unitId);
	mask = 0x1ff;
	r = 0;
	i = 0;
	p = (u16 *)(rec + 0xd8);
	do {
		v = *p;
		t = v;
		p++;
		if (t != 0) {
			t = mask & v;
			if (t == itemId) {
				t = 0xf800;
				t &= v;
				r = (t >> 11) + 1;
				break;
			}
		}
		i++;
	} while (i <= 0xe);
	return r;
}
