/* Cutscene layer: hand an item to a party member.
 *
 * Split out of asm/rom_8a000/rom_91584_c_a_c_c_c.s; the preceding functions
 * stay in asm/rom_8a000/rom_91584_c_a_c_c_c_a.s, listed before this one in
 * stage1.ld so the ROM layout is unchanged.
 */
#include "gba/types.h"

extern s32 _GiveItemTo(s32 memberId, s32 itemId);

/* Returns the member id on success, or -1 when the inventory refuses it --
 * which _GiveItemTo signals with any negative result, not a specific one.
 *
 * The second parameter is genuinely unused; callers pass something there and
 * this ignores it.
 *
 * Written with the success case as the `if` body rather than testing for
 * failure, because that is what puts the two blocks in the ROM's order.
 */
s32 Func_8091c1c(s32 itemId, s32 unused, s32 memberId)
{
    if (_GiveItemTo(memberId, itemId) >= 0)
        return memberId;
    return -1;
}
