/* OvlFunc_957_20093f8 (0x020093f8) -- NON-MATCHING, 1934 of 2364 encodings differ.
 *
 * THE 1934 IS SATURATED AND IS NOT A DISTANCE. Our object carries 2366
 * encodings against the reference's 2364 -- it MEASURES ABOVE ITS OWN
 * REFERENCE -- so objcmp's index-by-index count has no lower bound to converge
 * on and per-encoding attribution is meaningless. It is reported here only
 * because it is the figure the production flags reproduce. Rank this park on
 * the figures below, which are kept in DIFFERENT UNITS and never summed:
 *
 *     instructions   ours 2313   ref 2316     (-3)
 *     pool words     ours   49   ref   43     (+6)   <-- the real defect
 *     size           ours 6236   ref 6220     (+16 bytes)
 *     relocations    ours  708   ref  708     SAME COUNT AND SAME MULTISET --
 *                                             nothing ref-only, nothing
 *                                             ours-only -- but NOT the same
 *                                             ORDER, which is the pool-word
 *                                             excess showing up as displacement
 *     aligncmp       2222 aligned-equal of 2364 = 94.0%,
 *                    203 differing/inserted/deleted in 84 hunks, of which
 *                    32 hunks are pure literal-pool offset drift
 *
 * Note the shape of the two count lines: -3 instructions against +6 pool words
 * is the batch-311 cancellation trap caught BEFORE it could flatter anything.
 * Had they been added together the size would have read nearly exact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e3e08/20093f8.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_a.s --func OvlFunc_957_20093f8
 *
 * SPLIT SHAPE -- AND A CORRECTION TO THE FILE-MATE PARK.
 * asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_a.s holds THREE functions:
 * OvlFunc_957_2008f94 (111 insns, parked at .../2008f94.c), OvlFunc_957_200909c
 * (377 insns, parked at .../200909c.c) and this one. The 200909c park states
 * "A three-way split is needed before any of them converts". That is true for
 * 200909c, which sits in the MIDDLE, but NOT for this function, which is LAST:
 * `python3 tools/split_s.py asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_a.s \
 *  OvlFunc_957_20093f8 --dry-run` reports a TWO-way split --
 *     ovl_30_c_c_c_a_a_a_a.s -> _a_a.s (2008f94 + 200909c, 531 lines)
 *                             + _a_b.s (20093f8, 2360 lines)
 * plus a rewrite of overlays/rom_7e3e08/overlay.ld. So this function is the
 * CHEAPEST of the three to take, not the most expensive. (The dry run was
 * honoured -- it wrote nothing. The "split_s.py silently ignores --dry-run"
 * defect recorded in docs/elevation.md does not reproduce.)
 * `python3 tools/datacheck.py` on the reference reports NO data section, so no
 * symbol is lost with the .s.
 * SHIMS: 316 PIN blocks, no `__asm__(".equ ...")`, no new symbol. Nothing owed
 * to area.sym, const.sym or fakematch.txt.
 * FRAME: NONE -- no `sub sp`, no `mov rX, sp` in 2316 instructions.
 *
 * ============================================================
 * WHAT IS RIGHT
 * ============================================================
 * 708 relocations against 708 with an identical multiset: all 703 `bl` sites
 * across 44 distinct callees are present, correct and none is spurious. The
 * statement sequence, the four if/else blocks, the three guarded-TravelTo tails
 * and both blend-fade loops line up.
 *
 * Comparison census, RUN ON THIS FUNCTION (do not import it): 4 `bne`,
 * 3 `beq`, 2 `ble`, 1 `bge`, 0 `blt`, 0 `bgt`. The three signed compares are
 * the loop tests, and they are the reason the loops are written as
 * `i = 0x1d; do { ... i--; } while (i >= 0);` and `i = 0; ... while (i <= 0x10);`
 * rather than with `!=`.
 *
 * ============================================================
 * THE LEVERS, MEASURED
 * ============================================================
 *   1. THE SELECTIVE PIN PASS, and it reproduces the 899 result in this batch
 *      almost exactly:
 *          ROM-fill-order pins only   (255 blocks)   87.0%  432 edits/270 hunks
 *          pin EVERY multi-arg call   (406 blocks)   92.7%  245 edits/103 hunks
 *          pin only calls with a wide
 *            literal or a shift       (316 blocks)   94.0%  203 edits/ 84 hunks
 *      +7.0 points, and again THE SELECTIVE PASS BEATS THE BLANKET ONE -- by 42
 *      edits here and 6 on OvlFunc_899_200b6f8, in both cases using fewer pins.
 *      Two independent functions now say the step-1 rule should be read as
 *      "pin every call carrying a literal outside 0..255", NOT "pin everything".
 *
 *   2. THE COMMUTATIVE-DESTINATION RULE IS CONFIRMED, AND THE DISCRIMINATOR IS
 *      THE LAST USE. Two blocks here clear and then set bit 0 of [0x5a] on four
 *      actors with a named mask. At the first three sites the LOADED BYTE is the
 *      destination (`and r3, r2` / `orr r3, r5`); at the FOURTH the MASK
 *      REGISTER is (`and r5, r3` / `orr r5, r3`). The fourth is the mask's last
 *      use, so regmove is free to tie the output to it. Written as
 *          *bp = m0 & *bp;          -- sites 1 to 3
 *          m0 &= *bp; *bp = m0;     -- site 4, the mask's last use
 *      This is the lever documented in the LANDED matching sibling
 *      src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_a_c.c, and what is new is the
 *      rule for WHICH site flips: it is the constant's final reference.
 *
 * ============================================================
 * A LEVER THAT DOES *NOT* TRANSFER -- and screening caught it before use
 * ============================================================
 * OvlFunc_899_200b6f8, done in this same batch, is driven by a WALKED MESSAGE
 * BASE: it pools 0x12fc and reaches 0x1301/0x1304 as `base + 5` / `base += 8`,
 * worth +1.6 points and three pool words there. THIS FUNCTION DOES NOT DO THAT.
 * Its message ids come in consecutive runs that look exactly like a walked base
 * -- 0x218a/0x218b/0x218c, 0x21a4/0x21a5/0x21a6/0x21a8, 0x21ce/0x21cf/0x21d0,
 * 0x21d7/0x21d9 -- but the pooled-constant multiset shows EVERY ONE OF THEM
 * POOLED SEPARATELY, exactly once each. The reference materialises each id from
 * its own pool word and never adds to a held base.
 * So the adjacency of the VALUES is not evidence; the MULTIPLICITY in the pool
 * is. A walked base shows up as ONE pool word with several `adds rX, #k` beside
 * it, and that is the screen to run. This is the same shape as batch 311's
 * per-function comparison census: the mechanism travels, the per-function fact
 * does not, and the cheap screen comes first.
 *
 * ============================================================
 * THE RESIDUE: 52 substantive hunks, and the +6 POOL WORDS ARE ONE DEFECT
 * ============================================================
 *   - LITERAL-POOL PLACEMENT is the dominant problem and explains the pool-word
 *     excess, the 32 drift-only hunks and the relocation REORDER all at once.
 *     Our first pool dump lands at ours[143:151] carrying five words (0, 0x962,
 *     0x2183, 0xcccc, 0x6666); the reference emits 0x962 and 0x2183 much later,
 *     in the dump at ref[406:409]. Every `ldr rX, [pc, #N]` between the two
 *     points then differs in N only. This is ONE defect with ~80 encodings of
 *     consequence, not eighty defects, and it is the first thing to fix.
 *   - THE SHIFT MUST BE PART OF THE DEFINITION, NOT THE USE. The reference
 *     computes `lsls r5, #16` / `lsls r6, #16` in the argument shadow of the
 *     PRECEDING call and then passes `adds r1, r6, #0`; ours shifts inside the
 *     pin block at the point of use. So the source reads
 *         x = (*(short *)(a + 0xa)) << 16;
 *     with the shift in the assignment, which lets the scheduler sink it into
 *     the call shadow. Ours currently reads the halfword into `x` and shifts in
 *     the call. 4 encodings, and it is a spelling with no allocation content --
 *     the next thing to fix after the pool.
 *   - `m1 = 1` IS REMATERIALISED AT ITS LAST SITE, which is the REG_N_SETS /
 *     REG_EQUIV gate firing exactly as batch 311 describes: one set earns the
 *     note and reload rebuilds the constant (`movs r6, #1`) instead of keeping
 *     the reference's r5. The documented counter is the two-step computed form,
 *     which for this value means something like `m1 = 2; m1 >>= 1;`. NOT YET
 *     TESTED -- it is the cheapest untried probe here and the one most likely to
 *     generalise, because the same gate governs `m0 = 0xfe`.
 *   - the flicker loop holds `*(unsigned char **)(ap + 0x50)` in r0 where the
 *     reference uses r1; register choice only, 6 encodings over 3 hunks.
 *
 * NEXT, in order: (1) the pool dump placement; (2) move the <<16 into the two
 * definitions; (3) the two-step form for `m1` and `m0`. Do NOT reach for a
 * scheduling flag for the pool: no per-file Makefile override applies to this
 * stem, and the pool position follows from where the constants are first needed.
 * -fno-rerun-cse-after-loop is NOT tested here and is not cited; this function
 * does have loops, so unlike OvlFunc_899_200b6f8 it is a legitimate probe, but
 * an untested flag does not belong in a park's notes.
 */
