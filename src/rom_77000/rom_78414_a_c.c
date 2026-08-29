/* Equipment: may this unit's class use this item?
 *
 * Whole-file conversion of asm/rom_77000/rom_78414_a_c.s -- one function, so
 * the ROM layout is preserved without splitting the translation unit.
 */
#include "gba/types.h"

/* Only the two fields this function establishes are declared. The unit record
 * is 0x14C bytes (see include/combatant.h); the class index at +0x128 is not
 * named there yet, so it is declared locally rather than guessed into the
 * shared header.
 */
struct Unit {
    u8 pad_0[0x128];
    u8 classIdx;
};

struct ItemInfo {
    u8 pad_0[0x04];
    u16 classMask;  /* one bit per class, eight wide */
};

extern struct Unit *GetUnit(s32 unitId);
extern struct ItemInfo *GetItemInfo(s32 itemId);

/* Returns 1 when the unit's class may use the item, 0 otherwise.
 *
 * A class index above 7 always fails, which is what fixes the mask at eight
 * bits wide even though it is stored as a halfword.
 *
 * Our annotation for this address read it as ability rather than equipment
 * ("CanUseAbility"); the shape is the same either way and the name here is
 * this tree's.
 */
s32 CanEquipItem(s32 unitId, s32 itemId)
{
    struct Unit *unit = GetUnit(unitId);
    struct ItemInfo *info = GetItemInfo(itemId);
    s32 mask = info->classMask;

    if (unit->classIdx > 7)
        return 0;
    return (mask >> unit->classIdx) & 1;
}
