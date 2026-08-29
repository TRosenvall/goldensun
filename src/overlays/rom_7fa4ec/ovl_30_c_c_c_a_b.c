/* Overlay 970: record slot 0's height for later comparison.
 *
 * Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four
 * near-identical stubs differing only in the slot and the destination.
 *
 * The destination pointer is taken BEFORE the call on purpose. Reading the
 * global after the call lets gcc use a call-clobbered register; the ROM holds
 * the address across the call in a callee-saved one, which only happens if the
 * pointer is live over it.
 *
 * The stored field is pos.y -- +0x0C, the middle word of the position triple.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern int Data_180c __asm__(".L180c");

int OvlFunc_970_20083c0(void)
{
    int *dest = &Data_180c;
    Actor *actor = __MapActor_GetActor(0);

    *dest = actor->pos.y;
    return 0;
}
