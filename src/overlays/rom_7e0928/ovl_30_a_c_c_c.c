/* Cluster OvlFunc_956_200858c..OvlFunc_956_200858c extracted from goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7e0928/overlay.ld is
 * unchanged.
 *
 * Records an actor's tile column in a flag byte, then makes two six-argument
 * calls that differ in four of the six.
 *
 * Matched on the first screen with no lever, which is worth a line because the
 * shape looks like two blockers and is neither:
 *
 *   * TWO STACK ARGUMENTS PER CALL, filled before the register ones. That is
 *     what gcc does anyway -- the stack-arg class parked elsewhere is about the
 *     ORDER of the slots relative to the registers, not about their presence.
 *     Same finding as src/overlays/rom_7d0e88/ovl_1528_c_a.c.
 *   * 0x12 MATERIALISED TWICE, once into the stack slot and once into r0.
 *     Both are inside one argument block with no call between, so gcc builds it
 *     twice exactly as the ROM does. Constant-CSE only bites across a call.
 *
 * The value 0xb is shared between the two calls' second stack slots and IS a
 * named local, because it survives the first call -- that one is across a call,
 * and a literal would be rebuilt.
 *
 * `>> 20` on the 16.16 x coordinate discards the fraction and the low four
 * integer bits, giving a tile column rather than a world position.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __SetFlagByte(int id, int v);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_200858c(void)
{
    Actor *a;
    s32 x;
    s32 n;

    a = __MapActor_GetActor(0xd);
    x = a->pos.x >> 20;
    __SetFlagByte(0xdc << 2, x);
    n = 0xb;
    __Func_8010704(0x12, 0xa, 3, 1, 0x12, n);
    __Func_8010704(0x11, 0xb, 1, 1, x, n);
}
