/* OvlFunc_881_20097fc -- NOT MATCHING. 39 differing of 45; ours 43 lines
 * against the ROM's 45 -- SHORTER.
 * ref: asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a.s
 *
 * TWO INDEPENDENT BLOCKERS, both needing a basic-block boundary this
 * straight-line function does not have:
 *
 * 1. The `-1` TRIPLE passed to __Func_80933f8.  The ROM builds three separate
 *    `mov #1` and three `neg`; gcc builds one and copies (`mov r0, r2 /
 *    mov r1, r2`), which is the two missing instructions.  Same shape as the
 *    triple in src/non_matching/ovl_787e04/20093e4.c.
 * 2. __MapActor_SetSpeed(8, 0x6666, 0x3333): the ROM issues `mov r0, #8`
 *    BEFORE the two pool loads, gcc issues it after -- the pool-loads-first
 *    class.  (The very next call, __Func_80921c4, has the same two pool loads
 *    with r0 LAST, and ours matches it exactly.)
 *
 * CONTROL: with the triple spelled -1/-2/-3 the function is 45 lines and
 * 5 of 45, of which 2 are the deliberately-changed constants -- so blocker 2
 * is worth exactly 3 positions and blocker 1 the rest.
 *
 * MEASURED on the control, all leaving blocker 2 at 3 positions: void/void,
 * void/int, int/void, int/int return types on __MapTransitionIn (the preceding
 * call) and __MapActor_SetSpeed, plus deleting either declaration entirely.
 * int on __MapTransitionIn is 7 (worse).
 */
extern unsigned char *__MapActor_GetActor(int);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int, int, int, int);
extern void __WaitFrames(int);
extern void __MapActor_SetPos(int, int, int);
extern void __SetCameraTarget(int, int);
extern void __MapTransitionIn(void);
extern void __MapActor_SetSpeed(int, int, int);
extern void __Func_80921c4(int, int, int);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int);
extern void __Func_8091e9c(int);
extern void __CutsceneEnd(void);

void OvlFunc_881_20097fc(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    *(int *)(a + 0x1c) = 0xa0 << 9;
    *(int *)(a + 0x18) = 0xa0 << 9;
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
