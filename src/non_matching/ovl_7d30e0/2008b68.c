/* OvlFunc_948_2008b68  --  0x02008b68  [asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.s]
 *
 * NOT MATCHING. Best 140 lines against the ROM's 139. The .s holds this
 * function alone, so no split is needed when it is finished.
 *
 * A one-shot cutscene behind two flags: a camera move, a walk through four
 * waypoints, two screen-shake calls and a script handoff.
 *
 * THREE LEVERS LANDED, 147 lines and 147 differing down to 140 and ~84:
 *
 *   1. THE HOISTING SPILL, again and worse than in
 *      src/non_matching/ovl_7c460c/2008ff0.c. Left alone, gcc hoists the flag
 *      id and several `0x80 << n` constants into callee-saved registers and
 *      then SPILLS THREE HIGH REGISTERS to pay for them:
 *
 *          push {r5, r6, r7, lr} / mov r7, r10 / mov r6, r9 / mov r5, r8
 *          push {r5, r6, r7}
 *
 *      -- eight instructions the ROM does not have. Pinning the argument
 *      registers per call removed the spill. That is now two functions in a
 *      row where r8-r10 traffic in a function with no business touching them
 *      was the tell for a hoisted constant rather than for anything structural.
 *
 *   2. A SIGN-EXTENDED POOL WORD FOR A HALFWORD STORE. `*(short *)(s + 0x1e) =
 *      0xf8 << 8` gives `ldr r3, =0xfffff800`: the value is 0xf800, which does
 *      not fit a signed short, so gcc materialises the sign-extended pattern
 *      from the pool. The ROM builds it as `mov r3, #0xf8 / lsl r3, #8`.
 *      Computing it in an `int` local first and storing that gives the ROM's
 *      pair. Same family as the halfword-literal escape already recorded, but
 *      the trigger here is the SIGN of the value, not the store width alone.
 *
 *   3. BLOCK-SCOPED PINS THROUGHOUT, one nested declaration per call site,
 *      following the finding in 2008ff0 that pins reused across many calls are
 *      weaker than pins declared beside the call they serve.
 *
 * WHAT REMAINS -- THE SAME-VALUE MOV WALL, TWICE, and it is why this is
 * parked rather than finished. Both sites are movs that receive the SAME
 * constant, so nothing orders them:
 *
 *     rom   mov r0, #0x80 / mov r1, #0x80 / mov r2, #0x80 / lsl r1 / lsl r2 / lsl r0
 *     ours  mov r1, #0x80 / mov r2, #0x80 / mov r0, #0x80 / lsl r1 / lsl r2 / lsl r0
 *
 *     rom   mov r0, #0x1 / mov r1, #0x1 / neg r1, r1 / ldr r2, =0xe666 / neg r0, r0
 *     ours  mov r1, #0x1 / mov r0, #0x1 / neg r1, r1 / ldr r2, =0xe666 / neg r0, r0
 *
 * The second is written in the batch-192 INTERLEAVED form already -- each mov
 * followed by the negation that consumes it -- and it still comes out swapped.
 * That is worth recording, because batch 192 closed OvlFunc_881_200b2f0 with
 * exactly that interleave. So the interleave is not a general solution to the
 * same-value case; it worked there and does not here, and the difference
 * between the two sites has not been identified.
 *
 * AND ONE MORE, at the store before __Func_80921c4:
 *
 *     rom   mov r3, #0xa0 / lsl r3, #0xc / mov r1 / mov r2 / str r3, [r0, #0x28]
 *     ours  ... mov r5, r0 ... str r2, [r5, #0x28]
 *
 * gcc copies the actor out of r0 into r5 rather than storing through it. The
 * ROM keeps it in r0 until `mov r0, #0xf` overwrites it AFTER the store.
 * Pinning the stored value to r3 (so it stops colliding with the pinned r2)
 * did not remove the copy; nor did leaving r0 unpinned at that site. Measured:
 * plain int temp 141 lines, r3-pinned temp 140, r0 unpinned 141.
 *
 * NEXT: the same-value mov ordering is the blocker for this function and for
 * 2008ff0, which stalled at 2 of 157 on it. Two functions in two rounds is
 * enough to call it the dominant remaining wall in this material, and it
 * deserves a dedicated look rather than another per-function sweep. The one
 * lead: in 2008ff0 gcc ordered the pair by which SHIFT consumed first, and the
 * two calls in that function whose shifts ran the other way matched without
 * help.
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_SetScript(unsigned char *a, void *s);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8012330(int a, int b, int c);
extern unsigned char gScript_948__0200a6fc[];

void OvlFunc_948_2008b68(void)
{
    unsigned char *a;
    unsigned char *s;
    int f;
    int k;

    {
        register int q0 __asm__("r0");
        q0 = 0x9c8;
        if (__GetFlag(q0) == 0)
            return;
        q0 = 0x9c9;
        f = __GetFlag(q0);
        if (f != 0)
            return;
        q0 = 0x9c9;
        __SetFlag(q0);
    }
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x80; q1 = 0x80; q0 <<= 10; q1 <<= 7;
        __Func_80933d4(q0, q1);
    }
    __Func_8093500(0xf, 1);
    __Func_8093530();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x14; q0 = 0xf; q1 <<= 7;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81; q0 = 0xf; q1 <<= 1;
        __MapActor_Surprise(q0, q1);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 2; q0 = 0xf;
        __Func_80925cc(q0, q1);
    }
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x80; q1 <<= 9; q2 <<= 8; q0 = 0xf;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __PlaySound(0x98);
    a = __MapActor_GetActor(0xf);
    {
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q3 = 0xa0; q3 <<= 12; q1 = 0x92; q2 = 0xa6;
        *(int *)(a + 0x28) = q3;
        q1 <<= 2;
        __Func_80921c4(0xf, q1, q2 << 2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x14; q0 = 0xf; q1 <<= 7;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81; q1 <<= 1; q0 = 0xf;
        __MapActor_Surprise(q0, q1);
    }
    __CutsceneWait(0x1e);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x80; q0 = 0xf; q1 <<= 12; q2 <<= 7;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa6; q2 = 0xa6; q0 = 0xf; q1 <<= 2; q2 <<= 2;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xba; q2 = 0xa6; q0 = 0xf; q1 <<= 2; q2 <<= 2;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xce; q2 = 0xa6; q1 <<= 2; q2 <<= 2; q0 = 0xf;
        __Func_80921c4(q0, q1, q2);
    }
    __CutsceneWait(0xa);
    __PlaySound(0xd0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x80; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 <<= 11;
        __Func_8012330(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 1; q1 = 1; q1 = -q1; q2 = 0xe666; q0 = -q0;
        __Func_8012330(q0, q1, q2);
    }
    __CutsceneWait(0x1e);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xde; q2 = 0xa6; q1 <<= 18; q2 <<= 18; q0 = 0xf;
        __MapActor_SetPos(q0, q1, q2);
    }
    a = __MapActor_GetActor(0xf);
    s = *(unsigned char **)(a + 0x50);
    k = 0xf8;
    k <<= 8;
    *(short *)(s + 0x1e) = k;
    *(short *)(a + 6) = f;
    __Actor_SetAnimSpeed(a, 0);
    __Actor_SetScript(a, gScript_948__0200a6fc);
    __CutsceneEnd();
}
