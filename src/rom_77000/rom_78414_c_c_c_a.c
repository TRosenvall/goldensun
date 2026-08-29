/* BreakItem  --  0x08078a34, was goldensun/asm/rom_77000/rom_78414_c_c_c_a.s.
 *
 * The .s held this function alone, so no split was needed and the linker
 * script's existing line for that object now picks up this file's.
 *
 * PARKED FOR ELEVEN FORMULATIONS ON A DIFFERENCE THAT WAS NEVER THERE. The
 * park read:
 *
 *     rom    ldr  r3, =0x400
 *     ours   ldrh r3, .L2 / .word 0x400
 *
 * and concluded gcc pools a small constant as a HALFWORD when the target is
 * `unsigned short` while the ROM's word load means the expression was not
 * u16-typed. The first half is true and the conclusion does not follow.
 * Thumb-1 has no pc-relative `ldrh`; gas assembles gcc's line to the same
 * halfword the ROM's `ldr` is. tryc.py was comparing assembly TEXT and
 * normalising only the `=` spelling, so it reported a difference in the
 * mnemonic that does not exist in the encoding.
 *
 * The park closed with "worth retrying once another function in the corpus is
 * found that pools a small constant as a word -- that one will show the shape."
 * OvlFunc_914_2008b24 was that function, and the shape it showed is that there
 * is nothing to find: the C below is unchanged from the parked version and
 * `make compare` passes on it.
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
