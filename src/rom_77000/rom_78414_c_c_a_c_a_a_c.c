/* EquipItem  --  0x08078708, was asm/rom_77000/rom_78414_c_c_a_c_a_a_c.s.
 *
 * The .s held this function alone (plus its own 0x200 pool word), so it
 * converts whole: no split, and the linker script's existing line picks up
 * this file's object.
 *
 * The old .s header called it "AddInventoryItem"; the body EQUIPS the item in
 * `slot`: it clears bit 0x200 on any equipped item of the same kind (kind 6
 * skips the scan) and returns -2 if that item is flagged 0x02 in info byte 3.
 *
 * `u->items[slot]` through the struct is what gives the ROM's
 * `lsl / add #0xd8 / ldrh [u, off]` with slot*2 kept in a high register;
 * every raw pointer-arithmetic spelling folds to `add r3,r5,r7 / add #0xd8 /
 * ldrh [r3]` (18 off), and a precomputed `slot*2+0xd8` local is 86 off. The
 * inner scan is GetEquippedItem's loop unchanged. Same struct Unit layout as
 * BreakItem in rom_78414_c_c_c_a.c.
 */
struct Unit {
	unsigned char pad_0[0xd8];
	unsigned short items[16];
};

extern struct Unit *GetUnit(int id);
extern unsigned char *GetItemInfo(int id);
extern int CanEquipItem(int unit, int item);
extern void Func_8078bf0(int unit);
extern void CalcStats(int unit);

int EquipItem(int who, int slot)
{
	struct Unit *u;
	int item;
	int kind;
	int i;
	unsigned char *off;

	u = GetUnit(who);
	item = u->items[slot];
	if (CanEquipItem(who, item) == 0)
		return -1;
	if (item & 0x200)
		return 0;
	kind = GetItemInfo(item)[2];
	if (kind != 6) {
		i = 0;
		off = (unsigned char *)0xd8;
		while (i <= 0xe) {
			if (*(unsigned short *)(off + (int)u) & 0x200) {
				if (GetItemInfo(*(unsigned short *)(off + (int)u))[2] == kind)
					break;
			}
			off += 2;
			i++;
		}
		if (i != 0xf) {
			if (GetItemInfo(u->items[i])[3] & 2)
				return -2;
			u->items[i] &= 0xfdff;
		}
	}
	u->items[slot] |= 0x200;
	Func_8078bf0(who);
	CalcStats(who);
	return 0;
}
