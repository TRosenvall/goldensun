/* Menu: is this item selectable?
 *
 * Split out of asm/rom_a1000/rom_a5534_c_c_c.s. The preceding functions stay
 * in ..._c_c_c_a.s; the trailing .rodata blobs -- which rom_a5534_a_b.s
 * references by name -- are in ..._c_c_c_c.s. All three are listed in that
 * order in stage1.ld, so the ROM layout is unchanged.
 */
#include "gba/types.h"

struct MoveInfo {
    u8 pad_00[1];
    u8 flags;      /* bits 6 and 7 together mark the entry hidden */
    u8 pad_02[0x0a];
    u8 kind;       /* non-zero means it is not a plain item */
};

extern struct MoveInfo *_GetMoveInfo(s32 id);

/* Returns 1 only for a plain item that is not flagged hidden.
 *
 * The id is truncated to 14 bits before the lookup -- callers pack flags into
 * the top two. Written as a shift pair rather than `& 0x3fff` because that is
 * what byte-matches: the mask spelling makes gcc-2.96 materialise the constant
 * from the literal pool, where the ROM shifts.
 */
s32 Func_80a735c(s32 itemId)
{
    struct MoveInfo *info = _GetMoveInfo(((u32)itemId << 18) >> 18);

    if (info->kind == 0 && (info->flags & 0xc0) != 0xc0)
        return 1;
    return 0;
}
