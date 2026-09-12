/* OvlFunc_968_2009808  --  0x02009808  MATCHING (fakematch: one memory barrier)
 *
 * 95 instructions, 240 bytes, exact.  Measured 3x.
 *
 * THIS TU NEEDS THE EXPLICIT -O2 RULE IN THE MAKEFILE.  It is caught by the
 * mis-scoped rom_7f2f14/ovl_30_c_a_c_a_c_c% wildcard, which applies O1_CFLAGS:
 * 23 differing at -O1, 2 at -O2, EXACT at -O2.  Without the rule it screens
 * green and builds red.
 *
 * HOW THIS FUNCTION WAS PARKED FOR SO LONG, which is the part worth reading.
 * The park note asserted "THIS TU BUILDS AT -O1 and must be screened with --O1",
 * and named the ovl_30_c_a_c_a_c_a% wildcard -- the wrong one; this file is
 * caught by ovl_30_c_a_c_a_c_c%.  It then built an entire blocker class on the
 * -O1 output: a theory about where `sub sp, #8` lands relative to the prologue,
 * cross-referenced to two other parks, with three measured fill orders "all 23
 * because the sub sp displacement swamps them".  None of that exists at -O2.
 *
 *     makefile_flags() tells you what the BUILD will do.  It does not tell you
 *     what the TU should be SCREENED at.  A park that inherits a wildcard's -O1
 *     and then records -O1 as correct entrenches the error permanently.
 *
 * THE REAL RESIDUE at -O2 is two adjacent instructions, swapped:
 *
 *     rom   str r6, [r5, #0x44]  /  ldr r0, [r5, #8]
 *     ours  ldr r0, [r5, #8]     /  str r6, [r5, #0x44]
 *
 * Same base register, different constant offsets, so gcc proves they cannot
 * overlap and sched2 is free to hoist the load -- which it does, because the
 * load feeds the pinned r0 argument of the call and is on the critical path
 * while the store is a dead end.
 *
 * AXES ELIMINATED, all measured at -O2 on this source:
 *
 *     statement order, 4 permutations      2 (INERT -- not a LUID tie)
 *     -fno-strict-aliasing                 2 (INERT)
 *     -fno-schedule-insns                  2 (INERT)
 *     -fno-schedule-insns2                23 (REGRESSES -- sign rule says
 *                                             sched2 is already right and
 *                                             alias is the wrong axis)
 *     volatile on the store                2 (INERT -- gcc-2.96 still hoists a
 *                                             non-volatile load across it)
 *     volatile on the load                 2 (INERT)
 *     volatile on both                     6 (WORSE)
 *     literal 0 instead of the local z     6 (WORSE)
 *     z assigned immediately before use    4 (WORSE)
 *     declared struct instead of raw casts 2 (INERT -- kept anyway, it is the
 *                                             better spelling; include/actor.h
 *                                             declares velX at 0x44 and pos at
 *                                             0x08, so the raw *(int *)(a + n)
 *                                             casts the park used were simply
 *                                             worse code for the same output)
 *     __asm__ volatile ("" ::: "memory")   EXACT
 *
 * So the ROM's order IS reachable and costs ZERO instructions, but nothing in
 * plain C reaches it.  Aliasing cannot be the lever here: the two accesses are
 * disambiguated by constant OFFSET off one base, not by type, which is why
 * -fno-strict-aliasing and every alias-set spelling are inert.
 *
 * THE DEBT.  The barrier is scaffolding and is listed in
 * reports/fakematch-worklist.md.  What would retire it is a source shape where
 * the store and the load genuinely may-alias -- two pointers gcc cannot prove
 * equal.  The function already calls __MapActor_GetActor(0) twice, so
 * re-fetching is in this code's idiom, but a second call here costs a `bl` and
 * the length is already exact.  That is the specific thing to look for.
 */
#include "actor.h"
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
        ((struct Actor *)a)->velY = 0x9999;
        q2 = ((struct Actor *)a)->pos.z;
        z = 0;
        q2 += 0x90 << 15;
        ((struct Actor *)a)->velX = z;
        /* FAKEMATCH.  See the header: this barrier is the ONLY thing found that
         * keeps gcc from hoisting the pos.x load above the velX store. */
        __asm__ volatile ("" ::: "memory");
        q0 = ((struct Actor *)a)->pos.x;
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
