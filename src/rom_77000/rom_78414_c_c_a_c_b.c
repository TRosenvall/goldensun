/* Cluster Func_8078948..Func_8078948 extracted from goldensun/asm/rom_77000/rom_78414_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_77000/rom_78414_c_c_a_c_a.o and asm/rom_77000/rom_78414_c_c_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * PARKED, AND NOW MATCHED, on the same lever as LoadStatusIcon.
 *
 * It sat at 22 of 23 instructions with one pair swapped in the argument setup
 * for Func_8078ad0:
 *
 *     rom    mov r1, #1 / mov r0, r7
 *     ours   mov r0, r7 / mov r1, #1
 *
 * The ROM defers the r0 move to last. LEAVING THE CALLEE IMPLICITLY DECLARED
 * reverses the order gcc fills that call's own argument registers, and it
 * matches. So the `extern void Func_8078ad0(s32 itemId, s32 count);` that was
 * here is deliberately gone; putting it back costs the match.
 *
 * See src/rom_15000/rom_19ebc_a_c_c_c_a_c_b.c for why this is NOT the
 * declaration lever docs/elevation.md already describes. That one is about
 * the PRECEDING call keeping r0 live; this one is about the call's own
 * argument order. The two were conflated, and the older park note here
 * concluded "the order does not move" after trying the wrong one.
 *
 * KEEP: the item MUST be read into an s32, not a u16 -- a u16 local produces
 * ldrsh where the ROM has ldrh. That was the fourth occurrence of that trap
 * and it is unrelated to the fill-order question.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_00[0xd8];
    u16 items[16];
};

extern struct Unit *GetUnit(s32 unitId);
extern s32 Func_80788c4(s32 unitId, s32 slot);
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
