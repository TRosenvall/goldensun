/* Func_80ae7fc  --  0x080ae7fc, split out of asm/rom_a1000/rom_ad274_c_c_c.s;
 * the .rodata .Laf304 that followed stays in rom_ad274_c_c_c_c_c.s. Matched from scratch.
 *
 * `if (a || b) n++` lost the r4/r5 choice (.greg allocates the hoisted p[4]
 * pseudo before n). `if (p[4] & bit) n++; else if (p[0] & bit) n++;` is the
 * same code once the tails cross-jump, and is exact -- presumably by raising
 * n's reference count and so its priority (not re-read from the dump).
 */
#include "gba/types.h"

struct MenuState {
	u8 pad_0[0x208];
	u16 members[8];
	u8 pad_218;
	u8 memberCount;
};

struct Unit {
	u8 pad_0[0xf8];
	u32 has[4];
	u32 set[4];
};

extern struct MenuState *iwram_3001f2c;
extern struct Unit *_GetUnit(s32 id);

void Func_80ae7fc(u8 *out)
{
	struct MenuState *st = iwram_3001f2c;
	struct Unit *u;
	s32 i, j, k, n;
	u32 *p;

	for (i = 0; i < st->memberCount; i++) {
		u = _GetUnit(st->members[i]);
		n = 0;
		p = u->has;
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 20; k++) {
				if (p[4] & (1 << k))
					n++;
				else if (p[0] & (1 << k))
					n++;
			}
			p++;
		}
		out[i] = n;
	}
}
