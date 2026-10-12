/* Func_80a3d9c (0x080a3d9c) -- NON-MATCHING.
 *
 * NON-MATCHING, 14 of 30 encodings  (MEASURED, batch 342 brief A).
 *   INSTRUCTION COUNTS NOW EQUAL, ref 28 / ours 28, size 64/64, relocations ok
 *   -- so this figure IS A PLAIN DISTANCE.  It was PHASE before this batch:
 *   25 of 30 with instructions ref 28 / ours 24, i.e. FOUR SHORT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3d9c.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_a_c.s --func Func_80a3d9c
 *
 * PIECE: tools/piece.py reports TWO functions in that .s -- this one and
 * Func_80a3ddc (src/non_matching/rom_a1000/80a3ddc.c, no claim).  Either can be
 * cut out alone with tools/split_s.py.
 * PINS: tools/shimcount.py reports 0.
 *
 * ===== BATCH 342 BRIEF A: BLOCKER A IS DEFEATED.  THE PARK'S OWN PREDICTION
 * ===== THAT THE FUNCTION WOULD THEN "CLOSE BY ITSELF" IS WRONG.
 *
 * The park's blocker A was the three-instruction fold: gcc knows the loaded
 * halfword is sixteen bits wide, so `simplify_shift_const` rewrites
 * `(lshiftrt (and v 0xf800) 11)` to `(and (lshiftrt v 11) 0x1f)` and then drops
 * the AND as redundant, giving one `lsr` where the ROM has
 * `mov r3, #0xf8 / lsl r3, #8 / and r3, r2 / lsr r5, r3, #11`.  The park's
 * closing sentence was "the function turns entirely on making gcc not know the
 * loaded halfword is sixteen bits wide, and nothing in the source does that
 * ... this function will close by itself if that fold is ever defeated."
 *
 * TWO THINGS DEFEAT IT, AND THE PRECONDITION IS READABLE IN THE SOURCE.
 * `nonzero_bits` of a pseudo comes from `reg_nonzero_bits[]`, which
 * `set_nonzero_bits_and_sign_copies` fills in (**combine.c:850-918**), and the
 * accumulation at **combine.c:907** is
 *     reg_nonzero_bits[REGNO (x)] |= nonzero_bits (src, nonzero_bits_mode);
 * -- an OR over EVERY SET OF THE PSEUDO, not a property of one load.  There is
 * also an unconditional full-mask fallback at :916 for a set whose SET_DEST is
 * not the register itself, and :861 skips any pseudo live at the start of block
 * 0 entirely, after which the read at **:8027-8028** is guarded by
 * `reg_nonzero_bits[REGNO (x)]` being NONZERO and otherwise returns the whole
 * mode mask.  So:
 *
 *   1. GIVE THE MASKED VARIABLE A SECOND, UNBOUNDED SET.  `t = unitId;` ahead
 *      of the call, with the same `t` reloaded from `*p` in the loop, ORs a
 *      parameter's unbounded bits into `reg_nonzero_bits[t]`, after which
 *      `nonzero_bits (t)` is all-ones and the AND can no longer be proved
 *      redundant.  The ROM's four instructions appear.  This is the first
 *      member of the "gcc knows the loaded value's range" class in this tree to
 *      be defeated from source, and the lever should transfer to the two other
 *      parks the park names, Func_80788c4 and Func_801f730.
 *   2. USE A GOTO LOOP, NOT A `do`/`while`.  The park's mapping of the two
 *      reachable states said "a `do`/`while` OR GOTO LOOP keeps the build
 *      beside the AND" -- those are not the same thing and the difference is
 *      worth 8 encodings.  A `do`/`while` is a natural loop, so once the AND
 *      survives, loop.c hoists the whole `mov #0xf8 / lsl #8` build into the
 *      preheader, which costs r7 and a wider push.  A GOTO LOOP disables loop
 *      optimisation outright and the build stays inside the break arm, where
 *      the ROM has it, with the ROM's `push {r5, r6, lr}`.
 *
 * Measured, in order: park body 25 of 30 (instructions 28/24); the unbounded
 * second set alone, `do`/`while` 23 of 30 (28/28, AND alive, build hoisted);
 * plus the goto loop 15 of 30; plus `i = 0;` before `r = 0;` 14 of 30.
 *
 * WHAT IS LEFT -- A REGISTER ROTATION AT EXACT LENGTH, AND IT IS ONE DECISION.
 *
 *     rom    p in r0, the loaded halfword in r2, plus `mov r3, r2`
 *     ours   p in r2 (via `mov r2, r0`), the halfword in r0, no copy
 *
 * The two cost the same single instruction, so the lengths agree; `r` and
 * `itemId` are swapped between r5 and r6 as a consequence.  The ROM's
 * `mov r3, r2` is genuinely redundant -- r3 is dead by the next instruction and
 * `cmp r2, #0` would have done -- which is the park's BLOCKER B, and it still
 * stands.  The park's reading of it is correct and now better localised: cse
 * substitutes the original register into the compare and the copy's
 * destination then dies.  THE PARK'S OWN WIDENING FINDING DOES NOT RESCUE IT:
 * `u16 v; u32 t = v;` was crossed against all four choices of which name the
 * zero test and the mask read, seven prologue statement orders, and with and
 * without a separate `rec` -- 112 bodies, and the copy never appears; the best
 * is 14.  Per batch 336, the only thing that stops a cse equivalence is an
 * `ASM_OPERANDS` pattern, i.e. `:: "i"(0)` (**cse.c:5742-5745**), which is a
 * device and would carry pass-3 debt.
 *
 * MEASURED INERT ON THIS BODY, all exactly 14 of 30 -- and note the first list
 * is the whole flag axis RE-CROSSED after the source changed, which is the one
 * that mattered on this batch's other targets:
 *   all eight per-file flag groups (-O1 is 17, -fno-schedule-insns2 is 15, the
 *     other six are 14)
 *   all 16 anchor placements, nested ones included
 * MEASURED WORSE: `t = (u32)rec;` or `t = (u32)p;` as the unbounded seed
 *   instead of `t = unitId;` (25-28); the mask written `t = 0xf800; t &= v;`
 *   rather than `t &= 0xf800;` (25-29).
 *
 * CARRIED FORWARD FROM THE PARK, all still correct:
 *   THE PROLOGUE ORDER IS A PURE STATEMENT-ORDER LEVER (and it is still worth
 *     one encoding here: `i = 0;` before `r = 0;` is 14, every other order 15);
 *   `(v >> 11) & 0x1f` hoists `mov r7, #0x1f`; splitting 0xf800 into two
 *     instructions is reconstituted by CSE before combine; union and struct
 *     bitfield extraction give `lsl #0x10 / lsr #0x1b` or an `ldrb`;
 *   a genuine second read is byte-identical to `t = v;`.
 *
 * NEXT: blocker B alone.  The ROM spends an instruction on a copy that cse
 * deletes for us, and the two candidate handles are (a) a reason the compare
 * cannot read the load's own register, and (b) the `p`-in-r0 role, since the
 * `mov r2, r0` we spend and the `mov r3, r2` the ROM spends are the same
 * instruction budget -- fixing either should fix both.
 */
#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

s32 Func_80a3d9c(s32 unitId, s32 itemId)
{
	u8 *rec;
	u16 *p;
	u32 mask;
	s32 i;
	u32 t;
	s32 r;

	t = unitId;
	rec = _GetUnit(t);
	mask = 0x1ff;
	i = 0;
	r = 0;
	p = (u16 *)(rec + 0xd8);
again:
	t = *p;
	p++;
	if (t != 0) {
		if ((mask & t) == itemId) {
			t &= 0xf800;
			r = (t >> 11) + 1;
			goto out;
		}
	}
	i++;
	if (i <= 0xe)
		goto again;
out:
	;
	return r;
}
