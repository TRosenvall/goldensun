// fakematch
/* OvlFunc_881_2009b5c  --  0x02009b5c
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a.s.
 *
 * A map-entry cutscene: blank the camera, park the player at the origin, place
 * actor 8 and give it a movement rate, fade in, walk it to a mark, fade out,
 * then set flag 0x93e, clear 0x927 and hand off.
 *
 * FAKEMATCH, and it is the function that disproved a park. Two pieces:
 *
 *   pin r0/r1/r2 at __MapActor_SetSpeed        6 differing -> 3
 *   interleave each `-1` assignment with its
 *   own negation                               3 -> 0
 *
 * THE INTERLEAVED FORM IS THE POINT. The `-1` triple is written
 *
 *     p0 = 1; p0 = -p0;
 *     p1 = 1; p1 = -p1;
 *     p2 = 1; p2 = -p2;
 *     p3 = 0;
 *
 * rather than the three assignments followed by the three negations. Grouped,
 * gcc emits the `mov`s in its own order -- here r1, r2, r0 against the ROM's
 * r0, r1, r2 -- and no permutation of either group reaches it, because the
 * three registers receive the SAME value and nothing orders them. Interleaved,
 * each `mov` is pinned in place by the negation that immediately consumes it.
 *
 * That is what the sibling OvlFunc_881_200b2f0 was PARKED for, on six spellings
 * tying at 4 of 99. All six were permutations inside the grouped shape; the
 * interleaved form takes that function to zero as well, and its park has been
 * withdrawn. See the notebook entry "N spellings tie is only evidence if the
 * spellings differ in STRUCTURE".
 *
 * Note the ROM negates in the order r1, r2, r0 here and r2, r1, r0 in the
 * sibling, while both emit their `mov`s r0, r1, r2. Interleaving reproduces
 * both, because it makes the negation order follow the assignment order rather
 * than being a separate degree of freedom -- so the two functions need
 * different negation orders in the source and the same shape.
 */

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8091e9c(int n);

void OvlFunc_881_2009b5c(void)
{
    unsigned char *a;
    int v;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        register int p2 __asm__("r2");
        register int p3 __asm__("r3");
        p0 = 1;
        p0 = -p0;
        p1 = 1;
        p1 = -p1;
        p2 = 1;
        p2 = -p2;
        p3 = 0;
        __Func_80933f8(p0, p1, p2, p3);
    }
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x13e80000, 0x9180000);
    v = 0xa0 << 9;
    *(int *)(a + 0x1c) = v;
    *(int *)(a + 0x18) = v;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    {
        register int q0 __asm__("r0") = 8;
        register int q1 __asm__("r1") = 0x6666;
        register int q2 __asm__("r2") = 0x3333;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __Func_80921c4(8, 0x13c8, 0x918);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x93e);
    __ClearFlag(0x927);
    __Func_8091e9c(0x6b);
    __CutsceneEnd();
}
