// fakematch
/* OvlFunc_881_200b2f0  --  0x0200b2f0
 *
 * Was goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_c.s, which
 * held it alone.
 *
 * A map-entry cutscene: blank the camera, place actor 8 and face it, seed a
 * field at iwram_3001ebc + 0x1c0, fade in, walk the actor through five legs at
 * four different speeds, fade out.
 *
 * THIS FUNCTION WAS PARKED AT 4 OF 99 AND THE PARK WAS WRONG. The park claimed
 * the `-1` triple's `mov` order was unreachable, on the strength of six
 * spellings tying at exactly 4. All six shared an assumption I never examined:
 * they kept the three assignments together and the three negations together,
 * and varied only the order WITHIN those two groups. Interleaving them --
 *
 *     p0 = 1; p0 = -p0;
 *     p1 = 1; p1 = -p1;
 *     p2 = 1; p2 = -p2;
 *     p3 = 0;
 *
 * -- matches. So "six unrelated spellings tie" was not evidence, because the
 * spellings were not unrelated: they were six permutations inside one shape.
 * When a tie is used to declare something unreachable, the spellings have to
 * differ in STRUCTURE, not just in order.
 *
 * The `-1` triple is therefore reachable in both its shape AND its order. The
 * batch-148 entry calling it an unbroken class, already amended once to
 * "reachable by pinning", can now drop the caveat entirely.
 *
 * FIVE LEVERS, in the order they were found, 89 differing down to 0:
 *
 *   1. pin the four `-1` arguments and negate in place        89 -> 82
 *   2. the halfword store's value needs an `int` local, and
 *      it must be computed AFTER the GetActor call            82 -> 64
 *   3. pin the FIRST occurrence of each of the four repeated
 *      pooled constants -- 0x3333, 0x1999, 0x12a8, 0x1298     64 -> 23
 *   4. pin r0 at the five remaining call sites                23 -> 4
 *   5. interleave each assignment with its own negation        4 -> 0
 *
 * Step 2 is the one worth carrying. Written `*(short *)(p + 6) = 0xa0 << 8;`
 * the literal pools as `=0xffffa000`, because 0xa000 is negative at short width
 * -- ordinary blocker 1b, escaped with an `int` local. But naming it is not
 * enough: with the GetActor call in the same statement the local must survive
 * the call and gcc gives it a CALLEE-SAVED register, which widens the prologue.
 * Splitting the call out and computing the value after it lets the value live
 * in a scratch register, which is what the ROM does. THE INT-LOCAL ESCAPE FOR A
 * HALFWORD LITERAL HAS A PLACEMENT: after any call in the same statement.
 */

extern unsigned char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80936a0(int a, int b);
extern void __Func_8092158(int slot, int x, int z);
extern void __Func_8091e9c(int n);

void OvlFunc_881_200b2f0(void)
{
    int v;
    unsigned char *a;

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
    __MapActor_SetAnim(8, 2);
    __MapActor_SetPos(8, 0x13080000, 0xca << 18);
    a = __MapActor_GetActor(8);
    v = 0xa0 << 8;
    *(short *)(a + 6) = v;
    __WaitFrames(1);
    __Func_80936a0(0x13333, 1);
    __MapActor_SetPos(0, 0, 0);
    __SetCameraTarget(8, 1);
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __MapTransitionIn();
    {
        register int a0 __asm__("r0") = 8;
        register int a1 __asm__("r1") = 0x6666;
        register int a2 __asm__("r2") = 0x3333;
        __MapActor_SetSpeed(a0, a1, a2);
    }
    {
        register int e2 __asm__("r2") = 0xb2;
        register int e0 __asm__("r0") = 8;
        register int e1 __asm__("r1") = 0x12d8;
        e2 <<= 2;
        __Func_8092158(e0, e1, e2);
    }
    {
        register int b2 __asm__("r2") = 0x9a;
        register int b0 __asm__("r0") = 8;
        register int b1 __asm__("r1") = 0x12a8;
        b2 <<= 2;
        __Func_8092158(b0, b1, b2);
    }
    {
        register int f0 __asm__("r0") = 8;
        register int f1 __asm__("r1") = 0x4ccc;
        register int f2 __asm__("r2") = 0x2666;
        __MapActor_SetSpeed(f0, f1, f2);
    }
    {
        register int g2 __asm__("r2") = 0xec;
        register int g0 __asm__("r0") = 8;
        register int g1 __asm__("r1") = 0x12a8;
        g2 <<= 1;
        __Func_8092158(g0, g1, g2);
    }
    {
        register int c0 __asm__("r0") = 8;
        register int c1 __asm__("r1") = 0x3333;
        register int c2 __asm__("r2") = 0x1999;
        __MapActor_SetSpeed(c0, c1, c2);
    }
    {
        register int d2 __asm__("r2") = 0xe4;
        register int d0 __asm__("r0") = 8;
        register int d1 __asm__("r1") = 0x1298;
        d2 <<= 1;
        __Func_8092158(d0, d1, d2);
    }
    {
        register int h0 __asm__("r0") = 8;
        register int h1 __asm__("r1") = 0x1999;
        register int h2 __asm__("r2") = 0xccc;
        __MapActor_SetSpeed(h0, h1, h2);
    }
    {
        register int i2 __asm__("r2") = 0xdc;
        register int i0 __asm__("r0") = 8;
        register int i1 __asm__("r1") = 0x1298;
        i2 <<= 1;
        __Func_8092158(i0, i1, i2);
    }
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x28);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x6e);
}
