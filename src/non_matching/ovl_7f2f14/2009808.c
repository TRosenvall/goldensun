/* OvlFunc_968_2009808  --  0x02009808  [asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c.s]
 *
 * NOT MATCHING. Best 23 of 94, LENGTH EXACT. The candidate below is that form.
 *
 * THIS TU BUILDS AT -O1 and must be screened with `--O1`. It is caught by the
 * mis-scoped `rom_7f2f14/ovl_30_c_a_c_a_c_a%` wildcard that
 * src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_b.c already documents.
 * Checking `makefile_flags` before screening is what caught it here; a plain
 * -O2 screen would have reported a different residue and sent the work the
 * wrong way.
 *
 * THE BLOCKER IS WHERE `sub sp, #8` LANDS:
 *
 *     rom   mov r0, #0x0  /  sub sp, #0x8  /  mov r0, #0x1 / ...
 *     ours  sub sp, #0x8  /  mov r0, #0x0  /  ...
 *
 * The frame adjustment is needed for __Func_8010704's two stack arguments,
 * seventy instructions later. gcc emits it in the prologue and the ROM has
 * EXACTLY ONE body instruction hoisted above it -- the `mov r0, #0` feeding the
 * first __MapActor_GetActor. Ours hoists none. Everything after is displaced by
 * one, which is where most of the 23 comes from.
 *
 * That is the same class as src/non_matching/rom_8a000/809802c.c, which is
 * parked on the same question (`ldr r3, [r3] / sub sp, #0xc / ldr r5, [r3, #0x10]`
 * against our three loads hoisted instead of two) and which measured that
 * `--no-sched2` does NOT restore the ROM's order but breaks other things.
 * src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_b.c from batch 212 is the OTHER
 * side of it: there a volatile asm at the top of the body blocked a six
 * -instruction hoist that was otherwise correct, and removing it was the cure.
 * So the position is reachable in both directions by accident and in neither
 * on purpose.
 *
 * MEASURED, three orders for the crossed negation fill, all 23 because the
 * `sub sp` displacement swamps them:
 *
 *     negations in the movs' order (batch 212's cure)   23
 *     negations in the ROM's order, q3 between them     23
 *     negations in the ROM's order, q3 first            23
 *
 * The fill itself is crossed -- movs r0, r1, r2 against negations r2, r1, r0 --
 * so it needs a lever too, but WHICH lever cannot be judged while the first
 * divergence is at instruction 1. Fix the frame adjustment first; batch 208
 * recorded that a differing count is only meaningful for the first divergence,
 * and this is a clean instance.
 *
 * NEXT: the three functions in this class -- 809802c, 200a5f8 (elevated) and
 * this one -- should be read together. The elevated one is the only case where
 * the hoist came out right, and what it did differently was NOT have a barrier
 * at the top of the body. This function has no barrier either, so something
 * else decides. Comparing the two functions' first basic blocks is the next
 * step and was not done.
 */
extern void OvlFunc_968_2008058(int a, int b, int c, int d);
extern void OvlFunc_968_200894c(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetAnimSpeed(int slot, int n);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_2009808(void)
{
    unsigned char *a;
    register int z __asm__("r6");
    int e;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    { PIN4; q0 = 1; q1 = 1; q2 = 1; q0 = -q0; q1 = -q1; q2 = -q2; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    {
        PIN3;
        register int v __asm__("r3");
        v = 0x80; v <<= 7;
        q1 = 0xc0; q2 = 0xc0;
        *(short *)(a + 6) = v;
        q0 = 0; q1 <<= 10; q2 <<= 9;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    { PIN3; q2 = 0x8a; q2 <<= 2; q1 = *(short *)(a + 0xa); q0 = 0;
      __Func_8092158(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 0x16; q0 = 0; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0x81; q0 = 0; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 2; q0 = 0; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    {
        register int v __asm__("r3");
        v = 0xc0; v <<= 8;
        *(short *)(a + 6) = v;
    }
    __MapActor_SetAnim(0, 5);
    { PIN2; q1 = 0x18; q0 = 0; __MapActor_SetAnimSpeed(q0, q1); }
    __CutsceneWait(0x28);
    {
        PIN4;
        *(int *)(a + 0x48) = 0x9999;
        q2 = *(int *)(a + 0x10);
        z = 0;
        q2 += 0x90 << 15;
        *(int *)(a + 0x44) = z;
        q0 = *(int *)(a + 8);
        q1 = 0;
        q3 = 0xdf;
        OvlFunc_968_2008058(q0, q1, q2, q3);
    }
    e = 0x22;
    __Func_8010704(0x22, 0x23, 5, 1, e, e);
    OvlFunc_968_200894c(0);
    { PIN2; q1 = 0xf; q0 = 0; __Func_8092950(q0, q1); }
    __Func_8091e9c(0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
