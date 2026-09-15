/* Ability category lookup.
 *
 * Whole-file conversion of asm/rom_77000/rom_78414_c_a_a.s -- one function, so
 * the ROM layout is preserved without splitting the translation unit.
 */
#include "gba/types.h"

/* Only the byte this function establishes is declared. The record is wider;
 * the neighbouring rom_78414_a_c.c names a halfword at +0x04, which is why
 * this one stops at +0x02 rather than guessing the rest into a shared header.
 */
struct ItemInfo {
    u8 pad_0[0x02];
    u8 kind;
};

extern struct ItemInfo *GetItemInfo(s32 itemId);

/* Maps the record's kind byte to a category: kind 1 is category 1, kinds 2
 * through 5 and kind 9 are category 2, everything else is 0.
 *
 * Two things about the spelling are load-bearing and must not be tidied:
 *
 *  - The kinds are compared ONE AT A TIME with `==`. Written as a `switch`,
 *    gcc recognises 2..5 as contiguous and emits a range test
 *    (`cmp #5 / bgt / cmp #2 / bge`) instead of the ROM's five `cmp`/`beq`
 *    pairs.
 *  - `category = 0` is assigned BETWEEN the call and the load, not after it.
 *    That is what puts the running result in r2 and leaves the ROM's closing
 *    `mov r0, r2` in place; assigned after the load it takes r0 and gcc drops
 *    the move, which is one instruction short of the ROM.
 */
s32 Func_8078480(s32 itemId)
{
    struct ItemInfo *info;
    s32 kind;
    s32 category;

    info = GetItemInfo(itemId);
    category = 0;
    kind = info->kind;

    if (kind == 1)
        category = 1;
    else if (kind == 2)
        category = 2;
    else if (kind == 3)
        category = 2;
    else if (kind == 4)
        category = 2;
    else if (kind == 5)
        category = 2;
    else if (kind == 9)
        category = 2;

    return category;
}
