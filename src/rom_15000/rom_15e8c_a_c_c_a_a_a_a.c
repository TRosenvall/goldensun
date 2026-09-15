/* CloseUIBox -- 0x08016418, asm/rom_15000/rom_15e8c_a_c_c_a_a_a_a.s.
 *
 * Tears down a UI box: saves its rectangle into the shadow fields, then either
 * clears the region and zeroes the whole record, or parks it with a mode of 4.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s, no split, stage1.ld:302
 * verbatim with its asm/ prefix, no Makefile rule, no pins.
 *
 * EXACT ON THE FIRST CANDIDATE. Two things carried over from batch 265 and both
 * were written in without a measurement:
 *
 *   * THE ZERO IS A NAMED LOCAL, not sixteen bare literals. The ROM holds 0 in
 *     r6 from `mov r6, #0` before `strh r6, [r5, #0x16]` and reuses it for every
 *     one of the fifteen stores in the clear block. That is the same lever
 *     Func_942e0 needed next door, and it is assigned mid-sequence rather than
 *     at the top for the same reason: a zero assigned before the first call
 *     crosses it and takes a callee-saved register of its own.
 *   * THE FIELDS ARE WRITTEN ASCENDING, exactly as the ROM stores them, and the
 *     two 32-bit fields at 0x00 and 0x04 are `int` while everything from 0x08
 *     to 0x22 is `u16` -- the `str`/`strh` split in the ROM is the whole layout
 *     argument, and 0x24 is the resulting size.
 *
 * ONE READING WORTH RECORDING. The else arm stores the ARGUMENT register:
 * `strh r7, [r5, #0x18]` where r7 is the `flush` parameter. It is not a copy of
 * the argument -- that arm is only reached when `flush == 0`, and gcc knows it,
 * so it substitutes the register it already has for the constant. Written as
 * the plain `b->f18 = 0;` it is byte-identical. A store of a PARAMETER register
 * inside a branch guarded by that parameter being zero is a substituted
 * constant, not a field being assigned from the argument.
 *
 * EXACT: 96 bytes, 46 encodings, 2 relocations, measured three times.
 */
#include "gba/types.h"

struct UIBox {
    int f0;
    int f4;
    u16 f8;
    u16 fa;
    u16 fc;
    u16 fe;
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u16 f1a;
    u16 f1c;
    u16 f1e;
    u16 f20;
    u16 f22;
};

extern void Func_8016478(struct UIBox *b);
extern void ClearUIRegion(int x, int y, int w, int h);

void CloseUIBox(struct UIBox *b, int flush)
{
    int z;

    if (b != NULL) {
        Func_8016478(b);
        b->f1c = b->fc;
        b->f1e = b->fe;
        b->f20 = b->f8;
        z = 0;
        b->f16 = z;
        b->f22 = b->fa;
        if (flush != 0) {
            ClearUIRegion(b->fc, b->fe, b->f8, b->fa);
            b->f0 = z;
            b->f4 = z;
            b->f8 = z;
            b->fa = z;
            b->fc = z;
            b->fe = z;
            b->f10 = z;
            b->f12 = z;
            b->f14 = z;
            b->f16 = z;
            b->f18 = z;
            b->f1a = z;
            b->f1c = z;
            b->f1e = z;
            b->f20 = z;
            b->f22 = z;
        } else {
            b->f18 = 0;
            b->f1a = 4;
        }
    }
}
