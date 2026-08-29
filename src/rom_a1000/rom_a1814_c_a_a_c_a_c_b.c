/* Menu: draw the party's coin total.
 *
 * Split out of asm/rom_a1000/rom_a1814_c_a_a_c_a_c.s, which holds eleven
 * functions; the _a and _c parts stay as assembly and are listed around this
 * one in stage1.ld, so the ROM layout is unchanged.
 */
#include "gba/types.h"

struct State {
    u8 pad_00[0x10];
    u32 money;
};

extern struct State gState;
extern void _Func_801e9d4(u32 value, s32 digits, s32 window, s32 x, s32 a5);
extern void _Func_801e7c0(s32 label, s32 window, s32 x, s32 a4);

/* Seven digits at x 8, then the label at x 0x40. */
void Func_80a23c0(s32 window)
{
    _Func_801e9d4(gState.money, 7, window, 8, 0);
    _Func_801e7c0(0xb0b, window, 0x40, 0);
}
