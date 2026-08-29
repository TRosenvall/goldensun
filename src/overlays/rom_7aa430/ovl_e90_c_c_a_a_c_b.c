/* Cluster OvlFunc_923_2008ed0..OvlFunc_923_2008ed0 extracted from goldensun/asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_c.s.
 *
 * Slotted between ovl_e90_c_c_a_a_c_a.o and the rest of the overlay.
 *
 * A map-exit cutscene: install an update hook on slot 0, set its speed, walk
 * it, clear the hook, fade out and hand off.
 *
 * PREVIOUSLY PARKED, AND THE PARK WAS WRONG. This C is unchanged from the note
 * in src/non_matching/ovl_7aa430/2008ed0.c; only the compiler flags changed.
 * The Makefile rule `ovl_e90_c_c_a_a%` was written for a neighbouring TU and
 * captured this one by name prefix, so it was screened -- correctly, against
 * the Makefile -- at -O1, where it stands at 6 of 41 with r0 filled before r1.
 * At -O2 it matches.
 *
 * The park had the answer in it: it recorded that the twin
 * OvlFunc_924_2008f84 is the same 41 instructions, builds at -O2, and matches
 * with the IDENTICAL C. That was read as proof the difference is -O1. It is
 * the opposite -- two functions that match on the same C are the same TU
 * shape, and the flag rule was the thing out of place.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(Actor *a, int f);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int a);
extern void OvlFunc_923_2008cc0(void);

void OvlFunc_923_2008ed0(int arg)
{
    Actor *a;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __PlaySound(0xe4);
    a->update = OvlFunc_923_2008cc0;
    a->speed = 0x3333;
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -6);
    __MapActor_WaitMovement(0);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    a->update = 0;
    __CutsceneWait(0x1e);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(arg);
    __CutsceneEnd();
}
