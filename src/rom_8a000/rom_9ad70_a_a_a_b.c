/* Idle flicker: nudge a resting actor's palette at random.
 *
 * Split out of asm/rom_8a000/rom_9ad70_a_a_a.s; the rest stays in
 * ..._a_a_a_c.s, listed after this one in stage1.ld so the ROM layout is
 * unchanged.
 */
#include "actor.h"

/* The offset table lives in asm/rom_8a000/rom_9ad70_c_c.s, which already
 * exports it. `.L9f160` is not a C identifier, so the name is attached with
 * an asm label.
 *
 * NOT s8: gba/types.h defines that as plain `char`, which is UNSIGNED in this
 * build (Camelot compiled with __CHAR_UNSIGNED__). The ROM's `ldrsb` needs an
 * explicitly signed type, and `s8` here silently produces `ldrb`.
 */
extern signed char Data_809f160[] __asm__(".L9f160");

extern u32 Random(void);
extern void _Actor_SetColorswap(Actor *actor, s32 value);

/* Per-frame hook. Scales a random word down to a table index -- the shift
 * pair is a multiply-and-take-the-high-half, not a mask -- and applies the
 * signed offset it finds as a palette swap.
 */
void Func_809ad70(Actor *actor)
{
    _Actor_SetColorswap(actor, Data_809f160[(Random() << 3) >> 16]);
}
