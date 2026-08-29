/* Func_80f4100  --  0x080f4100, asm/rom_f4000/rom_f4008_c_c.s
 * ScalePalette. One of TWO byte-identical copies (tools/find_twins.py).
 *
 * Source asm: goldensun/asm/rom_f4000/rom_f4008_c_c.s
 *
 * BLOCKER CLASS: register allocation, driven by WHERE THE THREE PRODUCTS DIE.
 * Status: 54 lines against 54 with the form below, 41 differing -- close in
 * size and not close in shape.
 *
 * WHAT IT DOES. Scales each 5:5:5 channel of a palette independently: mask,
 * multiply by a 16.16 scale, shift back down and re-mask, so a channel that
 * overflows truncates into its own field instead of carrying into the next.
 *
 * THE ROM'S LOOP COMPUTES ALL THREE PRODUCTS BEFORE ANY SHIFT:
 *
 *     and r3, r2   (c & 0x1f)      and r2, r1   (c & 0x3e0)
 *     mul r3, r5   mul r2, r5      and r1, r4   (0x7c00 & c)   mul r1, r5
 *     lsr r4, r3, #16 ... lsr r2, #16 ... orr ... lsr r1, #16 ... orr
 *
 * so three products are live at once in r1/r2/r3, the three masks sit in r8,
 * r14 and r12 for the whole loop, and r4 holds the loaded colour. Eleven live
 * values, and it fits.
 *
 * TWO C SHAPES, AND NEITHER IS RIGHT:
 *
 *   named temporaries          64 lines. Gives the ROM's OPERATION ORDER --
 *   `r = ...; g = ...;         all three masks and muls, then all three shifts
 *    b = ...; *dst = ...;`     -- but gcc puts the three products in r8/r9/r10
 *                              and pushes all three, ten instructions of
 *                              prologue and epilogue the ROM does not have.
 *
 *   one expression             54 lines, the right size. gcc allocates only
 *   (the form below)           r1-r4 plus lr/r12/r8, and the register count is
 *                              right -- but each channel now runs
 *                              mask/mul/shift/mask to completion before the
 *                              next starts, which is not the ROM's order.
 *
 * The two properties trade off against each other and nothing tried gets both.
 *
 * MEASURED, all against the same reference:
 *
 *   named temps, `int`                        64 / 63 differ
 *   named temps, `u32`                        64 / 63
 *   named temps, shift folded into the temp   57 / 51
 *   `for (i = 0; i < count; i++)` with [i]    66 / 64
 *   post-increment and `--count` in the test  55 / 49
 *   ONE EXPRESSION (this file)                54 / 41
 *
 *   ... and by flag, on the best two shapes:
 *   one expression   -O2 54/41   -fno-gcse 54/41   -fno-rerun-cse 54/41
 *                    -fno-schedule-insns2 54/41    -O1 53/51
 *   incremental      -O2 55/49   -O1 54/36
 *
 * `int` versus `u32` for the products decides `asr` against the ROM's `lsr`
 * and is settled: they are unsigned. That was the only thing the type change
 * bought.
 *
 * WHAT WOULD MOVE IT is a shape that keeps three products live without giving
 * them their own pseudos -- which is what the ROM's compiler did and this one
 * will not from any of the six spellings above. Related to the register
 * ALLOCATION class in docs/elevation.md, but this one is not a permutation:
 * ours needs three more callee-saved registers than the ROM's does.
 */
#include "gba/types.h"

int Func_80f4100(u16 *src, u16 *dst, int scale, int count)
{
    u32 c;

    if (count > 0) {
        do {
            c = *src;
            *dst = ((((c & 0x1f) * scale) >> 16) & 0x1f)
                 | ((((c & (0xf8 << 2)) * scale) >> 16) & (0xf8 << 2))
                 | ((((c & (0xf8 << 7)) * scale) >> 16) & (0xf8 << 7));
            src++;
            dst++;
            count--;
        } while (count != 0);
    }
    return 0;
}
