/* Cluster OvlFunc_947_200a0f0..OvlFunc_947_200a0f0 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1528_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7d0e88/overlay.ld is
 * unchanged.
 *
 * Installs an update hook, hands the actor's integer position to a six-argument
 * routine, sets a per-slot flag and swaps in a new behaviour script.
 *
 * A SIX-ARGUMENT CALL THAT IS NOT THE stack-arg BLOCKER. Two of the arguments
 * go on the stack:
 *
 *     str r2, [sp] / str r3, [sp, #4] / mov r2, #1 / mov r1, #0xe
 *     mov r0, #0x14 / mov r3, #1 / bl __Func_8010704
 *
 * and that reproduced without help. The stack-arg-pair class parked elsewhere
 * in the tree is about the ORDER the two stack slots are filled relative to the
 * registers; here the ROM fills both slots first and then the registers, which
 * is what gcc does anyway. Worth knowing the class is not "any function with
 * stack arguments".
 *
 * The one thing needed was a declaration for __Func_8010704: the ROM fills r0
 * before r3 and gcc filled it after. That is the plain fill-order kind -- the
 * misplaced `mov r0` sits outside any other argument's construction -- so the
 * lever reaches it. Two positions differed before, none after.
 *
 * `slot + 0x1f5` is built at runtime (`ldr r3, =0x1f5 / add r0, r6, r3`) rather
 * than folded, which falls out of the addend being a pooled constant added to a
 * parameter; no lever needed.
 *
 * The position values are `>> 20` on the 16.16 fields, discarding the fraction
 * and the low four integer bits -- a tile coordinate rather than a world one.
 */
#include "gba/types.h"
#include "actor.h"

extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_947_200a0b8(Actor *a);
extern unsigned char OvlData_947_200ad64[];

void OvlFunc_947_200a0f0(int slot)
{
    Actor *a;
    s32 x;
    s32 z;

    a = __MapActor_GetActor(slot);
    __CutsceneStart();
    a->update = OvlFunc_947_200a0b8;
    x = a->pos.x >> 20;
    z = a->pos.z >> 20;
    __Func_8010704(0x14, 0xe, 1, 1, x, z);
    __SetFlag(slot + 0x1f5);
    __MapActor_SetBehavior(slot, OvlData_947_200ad64);
    __CutsceneEnd();
}
