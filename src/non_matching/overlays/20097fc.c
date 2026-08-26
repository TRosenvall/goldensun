/* OvlFunc_881_20097fc  --  0x020097fc, asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a.s
 *
 * BLOCKER CLASS: constant rematerialisation, the `-1` case the docs named after
 * OvlFunc_945_200c13c. 43 lines against 45.
 *
 *     rom    mov r0, #1 / mov r1, #1 / mov r2, #1 / mov r3, #0 /
 *            neg r1, r1 / neg r2, r2 / neg r0, r0
 *     ours   mov r2, #1 / neg r2, r2 / mov r0, r2 / mov r1, r2 / mov r3, #0
 *
 * `__Func_80933f8(-1, -1, -1, 0)` passes the same value in three registers. The
 * ROM builds it three times; gcc builds it once and copies, two instructions
 * shorter. Nothing in the source distinguishes three occurrences of one value.
 *
 * THIS COST A SCREEN AND THE TOOL NOW CATCHES IT. `pick_candidates.py` gained a
 * repeated-built-constant filter earlier in batch 85, but its first version
 * required the `mov` and the `neg` to be ADJACENT. gcc emits all of a call's
 * `mov`s and then all its `neg`s, so the separated form slipped through and
 * this function was offered anyway. The filter now matches each `neg rN, rN`
 * back to the `mov rN, #imm` that last set that register, and reports this one
 * as `[repeats -1]`.
 *
 * The rest of the reading is believed correct: the two stores at +0x1c and
 * +0x18 take the same value and are written in that order, and the camera
 * target, transitions and save bit are all straightforward.
 */
struct A { unsigned char pad00[0x18]; int f18; int f1c; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __SetCameraTarget(int slot, int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_881_20097fc(void)
{
    struct A *a;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    a->f1c = 0xa0 << 9;
    a->f18 = 0xa0 << 9;
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x6666, 0x3333);
    __Func_80921c4(8, 0x14a8, 0x918);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x66);
    __CutsceneEnd();
}
