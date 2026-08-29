/* Field actor scale table: store one slot's value.
 *
 * Split out of asm/rom_a1000/rom_ad274_c_a.s, which holds six functions; the
 * neighbours are asm/rom_a1000/rom_ad274_c_a_a.s and
 * asm/rom_a1000/rom_ad274_c_a_c.s, listed around this one in stage1.ld so the
 * ROM layout is unchanged.
 */
#include "gba/types.h"

/* The array is at +0x244 of the field state block. Only its stride is
 * established here, so the rest of the block is padding rather than guessed
 * fields.
 */
struct FieldState {
    u8 pad_0[0x244];
    u32 actorScale[0x40];
};

extern struct FieldState *iwram_3001f2c;

/* No null check and no bounds check -- callers are trusted. */
void Func_80ad5f4(s32 slot, u32 value)
{
    iwram_3001f2c->actorScale[slot] = value;
}
