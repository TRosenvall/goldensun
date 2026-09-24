/* Func_80b386c  --  0x080b386c, fourth of four in asm/rom_b0000/rom_b0070_c_c_c_c.s.
 * FAKEMATCH.  212 bytes, 92 encodings and 9 relocations identical (objcmp, 3 runs).
 *
 * THIS FILE CARRIES A VERIFICATION SHIM.  The line
 * exists ONLY so the assembler resolves the symbol in a standalone objcmp run
 * instead of leaving an R_ARM_ABS32 placeholder (the reference has a plain
 * literal pool word and no relocation there).  REMOVE IT BEFORE LANDING --
 * b386c_land.c in this directory is this file minus that one line.
 * `_MSG_182 = 0x0182;` ALREADY EXISTS in message.sym:221.  NO NEW .sym ENTRY.
 *
 * WHY IT IS A FAKEMATCH, AND WHY THE CLASS IS THE KNOWN ONE.  The ROM reads the
 * item halfword THREE times: `& 0x1ff`, `>> 11`, and again after the calls.  The
 * first two are adjacent with nothing between them, so cse_main commons them and
 * gcc emits one `ldrh`.  Six non-volatile spellings were measured -- two textual
 * derefs (45 of 92), a cached `unsigned short raw` local (82), 9/2/5 bitfields
 * (85, +3 insns), one struct read and one hand cast (85, +4 insns), and two
 * distinct `unsigned short *` pointers to the same element (89, -4 insns) -- and
 * FOUR FLAGS (-fno-gcse, -fno-cse-follow-jumps, -fno-rerun-cse-after-loop,
 * -fno-expensive-optimizations) all leave the count where it was, which places
 * the commoning in the FIRST cse pass where no flag reaches.  `volatile` on the
 * array is the only thing that keeps both loads, and it takes 45 -> 0 by itself:
 * fixing the load count also fixed the whole register assignment.  That is
 * docs/elevation.md's "The ldrh/ldrsh CSE class" conclusion holding at a site
 * where BOTH loads are unsigned, which is strictly harder than the ldrh/ldrsh
 * pair that section measured.  Add to fakematch.txt.
 *
 * THE OTHER TWO LEVERS ARE REAL AND BOTH ARE LOAD-BEARING.
 *
 * `ldr r0, =0x182` IS THE POOLED-SMALL-CONSTANT TELL.  0x182 = 0xc1 << 1 is
 * shiftable, so gcc builds it with `mov`/`lsl` from a const_int and can only
 * pool it if the operand is a SYMBOL.  `itemId + (int)&_MSG_182` (the spelling
 * src/rom_a1000/rom_a5534_a_c_c.c landed) is worth 66 -> 45; the bare literal
 * 0x182 is 66 of 92 and one instruction long.
 *
 * 0xc8d MUST LIVE IN A NAMED LOCAL SO 0xc88 CAN BE DERIVED FROM IT.  The ROM
 * holds 0xc8d in r5 across two calls and reaches the second message id with
 * `sub r5, #5`.  Written as two literals the two pool loads are independent --
 * cse's get_related_value cannot reach across the calls because each literal is
 * materialised by reload straight into r0, and a hard argument register is not
 * a substitution target.  `msg = 0xc8d; ... _Func_801e7c0(msg - 5, ...)` puts it
 * in a pseudo and the `sub` appears: 51 of 92 -> 45 (and it is the whole tail of
 * the function, sixteen instructions, that comes right with it).
 */
/* Func_80b386c -- asm/rom_b0000/rom_b0070_c_c_c_c.s, fourth of four.
 *
 * FAKEMATCH: `volatile` on the item halfword.  See the ladder below.
 *
 * VERIFICATION SHIM (this line only, not part of the landing):
 * _MSG_182 = 0x0182 ALREADY EXISTS in message.sym:221.  NO NEW .sym ENTRY.
 */

extern int _MSG_182;

struct Unit {
	unsigned char pad000[0xd8];
	volatile unsigned short items[15];
};

extern struct Unit *_GetUnit(int id);
extern void _Func_8016498(void *spr);
extern void _Func_801e7c0(int msg, void *spr, int x, int y);
extern void _Func_801ea08(int val, int digits, void *spr, int x, int y);
extern int _CanRemoveItem(int unitId, int idx);
extern int Func_80b19cc(int raw);

void Func_80b386c(void *spr, int unitId, int idx)
{
	struct Unit *u;
	int itemId;
	int count;
	int r;
	int total;
	int msg;

	u = _GetUnit(unitId);
	itemId = u->items[idx] & 0x1ff;
	count = (u->items[idx] >> 11) + 1;
	if (spr != 0) {
		_Func_8016498(spr);
		_Func_801e7c0(itemId + (int)&_MSG_182, spr, 0, 0);
		r = _CanRemoveItem(unitId, idx);
		if (r == -4) {
			_Func_801e7c0(0xc94, spr, 0, 8);
		} else if (r == -3) {
			_Func_801e7c0(0xc95, spr, 0, 8);
		} else {
			msg = 0xc8d;
			total = count * Func_80b19cc(u->items[idx]);
			_Func_801e7c0(msg, spr, 8, 8);
			_Func_801ea08(total, 5, spr, 0x28, 8);
			_Func_801e7c0(msg - 5, spr, 0x50, 8);
		}
	}
}
