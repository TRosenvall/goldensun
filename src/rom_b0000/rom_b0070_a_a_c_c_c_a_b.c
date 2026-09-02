/* Func_80b1868 -- 0x080b1868, from
 * goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_a.s.
 *
 * Equip the item in a unit's inventory slot: refuse if the item carries the
 * 0x200 flag, if the unit's class cannot use it, or if what is already in that
 * kind's slot is flagged un-removable; otherwise prompt, equip and narrate.
 *
 * THE ONE CONSTANT IS A SYMBOL, and 0xad0 proves it the same way 0x810 does in
 * Func_80b595c this batch: 0xad0 is 0xad << 4, so gcc BUILDS it -- `mov r0,
 * #0xad / lsl r0, #0x4` -- and pools only what it cannot synthesise in two
 * cheap instructions. The ROM spends a pool word on it anyway. Added to
 * message.sym as _MSG_ad0; the namespace is fixed by the consumer, since
 * _Func_8017658's first argument is a message id everywhere else in the tree.
 * Literal 0xad0 is 2 aligned of 112; the symbol is a match.
 *
 * THE RETURN TYPES WERE READ OFF THE REFERENCE BEFORE WRITING, not repaired
 * afterwards. _Func_8017658 fills r1, r2, r3 and then r0; _GetEquippedItem
 * fills r1 then r0. r0 last means gcc must be reserving it for a returned
 * value, so both are declared `int`. Every other call fills r0 first and is
 * left alone.
 *
 * The `& 0x200` test is a GENUINE SECOND READ of the same slot through the
 * struct, not a named local -- the read-count rule -- and the halfword array at
 * +0xd8 reproduces the ROM's `lsl r5,#1 / add r5,#0xd8 / ldrh r3,[r7,r5]`
 * including the base-first operand order with no pointer-arithmetic spelling.
 *
 * MEASURED (rom 112 lines):
 *   literal 0xad0                                        113 lines, 2 aligned
 *   (int)&_MSG_ad0 with the symbol defined               112, MATCH
 */
#include "message.h"

typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
typedef struct { unsigned char pad00[0x20]; int f20; } State;

extern unsigned char iwram_3001f2c[];

extern Unit *_GetUnit(int unit);
extern unsigned char *_GetItemInfo(int item);
extern int _CanEquipItem(int unit, int item);
extern int _GetEquippedItem(int unit, int kind);
extern void _Func_8019908(int unit, int a);
extern void Func_80b04dc(int msg);
extern int Func_80b0634(int a);
extern void _EquipItem(int unit, int slot);
extern void Func_80b1dec(int a, int unit);
extern void _PlaySound(int sfx);
extern void _Func_8019a54(void);
extern int _Func_8017658(int msg, int a, int b, int c);
extern int _Func_8017364(void);
extern void WaitFrames(int n);
extern void Func_80b0574(int msg);

int Func_80b1868(int unit, int slot)
{
	State *st;
	Unit *u;
	unsigned char *info;
	int item;
	int eq;

	st = *(State **)iwram_3001f2c;
	u = _GetUnit(unit);
	item = u->items[slot] & 0x1ff;
	info = _GetItemInfo(item);
	if ((u->items[slot] & (0x80 << 2)) != 0) {
		return 0;
	}
	if (_CanEquipItem(unit, item) == 0) {
		return 0;
	}
	eq = _GetEquippedItem(unit, info[2]);
	if (eq != -1) {
		if ((_GetItemInfo(u->items[eq])[3] & 2) != 0) {
			return 0;
		}
	}
	_Func_8019908(unit, 1);
	Func_80b04dc(0xca2);
	if (Func_80b0634(0) != 0) {
		return 0;
	}
	_EquipItem(unit, slot);
	if (st->f20 != 0) {
		Func_80b1dec(st->f20, unit);
	}
	if ((info[3] & 1) != 0) {
		_PlaySound(0x67);
		_Func_8019a54();
		_Func_8017658(MSG_ad0, 8, 4, 2);
		while (_Func_8017364() == 0) {
			WaitFrames(1);
		}
	}
	Func_80b0574(0xca3);
	return 1;
}
