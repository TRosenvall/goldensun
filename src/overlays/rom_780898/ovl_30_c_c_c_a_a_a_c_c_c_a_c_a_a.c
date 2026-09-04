// fakematch
/* OvlFunc_883_2008fec  --  0x02008fec
 *
 * From goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_a.s,
 * which held this function alone, so no split was needed.
 *
 * PARKED AT 9 OF 91, LENGTH EXACT, in three clusters and all of them argument
 * setup. 82 of 91 lines were already right -- all nineteen calls, both actor
 * fetches, the ten field copies, the message branch, the guarded TravelTo with
 * its two register-offset ldrsh loads, and the interwork epilogue.
 *
 * TWO SHAPES, BOTH ALREADY ON FILE:
 *
 *   1. THE SPLIT-CONSTANT INTERLEAVE, at __MapActor_SetSpeed:
 *
 *        rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#5 / lsl r1,#9 / lsl r2,#8
 *
 *      The ROM batches the two movs, drops the third argument in, then batches
 *      the two shifts. Pinning the three argument registers places it: 9 -> 6.
 *
 *   2. THE PRECOMPUTE BIND, at both __Func_80921c4 calls:
 *
 *        rom   mov r0, #5 / mov r1, #0x6e / ldr r2, =0x11b
 *        ours  ldr r2, =0x11b / mov r0, #5 / mov r1, #0x6e
 *
 *      The pool load exceeds the rtx_cost threshold in
 *      precompute_register_parameters, so gcc computes it before the two cheap
 *      movs; the ROM issues it last. Pinning r0 and r1 -- not r2 -- puts the
 *      cheap arguments first and leaves the pool load where it falls: 6 -> 0.
 *      Same fix as src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c.
 *
 * THE PARK NAMED PINNING AS THE ONLY REMAINING DIRECTION and deferred it as a
 * judgement call, since it makes the file a fakematch. Taking it is consistent
 * with this tree's established practice -- fakematch.txt is how that judgement
 * has been recorded for many batches -- and the park's own alternative, "a
 * source form that gives these constants a life beyond the call", it rejected
 * itself as inventing code to fit output. That reasoning was right and is why
 * the pin is the correct call rather than a shortcut.
 *
 * The park's pass diagnostics stand and were not re-run: --no-sched2 is WORSE
 * at 17, because post-reload scheduling is what makes the other seventeen call
 * sites come out right, and --no-rerun-cse is unchanged. Neither is a candidate
 * for the build.
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
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 5;
        q1 <<= 9;
        q2 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q0 = 5;
        q1 = 0x6e;
        __Func_80921c4(q0, q1, 0x11b);
    }
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
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q0 = 0;
        q1 = 0x6e;
        __Func_80921c4(q0, q1, 0x12f);
    }
    __CutsceneEnd();
}
