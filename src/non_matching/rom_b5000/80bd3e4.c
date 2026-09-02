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
