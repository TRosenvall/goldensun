/* GetEquippedItem  --  0x080787dc, from asm/rom_77000/rom_78414_c_c_a_c_a.s.
 *
 * Scans a unit's 15-entry item array (unit + 0xd8, one halfword each) for the
 * first equipped entry -- bit 0x200 set -- whose item info record carries the
 * requested kind byte, and returns its slot index, or -1 when none matches.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_77000/rom_78414_c_c_a_c_a_a.o and
 * asm/rom_77000/rom_78414_c_c_a_c_a_c_b.o in goldensun/stage1.ld.
 *
 * Two levers, both from docs/elevation.md, were needed:
 *
 *   the offset is the base, not the index.  The ROM addresses the array with
 *   `ldrh r3, [r5, r7]` -- r5 the running 0xd8+2i offset, r7 the unit pointer.
 *   Writing the natural `unit + off` puts the pointer in the base slot and
 *   emits `[r7, r5]`.  Declaring `off` as the pointer and holding the unit as
 *   a plain int puts them back the ROM's way round.
 *
 *   init order is source order.  `i = 0` before `off = 0xd8` emits `mov r6, #0`
 *   before `mov r5, #0xd8`; the reverse spelling emits them swapped.
 */
extern unsigned char *GetItemInfo(int id);
extern unsigned char *GetUnit(int who);

int GetEquippedItem(int who, int kind)
{
	int i;
	unsigned char *off;
	int u;
	unsigned char *info;

	u = (int)GetUnit(who);
	i = 0;
	off = (unsigned char *)0xd8;
	while (i <= 0xe) {
		if (*(unsigned short *)(off + u) & 0x200) {
			info = GetItemInfo(*(unsigned short *)(off + u));
			if (info[2] == kind)
				break;
		}
		off += 2;
		i++;
	}
	if (i == 0xf)
		i = -1;
	return i;
}
