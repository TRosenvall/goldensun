/* OvlFunc_884_20097c8 -- NON-MATCHING, 276 of 1082
 *
 * 276 is tools/objcmp.py's PRODUCTION-FLAG differing-encodings figure for the
 * body below (ref 1082 encodings, ours 1084).  No flag was used: objcmp's own
 * cflags_for() reports adjust=[] for this file, so the production flag set is
 * the plain one and the figure needs no qualifier.  aligncmp: 1053 aligned-equal,
 * 97.3% of ref, 37 differing/ins/del in 27 hunks.
 *
 * Source asm: asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c_b.s
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_784360/20097c8.c \
 *     asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c_b.s --func OvlFunc_884_20097c8
 *
 * SHAPE.  1048 instructions, 293 calls, 3 labels -- and TWO of the three are
 * POOL SKIPS (`b .L1c3c` and `b .L20a4` each sit immediately before a
 * `.pool_aligned`), so the only real control flow in the whole function is one
 * `if (actor != 0)` at ref 355.  Work density 2.9%: of 1048 instructions, 701
 * are argument fill, 293 are calls, 6 prologue, 14 copies, 4 branch, and only
 * THIRTY are work.  That makes it the lightest function of batch 313's four by
 * a factor of three (see the triage correction at the foot of this file).
 *
 * SPLIT SHAPE: NONE NEEDED.  The .s holds exactly ONE function
 * (`grep -c thumb_func_start` = 1), `tools/datacheck.py` is silent, and
 * `tools/shimcount.py` reports no shims.  This is a whole-file conversion;
 * split_s.py is not required (dry-run only was used, and nothing was cut).
 *
 * FRAME, ALL FOUR GREPS (docs/elevation.md's corrected triad + sp0):
 *   sub sp,#imm        1   `sub sp, #8`
 *   (add|sub) sp, rN   0   -- so the 8 bytes are the real frame, not a 508+ one
 *   mov rX, sp         0
 *   add rX, sp, #K     0   -- no stack aggregate
 *   str rX,[sp] off 0  1   with NO matching load  => outgoing argument space
 * The 8 bytes are entirely outgoing arguments for the one 6-argument call,
 * __CopyMapTiles (args 5 and 6 at [sp] and [sp,#4]).  Nothing is spilled.
 *
 * POOLED MULTISET: 28 distinct values, 49 load sites, 34 pool words.
 *   21 numeric: 0x101 x7, 0x26666 x5, 0x6666 x4, 0xcccc x3, 0x13333 x3,
 *     0x105 x2, 0x26a x2, 0x276 x2, 0x1001e x2, and 0x103, 0xccc, 0x11fa,
 *     0x1214, 0x4ccc, 0x1cccc, 0x39999, 0x4cccc, 0x26a0000, 0x3210000,
 *     0x32e0000, 0x3890000 once each.
 *   7 symbolic: iwram_3001ebc and gScript_884__0200ac00 / ac14 / ac90 / acf8 /
 *     ad74 / adf0.
 * OUR CONSTANT SET IS THE REFERENCE'S, ENTRY FOR ENTRY -- 28 distinct on both
 * sides, nothing in one and not the other.  Recording that explicitly because
 * the reference writes hex and gcc writes decimal, so an UN-NORMALISED diff of
 * the two pool lists reports all 28 as differing on both sides, which is
 * indistinguishable from having reconstructed the wrong constants.  Normalise
 * numerically (docs/elevation.md, batch 312's normalisation trap).
 *
 * THE EIGHT-BIT-MOVABLE-POOLED-CONSTANT SCREEN IS INERT HERE.  Smallest pooled
 * numeric value is 0x101: ZERO candidate sites, so no symbol spelling is in
 * question and no `.sym` entry is implied.  Fourth consecutive population in
 * which that sound mechanism has no shape to act on.
 *
 * ================= WHAT THE FUNCTION ACTUALLY WANTS =================
 *
 * TWO MECHANISMS, PULLING OPPOSITE WAYS, AND BOTH ARE NEEDED.
 *
 * (1) EIGHT NAMED LONG-LIVED CONSTANTS -- not five.
 * docs/ANALYSIS_OvlFunc_884_20097c8.c prescribes "FIVE NAMED LONG-LIVED
 * CONSTANTS, one per range", listing the r8/r11/r9/r10/r8' builds.  THAT COUNT
 * IS LOW BY THREE: it restricted itself to r8-r11 and missed the three held in
 * the LOW callee-saved registers.  The discriminator is cheap and exact --
 * BUILD MULTIPLICITY.  Every wide constant that lands in a callee-saved
 * register is built EXACTLY ONCE; every one that goes straight to an argument
 * register is built once PER SITE:
 *
 *   built once, held            built per site (n>1), NOT named
 *   r8  0xe0<<7 = 0x7000        0xf000  x3
 *   r11 0x80<<5 = 0x1000        0x4000  x3
 *   r7  0xc0<<6 = 0x3000        0x8000  x3
 *   r9  0xa0<<7 = 0x5000        0x100   x3
 *   r6  0x90<<8 = 0x9000        0x102   x9
 *   r10 0xc0<<8 = 0xc000        0x320   x3
 *   r8  0xb0<<8 = 0xb000        0x6000, 0x108, 0x110, 0x330 x1
 *   r5  0xd0<<8 = 0xd000
 *
 * That is a BETTER instrument than the "survives a call" heuristic for this
 * population: it is one pass over the reference, it needs no live-range
 * reasoning, and it gets the band doc's "hold the reference's NUMBER of
 * long-lived quantities" right on the first try.  Each is written in the
 * REG_N_SETS two-set form (`c7 = 0xe0; c7 <<= 7;`) at the reference's own
 * build point, so it earns no REG_EQUIV and keeps its register.
 *
 * (2) A BLANKET PIN PASS OVER THE REMAINING WIDE-LITERAL SITES.
 * With (1) alone the histogram is `mov +17 / ldr -14` -- gcc commons SIX pooled
 * values that the ROM reloads at every site.  Exactly six, accounting for all
 * fourteen missing `ldr`, with the other twenty-two values matching the ROM's
 * load count exactly:
 *       0x101   ref 7 loads, ours 1   -6
 *       0x26666 ref 5 loads, ours 1   -4
 *       0x1001e ref 2, 0x26a ref 2, 0x276 ref 2, 0x105 ref 2  -1 each
 * A pin's destination is a call-clobbered hard register, so a pinned site
 * REBUILDS -- which is precisely the lever those six need, and is the second
 * half of the ANALYSIS doc's own prescription ("pins only at the sites not
 * covered by (1)").
 *
 * ========== THE PIN PASS IS BLANKET HERE, NOT SELECTIVE ==========
 *
 * Batch 312 brief A recorded that over CALL SITES a selective pass beats a
 * blanket one, and batch 313's brief propagated that as step 1 for this
 * population.  ON THIS FUNCTION IT IS FALSE, AND IT IS FALSE ON THE SELECTIVE
 * PASS'S OWN AXIS.  Measured, all five with the eight named constants in place:
 *
 *   variant                       sites pins insns  enc  objcmp aligned hunks histogram
 *   no pins                          0    0  +3    1084   859   85.5%  154  mov+17 ldr-14 add-1 lsl+1
 *   SELECTIVE (the six values only) 20   59  -10   1072   845   87.5%  121  lsl-7 mov-3 add-1 ldr+1
 *   blanket, int pin on the symbol  59  175  -1    1080   829   93.4%   67  mov-2 ldr+1
 *   blanket, symbol site UNPINNED   57  169  +1    1084  *276*  *97.3%*  27  mov+2 ldr-1
 *   blanket, symbol POSITION only   59  173  +0    1082   422   94.3%   59  *FLAT, 0 ragged*
 *
 * The selective pass is WORSE than blanket on every axis AND worse than NO
 * PINS on the instruction count (-10 against +3).  The mechanism is the one
 * brief B gave for constants, and it reaches further than that brief allowed:
 * ALLOCATION IS ZERO-SUM.  Pinning only the twenty sites that carry the six
 * commoned values removes those allocnos and hands the registers to the NEXT
 * pooled constants in global.c's priority order, which are then commoned
 * instead -- visible as `lsl -7`, seven constant builds that vanished into
 * reuse somewhere else entirely.
 *
 * SO THE SITE/CONSTANT DISTINCTION IS NOT THE WHOLE RECONCILIATION.  Batch 312
 * separated "select over call sites" from "do not select over constants".  The
 * missing qualifier: WHEN THE SITES ARE CHOSEN BY THE CONSTANTS THEY CARRY,
 * SELECTION OVER SITES *IS* SELECTION OVER CONSTANTS, and the zero-sum rule
 * applies.  Brief A's selective pass selected by a site PROPERTY (does this
 * call carry any wide literal); the pass above selected by constant IDENTITY.
 * Same name, opposite results, and the discriminator is what the predicate
 * reads, not what it is applied to.
 *
 * THE ONE SITE THAT MUST NOT BE PINNED IS A HELD POINTER, AND THAT IS LEVER 1.
 * The two `__Func_8092a1c(slot, 0x1001e, gScript_884__0200ac00)` sites are the
 * whole difference between the last three rows.  The ROM loads that symbol ONCE
 * and reuses it across both calls.  Pin the site and the symbol reloads twice;
 * the pin is the cause and TYPING IS NOT THE CURE -- giving the pin a
 * `register unsigned char *` slot removes the four int/pointer warnings and is
 * BYTE-IDENTICAL in every figure (829 / -1 / 2 ragged, unchanged).  A pin's
 * destination is call-clobbered whatever its type.  Leaving the site unpinned
 * costs the 0x1001e rebuild (-1 ldr) and buys 553 differing positions
 * (829 -> 276).  Pointer reuse paying far more than a constant rebuild is
 * exactly Lever 1's POINTER-versus-CONSTANT discriminator, here as a direct
 * A/B on one call site.
 *
 * ========== BOUNDS MEASURED BYTE-IDENTICAL OR NEGATIVE ==========
 *
 *   * A ONE-SET LOCAL DOES NOT DEFEAT THE COMMONING.  Writing the 0x1001e
 *     argument as a fresh one-set local immediately before each call -- the
 *     REG_N_SETS lever's own shape, and its documented direction -- is
 *     BYTE-IDENTICAL to the bare literal (276 / +1 / 2 ragged, and the two .s
 *     files differ only in `.file`).  So REG_N_SETS governs whether a value
 *     EARNS a register; it does NOT govern whether cse1 commons a POOLED
 *     constant across two call sites.  Only a pin does.  A bound on that
 *     lever's reach from a direction the doc has not recorded.
 *
 *   * COMMUTATIVE OPERAND ORDER AT THE `&= 0xfe` / `|= 1` SITES IS INERT --
 *     ALL SIXTEEN COMBINATIONS BYTE-IDENTICAL.  This function has four such
 *     sites (two `and` at ref 366-377, two `orr` at ref 967-979), the same
 *     family that src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_b.c records
 *     as load-bearing ("the inline expression gets `and` right and `orr`
 *     wrong").  All 2^4 = 16 spellings of `m & *p` / `*p & m` and `one | *p` /
 *     `*p | one` give the SAME object: 276 of 1082, +1 instruction, 2 ragged
 *     entries, and w_1111 / w_0101 / w_1010 verified byte-identical to w_0000.
 *     The sibling's result is not contradicted, it is BOUNDED: there `p` was
 *     named at ONE of the two sites and the pointer setup was what moved.
 *     With `p` named at ALL FOUR sites, operand order carries nothing.  This
 *     extends batch 311's `mul`-operand bound to `and` and `orr`.
 *
 *   * NO FLAG WAS TRIED, DELIBERATELY.  The ANALYSIS doc records twelve flag
 *     settings measured against this exact mechanism in batch 307 with none
 *     reaching it, and names `-ffixed-r8..r11` as a RETRACTED theory that masks
 *     the commoning rather than preventing it.  band-800plus.md section 7's
 *     retraction stands.  The figure above is therefore a clean production-flag
 *     figure with no flag qualifier, which is what the headline requires.
 *
 * ========== THE REMAINING 37 POSITIONS IN 27 HUNKS ==========
 *
 * Read, not inferred.  They are a handful of distinct causes, not 37:
 *   a. ref 12-15: the __CopyMapTiles stack pair.  ROM materialises BOTH
 *      outgoing words before storing (`mov r3,#3 / mov r2,#1 / str r3,[sp] /
 *      str r2,[sp,#4]`); we reuse r3 for both and store one at a time.  Two
 *      positions.  Try pinning the 6-argument site, or two adjacent named
 *      locals for the stack words.
 *   b. ref 196-202: register NUMBERING around the held gScript pointer -- we
 *      put it in r6 and the slot constant in r5, the ROM the other way round.
 *      Six positions, one decision.
 *   c. ref 365-376 and 986-996: the and/orr destination choices.  NOT
 *      reachable by operand order (bound above); this is which of `p`, the
 *      loaded byte and the constant gets the destination, and the sibling's
 *      note says the lever is WHERE `p` IS NAMED, not how the expression reads.
 *      Name `p` at a SUBSET of the four sites -- that is the untried axis.
 *   d. one pool WORD ORDER difference (0x1001e one slot early) and one
 *      `movs r0,#180` ordering at ref 565.  Four positions, and pool order is
 *      a consequence of (a)-(c), not an independent target.
 *
 * NEXT, in order: (c) with `p` named at one, two and three of the four sites;
 * then (a); then (b).  Do NOT spend a pass on flags or on operand order -- both
 * are bounded above with figures.
 *
 * ========== CORRECTION TO BATCH 313's TRIAGE ==========
 *
 * The brief ranked batch 313's four at work densities 3.4 / 4.0 / 4.5 / 4.9%
 * and called `OvlFunc_896_200a7f8` the easiest.  Re-measured with a dataflow
 * classifier (a constant-origin set per basic block, so an `lsl` whose source
 * register was `mov`-set three instructions earlier counts as argument fill
 * rather than work -- these scripts interleave the fills, so an ADJACENCY test
 * misreads ~170 of them as work):
 *
 *   function            insns  argfill  calls  WORK  work%  genuine copies
 *   OvlFunc_884_20097c8  1048     701    293    30   2.9%        14
 *   OvlFunc_969_20092c8  1361     860    363    92   6.8%        28
 *   OvlFunc_883_200bfb0  2126    1414    513   134   6.3%        44
 *   OvlFunc_896_200a7f8  2227    1464    556   142   6.4%        42
 *
 * 884 is the easiest by a factor of THREE in work instructions, and 896 -- the
 * brief's easiest -- is the HARDEST, with 142 work instructions against 884's
 * 30.  The other three cluster indistinguishably at 6.3-6.8%, so work density
 * does not separate THEM at all; instruction count does.  Also: the brief's
 * `mov rlo,rhigh` column (29/28/27/25) counts constant-moves too; the genuine
 * non-constant copy counts are 42/14/44/28, and 884's is half what the brief
 * reported.  Ranking a call-script population needs the dataflow pass, not a
 * peephole.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetExtra(int slot, int v);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __ActorMessage(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_801776c(int a, int b);
extern void __Func_807808c(int n);
extern void __Func_8091a58(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *s);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern void OvlFunc_884_200a2c8(int a, int b);
extern void OvlFunc_884_200a2e0(int a, int b, int c);

extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_884__0200ac00[];
extern unsigned char gScript_884__0200ac14[];
extern unsigned char gScript_884__0200ac90[];
extern unsigned char gScript_884__0200acf8[];
extern unsigned char gScript_884__0200ad74[];
extern unsigned char gScript_884__0200adf0[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_884_20097c8(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    unsigned char m;
    unsigned char one;
    int c7, c1, c3, c5, c9, cc, cb, cd;

    __Func_807808c(1);
    __CutsceneStart();
    __CopyMapTiles(0x2a, 0x35, 0x2a, 0x36, 3, 1);
    { PIN4; q0 = 0xb40000; q1 = 0x100000; q2 = 0x26a0000; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1a), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1d), 0);
    __Func_8092b08(0, 1);
    __Func_8092b08(1, 1);
    __Func_8092b08(0x11, 1);
    __Func_8092b08(0x10, 1);
    __Func_8092b08(0xf, 1);
    { PIN3; q0 = 0; q1 = 0xd00000; q2 = 0x32e0000;
      __MapActor_SetPos(q0, q1, q2); }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x50);
    { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    c7 = 0xe0; c7 <<= 7;
    OvlFunc_884_200a2e0(0xc, c7, 0x14);
    { PIN1; q0 = 0x11fa;
      __MessageID(q0); }
    OvlFunc_884_200a2c8(0xc, 0xa);
    { PIN3; q0 = 0xb; q1 = 0x102; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    c1 = 0x80; c1 <<= 5;
    OvlFunc_884_200a2e0(0xb, c1, 0xa);
    OvlFunc_884_200a2c8(0xb, 0xa);
    __MapActor_DoAnim(0xc, 3);
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 2);
    OvlFunc_884_200a2c8(0xb, 0xa);
    { PIN3; q0 = 0xc; q1 = 0x100; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    c3 = 0xc0; c3 <<= 6;
    { PIN3; q0 = 0xc; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xb8; q2 = 0x26a;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_884_200a2e0(0xc, c3, 0x3c);
    OvlFunc_884_200a2c8(0xc, 0x14);
    { PIN3; q0 = 0xb; q1 = 0x10000; q2 = 0x8000;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa8; q2 = 0x26a;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xf000; q2 = 0xa;
      OvlFunc_884_200a2e0(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 4);
    OvlFunc_884_200a2c8(0xb, 0x14);
    OvlFunc_884_200a2e0(0xc, c7, 0xa);
    __Func_80925cc(0xc, 1);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1e; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1e; q1 = 0x6e0000; q2 = 0x2e80000;
      __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(2);
    __MapActor_SetAnim(0x1e, 3);
    __MapActor_SetBehavior(0x1e, gScript_884__0200ac14);
    __CutsceneWait(0x28);
    __Func_8092a1c(0xb, 0x1001e, gScript_884__0200ac00);
    __Func_8092a1c(0xc, 0x1001e, gScript_884__0200ac00);
    __MapActor_WaitScript(0x1e);
    __MapActor_SetIdle(0xb);
    __MapActor_SetIdle(0xc);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xb; q1 = 0x105; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x105; q2 = 0x78;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xb, c1, 0);
    OvlFunc_884_200a2e0(0xc, c7, 0x50);
    c5 = 0xa0; c5 <<= 7;
    OvlFunc_884_200a2e0(0xb, c5, 0x28);
    OvlFunc_884_200a2e0(0xb, c1, 0x14);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    OvlFunc_884_200a2e0(0xc, c5, 0x3c);
    OvlFunc_884_200a2e0(0xc, c3, 0x28);
    OvlFunc_884_200a2e0(0xc, c5, 0x3c);
    { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xb, c3, 0);
    { PIN3; q0 = 0xc; q1 = 0xb8; q2 = 0x276;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_884_200a2e0(0xc, c3, 0x14);
    OvlFunc_884_200a2e0(0xc, c5, 0x14);
    OvlFunc_884_200a2e0(0xc, c3, 0x14);
    { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa8; q2 = 0x276;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_884_200a2e0(0xb, c3, 0x28);
    OvlFunc_884_200a2e0(0xb, c5, 0x28);
    OvlFunc_884_200a2e0(0xb, c3, 0x28);
    { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_884_200a2e0(0xb, c1, 0xa);
    OvlFunc_884_200a2c8(0xb, 0x14);
    __MapActor_DoAnim(0xc, 3);
    OvlFunc_884_200a2c8(0xc, 0xa);
    { PIN3; q0 = 0xb; q1 = 0x100; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_884_200a2e0(0xb, c5, 0x14);
    OvlFunc_884_200a2e0(0xb, c3, 0x14);
    OvlFunc_884_200a2e0(0xb, c5, 0x14);
    OvlFunc_884_200a2e0(0xb, c5, 0x3c);
    __MapActor_DoAnim(0xb, 3);
    OvlFunc_884_200a2c8(0xb, 0xa);
    a = __MapActor_GetActor(0x1e);
    if (a != 0)
        __MapActor_SetPos(0x1f, *(int *)(a + 8), *(int *)(a + 0x10));
    __WaitFrames(2);
    m = 0xfe;
    p = __MapActor_GetActor(0x1e) + 0x23;
    *p = m & *p;
    p = __MapActor_GetActor(0x1f) + 0x23;
    *p = m & *p;
    __Func_8092b08(0x1e, 2);
    __Func_8092b08(0x1f, 2);
    { PIN3; q0 = 0x1f; q1 = 0x39999; q2 = 0x1cccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x1f, 2);
    __MapActor_SetBehavior(0x1f, gScript_884__0200ac90);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x1e, 3);
    { PIN3; q0 = 0x1e; q1 = 0x4cccc; q2 = 0x26666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_RunScript(0x1e, gScript_884__0200ac90);
    __CutsceneWait(0x3c);
    __Func_80925cc(0xc, 2);
    OvlFunc_884_200a2e0(0xc, c7, 0xa);
    OvlFunc_884_200a2c8(0xc, 0xa);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    OvlFunc_884_200a2e0(0xb, c1, 0xa);
    __MapActor_DoAnim(0xb, 3);
    OvlFunc_884_200a2c8(0xb, 0x14);
    __MapActor_SetAnim(0xc, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0xb, gScript_884__0200acf8);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x26666; q1 = 0x4ccc;
      __Func_80933d4(q0, q1); }
    p = __Func_8093554() + 0x55;
    *p = 0;
    { PIN4; q0 = 0xd70000; q1 = 0x100000; q2 = 0x3210000; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0xc, gScript_884__0200ad74);
    __MapActor_WaitScript(0xc);
    OvlFunc_884_200a2e0(0xc, c3, 0x78);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    c9 = 0x90; c9 <<= 8;
    OvlFunc_884_200a2c8(0xd, 0x14);
    __Func_8092adc(0, 0, 0);
    OvlFunc_884_200a2e0(1, c9, 0x14);
    cc = 0xc0; cc <<= 8;
    OvlFunc_884_200a2e0(0, cc, 0xa);
    cb = 0xb0; cb <<= 8;
    OvlFunc_884_200a2e0(1, cb, 0xa);
    __MapActor_SetAnim(0, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x28);
    __Func_80925cc(0x10, 2);
    __CutsceneWait(0x14);
    OvlFunc_884_200a2c8(0x10, 0xa);
    { PIN3; q0 = 0x10; q1 = 0x10000; q2 = 0x8000;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xd8; q2 = 0x320;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x4000; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8091a58(0xb4, 0);
    { PIN3; q0 = 0x10; q1 = 0x108; q2 = 0x320;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x6000; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x102; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xf000; q2 = 0xa;
      OvlFunc_884_200a2e0(q0, q1, q2); }
    OvlFunc_884_200a2c8(1, 0xa);
    __MapActor_DoAnim(0x11, 4);
    OvlFunc_884_200a2c8(0x11, 0xa);
    __Func_8092adc(1, c1, 0);
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(1, 4, 0x3c);
    cd = 0xd0; cd <<= 8;
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    OvlFunc_884_200a2e0(0xe, cd, 0xa);
    OvlFunc_884_200a2c8(0xe, 0x3c);
    { PIN3; q0 = 1; q1 = 0x102; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x102; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0x102; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0x102; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x102; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0x100; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_884_200a2c8(0x11, 0x3c);
    __MapActor_SetExtra(0, 0x11);
    __MapActor_SetExtra(1, 0x11);
    { PIN3; q0 = 0x11; q1 = 0xd8; q2 = 0x320;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0x4000; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_884_200a2c8(0x11, 0x3c);
    __Func_8091a58(0xcf, 0);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    { PIN3; q0 = 0x11; q1 = 0x110; q2 = 0x330;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0x8000; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 2);
    OvlFunc_884_200a2e0(1, c9, 0xa);
    OvlFunc_884_200a2c8(1, 0xa);
    __Func_8092adc(0xe, c3, 0);
    OvlFunc_884_200a2e0(0, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x10, 1);
    OvlFunc_884_200a2c8(0x10, 0xa);
    { PIN3; q0 = 1; q1 = 0xf000; q2 = 0xa;
      OvlFunc_884_200a2e0(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x10, 4);
    OvlFunc_884_200a2c8(0x10, 0xa);
    __MapActor_DoAnim(0x12, 3);
    __ActorMessage(0x12, 0);
    __Func_8092adc(1, cd, 0);
    __MapActor_SetAnim(0x12, 4);
    __ActorMessage(0x12, 0);
    __Func_809259c(0x12, 3);
    __ActorMessage(0x12, 0);
    __MapActor_DoAnim(0x10, 3);
    __MapActor_SetAnim(0x13, 3);
    __MapActor_SetAnim(0x11, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x18, 3);
    __MapActor_SetAnim(0x12, 3);
    __MapActor_SetAnim(0x1b, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x1c, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x19, 3);
    __MapActor_SetAnim(0x14, 3);
    __MapActor_DoAnim(0x15, 3);
    __MapActor_Jump(0xf, 2, 0xa);
    __MapActor_Jump(0xf, 4, 0x28);
    OvlFunc_884_200a2c8(0xf, 0xa);
    __Func_8092adc(1, cb, 0);
    OvlFunc_884_200a2e0(0, cc, 0x14);
    OvlFunc_884_200a2e0(0xf, cd, 0xa);
    OvlFunc_884_200a2c8(0xf, 0xa);
    OvlFunc_884_200a2e0(0xf, c9, 0x14);
    OvlFunc_884_200a2e0(0xf, c5, 0xa);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xe, 3);
    __MapActor_SetAnim(0x11, 3);
    __MapActor_SetAnim(0x14, 3);
    __MapActor_SetAnim(0x17, 3);
    __MapActor_SetAnim(0x1a, 3);
    __MapActor_SetAnim(0x1d, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(0xf, 3);
    __MapActor_SetAnim(0x12, 3);
    __MapActor_SetAnim(0x15, 3);
    __MapActor_SetAnim(0x18, 3);
    __MapActor_SetAnim(0x1b, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xd, 3);
    __MapActor_SetAnim(0x10, 3);
    __MapActor_SetAnim(0x13, 3);
    __MapActor_SetAnim(0x16, 3);
    __MapActor_SetAnim(0x19, 3);
    __MapActor_DoAnim(0x1c, 3);
    __CutsceneWait(0x50);
    __MapActor_Jump(0xb, 4, 0);
    __MapActor_Jump(0xe, 4, 0);
    __MapActor_Jump(0x11, 4, 0);
    __MapActor_Jump(0x14, 4, 0);
    __MapActor_Jump(0x17, 4, 0);
    __MapActor_Jump(0x1a, 4, 0);
    __MapActor_Jump(0x1d, 4, 0);
    __MapActor_Jump(0xc, 4, 0);
    __MapActor_Jump(0xf, 4, 0);
    __MapActor_Jump(0x12, 4, 0);
    __MapActor_Jump(0x15, 4, 0);
    __MapActor_Jump(0x18, 4, 0);
    __MapActor_Jump(0x1b, 4, 0);
    __MapActor_Jump(0xd, 4, 0);
    __MapActor_Jump(0x10, 4, 0);
    __MapActor_Jump(0x13, 4, 0);
    __MapActor_Jump(0x16, 4, 0);
    __MapActor_Jump(0x19, 4, 0);
    __MapActor_Jump(0x1c, 4, 0);
    { PIN2; q0 = 0x1214; q1 = 1;
      __Func_801776c(q0, q1); }
    __CutsceneWait(0x50);
    one = 1;
    p = __MapActor_GetActor(0) + 0x23;
    *p = *p | one;
    p = __MapActor_GetActor(1) + 0x23;
    *p = one | *p;
    { PIN3; q0 = 0; q1 = 0x102; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x102; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x4000; q2 = 0xa;
      OvlFunc_884_200a2e0(q0, q1, q2); }
    OvlFunc_884_200a2e0(1, c5, 0x14);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0, gScript_884__0200adf0);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x6666; q1 = 0xccc;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xd80000; q1 = 0x100000; q2 = 0x3890000; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(1, gScript_884__0200adf0);
    __CutsceneWait(0x3c);
    q = iwram_3001ebc;
    *(int *)(q + 0x1c0) = 0x100;
    *(int *)(q + 0x1c8) = 0x3c;
    __MapTransitionOut();
    __WaitMapTransition();
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __Func_8091e9c(0xa);
    __CutsceneEnd();
}
