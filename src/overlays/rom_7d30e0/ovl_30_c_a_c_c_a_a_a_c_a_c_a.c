// fakematch
/* OvlFunc_948_2008b68  --  0x02008b68
 *
 * WAS PARKED at ~84 of 139 as src/non_matching/ovl_7d30e0/2008b68.c, naming the
 * same-value mov wall as its blocker, at two sites, alongside a third residue
 * where gcc copied an actor pointer out of r0. All three are closed here and
 * the third turned out to be the one that mattered.
 *
 * 1. THE POINTER COPY WAS ONE VARIABLE DOING TWO JOBS, and this is the finding
 *    to carry forward. The park stored through `a` at two places far apart:
 *    once immediately after __MapActor_GetActor, and once where the pointer has
 *    to stay live across three more calls. One `unsigned char *a` for both
 *    means ONE pseudo, the second range forces it callee-saved, and the FIRST
 *    range inherits that -- `mov r5, r0` and a store through r5 where the ROM
 *    stores through r0. Giving the two ranges separate locals costs nothing and
 *    takes 84 of 139 to 2, with the length exact. The same mechanism appears in
 *    src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c from the other direction, where
 *    naming a pointer at all was what cost r0; here the name is fine and it is
 *    the REUSE that costs it. Read the live ranges, not the spelling.
 *
 * 2. THE TWO SAME-VALUE MOV SITES fall to the volatile-asm barrier described in
 *    src/overlays/rom_7c460c/ovl_314_c_a_c_a.c -- `q0 = K;` then
 *    `__asm__ volatile ("" : : "r" (q0));` before the other movs, at both the
 *    `0x80` triple and the `-1, -1, 0xe666` call. Torn down: removing just
 *    those two lines from the finished file gives 5 differing, and it is
 *    precisely the two mov triples the park named. They are load-bearing.
 *
 * 3. A METHOD NOTE THAT COST TIME HERE. Applying the two barriers FIRST, with
 *    the pointer residue still in place, scored 87 against the park's 84 -- the
 *    lever that finishes the function looked like a regression. The cause is
 *    that a divergence at instruction 52 shifts everything after it, so a
 *    correct fix downstream lands against misaligned ROM text and counts as
 *    noise. FIX THE EARLIEST DIVERGENCE BEFORE JUDGING ANY LEVER APPLIED AFTER
 *    IT; the differing count is only meaningful for the first divergence.
 *
 * The last two instructions were an ordinary transposition -- the ROM sets
 * `mov r0, #0xf` between the two shifts of the following call -- closed by
 * anchoring that call's r0 along with the r1 and r2 it already anchored.
 *
 * The park's own three levers are unchanged and still load-bearing: pinning the
 * argument registers per call removes an eight-instruction three-high-register
 * spill; the halfword store of `0xf8 << 8` is computed in an `int` local first,
 * because as a `short` the value does not fit and gcc pools the sign-extended
 * word; and every pin is block-scoped beside the call it serves.
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
    unsigned char *b;
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
    b = __MapActor_GetActor(0xf);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q3 = 0xa0; q3 <<= 12; q1 = 0x92; q2 = 0xa6;
        *(int *)(b + 0x28) = q3;
        q1 <<= 2; q0 = 0xf; q2 <<= 2;
        __Func_80921c4(q0, q1, q2);
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
        q0 = 0x80; __asm__ volatile ("" : : "r" (q0)); q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 <<= 11;
        __Func_8012330(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 1; __asm__ volatile ("" : : "r" (q0)); q1 = 1; q1 = -q1; q2 = 0xe666; q0 = -q0;
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
