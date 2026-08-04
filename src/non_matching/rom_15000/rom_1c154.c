/* Func_801c154 @ 0x0801c154
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s
 *
 * NOT SPLIT. The .s still holds both of its functions and the linker script is
 * untouched.
 *
 * Writes a 9-bit field into the halfword at +6, a byte at +4, and hands off.
 * Thirteen instructions against fifteen.
 *
 * PROGRESS ON narrow_constant, WHICH IS WORTH MORE THAN THE FUNCTION.
 *
 * src/non_matching/overlays/narrow_constant.c records that a named `int` mask
 * reproduces the ROM's 32-bit constant where a literal is narrowed to a byte or
 * halfword, and that the instruction ordering then "resists seven attempts".
 * This function adds the missing half of that.
 *
 * The ROM loads both masks as full words and REUSES ONE REGISTER for them:
 *
 *     ldr r3, =0x1ff / ldrh r4, [r0, #6] / and r1, r3
 *     ldr r3, =0xfffffe00 / and r3, r4 / orr r3, r1
 *
 * Two named mask variables give 32-bit `ldr` loads but need two registers, so
 * gcc pushes r5 and the function grows. ONE VARIABLE REASSIGNED between the two
 * uses gives both the 32-bit loads AND the register reuse:
 *
 *     m = 0x1ff;        v &= m;
 *     m = 0xfffffe00;   t &= m;
 *
 * That is thirteen instructions with the middle six exactly right, from
 * eighteen with two variables and eighteen with plain literals. If the
 * narrow_constant family shares this shape, the reassigned-single-variable form
 * is the thing to try there.
 *
 * TWO RESIDUES REMAIN.
 *
 * 1. r3 AND r4 ARE SWAPPED. The ROM puts the mask in r3 and the field in r4;
 *    gcc does the reverse. Both load the mask first, so it is not birth order --
 *    reordering the two source statements changes nothing at all. Note r4 is
 *    caller-saved in this tree (`-fcall-used-r4`), so gcc is free with it.
 *
 * 2. THE LITERAL POOL SITS IN A DIFFERENT PLACE, and this one may not be
 *    reachable from a single-function translation unit:
 *
 *        rom    bl Func_8003dec / b .L / <pool> / .L: pop {r0} / bx r0
 *        ours   bl Func_8003dec / pop {r0} / bx r0 / <pool>
 *
 *    gcc emits the branch-over-pool form when it has to dump the pool before
 *    the function ends. It DID emit exactly that form for the earlier
 *    narrowed-constant attempts in this same function, where the entries were
 *    halfwords -- so the placement is a consequence of what is in the pool and
 *    of what follows the function, not something the C controls directly.
 *
 *    The .s holds two functions. Converting BOTH into one .c, rather than
 *    splitting this one out alone, is the obvious next thing to try: the pool
 *    placement in the original depended on there being more code after this
 *    function, and a split destroys exactly that context. That has not been
 *    tried yet and is the reason this is parked rather than abandoned.
 *
 * TRIED:
 *   1. plain literals -- both masks narrowed, `ldrh` pool loads, 18 lines
 *   2. a named pointer to the +6 field -- costs r5 and an `add`, 18 lines
 *   3. two named u32 masks -- 32-bit loads, but two registers, 14 lines
 *   4. one reassigned u32 mask -- 13 lines, the form below
 *   5. declaring `t` before `m` and vice versa -- byte-identical to each other
 */
#include "gba/types.h"

struct S { u8 pad_00[4]; u8 f4; u8 pad_05; u16 f6; };

extern void Func_8003dec(struct S *p, s32 n);

void Func_801c154(struct S *p, u32 v, u32 b)
{
    u32 m;
    u32 t;

    m = 0x1ff;
    t = p->f6;
    v &= m;
    m = 0xfffffe00;
    t &= m;
    p->f6 = t | v;
    p->f4 = b;
    Func_8003dec(p, 0xfc);
}
