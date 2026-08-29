/* Func_80c00d8 @ 0x080c00d8
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bffb8_a_a_a_c.s
 *
 * NOT SPLIT. The .s still holds its remaining functions and the linker script
 * is untouched.
 *
 * Sets up a screen's worth of tile data: clear 0x100 bytes, fill 0x80 bytes
 * with 0x03ff03ff, write a 240-word ramp of packed tile indices, then fill
 * another 0x280 bytes. Its neighbour Func_80c0098 in the same original .s IS
 * elevated (src/rom_b5000/rom_bffb8_a_a_a_b.c) and the ramp loop here uses the
 * same literal-in-the-body lever that one needed.
 *
 * Blocker: CONSTANT-CSE, twice over, 33 instructions against 34.
 *
 * 1. THE VALUE 0x100 IS BUILT TWICE IN THE ROM and once by gcc. It is the size
 *    argument of the first call and also the amount the pointer advances:
 *
 *        rom    mov r1, #0x80 / lsl r1, #1   ... mov r3, #0x80 / lsl r3, #1 / add r6, r3
 *        ours   mov r5, #0x80 / lsl r5, #1   ... mov r1, r5 / add r7, r5
 *
 *    gcc computes it once into a callee-saved register and uses it for both,
 *    which costs it an extra live value and pushes r7.
 *
 * 2. THE FUNCTION POINTER IS RE-MATERIALISED for the last call. The ROM holds
 *    Func_80008d8 in r5 across the first two calls, then loads it AGAIN into r3
 *    for the fourth:
 *
 *        rom    ldr r5, =Func_80008d8 ... bl _call_via_r5 ... bl _call_via_r5
 *               ... ldr r3, =Func_80008d8 / bl _call_via_r3
 *
 *    r5 is callee-saved and nothing in the loop clobbers it, so gcc reuses it
 *    and never emits the second load.
 *
 * Both are the same defect in the same direction, and it is the direction worth
 * restating because a note in this tree once had it backwards: THE ROM'S
 * REDUNDANT FORM IS THE TARGET. gcc's common-subexpression elimination is what
 * has to be defeated. The ROM is not doing anything clever; it is rebuilding a
 * cheap value rather than keeping a register tied up, and gcc is too eager to
 * reproduce it.
 *
 * TRIED, all giving byte-identical output to each other (33 lines, same
 * registers, first diff at instruction 0):
 *   1. the pointer advance as `p += 64` on a u32* (a different source constant
 *      from the size argument's `0x80 << 1`, but the same machine value)
 *   2. the advance as `p += 0x80 >> 1`
 *   3. two separate function-pointer variables, fp and fp2
 *   4. fp2 declared in an inner scope to shorten its live range so it would
 *      land in a scratch register rather than a callee-saved one
 *
 * (3) and (4) are the trick that DID work for the two script pointers in
 * OvlFunc_945_200d068 -- separate variables there moved the first divergence by
 * thirty instructions. It does nothing here, which is the useful negative:
 * separate variables defeat gcc's reuse of a value it COMPUTED, not of a
 * constant it can rematerialise.
 *
 * This is the same wall as src/non_matching/ovl_7f2f14/20087d8.c, where the ROM
 * materialises -1 three times. Nothing in the tree defeats it yet.
 */
#include "gba/types.h"

extern void Func_80008d8(void *dst, u32 size, u32 value);

void Func_80c00d8(u32 *p)
{
    void (*fp)(void *, u32, u32);
    u32 v, i;

    fp = Func_80008d8;
    fp(p, 0x80 << 1, -1);
    p += 64;
    fp(p, 0x80, 0x3ff03ff);
    p += 32;
    v = 0x2010200;
    i = 0;
    do { i++; *p++ = v; v += 0x20002; } while (i <= 0xef);
    {
        void (*fp2)(void *, u32, u32) = Func_80008d8;
        fp2(p, 0xa0 << 2, 0x3ff03ff);
    }
}
