/* Func_80ab1f4 @ 0x080ab1f4 -- asm/rom_a1000/rom_aa538_c_c_a.s
 *
 * Source asm: goldensun/asm/rom_a1000/rom_aa538_c_c_a.s
 *
 * Blocker class 5, SCHEDULING. 18 of 19 instructions match; one pair is
 * swapped:
 *
 *     rom    add r0, #1 / ldr r3, [sp, #0x10]
 *     ours   ldr r3, [sp, #0x10] / add r0, #1
 *
 * gcc pulls the fifth stack argument up one slot ahead of the border
 * increment. Tried: the two coordinates in named locals computed before the
 * call, and inline in the call expression. No movement.
 */
#include "gba/types.h"

struct Window {
    u8 pad_00[0x0c];
    u16 col;
    u16 row;
};

extern void _Func_8022768(s32 x, s32 y, s32 a3, s32 a4, s32 a5);

/* Window-relative wrapper: adds the window's own column and row, plus one for
 * the border, and forwards everything else unchanged.
 */
void Func_80ab1f4(struct Window *window, s32 x, s32 y, s32 a4, s32 a5, s32 a6)
{
    _Func_8022768(window->col + x + 1, window->row + y + 1, a4, a5, a6);
}
