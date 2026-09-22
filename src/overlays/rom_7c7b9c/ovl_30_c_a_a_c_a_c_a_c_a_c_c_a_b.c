/* Cluster OvlFunc_943_2009db0..OvlFunc_943_2009db0 extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a.s.
 *
 * Total .text for this TU = 480 bytes (= 0x1e0). Never attempted before batch 279.
 * THIRTEEN PIN3 SITES AND ONE ZERO-COST SCHEDULING BARRIER -- one fakematch row, and the
 * heaviest scaffolding landed in this run of batches. No flags.
 *
 * THE SCAFFOLDING IS THE TREE'S OWN ESTABLISHED FORM, NOT A NOVEL LIBERTY: 181 landed files use
 * the PIN2/PIN3/PIN4 macros, and the great majority are booked in fakematch.txt. The
 * `do { } while (0);` barrier likewise has precedent in landed files. Verified at landing rather
 * than assumed, because thirteen sites is a lot and I wanted to know whether it was in convention.
 *
 * LADDER, and every one of the 14 items is load-bearing (greedy drop costs 2-17 differing each,
 * and dropping the barrier costs 2):
 *     plain C                                                39 differing, 171 vs 164 insns
 *     + 4 PIN3 on the __MapActor_SetSpeed sites              22, and now the correct LENGTH
 *     + 9 more PIN3 fill-order sites                          4
 *     + site-1 shift order                                    2
 *     + the barrier BEFORE the pinned block                    0
 *
 * The first four pins carry the repeated 0x26666 / 0x13333 and kill the r6/r8 commoning together
 * with the `mov r6, r8 / push {r6}` prologue -- the recorded CSE-shared-expensive-constants class
 * in its beatable form.
 *
 * SITE 1 wants `q1 <<= 16;` BEFORE `q2 <<= 18;`. A 30-permutation sweep gives 2 for every order
 * with that relation and 4-6 otherwise, so it is the RELATION not the absolute position.
 *
 * SITE 9 IS THE INTERESTING ONE, AND IT IS WHY THE BARRIER IS THERE. The ROM puts `mov r0, #0x15`
 * first, and that is UNREACHABLE BY PIN ORDER. `.23.sched2` shows insn 583 (`r2 = 0x99`) at prio
 * 72 against insn 495 (`r0 = 0x15`) at 71 -- and on a tie `rank_for_schedule`'s dependent-count
 * term ALSO favours the r1/r2 fills, 10 successors against 3, **because a call SETS r0 and only
 * USES r1/r2, so r0-fills accumulate no anti-dependences at all.** That asymmetry is structural
 * and worth carrying: an r0 argument fill can never win a sched2 tie against an r1/r2 fill on
 * dependent count.
 *
 * 36 spellings measured at 2-8 there: all 12 pin permutations; 24 permutations crossed with
 * `do{}while(0)` and `__asm__ volatile("")` barriers placed AFTER the fills; `q2 = 0x264;`;
 * `q2 = 0x99 << 2;`; `q2 = q2 << 2`; `q2 *= 4`; three named locals (6); the bare call (6). ONLY
 * the barrier placed BEFORE the fill block reaches 0. `__asm__ volatile("")` in that same
 * position is also exact; the empty loop is preferred because it emits nothing at all.
 *
 * Flags measured, none helping: -fno-schedule-insns 22, -fno-rerun-cse-after-loop 22, -fno-gcse
 * 22, -fno-schedule-insns2 36.
 *
 * THE CALL-FAMILY SELECTOR FOUND THIS FUNCTION'S IDIOM SOURCE, and stem adjacency would not have.
 * src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_b.c (OvlFunc_943_200a9d4) was reached by
 * grepping src/ for `__LoadFieldActors`, and supplied about thirty prototypes verbatim, the
 * `*(int *)(iwram_3001ebc + 0x1c0)` idiom, the PIN3 macro itself, and the "do not name the actor
 * pointer" rule. The park src/non_matching/ovl_7c7b9c/2009920.c independently confirmed the
 * "r0 in the middle of a three-argument call" blocker class and that -fno-schedule-insns* does
 * not reach it.
 */
struct Actor {
    unsigned char pad00[0x64];
    short f64;
};

extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char L5208[] __asm__(".L5208");
extern char *iwram_3001ebc;
extern int gScript_943__0200c4ec[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(void *p);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *p);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitScript(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __Func_8092950(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093304(int a);
extern void __Func_8091e9c(int a);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_800c5b4(void);
extern void __Func_800c5fc(void);
extern void OvlFunc_943_200b9ec(int a);
extern void OvlFunc_943_2008bb8(void);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_943_2009db0(void)
{
    int zero;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __LoadFieldActors(L5208);
    __WaitFrames(1);
    { PIN3; q1 = 0xf8; q2 = 0xb6; q0 = 0x15; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x202;
    __MapTransitionIn();
    { PIN3; q0 = 0x15; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xad; q0 = 0x15; q1 = 0xf2; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0xc4; q2 = 0x2a6; __Func_80921c4(q0, q1, q2); }
    __Func_80921c4(0x15, 0xb6, 0x28e);
    __Func_80925cc(0x15, 2);
    __MessageID(0x1e44);
    OvlFunc_943_200b9ec(0xa015);
    { PIN3; q0 = 0; q1 = 0x26666; q2 = 0x13333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x9a; q2 = 0x261; q0 = 0; __MapActor_TravelTo(q0, q1, q2); }
    __PlaySound(0x92);
    zero = 0;
    __MapActor_GetActor(0x18)->f64 = zero;
    __MapActor_GetActor(0x19)->f64 = zero;
    __MapActor_GetActor(0x1a)->f64 = zero;
    { PIN3; q1 = 0x80; q2 = 0xf2; q1 <<= 14; q0 = 0x18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xa8; q2 = 0xf8; q0 = 0x19; q1 <<= 15; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x95; q0 = 0x1a; q1 <<= 13; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0x26666; q2 = 0x13333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x26666; q2 = 0x13333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x13333; q0 = 0x1a; q1 = 0x26666; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0x18, gScript_943__0200c4ec);
    __MapActor_SetBehavior(0x19, gScript_943__0200c4ec);
    __MapActor_SetBehavior(0x1a, gScript_943__0200c4ec);
    __Func_8092950(0x18, 3);
    __Func_8092950(0x19, 3);
    __Func_8092950(0x1a, 3);
    do {
        __WaitFrames(1);
    } while (__MapActor_GetActor(0x18)->f64 == 0);
    OvlFunc_943_2008bb8();
    do { } while (0);
    { PIN3; q0 = 0x15; q2 = 0x99; q1 = 0xc4; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitScript(0x18);
    __CutsceneWait(0xa);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneWait(0xa);
    __Func_800c5b4();
    __Func_8093304(0x15);
    __Func_8019aa0(0x1e45, 1, 0);
    __Func_800c5fc();
    __Func_8091e9c(0xc);
}
