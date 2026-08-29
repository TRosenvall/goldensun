/* Cluster OvlFunc_968_2008754..OvlFunc_968_2008754 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f2f14/ovl_30_a_a_c_a_a.o and asm/overlays/rom_7f2f14/ovl_30_a_a_c_a_c.o in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * THE THIRD TWIN of the map-exit cutscene: install an update hook on slot 0,
 * set its speed, walk it, clear the hook, fade out and hand off. The other two
 * are OvlFunc_923_2008ed0 (rom_7aa430) and OvlFunc_924_2008f84 (rom_7ac2d8).
 *
 * The body is character-for-character the body of the -O2 twin
 * src/overlays/rom_7ac2d8/ovl_f84_a_a_b.c, including which callees are left
 * IMPLICITLY DECLARED. __Func_809228c and __Func_8092950 have no extern here
 * on purpose: an implicitly declared callee returns int, so gcc keeps r0 live
 * and fills the next call's r0 last, which is the order the ROM uses. Declare
 * either one and the r0/r1 fill order inverts. See docs/elevation.md.
 *
 * Two things differ from the twins, both at the ends of the function.
 *
 *  1. The twins take the final argument as a parameter. This one READS IT, and
 *     reads it EARLY -- the ROM loads r6 from iwram_3001ebc before the first
 *     call, then holds it in a callee-saved register across fifteen calls:
 *
 *         ldr r3, =iwram_3001ebc / mov r0, #0 / ldr r6, [r3] / bl __MapActor_GetActor
 *
 *     So the load has to be its own statement at the top of the body rather
 *     than an expression at the point of use, or gcc sinks it to the bottom.
 *
 *  2. The tail builds the 0x16c offset AT RUNTIME rather than folding it into
 *     the addressing mode:
 *
 *         mov r3, #0xb6 / lsl r3, #1 / add r6, r3 / mov r3, #0 / ldrsh r0, [r6, r3]
 *
 *     That is the statement-form lever -- writing the address arithmetic as
 *     separate statements over a typed base reproduces the runtime add, where
 *     any folded spelling (`*(short *)(base + 0x16c)`) emits a single
 *     `ldrsh r0, [r6, #...]`-style access with the constant folded in. Same
 *     shape as GetEntrances in src/overlays/rom_79aad8/ovl_314_a.c.
 *
 * Matched on the first screen, 46 lines against 46.
 */
#include "actor.h"

extern unsigned int iwram_3001ebc;

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
extern void OvlFunc_968_20086a0(void);

void OvlFunc_968_2008754(void)
{
    Actor *a;
    unsigned int base;
    unsigned int off;

    base = iwram_3001ebc;
    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __PlaySound(0xe4);
    a->update = OvlFunc_968_20086a0;
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
    off = 0xb6;
    off <<= 1;
    base += off;
    off = 0;
    __Func_8091e9c(*(short *)((char *)base + off));
    __CutsceneEnd();
}
