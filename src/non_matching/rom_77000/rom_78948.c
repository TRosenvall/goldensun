/* Func_8078948 @ 0x08078948 -- asm/rom_77000/rom_78414_c_c_a_c.s
 *
 * Blocker class 6, ARGUMENT REGISTER FILL ORDER. 22 of 23 instructions match;
 * one pair is swapped:
 *
 *     rom    mov r1, #1 / mov r0, r7
 *     ours   mov r0, r7 / mov r1, #1
 *
 * The ROM fills r0 LAST, from the callee-saved register holding the value;
 * gcc fills it first. Tried: the constant as a literal and as a named local;
 * the item value in s32 and u16 (u16 is wrong for a different reason -- it
 * produces ldrsh, see below).
 *
 * This is the same shape as LoadStatusIcon (src/non_matching/rom_15000/
 * rom_1a2ec.c), which is why it is now recorded as its own class rather than
 * filed under scheduling: in both cases every argument is correct and only
 * the order of the register moves differs, and in both the ROM defers the r0
 * move to last.
 *
 * Note the item MUST be read into an s32, not a u16 -- a u16 local produces
 * ldrsh where the ROM has ldrh. Fourth occurrence of that trap.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_00[0xd8];
    u16 items[16];
};

extern struct Unit *GetUnit(s32 unitId);
extern s32 Func_80788c4(s32 unitId, s32 slot);
extern void Func_8078ad0(s32 itemId, s32 count);
extern void _Func_8091858(void);

/* Consumes the item in the slot and, unless that failed, tells the field
 * layer it was used. The item id is read BEFORE the consume, because the
 * consume clears the slot. Returns whatever the consume returned.
 */
s32 Func_8078948(s32 unitId, s32 slot)
{
    struct Unit *unit = GetUnit(unitId);
    s32 item = unit->items[slot];
    s32 result = Func_80788c4(unitId, slot);

    if (result != -1) {
        Func_8078ad0(item, 1);
        _Func_8091858();
    }
    return result;
}
