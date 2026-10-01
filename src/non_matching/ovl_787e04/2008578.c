/* OvlFunc_887_2008578 -- NON-MATCHING, 4 ENCODINGS OF 453.  Size 1172 = 1172,
 * instruction count 453 = 453, relocations identical.  A TRUE DISTANCE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_787e04/2008578.c \
 *     asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_a_c.s \
 *     --func OvlFunc_887_2008578
 *   XX ENCODINGS differ in 4 place(s) (ref 453, ours 453)
 *      first at index 129: ref 4653  ours 4652
 *
 * Blocker class: reload's ROUND-ROBIN SPILL-REGISTER COUNTER
 * (allocate_reload_reg, reload1.c:4925-4945; `last_spill_reg` is function-scoped
 * state advancing once per successful reload-register allocation).  Both residue
 * instructions are RELOAD-CREATED high-to-low copies -- `zero` lives in r10 and
 * `g` in r9, and *thumb_movsi_insn cannot store from a high register -- so
 * QTY_CMP_PRI never sees them and this is not a local-alloc quantity.
 *
 * BATCH 314 CONFIRMED THAT ATTRIBUTION AGAINST THE BRIEF'S `REG_N_SETS`
 * RE-READING, AND IT SURVIVES.  `zero` is set ONCE and used ONCE across many
 * calls, so REG_N_SETS == 1 and it is exactly the shape update_equiv_regs gates
 * on.  Every two-step computed form that makes it TWO-SET is INERT at 4:
 * `zero = 0; zero <<= 7;`, `zero = 1; zero -= 1;`, `zero = 1; zero >>= 1;`,
 * `zero = 0x80; zero &= 0;`, and the same treatments on `g` (`g = &g[0]`, a
 * do-while barrier, `t = *g`), singly and in combination.  So this is NOT a
 * one-set/two-set story in disguise.  Also measured: a do-while barrier on
 * `zero` costs 10, and DELETING the existing `do { } while (0);` costs 30 -- that
 * barrier is load-bearing and worth 26.
 *
 * ================== PINS: 45, DOWN FROM 93, AND WHY THE REST STAY ==================
 * tools/shimcount.py reports 45 register pins across 15 PIN macro sites.  The
 * previous revision of this park carried 93 across 34 sites; batch 314 removed
 * 19 whole sites (they are now ORDINARY C CALLS with the constants folded back
 * into the argument list) with NO change to the distance -- 4, first 129,
 * size 0, relocations ok, identical on every figure.
 *
 * A PER-SITE SWEEP ESTABLISHED WHICH 15 MUST STAY.  Control: `PIN<n>` replaced by
 * plain `int` temporaries, which removes only the register constraint and keeps
 * the block, the temporaries and the assignment order.  14 sites are
 * individually load-bearing (6 of them catastrophically: 323, 295, 286, 225,
 * 120, 105).  All 34 unpinned reads 425 of 453 and +12 bytes: the pins carry
 * essentially the whole function and it cannot be depinned wholesale.
 *
 * *** THE 15th PIN IS LOAD-BEARING ONLY IN COMPANY, AND THIS IS A GENERAL LAW. ***
 * Dropping all 20 individually-inert pins at once costs 61 (4 -> 65).  Bisection
 * isolates it to a PAIR -- the two sites below, each INERT alone and 65 together,
 * with every subset containing both reading 65 and every subset missing either
 * reading 4:
 *
 *     q2 = 0xaa; q2 <<= 2;   ... __Func_8092158(q0, q1, q2)
 *     q2 = 0xaa; q2 <<= 2;   ... __Func_80921c4(q0, q1, q2)
 *
 * `grep -n 0xaa` returns exactly those two lines: they are the only two sites
 * that materialise the same constant (0x2a8).  MECHANISM -- this is
 * src/non_matching/rom_c9000/80cdd58.c's recorded `invalidate_for_call` lever
 * needing TWO pins instead of one.  cse1 unifies two pseudos holding the same
 * CONST_INT; the unified pseudo then crosses calls and local_alloc gives it a
 * CALLEE-SAVED register, which perturbs the allocation globally.  A hard
 * call-clobbered register is invalidated by invalidate_for_call, so EITHER pin
 * alone breaks the unification -- unification needs TWO UNPINNED PEERS.
 * CONTROL: with the pair unpinned and one site's constant changed 0xaa -> 0xab
 * so the two values differ, 65 collapses to 5.  That isolates the sharing of the
 * constant as the entire cause.
 *
 * A SECOND, INDEPENDENT INSTANCE of the same law, found the same way.  Dropping
 * only the WIDEST pin at a site is individually inert at 32 of the 34 sites, but
 * combining eight of those costs 15, and bisection again isolates one pair:
 *
 *     q1 = 0xc0; q2 = 0xc0; q1 <<= 9; q2 <<= 8;  __MapActor_SetSpeed(...)   x2
 *
 * `grep -n 0xc0` returns exactly those two lines -- again the only two sites
 * sharing materialised constants.
 *
 * CONSEQUENCE FOR ANY DEPINNING PASS, AND IT IS THE REASON THE COUNT ABOVE IS
 * HONEST RATHER THAN MINIMAL: a pin whose job is to defeat cse unification of a
 * value occurring N times is load-bearing ONLY AS A GROUP.  Removing any one
 * leaves N-1 >= 1 pinned and nothing changes, so a cumulative greedy that
 * removes one pin at a time and requires byte-identity CANNOT SEE IT -- every
 * single step is genuinely inert and the cliff arrives only when the
 * last-but-one pin of the group goes.  Worse, a greedy that happens to try the
 * first of a pair before the second ACCEPTS the first, then REJECTS the second,
 * and reports a fixpoint one pin short.  The cheap guard: GROUP THE PIN SITES BY
 * THE VALUE THEY MATERIALISE (one grep per constant) AND REMOVE EACH GROUP AS A
 * UNIT.  A group of size N has one interesting removal, not N.
 *
 * Pushing past 45 is not free and was measured: width-reducing the 15 survivors
 * costs 15 (the 0xc0 pair), and `PIN3 -> only q0 pinned` on them costs 417.
 *
 * A pinned landing needs a fakematch.txt row; there is still none.
 *
 * THE RESIDUE (unchanged):
 *
 *     ref                      ours
 *     str  r3, [r6, #8]        str  r3, [r6, #8]
 *     mov  r3, sl              mov  r2, sl
 *     str  r3, [r6, #12]       str  r2, [r6, #12]
 *     ldr  r3, [pc]            ldr  r3, [pc]
 *     ...two calls...
 *     mov  r2, r9              mov  r3, r9
 *     ldr  r1, [r2, #0]        ldr  r1, [r3, #0]
 *
 * The ROM uses r3 then r2; we use r2 then r3 -- a complementary swap across two
 * reload-created copies, i.e. a PHASE DIFFERENCE in last_spill_reg.  AN IDENTICAL
 * INSTRUCTION STREAM UP TO THE DIVERGENCE DOES NOT IMPLY IDENTICAL RELOAD STATE:
 * everything before index 129 is equal, yet the counter is out of phase, because
 * inherited and shared reloads advance last_spill_reg WITHOUT EMITTING AN
 * INSTRUCTION.  The only handle remains the COUNT of reload-register allocations
 * EARLIER in the function.  `p8 += 0` as an earlier-reload probe: inert.
 *
 * THE BRIEF'S ALIAS-SET DEPENDENT-COUNT LEVER CANNOT REACH THIS PARK: that lever
 * moves rank_for_schedule's dependent count and needs a MEM as one of two
 * COMPETING insns.  This residue is not scheduling at all -- it is a reload
 * spill-register choice, and -fno-schedule-insns2 was already shown to leave the
 * whole region unchanged.
 */
