/* Map interaction: read the slot record for whatever the player is facing.
 *
 * Whole-file conversion of asm/rom_8a000/rom_8d9a4_c_a_c_c_c_a.s -- one
 * function, so the ROM layout is preserved without splitting the translation
 * unit.
 */
#include "gba/types.h"

extern u8 *iwram_3001ebc;
extern s32 GetMapActorIndex(void);

/* Returns the first word of the 8-byte record at +0x11C for the map actor the
 * player is standing on, or 0 when there is none.
 *
 * The index is a plain -1 sentinel, not a count, so the guard has to be an
 * equality test rather than a range check.
 *
 * The stride is written as a shift because that is what byte-matches: the
 * struct-indexed spelling of the same address arithmetic makes gcc-2.96
 * allocate the base pointer and the scaled index the other way round.
 */
u32 Func_808ed4c(void)
{
    s32 index = GetMapActorIndex();

    if (index == -1)
        return 0;
    return *(u32 *)(iwram_3001ebc + (index << 3) + 0x11c);
}
