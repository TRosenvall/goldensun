/* Cluster OvlFunc_898_2008938..OvlFunc_898_2008938 extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_a_c_a_c_c_c_a_c_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_793768/overlay.ld is
 * unchanged.
 *
 * INSTRUCTION-FOR-INSTRUCTION IDENTICAL to
 * src/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_a_a.c in another overlay --
 * twenty-eight instructions, same registers, same constants, same callees.
 * Only the function name differs. The C is that file's C with the name changed
 * and it matched on the first screen.
 *
 * Read that header for the three things this needs, all of which carry over
 * unchanged:
 *
 *   * the ROM caches a zero in r8 across four calls, spending a push and a pop
 *     to do it, so the final store has to come from a NAMED LOCAL rather than a
 *     literal -- the constant-CSE question running the other way round;
 *   * __MapActor_SetAnim and __ActorMessage both need declaring, the plain
 *     fill-order kind where the misplaced `mov r0` sits outside any other
 *     argument's construction;
 *   * the pointer must be built BEFORE the zero is assigned, or the first
 *     divergence moves from instruction 12 to instruction 7.
 *
 * Two overlays shipping a byte-identical helper is worth noting for the naming
 * pass: whatever this is, it is generic enough that two areas' code included
 * the same source.
 */
#include "gba/types.h"
#include "actor.h"

extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_898_2008938(int slot)
{
    Actor *a;
    u8 *p;
    u8 v;

    a = __MapActor_GetActor(slot);
    p = (u8 *)a + 0x5b;
    v = 0;
    *p = 1;
    __CutsceneStart();
    __MapActor_SetAnim(slot, 1);
    __CutsceneWait(2);
    __ActorMessage(slot, 0);
    __CutsceneEnd();
    *p = v;
}
