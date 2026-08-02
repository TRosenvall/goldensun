/* Menu icons: load the item icon for a move.
 *
 * Split out of asm/rom_15000/rom_19ebc_a_c_c.s, which holds twelve functions;
 * the neighbouring _a/_c parts stay as assembly and are listed around this one
 * in stage1.ld, so the ROM layout is unchanged.
 */
#include "gba/types.h"

struct MoveInfo {
    u8 pad_00[4];
    u8 iconId;
};

extern struct MoveInfo *_GetMoveInfo(s32 id);
extern void LoadItemIconID(s32 iconId, s32 a1, s32 a2, s32 a3, s32 a4);

/* Resolves the move record, takes its icon id from +0x04 and forwards
 * everything else through unchanged. The placement arguments are passed on
 * without being looked at, so they are left unnamed here.
 */
void LoadOldMoveIcon(s32 moveId, s32 a1, s32 a2, s32 a3, s32 a4)
{
    struct MoveInfo *info = _GetMoveInfo(moveId);

    LoadItemIconID(info->iconId, a1, a2, a3, a4);
}
