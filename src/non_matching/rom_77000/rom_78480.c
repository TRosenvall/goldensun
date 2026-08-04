/* Func_8078480 @ 0x08078480 -- asm/rom_77000/rom_78414_c_a_a.s
 *
 * Source asm: goldensun/asm/rom_77000/rom_78414_c_a_a.s
 *
 * gcc-2.96 FOLDS THE RANGE, the ROM does not.
 *
 * The ROM tests 2, 3, 4, 5 and 9 as five separate compare-and-branch pairs.
 * Every formulation here turns 2..5 into an unsigned range check:
 *
 *     rom    cmp r3, #2 / beq / cmp r3, #3 / beq / cmp r3, #4 / beq / cmp r3, #5 / beq
 *     ours   add r3, #0xfe / lsl / cmp / bls        (i.e. (kind - 2) <= 3)
 *
 * Tried: a switch with grouped case labels; a chain of || comparisons; and a
 * chain of separate else-if arms. All three fold. The ROM also keeps the
 * result in r2 and copies it to r0 at the end, which the folded versions do
 * not need to do.
 *
 * Unfolded compare chains do occur elsewhere in this ROM, so some C shape
 * reaches them -- it just is not any of these three. Worth revisiting once one
 * of those is decompiled and the shape is known.
 */
#include "gba/types.h"

struct ItemInfo {
    u8 pad_0[2];
    u8 kind;
};

extern struct ItemInfo *GetItemInfo(s32 itemId);

/* Maps an item's kind byte to a category: 1 stays 1; 2, 3, 4, 5 and 9 all
 * become 2; anything else is 0.
 */
s32 Func_8078480(s32 itemId)
{
    struct ItemInfo *info = GetItemInfo(itemId);
    u8 kind = info->kind;
    s32 result = 0;

    if (kind == 1)
        result = 1;
    else if (kind == 2 || kind == 3 || kind == 4 || kind == 5 || kind == 9)
        result = 2;
    return result;
}
