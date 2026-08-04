/* OvlFunc_923_2008ed0  [ovl_7aa430]  --  0x02008ed0
 *
 * Source asm: goldensun/asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_c.s
 *
 * NOTE THIS TU IS BUILT AT -O1. The Makefile rule
 * `asm/overlays/rom_7aa430/ovl_e90_c_c_a_a%.o` covers it, and tools/tryc.py
 * picks that up from the --ref path. A screen at -O2 would be meaningless.
 *
 * A map-exit cutscene: install an update hook on slot 0, set its speed, walk
 * it, clear the hook, fade out and hand off. Forty-one instructions against
 * forty-one, forty of them identical.
 *
 * Blocker: ARGUMENT FILL ORDER on one call. The ROM fills r1 before r0 for
 * __Func_809228c; gcc does the reverse:
 *
 *     rom    mov r2,#6 / neg r2,r2 / mov r1,#0 / mov r0,#0 / bl __Func_809228c
 *     ours   mov r2,#6 / neg r2,r2 / mov r0,#0 / mov r1,#0 / bl __Func_809228c
 *
 * THE DECLARATION LEVER DOES NOT REACH IT HERE, which is the point worth
 * recording. That lever -- an implicitly declared callee returns int, so gcc
 * keeps r0 live and fills the next call's r0 last -- retired the
 * arg-fill-order class in batch 07 and has decided a dozen functions since.
 * Both directions were tried:
 *
 *   1. __Func_809228c left implicit (the form below)
 *   2. __Func_809228c declared
 *   3. the PRECEDING call, __MapActor_SetAnim, made implicit
 *
 * All three produce r0 first. The obvious suspect is -O1: the lever was found
 * and has only ever been exercised at -O2, and this is the first -O1 function
 * it has been asked to decide. Worth testing that hypothesis directly on a
 * function that already matches, rather than assuming it here.
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
