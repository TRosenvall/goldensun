/* Func_80ae714 -- 0x080ae714, from goldensun/asm/rom_a1000/rom_ad274_c_c.s.
 *
 * Marks which party members cannot give to the chosen receiver: for each
 * member, clear the flag, then set it when the member is not the receiver and
 * the transfer test refuses. Returns how many were marked.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_ad274_c_c_a.o and rom_ad274_c_c_c.o in goldensun/stage1.ld.
 *
 * THE DUPLICATE-CONSTANT WARNING WAS A FALSE ALARM, and how it is false is the
 * useful part. The selector flags `ldr r3, =0x219` appearing twice as the
 * duplicate-constant CSE wall. It is not a duplicated constant: it is the LOOP
 * BOUND BEING RE-READ EVERY ITERATION. A call intervenes, and 0x219 is too
 * large for an `ldrb` immediate offset, so each read costs its own pool load.
 * Writing the bound directly in the `for` condition and never caching it
 * reproduces both materialisations on the first screen.
 *
 * > Before parking on a repeated pooled constant, check whether it is an
 * > OFFSET being re-read rather than a VALUE being rebuilt. A re-read is the
 * > read-count rule and yields to spelling; a rebuild is the wall.
 *
 * MEASURED (rom 47 lines):
 *   bound read in the `for` condition, struct field       47 lines, MATCH
 *   the same through a flat `u8 *` and `st[0x219]`        47, MATCH (byte-identical)
 *   the bound hoisted into a local before the loop        52, 51
 *   `*out` with `out++` at the loop tail                  47, 5
 *
 * The struct form is kept over the flat pointer because the elevated sibling
 * from this same original file, src/rom_a1000/rom_ad274_c_a_b.c, already
 * declares the module block as a padded struct pointer.
 */
#include "gba/types.h"

struct MenuState {
	u8 pad_0[0x219];
	u8 memberCount;
};

extern struct MenuState *iwram_3001f2c;
extern s32 Func_80ae778(s32 receiver, s32 donor);

s32 Func_80ae714(u8 *out, s32 receiver)
{
	struct MenuState *st = iwram_3001f2c;
	s32 i;
	s32 n;

	n = 0;
	for (i = 0; i < st->memberCount; i++) {
		out[i] = 0;
		if (i != receiver && Func_80ae778(receiver, i) == 0) {
			out[i] = 1;
			n++;
		}
	}
	return n;
}
