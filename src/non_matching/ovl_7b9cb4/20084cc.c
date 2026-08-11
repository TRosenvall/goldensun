/* OvlFunc_932_20084cc  [ovl_7b9cb4]
 *
 * Source asm: goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function.
 *
 * Twenty-seven instructions against twenty-seven, TWENTY-THREE IDENTICAL, and
 * the four that differ are the order two stack arguments are built and stored.
 *
 * Blocker: STACK-ARG-PAIR. The ROM materialises BOTH stack values into separate
 * registers before storing either, with the actor copy wedged between:
 *
 *     rom    mov r3, #0x11 / mov r2, #0xd / mov r5, r0
 *            str r3, [sp]  / str r2, [sp, #0x4]
 *
 *     ours   mov r3, #0x11 / str r3, [sp]
 *            mov r3, #0xd  / str r3, [sp, #0x4] / mov r5, r0
 *
 * gcc reuses one register and interleaves each build with its own store. Same
 * instruction count, four positions apart.
 *
 * THIS IS THE CLASS PROPER, and it is worth contrasting with the two functions
 * that merely LOOK like it. src/overlays/rom_7d0e88/ovl_1528_c_a.c and
 * src/overlays/rom_7e0928/ovl_30_a_c_c_c.c both pass arguments on the stack and
 * both matched with no lever, because in those the ROM fills the slots first
 * and then the registers -- which is what gcc does anyway. The blocker is not
 * "has stack arguments"; it is specifically the ROM keeping two slot values
 * live simultaneously where gcc serialises them.
 *
 * TRIED:
 *   1. the two values as literals at the call site (the form below) -- 4 differ,
 *      the closest
 *   2. the two values as named s32 locals assigned before __MapActor_GetActor,
 *      to force them live together -- 13 differ; gcc hoists both materialisations
 *      above the call instead, which is further away
 *
 * (2) is the obvious idea and it overshoots: making the values live EARLIER is
 * not the same as making them live SIMULTANEOUSLY, and gcc takes the first
 * reading.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_20084cc(void)
{
    Actor *a;

    a = __MapActor_GetActor(9);
    __Func_8010704(0x1d, 1, 3, 1, 0x11, 0xd);
    if (a != 0)
        *((u8 *)a + 0x55) = 2;
    __SetFlag(0x201);
}
