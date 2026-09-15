/* Func_809a738 -- 0x0809a738, asm/rom_8a000/rom_9a44c_a_c.s.
 *
 * The update function Func_809a7f4 next door installs: steps the actor by a
 * RANDOMISED radius each tick, drifts the heading, occasionally arms a
 * countdown that biases the drift the other way, and after 0x65 ticks hands off
 * to a script.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s after Func_809a7f4 was split
 * out, no further split, stage1.ld:1047 verbatim, no Makefile rule, no pins.
 *
 * EXACT ON THE FIRST CANDIDATE, entirely on what its file-mate had just cost.
 * Three things transferred with no measurement:
 *
 *   * THE INDIRECT CALL IS include/math.h's `fx32_multiply`, not a C function
 *     pointer. The ROM's `.call_via r5` macro is `mov r12, pc / bx r5` and
 *     carries NO relocation; a plain function pointer emits `bl _call_via_rN`
 *     and does. That distinction cost the file-mate an hour and cost this one
 *     nothing.
 *   * BOTH PRODUCTS BEFORE EITHER STORE, so the first survives the sin call in
 *     a callee-saved register.
 *   * THE SCALE IS A NAMED LOCAL -- here `Random() + (0x80 << 10)`, which has to
 *     be named anyway.
 *
 * TWO READINGS WORTH KEEPING:
 *
 *   `mov r1, #0xca / lsl r1, #15` compared against `lsl r3, #16` is a HALFWORD
 *   COMPARE AGAINST 0x65, not a comparison with 0x650000. gcc compares a value
 *   it has truncated by shifting BOTH sides left 16, so read the constant as
 *   `0xca << 15 >> 16`. Written as `a->f64 == 0x65` it comes out exactly.
 *
 *   `bl Random / lsl r0, #5 / lsr r0, #16` is `(Random() << 5) >> 16` written
 *   literally -- an unsigned narrowing, not a divide. The second site shifts by
 *   4 instead of 5 and adds 8. Both reproduce as written.
 *
 * The countdown field is `short`: the ROM reads it with `ldrsh` for the `!= 0`
 * test and `ldrh` for the decrement, which is the ordinary signed-field pattern
 * -- the subtract's result is truncated by the `strh` so gcc uses the cheaper
 * unsigned load there. One field, two load forms, one declaration.
 *
 * EXACT: 188 bytes, 85 encodings, 8 relocations, measured three times, clean on
 * tools/tryc.py.
 */
#include "math.h"

struct A {
    u8 pad00[6];
    u16 f6;
    int f8;
    u8 pad0c[4];
    int f10;
    u8 pad14[0x50];
    u16 f64;
    short f66;
};

extern unsigned Random(void);
extern unsigned char Data_9f0b0[];
extern void _Actor_SetScript(struct A *a, unsigned char *s);

void Func_809a738(struct A *a)
{
    int ang;
    int m;
    int x;
    int y;

    m = Random() + (0x80 << 10);
    ang = a->f6;
    x = fx32_multiply(m, cos(ang));
    y = fx32_multiply(m, sin(ang));
    a->f8 = a->f8 + x;
    a->f10 = a->f10 + y;
    a->f6 = a->f6 + 0xfff0;
    if (a->f66 != 0) {
        a->f66 = a->f66 - 1;
        a->f6 = a->f6 + (0x80 << 4);
    } else if (((Random() << 5) >> 16) == 0) {
        a->f66 = ((Random() << 4) >> 16) + 8;
    }
    a->f64 = a->f64 + 1;
    if (a->f64 == 0x65)
        _Actor_SetScript(a, Data_9f0b0);
}
