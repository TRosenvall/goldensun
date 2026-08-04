/* OvlFunc_968_20087d8  [ovl_7f2f14]  --  0x020087d8
 *
 * Source asm: goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_c_a_c.s
 *
 * The .s holds ONLY this function and no data, so it converts directly with no
 * split -- the .s is still in place and the linker script is untouched.
 *
 * The map-ENTRY cutscene, counterpart to the map-exit twin that matched as
 * src/overlays/rom_7f2f14/ovl_30_a_a_c_a_b.c. Behind a one-shot flag guard:
 * fade in, place the actor, walk it, mark it done. Twenty calls.
 *
 * Blocker: CONSTANT-CSE. Seventy lines against seventy-two, and the two
 * missing lines are the whole diff. The ROM materialises -1 THREE TIMES:
 *
 *     rom    mov r0, #0x1 / mov r1, #0x1 / mov r2, #0x1 / mov r3, #0x0
 *            neg r0, r0   / neg r1, r1   / neg r2, r2
 *
 *     ours   mov r2, #0x1 / neg r2, r2 / mov r0, r2 / mov r1, r2 / mov r3, #0x0
 *
 * gcc builds the value once and copies it into the other two argument
 * registers. Note which side is which: THE ROM'S REDUNDANT FORM IS THE TARGET
 * and gcc's common-subexpression elimination is what has to be defeated. The
 * ROM is not doing something clever here; it is doing the same cheap thing
 * three times, and gcc is too smart to reproduce it.
 *
 * Not the -O1 class. Screened both ways:
 *
 *     tools/tryc.py scratch/cand2.c --ref .../ovl_30_a_a_c_a_c.s        -> 70 vs 72
 *     tools/tryc.py scratch/cand2.c --ref .../ovl_30_a_a_c_a_c.s --O1   -> 70 vs 72
 *
 * -O1 moves the `mov r7, r5` / `add r7, #0x55` pair into the right place and
 * shortens the diff, but the CSE survives it unchanged, so a per-file -O1 rule
 * would not buy the match. Everything after the __Func_80933f8 call agrees.
 *
 * WHAT IS ALREADY SOLVED HERE, and worth keeping if this is picked back up:
 *
 *  1. `p = &a->interactFlag` as its own variable reproduces the ROM holding
 *     r7 = r5 + 0x55 across the whole body rather than recomputing it at each
 *     of the two stores.
 *  2. The two zero stores are spelled `*p = flag` and `a->update = flag`, not
 *     `= 0`. The ROM writes r6 -- the __GetFlag RESULT -- into both, because
 *     the `cmp r6, #0 / bne` above proved r6 is zero on this path. Spelling
 *     them as literal zeros emits a separate `mov r3, #0` and costs two more
 *     lines. This one gcc DOES reproduce, which is the interesting half: it
 *     will happily reuse a value the compare established, and will not
 *     duplicate a constant it can copy. Both behaviours push the same
 *     direction, and here only one of them is what the ROM did.
 *  3. The integer position halves at 0x0A and 0x12 are the high halfwords of
 *     pos.x and pos.z, reached as `((short *)&a->pos.x)[1]`. Thumb has no
 *     immediate-offset `ldrsh`, so the `mov r3, #0x12 / ldrsh r2, [r5, r3]`
 *     register-offset form is automatic and needs no lever.
 *
 * What would match is an unanswered question, and it is the same one behind
 * the rest of the constant-CSE class: what makes gcc-2.96 materialise the same
 * small constant twice instead of copying it. Nothing in the tree defeats it
 * yet.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int slot, fx32 x, fx32 z);
extern void __MapActor_SetSpeed(int slot, fx32 a, fx32 b);
extern void __Actor_SetSpriteFlags(Actor *a, int f);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_809202c(void);
extern void OvlFunc_968_20086a0(void);

void OvlFunc_968_20087d8(void)
{
    Actor *a;
    int flag;
    unsigned char *p;

    a = __MapActor_GetActor(0);
    flag = __GetFlag(0x109);
    if (flag != 0)
        return;
    __CutsceneStart();
    p = &a->interactFlag;
    __Func_80933f8(-1, -1, -1, 0);
    *p = flag;
    __MapActor_SetPos(0, ((short *)&a->pos.x)[1] << 16,
                      (((short *)&a->pos.z)[1] << 16) + 0xfff00000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xe4);
    a->update = OvlFunc_968_20086a0;
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __Func_8092304(0, 0, 8);
    __Func_8092950(0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __Func_8092304(0, 0, 8);
    *p = 3;
    a->update = (actorfun_t)flag;
    __Func_809202c();
    __CutsceneEnd();
}
