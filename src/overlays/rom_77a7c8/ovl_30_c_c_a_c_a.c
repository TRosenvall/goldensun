/* OvlFunc_881_200b84c -- 0x0200b84c
 *
 * The escape cutscene: fetch the leader and one other actor, start the scene,
 * fade to black, then drift both actors' depth for sixty frames before running
 * the map transition out and setting the destination.
 *
 * THIS FUNCTION IS A CLEAN TEST OF THREE ENTRIES ADDED IN THE LAST TWO BATCHES,
 * and each one was worth several lines:
 *
 *  - DO NOT NAME THE LOOP-INVARIANT (batch 220). The per-frame step is added to
 *    two fields inside the loop. Named as `int step = 0x3333;` gcc
 *    rematerialises it and the function comes out FIVE LINES SHORT -- it never
 *    needs the fourth callee-saved register, so the r8 save/restore pair
 *    disappears from both prologue and epilogue. Written as a bare literal at
 *    both use sites it pools, loop.c hoists it into r7, and the length is
 *    exact. That the shortfall showed up in the PROLOGUE rather than at the
 *    loop is what makes this easy to misread as an allocation problem.
 *
 *  - THE `-1` TRIPLE NEEDS PINS AND BARRIERS. The four arguments are built as
 *    `mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r2 / mov r3,#0 / neg r1 /
 *    neg r0`. Pinning the four argument registers alone gets the negations
 *    right and emits the three movs in REVERSE order; one barrier after each of
 *    the first two movs fixes the order. n movs needing a given order need n-1
 *    barriers, as recorded.
 *
 *  - THE COUNTER DECREMENTS AFTER THE CALL (batch 215). Written before
 *    `__WaitFrames`, gcc hoists the `sub` above the second field store; written
 *    after, it lands where the ROM has it. The ROM's `mov r0, #1 / sub r6, #1 /
 *    bl` reads as a decrement before the call and is not one.
 *
 * The pooled `2` passed to __SetDestMap is `_CONST_2`, already in const.sym --
 * a value the cheapest possible `mov` could build, which is the strongest form
 * of that file's symbol tell. tools/objcmp.py reports a RELOCATION-NAME
 * difference for it (our `.word _CONST_2` against the reference's `.word 2`);
 * that is the documented phantom for a linker symbol standing where the
 * disassembly shows a resolved literal, and `make compare` is the authority.
 *
 * The zero stored to three fields and the one stored to two are single named
 * locals reused, matching the ROM's register reuse, and the second byte address
 * is reached by advancing the first pointer rather than recomputing it.
 */
extern unsigned char gState[];
extern int _CONST_2;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __SetDestMap(int a, int b);

void OvlFunc_881_200b84c(void)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int z;
    int one;
    int i;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    register int p3 __asm__("r3");

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    b = __MapActor_GetActor(0x36);
    __CutsceneStart();
    p0 = 1;
    __asm__ volatile ("" : : "r" (p0));
    p1 = 1;
    __asm__ volatile ("" : : "r" (p1));
    p2 = 1;
    p2 = -p2;
    p3 = 0;
    p1 = -p1;
    p0 = -p0;
    __Func_80933f8(p0, p1, p2, p3);
    __PlaySound(0xdb);
    __Actor_SetSpriteFlags(a, 0);
    z = 0;
    b[0x55] = z;
    p = a + 0x55;
    *p = z;
    *(int *)(a + 0x28) = z;
    p += 0xc;
    one = 1;
    *p = one;
    b[0x61] = one;
    i = 0x3b;
    do {
        *(int *)(a + 0x28) += 0x3333;
        *(int *)(b + 0x28) += 0x3333;
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
    __SetFlag(0x91 << 1);
    __SetDestMap((int)&_CONST_2, 0x1b);
}
