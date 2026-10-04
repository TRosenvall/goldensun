/* Func_80bd3e4 (0x080bd3e4) -- NON-MATCHING.
 * Blocker class: A SAVED HIGH REGISTER AND A FRAME THE BODY DOES NOT NEED.
 *
 * EIGHT instructions of thirty-two, and THE ENTIRE BODY IS EXACT. All eight are
 * the prologue and epilogue:
 *
 *     rom    mov r5, r9 / push {r5} / mov r3, r9 / sub sp, #0x4 / str r3, [sp]
 *            ... body, instruction for instruction ours ...
 *            add sp, #0x4 / pop {r3} / mov r9, r3
 *
 *     ours   (none of it)
 *
 * A weighted random pick: roll a byte, walk a table of eight weights
 * accumulating them, and return the index where the roll falls short -- or zero
 * if it falls short at entry or runs past the end.
 *
 * THE BODY NEEDED NOTHING. The first spelling reproduced every instruction: the
 * `& 0xff` on the call result with the mask materialised before the first load,
 * the accumulator seeded from p[0], the guard `if (r >= acc)` around the whole
 * loop, the `i > 7` bound tested after the increment, and the two exits sharing
 * the zero default. Only the frame differs.
 *
 * WHY THE FRAME IS NOT REACHABLE. The ROM saves r9 -- twice. Once through r5
 * into the push list, and again into a four-byte stack slot which the epilogue
 * never reads: the `pop {r3} / mov r9, r3` restores from the pushed copy, so
 * `str r3, [sp]` is dead. That is gcc-2.96's shape when a function both needs a
 * callee-saved high register AND has a frame.
 *
 * Neither condition is present here. Only ONE value is live across the call --
 * the table pointer, which the ROM and we both put in r5 -- so nothing forces
 * r8-r11; and no local has its address taken, so nothing forces a frame.
 * Producing either would mean adding a value the source does not have, and
 * adding one costs more instructions than the five it would explain.
 *
 * THE READING: the original had something else live across the call, or an
 * addressable local, which a later pass removed the uses of while the frame and
 * the save survived. That is a property of the ORIGINAL translation unit's
 * intermediate state, not of its C, and there is nothing in the source to
 * recover it from.
 *
 * This is the cleanest specimen yet of a residue that is entirely
 * prologue/epilogue with a byte-exact body, and it belongs beside
 * src/non_matching/rom_c0/2dd8.c, where gcc emits a prologue the ROM does not
 * have for the opposite reason -- there gcc adds one, here gcc omits one.
 *
 * MEASURED (rom 32 lines): the spelling below, 24 lines, 8 aligned of 32, and
 * every one of the eight outside the body.
  *
 * ==================== VERDICT RETRACTED, BATCH 300 ====================
 * This park concluded its 8-instruction prologue/epilogue residue was NOT
 * REACHABLE -- "nothing in the source to recover it from".  That is wrong.
 *
 * Func_80bd3e4 IS A GNU C NESTED FUNCTION inside Func_80bd424.  Written nested,
 * it is BYTE-IDENTICAL, 32 of 32.  gcc-2.96 Thumb's STATIC_CHAIN_REGNUM is r9,
 * and the tell is in the CALLER: each of Func_80bd424's three pick cases does
 * `add r3, sp, #0x1c / mov r9, r3 / bl Func_80bd3e4`.  The "dead" `str r3,[sp]`
 * this park could not explain is the STATIC CHAIN SLOT, and the frame exists to
 * hold it.  A five-line control reproduces both sides verbatim.
 *
 * IT CANNOT LAND ALONE.  A nested function must share its parent's translation
 * unit, so Func_80bd3e4 and Func_80bd424 land together in one object, and
 * Func_80bd424 is not yet exact (402 of 417 -- see
 * src/non_matching/rom_b5000/80bd424.c).  Retire this park when that one closes;
 * the text cut goes between Func_80bd424 and Func_80bd7a4, not between these two.
 *
 * NON-MATCHING, 30 of 32 encodings differ STANDALONE -- and that figure is an
 * ARTEFACT, not this function's distance.  The operative figure is the NESTED
 * one, which is ZERO (see BATCH 316 below).
 *
 * *** THIS PARK CARRIED A WRONG FIGURE FROM BATCH 300 TO BATCH 316. ***
 * The header claimed "8 of 32" while the body on disk measured 30 -- the body is
 * byte-identical across that whole span, so the claim was stale from the moment
 * the recipe was added, and `parkcheck.py` would have said MISMATCH on any day
 * someone ran it on this file.  Nobody did.  A recipe is not verification; it is
 * only the PRECONDITION for verification, and adding one to an old prose figure
 * without re-measuring is how a wrong number survives sixteen batches.
 *
 * Verify with (the figure below is for the function AS WRITTEN HERE, standalone;
 * written NESTED inside Func_80bd424 it is byte-identical):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/80bd3e4.c \
 *       asm/rom_b5000/rom_bbb0c_a_a_c_a.s --func Func_80bd3e4
 *
 * ===== BATCH 316: THIS PARK IS SOLVED; ONLY ITS SIBLING IS LEFT =====
 * Measured again under the nested recipe: as `Func_80bd3e4.0` inside
 * src/non_matching/rom_b5000/80bd424.c this function is **30 of 30 with ZERO
 * differing lines** -- byte-identical, the one halfword in this park's own
 * "8 short" figure being an isolation artefact (an alignment pad that exists
 * only when the function is assembled on its own). So the "8 of 32" figure
 * describes a problem this function NO LONGER HAS, and nothing here is blocked
 * on this park's prologue or frame.
 *
 * The landing is gated entirely on the sibling Func_80bd424, which batch 316
 * moved from 399 instructions / 337 differing to **405 / 333** against the
 * reference's 414 -- see the RESIDUE A correction in 80bd424.c, where the kept
 * sign extension at info+0x35 turned out to be a spelling after all. When
 * 80bd424 lands, both functions land in the SAME commit (they must stay in one
 * object, since one is nested inside the other) and this park is retired
 * without a separate candidate.
*/
extern int _RPGRandom(void);

int Func_80bd3e4(unsigned char *p)
{
	int r;
	int acc;
	int i;
	int res;

	r = _RPGRandom() & 0xff;
	acc = p[0];
	res = 0;
	i = 0;
	if (r >= acc) {
		for (;;) {
			i++;
			if (i > 7)
				break;
			acc += p[i];
			if (r < acc) {
				res = i;
				break;
			}
		}
	}
	return res;
}