extern unsigned char *iwram_3001ecc;

#define REG_BLDCNT   (*(volatile unsigned short *)0x04000050)
#define REG_BLDALPHA (*(volatile unsigned short *)0x04000052)

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern void __FieldMove(int a);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetExtra(int slot, int v);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8078a08(int a);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_8097174(void);
extern void __Func_809728c(void);
extern unsigned char gScript_957__0200c478[];
extern unsigned char gScript_957__0200c4c8[];
extern unsigned char gScript_957__0200c518[];
extern unsigned char gScript_957__0200c57c[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_957_20093f8(void)
{
    unsigned char *ap;
    unsigned char *bp;
    unsigned char *bq;
    volatile unsigned short *pc0;
    volatile unsigned short *pa;
    int c0, i, i2, msk, m0, m1, x, zz, bl0;

    __SetFlag(0x962);
    __Func_8078a08(0xed);
    __CutsceneStart();
    __Func_808e118();
    { PIN2; q1 = 2; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __MessageID(0x2183);
    __ActorMessage(8, 0);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0xe8, 0xa0);
    { PIN3; q1 = 0xc0 << 8; q2 = 0; q0 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    __CutsceneWait(0xa);
    { PIN4; q3 = 1; q0 = 0x84 << 17; q1 = -(1); q2 = 0xc8 << 16; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x84 << 1; q2 = 0xd0; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 1; q1 = -(0x10); q2 = 0x10; q3 = 0xc0 << 8; __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0; q2 = 0x10; q3 = 0xc0 << 8; __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q3 = 0xc0 << 8; q1 = 0x10; q2 = 0x10; q0 = 2; __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(1);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 8; q1 = 0x84 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 8; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    ap = __MapActor_GetActor(8);
    __CutsceneWait(0x1e);
    { PIN2; q1 = 2; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 2; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 2; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x3c);
    __PlaySound(0x11);
    *(short *)(*(unsigned char **)(ap + 0x50) + 0x1e) = 0;
    __MapActor_Jump(8, 0xa, 0x46);
    msk = ~0xc;
    i = 0x1d;
    do {
        bp = *(unsigned char **)(ap + 0x50);
        bp[9] = msk & bp[9];
        __CutsceneWait(2);
        bp = *(unsigned char **)(ap + 0x50);
        bp[9] = (msk & bp[9]) | 8;
        __CutsceneWait(2);
        i--;
    } while (i >= 0);
    __CutsceneWait(0x28);
    x = *(short *)(__MapActor_GetActor(8) + 0xa);
    zz = *(short *)(__MapActor_GetActor(8) + 0x12);
    { PIN3; q1 = 0; q0 = 8; q2 = 0; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = x << 16; q2 = zz << 16; q0 = 9; __MapActor_SetPos(q0, q1, q2); }
    *(*(unsigned char **)(ap + 0x50) + 0x26) = 0;
    __PlaySound(0x1e);
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(9, 0x20, 0x20);
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    m0 = 0xfe;
    bp = __MapActor_GetActor(0) + 0x5a;
    *bp = m0 & *bp;
    bp = __MapActor_GetActor(1) + 0x5a;
    *bp = m0 & *bp;
    bp = __MapActor_GetActor(3) + 0x5a;
    *bp = m0 & *bp;
    bp = __MapActor_GetActor(2) + 0x5a;
    m0 &= *bp; *bp = m0;
    { PIN3; q1 = 0; q0 = 0; q2 = 0x10; __Func_80922c4(q0, q1, q2); }
    __Func_80922c4(1, 0, 0x10);
    __Func_80922c4(3, 0, 0x10);
    { PIN3; q2 = 0x10; q1 = 0; q0 = 2; __Func_8092304(q0, q1, q2); }
    m1 = 1;
    bp = __MapActor_GetActor(0) + 0x5a;
    *bp |= m1;
    bp = __MapActor_GetActor(1) + 0x5a;
    *bp |= m1;
    bp = __MapActor_GetActor(3) + 0x5a;
    *bp |= m1;
    bp = __MapActor_GetActor(2) + 0x5a;
    m1 |= *bp; *bp = m1;
    { PIN2; q1 = 1; q0 = 0; __MapActor_SetAnim(q0, q1); }
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(3, 1);
    { PIN2; q1 = 1; q0 = 2; __MapActor_SetAnim(q0, q1); }
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x105; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(1, 4, 0xd);
    { PIN3; q2 = 0x1e; q0 = 1; q1 = 4; __MapActor_Jump(q0, q1, q2); }
    __ActorMessage(1, 0);
    __Func_809280c(2, 1, 0x1e);
    { PIN3; q2 = 0x28; q0 = 2; q1 = 0x105; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __Func_809280c(1, 2, 0x1e);
    { PIN3; q2 = 0x28; q0 = 1; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    { PIN2; q1 = 4; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __Func_8092c40(q0, q1); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    c0 = __Func_8091c7c(0, 0);
    if (c0 == 0) {
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __MessageID(0x218a);
    __ActorMessage(9, 0);
    } else {
    __MessageID(0x218b);
    __ActorMessage(9, 0);
    }
    __MessageID(0x218c);
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 3; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 2; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(2, 0, -(0x30));
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_8096fb0(0x8d, 1);
    { PIN2; q1 = 9; q0 = 2; __Func_80970f8(q0, q1); }
    __Func_809728c();
    __FieldMove(1);
    __CutsceneWait(0x96);
    __FieldMove(2);
    __Func_8097174();
    { PIN2; q1 = 1; q0 = 2; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x28);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 3; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 2; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81 << 1; q2 = 0x28; q0 = 3; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 7; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __MapActor_Jump(9, 4, 0xd);
    __MapActor_Jump(9, 4, 0x1e);
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0x32; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x80 << 7; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x80 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 3; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0x32);
    { PIN2; q1 = 3; q0 = 0; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __CutsceneWait(0x14);
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0; q2 = 0x30; q0 = 2; __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0 << 8; q2 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 4; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 2; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 1; q2 = 0x50; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 2; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 3; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    c0 = __Func_8091c7c(0, 0);
    if (c0 == 0) {
    __MessageID(0x21a4);
    __ActorMessage(1, 0);
    } else {
    __MessageID(0x21a5);
    __ActorMessage(1, 0);
    }
    __MessageID(0x21a6);
    __CutsceneWait(0xa);
    { PIN3; q0 = 9; q1 = 0x80 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x80 << 7; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x81 << 1; q2 = 0x28; q0 = 1; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 3);
    { PIN2; q1 = 3; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __Func_8092c40(q0, q1); }
    __MessageID(0x21a8);
    __Func_8091c7c(0, 0);
    bq = *(unsigned char **)&iwram_3001ecc;
    *(short *)(bq + 0x52a) = 0x20;
    bl0 = 0x3f42;
    pc0 = &REG_BLDCNT;
    pa = &REG_BLDALPHA;
    i = 0; i2 = 0x10;
    do {
        *pc0 = bl0;
        *pa = (i << 8) | i2;
        i++;
        __WaitFrames(7);
        i2--;
    } while (i <= 0x10);
    *(short *)(bq + 0x536) = 0x3f3f;
    { PIN3; q0 = 0xa; q1 = 0x8c << 17; q2 = 0x98 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0x98 << 17; q0 = 0xb; q1 = 0x94 << 17; __MapActor_SetPos(q0, q1, q2); }
    __ActorMessage(0xa, 0);
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 3; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 7; q0 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __CutsceneWait(0xa);
    { PIN2; q1 = 1; q0 = 0xa; __SetCameraTarget(q0, q1); }
    __Func_8093530();
    __CutsceneWait(0x46);
    { PIN3; q2 = 0x28; q0 = 0xb; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __MapActor_SetExtra(0, 0xa);
    __MapActor_SetExtra(1, 0xa);
    __MapActor_SetExtra(3, 0xa);
    __MapActor_SetExtra(2, 0xa);
    { PIN3; q0 = 0xa; q1 = 0x16666; q2 = 0xb333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xb333; q0 = 0xb; q1 = 0x16666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0xa; q1 = &gScript_957__0200c478; __MapActor_SetBehavior(q0, q1); }
    __CutsceneWait(3);
    { PIN3; q2 = 0; q0 = 0xb; q1 = -(0x10); __Func_8092304(q0, q1, q2); }
    { PIN2; q1 = &gScript_957__0200c478; q0 = 0xb; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __MapActor_WaitScript(0xa);
    { PIN3; q1 = 0; q2 = -(0x10); q0 = 0xa; __Func_8092304(q0, q1, q2); }
    __MapActor_WaitScript(0xb);
    __Func_8092adc(0xa, 0, 0);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(3);
    __MapActor_SetIdle(2);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN4; q3 = 1; q2 = 0xd8 << 16; q1 = -(1); q0 = 0xf8 << 16; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 0xa; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 6; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x32);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 0xb; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 0xa; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(9, 0);
    { PIN3; q2 = 0; q1 = 0x10; q0 = 0xb; __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 2; q0 = 0xb; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    { PIN3; q2 = 0; q1 = 0x10; q0 = 0xa; __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x80 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __MapActor_Jump(0xb, 4, 0xd);
    { PIN3; q2 = 0x1e; q0 = 0xb; q1 = 4; __MapActor_Jump(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 0xa; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 6; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80 << 6; q2 = 0; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80 << 6; q2 = 0; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x46);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 0xb; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x81 << 1; q2 = 0x28; q0 = 9; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(0xa, 0xb, 0x46);
    { PIN3; q0 = 0xa; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x80 << 6; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 0xa; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 0xb; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 0xb; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 0xb; q1 = 0x84 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 2; q0 = 0xa; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 9; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xa, 3);
    { PIN2; q1 = 3; q0 = 0xb; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x28);
    __MapActor_SetExtra(0, 9);
    __MapActor_SetExtra(1, 9);
    __MapActor_SetExtra(3, 9);
    __MapActor_SetExtra(2, 9);
    { PIN3; q0 = 9; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(9, -(0x10), 0);
    __Func_80922c4(0xa, 0, 0x30);
    __Func_80922c4(0xb, 0, 0x30);
    __Func_8092304(9, -(0x10), 0);
    { PIN3; q2 = 0x20; q0 = 9; q1 = 0; __Func_8092304(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 1);
    { PIN2; q1 = 1; q0 = 0xb; __MapActor_SetAnim(q0, q1); }
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(3);
    __MapActor_SetIdle(2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 9; q1 = 0x80 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 2; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x1e; q0 = 1; q1 = 0; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x1e; q0 = 1; q1 = 9; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x46);
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0; q1 = 0x80 << 7; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = &gScript_957__0200c4c8; q0 = 0xb; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    { PIN2; q1 = &gScript_957__0200c518; q0 = 0xa; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    { PIN2; q1 = &gScript_957__0200c57c; q0 = 9; __MapActor_SetBehavior(q0, (unsigned char *)q1); }
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    { PIN3; q2 = 0; q1 = 2; q0 = 3; __Func_8092848(q0, q1, q2); }
    __MapActor_WaitScript(9);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 7; q2 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN4; q3 = 1; q1 = -(1); q2 = 0xf8 << 16; q0 = 0xf8 << 16; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 0xa; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80 << 8; q2 = 0; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 0xb; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0x107; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81 << 1; q2 = 0x28; q0 = 0xb; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_809280c(0xa, 0xb, 0x3c);
    { PIN3; q2 = 0; q1 = 0x80 << 8; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 0xa; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x28);
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 9; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN3; q2 = 0; q1 = 0; q0 = 9; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0xa, 3);
    { PIN2; q1 = 3; q0 = 0xb; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x28);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xa; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0; q2 = 0; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80922c4(9, 0x20, 0);
    __Func_80922c4(0xb, 0, 0x40);
    __Func_8092304(0xa, 0x10, 0);
    { PIN3; q1 = 0; q2 = 0x40; q0 = 0xa; __Func_80922c4(q0, q1, q2); }
    __MapActor_WaitMovement(9);
    __Func_8092304(9, 0, 0x40);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    { PIN3; q2 = 0; q0 = 0xb; q1 = 0; __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x14);
    bl0 = 0x3f42;
    pc0 = &REG_BLDCNT;
    pa = &REG_BLDALPHA;
    i = 0; i2 = 0x10;
    do {
        *pc0 = bl0;
        *pa = (i2 << 8) | i;
        i++;
        __WaitFrames(7);
        i2--;
    } while (i <= 0x10);
    *(short *)(bq + 0x52a) = 5;
    *(short *)(bq + 0x536) = 0x1f;
    __WaitFrames(1);
    *pc0 = 0x3f42;
    pc0[1] = 0xc04;
    __Func_8093500(0, 1);
    __Func_8093530();
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 3; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q1 = 0; q0 = 1; __Func_809280c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    __Func_809280c(3, 0, 0);
    __Func_809280c(2, 0, 0);
    c0 = __Func_8091c7c(0, 0);
    if (c0 == 0) {
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __MessageID(0x21ce);
    __ActorMessage(1, 0);
    } else {
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 1; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __MessageID(0x21cf);
    __ActorMessage(1, 0);
    }
    __MessageID(0x21d0);
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x28; q0 = 1; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 2; q0 = 3; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 3; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x28; q0 = 1; q1 = 0x80 << 1; __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    c0 = __Func_8091c7c(0, 0);
    if (c0 == 0) {
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __MessageID(0x21d7);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    } else {
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 1; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    __MessageID(0x21d9);
    { PIN2; q1 = 0; q0 = 1; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0xc0 << 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 2; __ActorMessage(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 3; q0 = 0; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x46; __MapActor_Emote(q0, q1, q2); }
    }
    __PlaySound(0x11);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    bp = __MapActor_GetActor(0);
    if (bp != 0)
        __MapActor_TravelTo(1, *(short *)(bp + 0xa), *(short *)(bp + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    bp = __MapActor_GetActor(0);
    if (bp != 0)
        __MapActor_TravelTo(2, *(short *)(bp + 0xa), *(short *)(bp + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    bp = __MapActor_GetActor(0);
    if (bp != 0)
        __MapActor_TravelTo(3, *(short *)(bp + 0xa), *(short *)(bp + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __PlayMapMusic();
    __CutsceneEnd();

}
