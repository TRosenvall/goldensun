/* Func_80b8530 @ 0x080b8530 -- asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 *
 * Blocker class 2, REGISTER BIRTH ORDER, down to ONE instruction:
 *
 *     rom    lsr r3, r0, #8      (result into a fresh register)
 *     ours   lsr r0, r0, #8      (reused in place)
 *
 * gcc reuses r0 -- the register the call returned in -- for the shifted value;
 * the ROM moves it to r3, which is what REG_ALLOC_ORDER hands a NEW pseudo.
 *
 * Tried: the height and the final result as one variable (that forces the
 * value live across the second call, which costs a callee-saved register and
 * an extra push); as two separate variables with the call result named
 * separately; and with the early return spelled as a nested if. The
 * one-variable form is 29 instructions like the ROM but diverges at
 * instruction 0; the others diverge here at 9.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_00[0x128];
    u8 classIdx;
};

extern struct Unit *_GetUnit(s32 unitId);
extern s32 GetEnemyHeight(s32 classIdx);
extern s32 Func_80c23c0(s32 classIdx);

/* Resolves the character's class to a draw slot, defaulting when neither
 * lookup reports one. Note the unit record is resolved TWICE -- once per
 * lookup -- rather than cached; that repetition is in the ROM.
 */
s32 Func_80b8530(s32 unitId)
{
    s32 height = (u8)GetEnemyHeight(_GetUnit(unitId)->classIdx) << 16;

    if (height != 0)
        return height;
    if (Func_80c23c0(_GetUnit(unitId)->classIdx) != 0)
        return 0x180000;
    return 0x300000;
}
