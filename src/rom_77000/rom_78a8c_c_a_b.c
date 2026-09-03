/* Func_8078af8  --  0x08078af8
 *
 * The tail of goldensun/asm/rom_77000/rom_78a8c_c_a.s; the two functions ahead
 * of it stay in _c_a_a.s, and the file-local label one of them reaches goes with
 * them. No data anywhere, so the split is a pure text cut, verified byte-neutral
 * before this landed.
 *
 * Counts how many of an item a unit holds across its fifteen inventory slots.
 * If the item's record carries the stackable flag, the quantity packed into the
 * slot's high bits plus one is returned immediately; otherwise duplicate slots
 * are tallied.
 *
 * THE .s HEADER COMMENT ON THIS FUNCTION IS WRONG. It calls the function a slot
 * finder returning an inventory index and describes the flag test as bit 10.
 * It returns a COUNT, and the tested bit is 0x10 of the record's fourth byte.
 * The annotation was written from a call trace rather than from the body; the
 * corrected reading is above.
 *
 * THE CALLEE-GREP PAID FOR THE WHOLE FUNCTION, and for the second round running
 * CALLEE-SET IDENTITY BEAT FILENAME ADJACENCY. Intersecting the two callee names
 * across src/ found a solved function in a different split of a different
 * parent: same callee pair, same object, same fifteen halfword slots, same
 * register-offset addressing. Its header already carried the two levers this one
 * needed structurally -- THE OFFSET IS THE BASE, NOT THE INDEX (declare the
 * running offset as the pointer and hold the unit as a plain int, or the
 * addressing comes out reversed), and INIT ORDER IS SOURCE ORDER. Transcribing
 * that skeleton gave correct registers, addressing and spill on the FIRST
 * screen. Meanwhile the stem-adjacent file -- the literal neighbouring split of
 * this same parent, which also calls one of the two helpers -- was worth
 * nothing.
 *
 * LOOP ROTATION IS PER-FUNCTION, NOT PER-FAMILY, and that was the only thing
 * left to find. The sibling matches with a top-tested `while`; this one requires
 * `do/while`. The distinguishing feature is what follows the loop: the sibling
 * has a post-loop statement that gives gcc a join to test into, whereas here
 * every exit goes straight to the return, and gcc will not rotate a `while` on
 * its own -- it branches into the body and leaves the latch at the head. Two
 * screens, 18 differing and then exact.
 *
 * THE LATCH INCREMENT ORDER IS SOURCE ORDER TOO. Writing the pointer advance
 * before the counter emits them in that order; the ROM wants the counter first,
 * so the source reads counter-then-pointer -- the reverse of the sibling's. Same
 * rule as the recorded init-order note, applied at the back edge instead of the
 * preamble.
 *
 * Four levers deliberately not used, recorded so they are not re-tried: the high
 * registers are a non-blocker and fell out of bare literals, one of them being
 * the constant-a-Thumb-immediate-cannot-encode case; there is no static chain,
 * since r9 is untouched and both high registers are written; the
 * constant-as-destination lever was unnecessary; and the two reads of the same
 * slot are a genuine second read, because gcc does not CSE across the call.
 */
extern unsigned char *GetItemInfo(int id);
extern unsigned char *GetUnit(int who);

int Func_8078af8(int who, int item)
{
	int i;
	unsigned char *off;
	int u;
	int count;

	u = (int)GetUnit(who);
	count = 0;
	item &= 0x1ff;
	i = 0;
	off = (unsigned char *)0xd8;
	do {
		if ((*(unsigned short *)(off + u) & 0x1ff) == item) {
			if (GetItemInfo(item)[3] & 0x10) {
				count = (*(unsigned short *)(off + u) >> 11) + 1;
				break;
			}
			count++;
		}
		i++;
		off += 2;
	} while (i <= 0xe);
	return count;
}
