/* BreakItem @ 0x08078a34 -- asm/rom_77000/rom_78414_c_c_c_a.s
 *
 * Source asm: goldensun/asm/rom_77000/rom_78414_c_c_c_a.s
 *
 * ONE INSTRUCTION SHORT, and the one that is left is a known gcc-2.96
 * fingerprint rather than anything about this function.
 *
 *     rom    ldr  r3, =0x400        <- word-sized pool load
 *     ours   ldrh r3, .L2 / .word 0x400
 *
 * Every other instruction, including the redundant `mov r3, r2` that only
 * appears when the item halfword is read twice, is identical. camelot-gcc's
 * fingerprint list records that gcc-2.96 pools a small constant AS A HALFWORD
 * when the target is an `unsigned short`, which is exactly what happens here;
 * the ROM's word-sized load means the original expression was not
 * unsigned-short-typed at that point, and it is not yet known what C shape
 * gets gcc there.
 *
 * Eleven formulations were tried and measured: the OR computed in s32, u32 and
 * u16; the constant on either side; via a u16* alias; through a named
 * temporary; and with an explicit (u16) cast on the store. Every one of them
 * either produced this same halfword pool load or regressed further by
 * dropping the double read. The version below is the closest.
 *
 * Worth retrying once another function in the corpus is found that pools a
 * small constant as a word -- that one will show the shape.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_0[0xd8];
    u16 items[16];
};

extern struct Unit *GetUnit(s32 unitId);

/* Sets bit 10 of the slot's halfword. Returns 0 on success, -1 for an empty
 * slot.
 */
s32 BreakItem(s32 unitId, s32 slot)
{
    struct Unit *unit = GetUnit(unitId);

    if (unit->items[slot] == 0)
        return -1;
    unit->items[slot] |= 0x400;
    return 0;
}
