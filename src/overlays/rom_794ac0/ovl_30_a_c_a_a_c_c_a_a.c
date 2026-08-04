/* Cluster OvlFunc_899_2008378..OvlFunc_899_2008378 extracted from goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_794ac0/overlay.ld is
 * unchanged.
 *
 * Sets a per-actor byte while a short cutscene plays, then clears it.
 *
 * THE ROM CACHES A ZERO ACROSS FOUR CALLS, and that is the interesting part
 * because it runs OPPOSITE to the constant-CSE class:
 *
 *     mov r3, #0 / ... / mov r8, r3 / ... four calls ... / mov r3, r8 / strb r3, [r5]
 *
 * It spends a `push {r6}` / `pop {r3}` on saving r8 in order to keep the value
 * 0 alive, where gcc would simply emit `mov r3, #0` again at the end. Here the
 * ROM is the one doing more caching, not less.
 *
 * The fix is the ordinary named-variable lever: a `u8 v = 0;` assigned before
 * the cutscene and stored after it is live across the calls, so gcc must give
 * it a callee-saved register. Writing the final store as a literal `*p = 0`
 * loses the r8 traffic entirely and is four instructions short.
 *
 * Worth pairing with src/non_matching/ovl_794ac0/200852c.c, parked in the same
 * overlay: there gcc caches a constant the ROM rebuilds, here the ROM caches a
 * constant gcc would rebuild. Both are the same question -- who keeps what in a
 * register across a call -- and the answer has to be read off the ROM each
 * time rather than assumed in either direction.
 *
 * TWO FILL-ORDER FIXES, both the plain kind. __MapActor_SetAnim and
 * __ActorMessage each want r0 filled first and gcc filled it last; declaring
 * both matches. Note the misplaced `mov r0` is OUTSIDE the other argument's
 * setup in both cases, which is the tell that separates this from
 * arg-interleave -- see src/non_matching/ovl_794ac0/2008428.c, where it sits
 * INSIDE and no declaration reaches it.
 *
 * Statement order is load-bearing: the pointer must be built BEFORE the zero is
 * assigned. Reversed, gcc emits `mov r8, r3` before `add r5, #0x5b` and the
 * first divergence moves from instruction 12 to instruction 7.
 */
#include "gba/types.h"
#include "actor.h"

extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_899_2008378(int slot)
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
