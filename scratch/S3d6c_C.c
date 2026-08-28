#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

/* Counts the non-empty slots among the fifteen inventory halfwords at +0xD8.
 *
 * The slot format, worth recording: bits 0..8 the item id, bit 9 locked
 * (equipped or a key item), bits 11..15 the quantity less one. That is why
 * consuming one unit subtracts 0x800.
 */
s32 Func_80a3d6c(s32 unitId)
{
    u16 *slot = (u16 *)(_GetUnit(unitId) + 0xd8);
    s32 mask;
    s32 count;
    s32 i;

    count = 0;
    mask = 0x1ff;
    for (i = 14; i >= 0; i--) {
        if ((*slot++ & mask) != 0)
            count++;
    }
    return count;
}
