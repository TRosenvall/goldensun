/* Cluster OvlFunc_924_2008f84..OvlFunc_924_2008f84 extracted from goldensun/asm/overlays/rom_7ac2d8/ovl_f84_a_a.s.
 *
 * Split out of that .s; the sibling part stays as assembly.
 *
 * A map-exit cutscene: install an update hook on slot 0, set its speed, walk
 * it, clear the hook, fade out and hand off. Fourteen calls in forty-one
 * instructions.
 *
 * THIS FUNCTION IS THE -O1 CONTROL. Its twin OvlFunc_923_2008ed0 in
 * rom_7aa430 is the same forty-one instructions and is PARKED: the same C
 * fails there on one argument pair, and the declaration lever -- which decides
 * whether a callee's r0 is filled first or last -- does not move it in either
 * direction.
 *
 * The difference between the two is the optimisation level. rom_7aa430's TU
 * builds at -O1 under the Makefile rule `ovl_e90_c_c_a_a%.o`; this one builds
 * at -O2. The park note flagged -O1 as the suspect and said to test it on a
 * function that already matches rather than assuming it. This is that test,
 * and it confirms it: identical C, matches at -O2, fails at -O1.
 *
 * So the declaration lever is an -O2 behaviour. Nine batches of functions have
 * been decided by it and every one of them was -O2; this is the first time it
 * has been asked about -O1 code, and the answer is that it does not apply.
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
extern void OvlFunc_924_2008cd0(void);

void OvlFunc_924_2008f84(int arg)
{
    Actor *a;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __PlaySound(0xe4);
    a->update = OvlFunc_924_2008cd0;
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