struct HalfWord { unsigned short v; };

extern unsigned char *iwram_3001ebc[];
extern unsigned char ActorCmd_ARRAY_887__02009ab4[];
extern unsigned char gScript_887__02009b04[];
extern unsigned char gScript_887__02009b34[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __StartThunder(void);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_AddSpriteLayer(unsigned char *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_800c5b4(void);
extern void __Func_800c5fc(void);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093304(int a);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void OvlFunc_887_20097e4(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_887_2008578(void)
{
    unsigned char **g;
    unsigned char *p7;
    unsigned char *p6;
    register int p8 __asm__("r8");
    unsigned char *p;
    unsigned char *t;
    unsigned char *sc;
    register unsigned char bit __asm__("r6");
    int m;
    int zero;
    struct HalfWord z;

    g = iwram_3001ebc;
    do { } while (0);
    t = g[0];
    p7 = *(unsigned char **)((unsigned char *)iwram_3001ebc - 0x4c);
    p6 = *(unsigned char **)(t + (0xf0 << 1));
    p8 = *(int *)(__MapActor_GetActor(0x11) + 0x50);
    __CutsceneStart();
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapActor_SetAnim(0, 0x12);
    zero = 0;
    { unsigned char *r = (unsigned char *)p8;
      int h = 0x555;
      *(short *)(r + 0x1e) = h; }
    z.v = 0;
    __MapActor_GetActor(0x11)[0x55] = z.v;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
    __MapActor_SetPos(0x11, 0x90 << 18, 0x28a0000);
    __Func_80118a8(7);
    __MapActor_SetPos(8, 0x2160000, (0xac) << 18);
    __Func_800c5b4();
    __Func_8093304(8);
    do { } while (0);
    m = 0xe52;
    __Func_8019aa0(m, 1, 0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x80; q0 <<= 9; q1 = 0x80; q2 = 0x80; q1 <<= 9; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8093304(8);
    { PIN3; q1 = 1; q0 = m + 1; q2 = 0; __Func_8019aa0(q0, q1, q2); }
    __Func_800c5fc();
    __CutsceneWait(0x28);
    *(int *)(p7 + 0xec) = 0xa4 << 17;
    *(int *)(p7 + 0xf0) = 0x96 << 18;
    *(int *)(p7 + 0xf4) = 0x9c << 18;
    *(int *)(p7 + 0xf8) = 0xcc << 18;
    *(int *)(p6 + 8) = 0x8d << 18;
    do { } while (0);
    *(int *)(p6 + 0xc) = zero;
    do { } while (0);
    *(int *)(p6 + 0x10) = 0x2b30000;
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(g[0] + (0xe0 << 1)) = 0x209;
    *(int *)(g[0] + (0xe4 << 1)) = 0x40;
    __StartThunder();
    { short *pp = (short *)(g[3] + 0x1f84);
      int one = 1;
      *pp = one; }
    __Func_8095240();
    __WaitFrames(0x1e);
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8095268();
    __MapActor_DoAnim(8, 4);
    __MessageID(m + 2);
    { PIN3; q2 = 0x3c; q0 = 0x9008; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0, 2);
    __CutsceneWait(0x28);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x14; q0 = 0x9008; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0, 2);
    __Func_80118c0(7);
    __CutsceneWait(0x14);
    __Func_80118a8(8);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x13);
    __Func_8092158(0, 0x22d, 0x2a7);
    __Func_80118c0(8);
    __Func_80118a8(9);
    __Func_8092158(0, 0x22b, (0xaa) << 2);
    __CutsceneWait(0x1e);
    __Func_8092adc(8, (0xd0) << 8, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __MapActor_Jump(0, 4, 0);
    { PIN3; q2 = 0x2a2; q0 = 0; q1 = 0x21f; __Func_80921c4(q0, q1, q2); }
    __Func_8092b08(0, 3);
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x9008; q1 = 0; __ActorMessage(q0, q1); }
    OvlFunc_887_20097e4();
    __Func_809259c(8, 2);
    __Func_8093040(0x9008, 0, 0x14);
    p = __MapActor_GetActor(8) + 0x5a;
    *p = 0xfe & *p;
    { PIN3; q2 = 0xaa; q2 <<= 2; q1 = 0x21e; q0 = 8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    p = __MapActor_GetActor(8) + 0x5a;
    bit = 1;
    *p = bit | *p;
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __Actor_AddSpriteLayer(__MapActor_GetActor(0), 0xe2);
    __SetFlag(0x21);
    __PlaySound(0x7e);
    __Func_8092950(0, 7);
    __CutsceneWait(0xa);
    __Func_8092950(0, 0);
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(8) + 0x5a;
    *p = 0xfe & *p;
    __Func_80921c4(8, 0x216, (0xac) << 2);
    __CutsceneWait(1);
    p = __MapActor_GetActor(8) + 0x5a;
    *p = bit | *p;
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __SetCameraTarget(8, 1);
    p = __MapActor_GetActor(0) + 0x23;
    sc = ActorCmd_ARRAY_887__02009ab4;
    *p = bit | *p;
    __MapActor_SetBehavior(8, sc);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0, sc);
    __MapActor_WaitScript(8);
    { PIN3; q0 = 8; q1 = 0x1a3; q2 = 0x295; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xcc; q2 = 0x295; q0 = 8; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(8, 1);
    __MapActor_SetAnim(0, 1);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x8008; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
        *(short *)(g[0] + (0xec << 1)) += 1;
    __CutsceneWait(0x14);
    __Func_8093040(0x8008, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(8, gScript_887__02009b04);
    __MapActor_SetBehavior(0, gScript_887__02009b34);
    __CutsceneWait(0x14);
    *(int *)(g[0] + (0xe0 << 1)) = 0x201;
    *(int *)(g[0] + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x14);
}
