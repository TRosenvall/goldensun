/* Func_807882c  --  0x0807882c, from asm/rom_77000/rom_78414_c_c_a_c_a.s.
 *
 * The same scan as GetEquippedItem next door, but it returns the item info
 * record itself rather than the slot index, and takes the unit pointer
 * directly instead of looking it up.  Null when nothing matches.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_77000/rom_78414_c_c_a_c_a_b.o and
 * asm/rom_77000/rom_78414_c_c_a_c_a_c_c.o in goldensun/stage1.ld.
 *
 * Same two levers as the sibling: `off` carries the pointer type so the ROM's
 * `ldrh r3, [r5, r6]` base/index roles come out right, and `i = 0` is written
 * before `off = 0xd8` because these two inits are emitted in source order.
 * The bottom-tested do/while is what the ROM's fallthrough-into-the-body entry
 * asks for -- GetEquippedItem's `b` to the guard is the top-tested spelling.
 */
extern unsigned char *GetItemInfo(int id);

unsigned char *Func_807882c(int unit, int kind)
{
	int i;
	unsigned char *off;
	unsigned char *info;

	i = 0;
	off = (unsigned char *)0xd8;
	do {
		if (*(unsigned short *)(off + unit) & 0x200) {
			info = GetItemInfo(*(unsigned short *)(off + unit));
			if (info[2] == kind)
				return info;
		}
		i++;
		off += 2;
	} while (i <= 0xe);
	return 0;
}
