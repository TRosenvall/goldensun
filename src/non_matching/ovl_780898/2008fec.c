/* OvlFunc_883_2008fec -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_a.s
 *
 * BLOCKER: ARGUMENT FILL ORDER. 9 of 91, LENGTH EXACT, and it is a SECOND
 * INDEPENDENT CONFIRMATION of the argument-temporary boundary recorded in
 * docs/elevation.md under "the split-constant interleave is reachable".
 *
 * 82 of 91 lines are exact: all nineteen calls, both actor fetches, the ten
 * field copies and stores, the message branch, the guarded TravelTo with its
 * two register-offset ldrsh loads, and the interwork epilogue. The residue is
 * two clusters, both of them argument setup for two adjacent calls.
 *
 * CLUSTER A -- __MapActor_SetSpeed(5, 0x80 << 9, 0x80 << 8):
 *
 *     rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#0x5 / lsl r1,#9 / lsl r2,#8
 *     ours  mov r1,#0x80 / mov r2,#0x80 / lsl r1,#9  / lsl r2,#8  / mov r0,#0x5
 *
 * The ROM batches the two `mov`s, drops the third argument in, then batches
 * the two `lsl`s. This is the split-constant interleave in its two-constant
 * form.
 *
 * CLUSTER B -- __Func_80921c4(5, 0x6e, 0x11b):
 *
 *     rom   mov r0,#0x5 / mov r1,#0x6e / ldr r2,=0x11b
 *     ours  ldr r2,=0x11b / mov r0,#0x5 / mov r1,#0x6e
 *
 * Plain fill order: gcc hoists the pool load ahead of the two immediates.
 * 0x11b is 283, so it cannot be a `mov` -- the ROM pool-loads it too, and only
 * the position differs.
 *
 * WHY THIS CONFIRMS THE BOUNDARY. The statement-split lever that landed
 * Func_80b9a70 works only on values that OUTLIVE their construction; gcc
 * rematerialises argument temporaries during argument fill and discards the
 * source's statement structure. Every value in both clusters dies at its call.
 * Predicted unreachable by naming -- and measured:
 *
 *   baseline                                                    9 differ
 *   cluster A args named as locals, slot assigned in the gap    9 differ
 *   cluster B args named as locals in the ROM's order           9 differ
 *   both sites named                                            9 differ
 *
 * ALL FOUR ARE BYTE-IDENTICAL. Three spellings that look materially different
 * in source produce one output, which is what "rematerialised at fill" means
 * in practice. The first confirmation was ovl_7cb2c0/200dca4.c; this is a
 * second, on a different overlay, a different callee and a two-constant rather
 * than one-constant form.
 *
 * PASS DIAGNOSTICS, run to locate the decision rather than to change the build:
 *   --no-sched2      17 differ, first divergence moves 29 -> 24   (WORSE)
 *   --no-rerun-cse    9 differ, unchanged
 *
 * The sched2 result is the useful one and it matches what ovl_7cb2c0/200dca4.c
 * found: post-reload scheduling is what makes the OTHER seventeen call sites in
 * this function come out right. The fill order is not sched2's doing and
 * turning it off only breaks what already works. Neither flag is a candidate
 * for the build.
 *
 * NOT TRIED, and the only directions left: register pinning, which would make
 * this a fakematch (see ovl_7fb4a8/20087b0.c for the same judgement call), or
 * a source form that gives these constants a life beyond the call -- which the
 * ROM gives no evidence for and which would be inventing code to fit output.
 */
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_883_2008fec(void)
{
    char *a;
    char *b;
    char *p;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(5);
    __CutsceneStart();
    *(int *)(b + 8) = *(int *)(a + 8);
    *(int *)(b + 0xc) = *(int *)(a + 0xc);
    *(int *)(b + 0x10) = *(int *)(a + 0x10);
    *(int *)(b + 0x38) = 0x80 << 24;
    *(int *)(b + 0x3c) = 0x80 << 24;
    *(int *)(b + 0x40) = 0x80 << 24;
    *(int *)(b + 0x24) = 0;
    *(int *)(b + 0x28) = 0;
    *(int *)(b + 0x2c) = 0;
    *(int *)(b + 0x14) = *(int *)(a + 0xc);
    __WaitFrames(1);
    __MapActor_SetSpeed(5, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(5, 0x6e, 0x11b);
    __Func_8092848(0, 5, 2);
    __MessageID(0xf39);
    if (*(int *)(a + 8) < *(int *)(b + 8))
        __Func_8093040(0xa005, 0, 2);
    else
        __Func_8093040(0x8005, 0, 2);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(2);
    __MapActor_SetAnim(5, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(5);
    __MapActor_SetPos(5, 0, 0);
    __Func_80921c4(0, 0x6e, 0x12f);
    __CutsceneEnd();
}
