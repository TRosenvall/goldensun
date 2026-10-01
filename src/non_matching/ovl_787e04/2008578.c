/* OvlFunc_887_2008578 -- NON-MATCHING, 4 ENCODINGS OF 453.  Size 1172 = 1172,
 * instruction count 453 = 453, relocations identical.  A TRUE DISTANCE.
 * Production flags, no per-file Makefile adjustment (checked).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_787e04/2008578.c \
 *     asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_a_c.s \
 *     --func OvlFunc_887_2008578
 *   XX ENCODINGS differ in 4 place(s) (ref 453, ours 453)
 *      first at index 129: ref 4653  ours 4652
 *
 * tools/shimcount.py: 45 register pins across 15 PIN macro sites.  A pinned
 * landing needs a fakematch.txt row; there is still none.
 *
 * ================== BATCH 316 CORRECTION TO THE BLOCKER ==================
 *
 * The park named `allocate_reload_reg`'s ROUND-ROBIN SPILL-REGISTER COUNTER
 * (reload1.c:4925-4945, `last_spill_reg`) as the blocker.  THE FUNCTION'S OWN
 * RELOAD TRACE REFUTES THAT FOR THE INSN THAT DIFFERS.
 *
 * `.18.greg` prints exactly FIFTEEN reload-register allocations, and only two
 * registers ever appear:
 *
 *     insn   10  -> reg 3        insn  328  -> reg 3  (reload 2)
 *     insn   25  -> reg 2        insn  353  -> reg 3
 *     insn   37  -> reg 2        insn  359  -> reg 3
 *     insn  117  -> reg 3        insn  371  -> reg 3
 *     insn  381  -> reg 2        insn  385  -> reg 2
 *     insn 1033/1039/1108/1114/1126 -> reg 3
 *
 * The residue at output index 129 is `.19.flow2` insn 1204,
 * `(set (reg:SI 2 r2) (reg/v:SI 10 sl))`, feeding insn 328
 * `(set (mem (plus (reg 6 r6) 12)) (reg:SI 2 r2))` -- i.e. `*(p6+0xc) = zero`.
 * For insn 328 `allocate_reload_reg` printed `Using reg 3`, yet the EMITTED
 * copy is r2.  So the register in the differing instruction was NOT produced
 * by the round-robin the park names; only reload 2 of insn 328 went through
 * allocate_reload_reg at all, and the copy that differs never appears in the
 * trace.  It is set in `choose_reload_regs` (inheritance), one stage later.
 *
 * WHY THIS MATTERS: every probe the park and batch 314 aimed at "the COUNT of
 * reload-register allocations EARLIER in the function" was aimed at a
 * mechanism that does not decide this insn.  That is the full explanation for
 * their uniform inertness, measured again below.
 *
 * ============ BATCH 316 RE-SWEEPS AGAINST THE 45-PIN BASELINE ============
 *
 * The park warned its inert list was taken at 93 pins.  Both sweeps were
 * re-run against the current body.  NEITHER FOUND AN IMPROVEMENT.
 *
 * 1. EVERY `extern void` -> `extern int` (42 variants, brief lever 2).
 *    30 EXACTLY INERT at 4.  12 worse: __MapActor_SetPos 19,
 *    __MapActor_SetAnim 12, __MapActor_SetSpeed 11, __Func_80921c4 11,
 *    __Func_8093040 10, __Func_8092adc 9, __Func_8019aa0 8 (first moves to 83),
 *    __MapActor_Jump 7, __Func_800c5fc 6 (first moves to 98), __ActorMessage 6,
 *    __Func_809259c 6, __Func_8092b08 6, OvlFunc_887_20097e4 6.
 *    The park recorded 13 wrong-way movers at 93 pins; at 45 pins it is 12.
 *    Nothing reaches 3 or below, so the lever is live and unhelpful here.
 *
 * 2. A 33-VARIANT PHASE SEARCH over everything that could add one reload
 *    allocation before index 129.  28 EXACTLY INERT at 4:
 *    - pinning each of FOURTEEN currently-plain call sites before the
 *      divergence (__CutsceneStart, __MapActor_SetPos x4, __MapActor_SetAnim,
 *      __Func_80118a8, __Func_800c5b4, __Func_8093304, __Func_8019aa0,
 *      __CutsceneWait, __Func_800c5fc, __Actor_SetSpriteFlags x2) -- ALL 14
 *      INERT, which is independent evidence that a pin on an argument register
 *      the value already occupies creates NO reload at all;
 *    - four of five `do { } while (0)` barrier placements;
 *    - three two-step computed forms for `zero` (2>>2, 0x100>>9, 0x12-0x12);
 *    - `register int zero __asm__("r10")` (inert: zero is already in sl);
 *    - a named `g[0]`, a dead `g` re-read, a named `p6+8` base, a stored temp
 *      for the zero, and reordering the `h` block.
 *    Worse: `zero` declared earlier 8 (first 48), the p6 0x10/0xc store order 6,
 *    a barrier after `m = 0xe52` 6 (first 82), `register int zero __asm__("r3")`
 *    388, `register unsigned char **g __asm__("r9")` 352.
 *
 * 3. FLAG SWEEP, 29 flags.  Nothing helps.  -fno-rerun-cse-after-loop 7,
 *    -fno-schedule-insns2 102, -fno-gcse 116, -fno-regmove 175,
 *    -fno-force-mem 175, -fno-strict-aliasing 279.  Not flag-conditional.
 *
 * THE RESIDUE (unchanged):
 *
 *     ref                      ours
 *     str  r3, [r6, #8]        str  r3, [r6, #8]
 *     mov  r3, sl              mov  r2, sl        <- index 129
 *     str  r3, [r6, #12]       str  r2, [r6, #12]
 *     ldr  r3, [pc]            ldr  r3, [pc]
 *     ...__Func_800fe9c, __WaitFrames...
 *     mov  r2, r9              mov  r3, r9        <- index 136
 *     ldr  r1, [r2, #0]        ldr  r1, [r3, #0]
 *     ...
 *     mov  r2, r9              mov  r2, r9        <- index 148, AGREES AGAIN
 *
 * Two complementary reload copies out of HI registers (`zero` in sl, `g` in
 * r9), and only r2/r3 are ever candidates, so this is a BINARY choice made
 * twice.  The brief's alias-set dependent-count lever cannot reach it (not
 * scheduling), and lever 2 and the phase search are now both exhausted.
 *
 * NEXT, AND IT IS A DIFFERENT PASS FROM THE ONE THE PARK NAMED: read
 * `choose_reload_regs`'s INHERITANCE decision for `.19.flow2` insn 1204
 * (reload1.c, `reg_last_reload_reg` / `reload_reg_free_for_value_p`).  The
 * question is which register already "held" sl's value when reload reached
 * insn 328.  That is the only unexamined mechanism left here
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
