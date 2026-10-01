/* OvlFunc_899_200b6f8 (0x0200b6f8) -- NON-MATCHING, 1100 of 1486 encodings differ.
 *
 * THE 1100 IS NOT A DISTANCE. Our instruction count is 1455 against the
 * reference's 1459, so objcmp's index-by-index count is position-poisoned from
 * the first missing instruction onward. The figures that ARE meaningful, and
 * which this park is ranked on, are kept in DIFFERENT UNITS and never summed:
 *
 *     instructions   ours 1455   ref 1459     (-4)
 *     pool words     ours   24   ref   25     (-1)
 *     size           ours 3784   ref 3796     (-12 bytes)
 *     relocations    ours  394   ref  394     IDENTICAL, and in identical
 *                                             symbol order; no ref-only and no
 *                                             ours-only entry
 *     aligncmp       1403 aligned-equal of 1486 = 94.4%,
 *                    98 differing/inserted/deleted in 60 hunks, of which
 *                    20 hunks are pure literal-pool offset drift
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_794ac0/200b6f8.c \
 *     asm/overlays/rom_794ac0/ovl_30_c_a_a_c_c.s --func OvlFunc_899_200b6f8
 *
 * SPLIT SHAPE: NONE NEEDED. asm/overlays/rom_794ac0/ovl_30_c_a_a_c_c.s holds
 * exactly ONE function, this one. `python3 tools/datacheck.py` on it reports no
 * data section, so deleting the .s takes no symbol with it and the linker script
 * needs no change.
 * SHIMS: 159 PIN blocks (`tools/shimcount.py` counts 428 `register ... __asm__`
 * declarations, because PIN2/PIN3 expand to two and three each). No
 * `__asm__(".equ ...")` measurement shim and no new symbol: nothing is owed to
 * area.sym, const.sym or fakematch.txt.
 * FRAME: NONE. There is no `sub sp` and no `mov rX, sp` anywhere in 1459
 * instructions, so no spill slot exists, and none of the aggregate-ordering or
 * declaration-order-for-spill-slots material applies to this function.
 *
 * ============================================================
 * WHAT IS RIGHT: THE WHOLE CALL GRAPH AND EVERY SYMBOL
 * ============================================================
 * 394 relocations against 394, in the same order, with no difference in either
 * direction. That covers all 387 `bl` sites across 43 distinct callees and all
 * 19 pooled symbol references. So the statement sequence, the three `if` blocks
 * and every argument that is a symbol are correct; what is left is register
 * allocation and the materialisation spelling of a handful of constants.
 *
 * The function is a 15-turn cutscene: three actors posed and sprite-flagged, a
 * map transition, then a long straight-line script of poses, emotes, timed
 * pauses and three dialogue lines, closing on __CutsceneEnd. Control flow is
 * three `if (call() != 0)` blocks with no else and no loop -- the comparison
 * census is 3 `beq` and ZERO `bne`, `blt`, `ble`, `bgt`, `bge`, so NO loop-shape
 * lever and NO signed-compare lever applies here, and (per batch 311's
 * discipline note) -fno-rerun-cse-after-loop is MEANINGLESS on this function
 * and was not tested.
 *
 * ============================================================
 * THE LEVERS, MEASURED, LARGEST GAIN FIRST
 * ============================================================
 * Ranked on aligncmp because the instruction counts differ throughout; the
 * objcmp "differ" count cannot see any of this.
 *
 *   1. THE BLANKET PIN PASS IS CONFIRMED ON THIS POPULATION, and it is by far
 *      the largest single move:
 *          ROM-fill-order pins only (104 blocks)   84.7%  309 edits/157 hunks
 *          pin EVERY multi-argument call (310)     92.7%  129 edits/ 78 hunks
 *          pin only calls carrying a literal
 *            outside 0..255 or a shift (159)       92.8%  123 edits/ 76 hunks
 *      +8.1 points. Note the SELECTIVE pass beats the blanket one by 6 edits
 *      while using HALF the pins, so "pin everything" is not the optimum --
 *      "pin every call with a wide literal" is.
 *
 *   2. NAMED MUTABLE MESSAGE BASES, +1.6 points (92.8% -> 94.4%, 123 -> 98
 *      edits) and it is what removed three pool words. The ROM holds a message
 *      id in a call-saved register and WALKS it:
 *          n  = 0x12fc; __MessageID(n);        ...  __Func_801776c(n + 5, 1);
 *          n += 8;      __Func_801776c(n, 1);
 *          n2 = 0x1324; __Func_801776c(n2, 1); ...  n2 += 1; __MessageID(n2);
 *      Written as the literals 0x1301 / 0x1304 / 0x1325 each one pools its own
 *      word and costs a `ldr rX,[pc]` where the ROM pays `adds r0, r6, #5`.
 *      The same statement also names the script pointer that two consecutive
 *      __MapActor_SetBehavior calls share (`ldr r5, =gScript_899__0200d4c8`,
 *      then `mov r1, r5` twice).
 *      THE TELL IS GENERAL AND CHEAP: a pool word that is another pool word
 *      plus a small offset is a WALKED BASE, not two constants. Here
 *      0x1301 = 0x12fc+5, 0x1304 = 0x12fc+8 and 0x1325 = 0x1324+1 were all
 *      visible in the pooled-constant multiset before a line was written.
 *
 * ============================================================
 * MEASURED NEGATIVE OR INERT -- each of these is a BOUND, not a failure
 * ============================================================
 *   - HARD-PINNING THE THREE HIGH REGISTERS TO THE ROM'S OWN ASSIGNMENT MAKES
 *     THINGS WORSE. The ROM's high registers are r8 = 0xec<<1, r9 = 0xe4<<1,
 *     sl = &iwram_3001ebc. Ours allocates three high registers too, but to
 *     different quantities. Forcing the ROM's choice:
 *          baseline (this file)                94.4%   98 edits   1455 insns
 *          register int j __asm__("r8")        93.7%  116 edits   1458 insns
 *          + register int k __asm__("r9")      93.7%  114 edits   1461 insns
 *          + **g __asm__("r10") for the global 94.5%  117 edits   1465 insns
 *     THIS IS AN INSTRUCTIVE SHAPE AND IT IS A COUSIN OF THE EXACT-SIZE-BY-
 *     COINCIDENCE TRAP: the first pin moves the instruction COUNT from -4 to
 *     -1 (1458 against 1459) while moving the edit distance the WRONG WAY by 18.
 *     A count converging is not a stream converging. Ranking on the count alone
 *     would have adopted the worse candidate.
 *
 *   - BLOCK-SCOPING A CONSTANT LOCAL IS BYTE-IDENTICAL HERE. `int m = 0xe0<<1`
 *     hoisted to the top of the function and the same value declared inside its
 *     own `{ }` produce the SAME OBJECT -- 1455 instructions, 24 pool words,
 *     1403 aligned-equal, 98 edits, identical hunk list. gcc's live-range
 *     analysis has already narrowed the range before any allocator sees the
 *     scope, so C block scope is not a lever on a constant local. This bounds
 *     the declaration-placement family from a new side: the existing members act
 *     through the REG_EQUIV note and through priority order, and neither is
 *     reachable by moving a brace.
 *
 *   - DROPPING THE NAMED ZERO REGRESSES, which was the opposite of the
 *     prediction. At one site the ROM POOLS a zero (`ldr r5, .L3c6c  @ 0`,
 *     then `strb r5`), which by the batch-311 rule should mean a SYMBOL --
 *     *thumb_movsi_insn would build any eight-bit value with a `mov`. So the
 *     named `int z = 0` carrier was removed and the site written three ways:
 *          z kept, site reads `z`              94.4%   98 edits  24 pool words
 *          z dropped, site reads `0`           93.5%  113 edits  24 pool words
 *          z dropped, site `(int)&_AREA_00`    93.5%  114 edits  25 pool words
 *          z dropped, site `(int)&_CONST_0`    93.5%  114 edits  25 pool words
 *     Two separable facts, and they point opposite ways:
 *       (a) the SYMBOL SPELLING DOES buy the pool word -- 25 against the
 *           reference's 25, where the plain literal gives 24. `_AREA_00` and
 *           `_CONST_0` are byte-identical to each other, as batch 311 recorded,
 *           so the encodings cannot name which it is and the relocation list is
 *           the only instrument; here BOTH add a relocation the reference does
 *           not have, so NEITHER is admitted and no row is owed to const.sym.
 *       (b) the named zero is worth 15 edits ELSEWHERE, because a long-lived 0
 *           in a register is what dozens of `q2 = 0` argument fills reuse.
 *     Net: keep the carrier, leave the pooled zero unsolved. This is a real
 *     refinement of the pooled-small-constant rule: the rule predicts a symbol
 *     at THAT SITE and the prediction survives (the pool word appears), but
 *     adopting it costs more at every OTHER site than it buys at this one.
 *
 * ============================================================
 * THE REMAINING RESIDUE, 40 substantive hunks, and what each needs
 * ============================================================
 *   - 20 of the 60 hunks are literal-pool OFFSET DRIFT only: identical
 *     `ldr rX, [pc, #N]` with N differing because our pool dumps land in
 *     different places. Not independent defects; they close when the mid-function
 *     pool dump at ref[546:551] lands where the reference puts it.
 *   - THE HIGH-REGISTER PERMUTATION is the largest real cluster (about 20
 *     encodings over 6 hunks). The ROM gives r8 to the halfword offset 0xec<<1,
 *     and because Thumb's `ldrh rX,[rY,rZ]` needs all three registers LOW, an
 *     offset living in r8 FORCES the ROM's `add r2, r8` / `ldrh r3,[r2,#0]`
 *     pointer form. Ours keeps that offset low and gets the indexed
 *     `ldrh r3,[r2,r5]` form at both of its sites. So the pointer-versus-indexed
 *     difference is NOT an addressing-mode choice to be spelled around -- it is
 *     downstream of the register CLASS, and the pins above show it cannot be
 *     bought by naming the class directly.
 *   - FOUR INSERT/DELETE PAIRS of a single `movs r2, #0xc4`-class argument fill
 *     sitting one slot early. Same encoding on both sides, one position apart;
 *     a scheduling boundary, not a wrong value.
 *   - THE EPILOGUE builds 0x209 as (0xe0<<1) + 0x49, reusing the live offset,
 *     where the reference POOLS 0x209 and rebuilds the offset. Scoping the
 *     epilogue offset into its own block was tried and is 3 edits WORSE
 *     (94.3%, 101 edits), so the reuse is not reachable by scope either.
 *
 * NEXT, in order: (1) place the mid-function pool dump, which alone addresses
 * 20 hunks; (2) find the spelling that raises the halfword offset's allocation
 * priority WITHOUT a hard pin -- an extra reference to it is the obvious probe,
 * since an extra use is the discriminator elsewhere in this family; (3) the four
 * one-slot argument fills.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(unsigned char *a, int flags);
extern void __DeleteActor(unsigned char *a);
extern void __Func_801776c(int id, int b);
extern unsigned char *__Func_808e078(int a, int b, int c);
extern void __Func_8091890(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern unsigned char gScript_899__0200d2fc[];
extern unsigned char gScript_899__0200d354[];
extern unsigned char gScript_899__0200d3ac[];
extern unsigned char gScript_899__0200d444[];
extern unsigned char gScript_899__0200d4c8[];
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);
extern int OvlFunc_899_200af84(void);
extern void OvlFunc_899_200af98(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_899_200b6f8(void)
{
    unsigned char *p0;
    unsigned char *q;
    unsigned char *r;
    unsigned char *a1;
    unsigned char *s1;
    int c0, h, v, w, z, j, k, m, n, n2;

    z = 0;
    p0 = iwram_3001ebc;
    __SetFlag(0x855);
    __CutsceneStart();
    q = __MapActor_GetActor(0xc) + 0x23;
    v = 1; v |= *q; *q = v;
    { PIN3; q1 = 0xda << 2; q0 = 0xf; q2 = 0x1a9; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xda << 2; q2 = 0x199; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0xda << 2; q2 = 0x179; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc2 << 18; q2 = 0xc4 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc6 << 18; q2 = 0xc4 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xc4 << 17; q0 = 0xc; q1 = 0xca << 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 5);
    __MapActor_SetAnim(0xb, 5);
    __MapActor_SetAnim(0xc, 5);
    __Func_809280c(0xb, 0, 0);
    __Func_809280c(0xa, 0, 0);
    { PIN3; q2 = 0; q1 = 0; q0 = 0xc; __Func_809280c(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 1);
    { PIN3; q0 = 0xd; q1 = 0xc0 << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xc0 << 18; q2 = 0xd4 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xc4 << 2; q2 = 0xd4 << 1; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xca << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    __Func_809280c(0xd, 9, 0);
    __Func_809280c(8, 9, 0);
    __Func_809280c(0xe, 0xa, 0);
    __Func_809280c(9, 0xa, 0);
    { PIN3; q0 = 0; q1 = 0xc6 << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xca << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc2 << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    __Func_809280c(0, 0xa, 0);
    __Func_809280c(1, 0xa, 0);
    __Func_809280c(2, 0xa, 0);
    k = 0xe4 << 1;
    *(int *)(iwram_3001ebc + k) = 0x1e;
    m = 0xe0 << 1;
    *(int *)(iwram_3001ebc + m) = m + 0x41;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    { PIN2; q1 = 2; q0 = 0xa; __Func_80925cc(q0, q1); }
    n = 0x12fc;
    __MessageID(n);
    OvlFunc_899_200c5f4(0xa, 0x14);
    { PIN2; q1 = 1; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    __MapActor_SetAnim(0xd, 3);
    { PIN3; q2 = 0x14; q0 = 8; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    __Func_809259c(0xb, 2);
    { PIN2; q1 = 2; q0 = 0xc; __Func_809259c(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xd; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x2ea; q2 = 0xcc << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q0 = 0xe; q1 = 0xb0 << 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xb; q1 = gScript_899__0200d354; __MapActor_SetBehavior(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = (int)gScript_899__0200d354; q0 = 0xa; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __CutsceneWait(0xf);
    { PIN2; q1 = (int)gScript_899__0200d354; q0 = 0xc; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __CutsceneWait(0x23);
    { PIN2; q1 = (int)gScript_899__0200d2fc; q0 = 8; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __CutsceneWait(0x14);
    __MapActor_WaitMovement(0xd);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xd; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xc4 << 2; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(9, 0, 0);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 2; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    OvlFunc_899_200c60c(0xe, 0, 0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    OvlFunc_899_200c63c(0xe, 3, 0x14);
    { PIN3; q2 = 0xa; q0 = 0xe; q1 = 0x80 << 6; __Func_8092adc(q0, q1, q2); }
    OvlFunc_899_200c5f4(0xe, 0x14);
    OvlFunc_899_200c624(0, 1, 0x32);
    OvlFunc_899_200c624(0, 2, 0x32);
    __Func_809280c(0, 9, 0);
    __Func_809280c(1, 9, 0);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 9; OvlFunc_899_200c60c(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    OvlFunc_899_200c63c(2, 3, 0x32);
    __Func_8092848(9, 0xe, 0);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN2; q1 = (int)gScript_899__0200d3ac; q0 = 0xe; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __CutsceneWait(0x32);
    { PIN2; q1 = (int)gScript_899__0200d444; q0 = 9; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc6 << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0 << 8; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 2; q1 = 0xc6 << 2; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(2, 0, 0);
    { PIN3; q0 = 1; q1 = 0xca << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xd0 << 8; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x64);
    OvlFunc_899_200c60c(0xe, 9, 0x3c);
    OvlFunc_899_200c60c(9, 0xe, 0x28);
    OvlFunc_899_200c63c(9, 3, 0x28);
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __PlaySound(0x7c);
    __MapActor_SetAnim(0xf, 4);
    { PIN3; q2 = 0xd4 << 17; q0 = 0x12; q1 = 0xda << 18; __MapActor_SetPos(q0, q1, q2); }
    __Func_8092b08(0x12, 1);
    { PIN3; q0 = 0x12; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = -(8); q1 = 0; q0 = 0x12; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(0x12);
    { PIN2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN2; q0 = n + 5; q1 = 1; __Func_801776c(q0, q1); }
    __MapActor_SetAnim(0xf, 2);
    __MapActor_SetPos(0x12, 0, 0);
    j = 0xec << 1;
    (*(unsigned short *)(iwram_3001ebc + j))++;
    __Func_80925cc(0xe, 1);
    OvlFunc_899_200c5f4(0xe, 0x14);
    OvlFunc_899_200c624(0, 1, 0x28);
    OvlFunc_899_200c60c(9, 0xe, 0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x1e);
    __Func_809280c(0, 0xe, 0);
    OvlFunc_899_200c60c(1, 0xe, 0x28);
    { PIN3; q2 = 0; q1 = 0; q0 = 0xe; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN2; q1 = 2; q0 = 0xe; __Func_80925cc(q0, q1); }
    __PlaySound(0x7c);
    { PIN2; q1 = 4; q0 = 0x10; __MapActor_SetAnim(q0, q1); }
    q = __MapActor_GetActor(0x13) + 0x55;
    *q = z;
    { PIN2; q1 = 1; q0 = 0x13; __Func_8092b08(q0, q1); }
    { PIN3; q0 = 0x13; q1 = 0xda << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = -(8); q1 = 0; q0 = 0x13; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(0x13);
    { PIN2; q1 = 2; q0 = 0x13; __Func_80925cc(q0, q1); }
    n += 8;
    __CutsceneWait(0x3c);
    { PIN2; q0 = n; q1 = 1; __Func_801776c(q0, q1); }
    __MapActor_SetAnim(0x10, 2);
    __MapActor_SetPos(0x13, 0, 0);
    (*(unsigned short *)(iwram_3001ebc + j))++;
    { PIN3; q1 = 0x81 << 1; q2 = 0; q0 = 9; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(9, 0x14);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c63c(0xe, 3, 0x32);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 0; OvlFunc_899_200c60c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x1e);
    { PIN3; q1 = 0xd0 << 8; q2 = 0; q0 = 0xe; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0x80 << 1; q2 = 0; q0 = 0xe; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0xd6 << 2; q2 = 0xbc << 1; q0 = 0xe; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 0xe; q1 = 9; OvlFunc_899_200c60c(q0, q1, q2); }
    OvlFunc_899_200c5f4(0xe, 0x14);
    __Func_809280c(9, 0xe, 0);
    { PIN3; q1 = 0x80 << 1; q2 = 0; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c60c(2, 0xe, 0x1e);
    OvlFunc_899_200c60c(9, 2, 0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    __ActorMessage(9, 0);
    { PIN3; q1 = 0xa0 << 7; q2 = 0; q0 = 0xe; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    OvlFunc_899_200c60c(2, 9, 0x14);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    __Func_8092848(0, 2, 0);
    { PIN3; q2 = 0x14; q0 = 1; q1 = 2; OvlFunc_899_200c60c(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x28);
    { PIN3; q2 = 0x1e; q0 = 2; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    __Func_809280c(0, 9, 0);
    __Func_809280c(1, 9, 0);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 9; OvlFunc_899_200c60c(q0, q1, q2); }
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    { PIN3; q2 = 0; q1 = 0x81 << 1; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(2, 0x28);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    __MapActor_SetAnim(0, 4);
    __MapActor_DoAnim(1, 4);
    { PIN2; q1 = 3; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN3; q2 = 0x14; q0 = 0; q1 = 1; OvlFunc_899_200c624(q0, q1, q2); }
    __Func_80925cc(2, 2);
    OvlFunc_899_200c63c(2, 4, 0x1e);
    { PIN3; q0 = 2; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xcc << 1; q0 = 2; q1 = 0xc8 << 2; __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(2, 2);
    __ActorMessage(2, 0);
    __Func_809280c(0, 9, 0);
    OvlFunc_899_200c60c(1, 9, 0x1e);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81 << 1; q0 = 2; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN2; q1 = 1; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x101; q2 = 0; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    { PIN2; q1 = 1; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x28);
    { PIN3; q1 = 0x105; q2 = 0; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 9; q1 = 0xd2 << 2; q2 = 0xd4 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 9; q1 = 0; OvlFunc_899_200c624(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    OvlFunc_899_200c63c(0, 3, 0x14);
    { PIN3; q2 = 0; q1 = 0xa0 << 7; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x14);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x1e);
    { PIN3; q2 = 0x14; q0 = 9; q1 = 0xe; OvlFunc_899_200c624(q0, q1, q2); }
    OvlFunc_899_200c5f4(9, 0x14);
    { PIN3; q2 = 0xcc << 1; q0 = 0xe; q1 = 0xd6 << 2; __Func_80921c4(q0, q1, q2); }
    s1 = gScript_899__0200d4c8;
    { PIN2; q0 = 9; q1 = (int)s1; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    { PIN2; q0 = 0xe; q1 = (int)s1; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    { PIN3; q0 = 1; q1 = 0xc6 << 2; q2 = 0xe4 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc2 << 2; q2 = 0xd8 << 1; q0 = 2; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(1);
    { PIN3; q1 = 0xd0 << 8; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(2);
    { PIN3; q1 = 0xd0 << 8; q2 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitScript(9);
    { PIN3; q1 = 9; q2 = 0; q0 = 0; __Func_809280c(q0, q1, q2); }
    q = __MapActor_GetActor(0xe);
    q[0x5b] = 1;
    w = 0x80 << 24;
    *(int *)(q + 0x38) = w;
    *(int *)(q + 0x3c) = w;
    *(int *)(q + 0x40) = w;
    { PIN3; q2 = 0; q0 = 9; q1 = 0x80 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 0xe; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0x32);
    __Func_809280c(9, 0, 0);
    { PIN3; q0 = 1; q1 = 0xca << 2; q2 = 0xdc << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xb0 << 8; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(9, 0x14);
    OvlFunc_899_200c624(0, 1, 0x32);
    __Func_809280c(0, 9, 0);
    OvlFunc_899_200c60c(1, 9, 0x14);
    OvlFunc_899_200c63c(9, 3, 0x14);
    { PIN3; q0 = 9; q1 = 0xba << 2; q2 = 0xcc << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xba << 2; q2 = 0xcc << 1; q0 = 0xe; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(9);
    __MapActor_WaitMovement(0xe);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0; q1 = 0xc6 << 2; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    OvlFunc_899_200c624(0, 1, 0x1e);
    { PIN3; q1 = 0x105; q2 = 0; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    __Func_809280c(0, 2, 0);
    { PIN3; q2 = 0x14; q0 = 1; q1 = 2; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x28);
    OvlFunc_899_200c63c(2, 3, 0x1e);
    { PIN3; q2 = 0x28; q0 = 0; q1 = 1; OvlFunc_899_200c624(q0, q1, q2); }
    __MapActor_SetAnim(0, 4);
    OvlFunc_899_200c63c(1, 4, 0x1e);
    __Func_809280c(0, 2, 0);
    { PIN3; q2 = 0x14; q0 = 1; q1 = 2; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x28);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x14);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x101; q2 = 0; q0 = 1; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 1; q2 = 0; q0 = 1; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81 << 1; q2 = 0; q0 = 1; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0x14; q0 = 2; q1 = 4; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q2 = 0x14; q0 = 1; q1 = 3; OvlFunc_899_200c63c(q0, q1, q2); }
    OvlFunc_899_200c5f4(1, 0x1e);
    { PIN2; q1 = 0; q0 = 2; __Func_8092c40(q0, q1); }
    c0 = __Func_8091c7c(0, 0);
    if (c0 != 0) {
    __CutsceneWait(0x14);
    { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x14);
    OvlFunc_899_200c63c(2, 4, 0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    }
    OvlFunc_899_200c63c(2, 3, 0x1e);
    { PIN3; q0 = 2; q1 = 0xc8 << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q0 = 2; q1 = 0xd6 << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q0 = 2; q1 = 0xd6 << 2; q2 = 0xbc << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q2 = 0; q1 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_80925cc(2, 2);
    { PIN3; q2 = 0x41; q1 = 0x11; q0 = 2; a1 = __Func_808e078(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN2; q1 = 1; q0 = n2 = 0x1324; __Func_801776c(q0, q1); }
    __DeleteActor(a1);
    { PIN2; q1 = 2; q0 = 0x11; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c63c(2, 3, 0x14);
    { PIN3; q0 = 2; q1 = 0xd6 << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q0 = 2; q1 = 0xc8 << 2; q2 = 0xe4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q0 = 2; q1 = 0xc2 << 2; q2 = 0xd4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_8092848(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x1e);
    n2 += 1;
    { PIN3; q2 = 0x14; q1 = 3; q0 = 2; OvlFunc_899_200c63c(q0, q1, q2); }
    __MessageID(n2);
    OvlFunc_899_200c5f4(2, 0x14);
    __MapActor_SetAnim(0, 3);
    r = p0 + (0xec << 1);
    h = *(short *)r;
    c0 = OvlFunc_899_200af84();
    if (c0 != 0) {
    __MessageID(0x132a);
    __ActorMessage(2, 0);
    OvlFunc_899_200af98();
    }
    __Func_8091890(2);
    *(unsigned short *)r = h;
    OvlFunc_899_200c63c(1, 3, 0x32);
    { PIN3; q0 = 2; q1 = 0xc2 << 2; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    { PIN3; q1 = 0xba << 2; q2 = 0xcc << 1; q0 = 2; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x14; q0 = 0; q1 = 1; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x14);
    OvlFunc_899_200c63c(0, 3, 0x14);
    __MapActor_SetAnim(1, 2);
    q = __MapActor_GetActor(0);
    if (q != 0)
        __MapActor_TravelTo(1, *(short *)(q + 0xa), *(short *)(q + 0x12));
    __MapActor_WaitMovement(1);
    { PIN3; q1 = 0; q2 = 0; q0 = 1; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __CutsceneEnd();

}
