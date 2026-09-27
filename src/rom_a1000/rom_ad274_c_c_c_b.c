/* Func_80ae778  --  0x080ae778, split out of asm/rom_a1000/rom_ad274_c_c_c.s;
 * Func_80ae7fc follows in rom_ad274_c_c_c_c_b.c and the .rodata .Laf304
 * stays in rom_ad274_c_c_c_c_c.s. Matched from scratch.
 *
 * s8 in include/gba/types.h is plain `char`, UNSIGNED here: the ROM's `ldrsb`
 * needs a literal `signed char` array. A redundant `j = i; if (i < count)
 * while (++j < count)` reproduces the ROM's duplicated `cmp i, count` at the
 * inner loop's head.
 */
#include "gba/types.h"

struct MenuState {
	u8 pad_0[0x219];
	u8 memberCount;
};

extern struct MenuState *iwram_3001f2c;
extern void Func_80ae7fc(signed char *out);

s32 Func_80ae778(s32 giver, s32 receiver)
{
	struct MenuState *st = iwram_3001f2c;
	signed char counts[16];
	u8 i, j;
	s32 ok;

	Func_80ae7fc(counts);
	counts[giver]--;
	counts[receiver]++;
	ok = 1;
	for (i = 0; i < st->memberCount; i++) {
		j = i;
		if (i < st->memberCount) {
			while (++j < st->memberCount) {
				if (counts[i] - counts[j] > 1 || counts[i] - counts[j] < -1) {
					ok = 0;
					break;
				}
			}
		}
	}
	return ok;
}
