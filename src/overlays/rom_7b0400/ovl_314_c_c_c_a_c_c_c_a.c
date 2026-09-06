// fakematch
/* OvlFunc_925_200af18 -- extracted from
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.s, where it is the FIRST of
 * FIVE functions (OvlFunc_925_200af18, _200b060, _200b1c0, _200b208,
 * _200b324).  The .s carries no `.data`, no `.incbin` and no `.section` other
 * than the implicit .text, so a split needs only the `(.text)` remap.
 *
 * VERDICT (objcmp, in-container, against the ORIGINAL asm/ path):
 *   OK OvlFunc_925_200af18 -- 328 bytes, 143 encodings and 18 relocations identical
 *
 * Two actors (slots 0x16 and 0x18) are told 0x100, hidden, then alternate
 * emitting a particle every other frame for 32 frames; at the end both are
 * placed at (0, 0).
 *
 * ------------------------------------------------------------------
 * TEMPLATE: THE SIBLING, AND WHICH OF ITS PREDICTIONS HELD
 * ------------------------------------------------------------------
 * OvlFunc_925_200b060 (function 2 of this same .s, batch 236) is the template.
 * solved_twins.py finds no hit and the opcode streams differ -- they are
 * SIBLINGS, not twins -- but the whole body outside the two __Func_8092950
 * calls is instruction-for-instruction the same shape, with two constants
 * swapped: af18 has `ldr =0xfff00000` on the vertical and `0x80 << 11` as the
 * fifth argument where b060 has `0x80 << 14` and `ldr =0xfffc0000`.
 *
 * The sibling made two predictions.  BOTH HELD, and one of them held in the
 * wrong direction:
 *
 *   "LEVER 1 (x2/y2) will be needed there too"  -- CORRECT and carried over
 *      unchanged.  The else-arm gets its own `x2`/`y2`; one shared pair is one
 *      pseudo spanning the whole if/else and gcc-2.96 has no live-range
 *      splitting.  Never measured here in the broken form, because the sibling
 *      had already measured it at 102 of 152.
 *
 *   "LEVER 2 will not -- af18's two 0x100 calls are straight-line with no
 *    branch to dominate, which is the boundary the basic-block lever genuinely
 *    cannot cross."  -- CORRECT ABOUT THE LEVER, WRONG ABOUT THE BOUNDARY.
 *      The basic-block lever is indeed unreachable, and so is every other
 *      ordinary spelling (table below).  But "no dominating branch" is not the
 *      end of the road: docs/elevation.md's "CORRECTED: a CALL-CLOBBERED PIN
 *      reaches this class without any branch" is exactly this case, and it
 *      names the remedy.  A prediction that a lever will not transfer is not a
 *      prediction that the function is blocked.
 *
 * Everything else the b060 header records -- `struct P` exactly 0x28 bytes so
 * `sub sp, #0x38` comes out, the 16.16 masking spelled `>> 16 << 16`,
 * __Random declared UNSIGNED so the shifts are `lsr`, the do/while counter
 * unsigned in a high register -- carried over untouched and is not restated.
 *
 * ------------------------------------------------------------------
 * THE ONLY RESIDUE: 0x100 BUILT TWICE IN ONE BASIC BLOCK
 * ------------------------------------------------------------------
 * The plain candidate is 143 encodings against 143 with 9 differing, all of
 * them in one window:
 *
 *   rom   mov r1,#0x80 / mov r0,#0x16 / lsl r1,#1 / bl __Func_8092950
 *         mov r1,#0x80 / lsl r1,#1 / mov r0,#0x18 / bl __Func_8092950
 *   ours  mov r5,#0x80 / lsl r5,#1   (hoisted above `mov r10, r0`)
 *         mov r0,#0x16 / mov r1,r5 / bl / mov r1,r5 / mov r0,#0x18 / bl
 *
 * Note what is ABSENT: no extra `push`.  The ROM already spends r5-r7 and
 * r8-r10, the hoisted value shares r5 with the loop's `x`, and 2 + 2 movs is
 * the same four instructions the ROM spends rebuilding.  So the cheapest tell
 * for this class -- "compare the prologues, a push the ROM does not have" --
 * IS SILENT HERE, and so is the length tell.  Only the diff text shows it.
 * Recording that because both recognisers are documented as the first thing to
 * reach for on a repeated constant.
 *
 * A perturb probe settles that this is the whole story: changing the second
 * call's constant to 0x81 << 1 drops the residue to 3 (the two constants
 * themselves plus one), i.e. nothing else in the function is wrong.
 *
 * ------------------------------------------------------------------
 * THE LEVER: A CALL-CLOBBERED PIN ON r1 AT THE FIRST SITE
 * ------------------------------------------------------------------
 * r0-r3 are call-clobbered, so a value pinned to r1 cannot survive a `bl` and
 * gcc has no pseudo left to common; the second site rebuilds on its own.
 * Pinning the FIRST occurrence is what matters -- cse1 substitutes into a
 * later launder's own initialiser before it can do anything, so a pin on site
 * 2 alone is worthless (measured: 9, bit-identical to no pin).
 *
 * The pin has TWO SEPARATE JOBS here and the teardown separates them cleanly:
 *
 *   r1 pin alone .......... kills the CSE outright.  Site 2 comes out exact.
 *                           Residue 2, and it is only site 1's interleave.
 *   r0 pin + split shift .. supplies the interleave.  The ROM writes
 *                           `mov r1,#0x80 / mov r0,#0x16 / lsl r1,#1`, so the
 *                           `p0 = 0x16;` statement has to sit BETWEEN
 *                           `p1 = 0x80;` and `p1 <<= 1;`.  Fold the constant
 *                           into one statement and there is nowhere for the
 *                           `mov r0` to go: 2 differing.
 *
 * Site 2's own order (`mov r1 / lsl r1 / mov r0`) is gcc's natural one and
 * needs no pin at all.  Pinning it as well is byte-identical -- INERT
 * SCAFFOLDING, so it is not in the file.
 *
 * ------------------------------------------------------------------
 * MEASURED (all 142 lines; tryc `--quiet`, default flag group)
 * ------------------------------------------------------------------
 *   plain literals, default flags                                    9
 *   ... + named k1/k2 assigned AT the site                           9
 *   ... + `k = 0x80; k <<= 1;` split-build, one local for both       9
 *   ... + no prototype for __Func_8092950                            9
 *   ... + -fno-rerun-cse-after-loop                                  9
 *   ... + -fno-expensive-optimizations                               9
 *   ... + -fno-thread-jumps                                          9
 *   ... + -fno-gcse                                                133 (138 lines)
 *   ... + -fno-cse-follow-jumps                                    135 (137 lines)
 *   ... + -fno-force-mem                                            93 (144 lines)
 *   ... + -O1                                                      141 (137 lines)
 *   ... + --no-sched2                                               53
 *   ... + `volatile int k` for the shared 0x100                    128
 *   ... + second 0x100 perturbed to 0x81 << 1 (CSE probe)            3
 *   pin on SITE 2 only                                               9
 *   pin on SITE 1, r0 only                                           9
 *   pin on SITE 1, r1 only                                           2
 *   pin on SITE 1, both, `p1 = 0x80 << 1;` in one statement          2
 *   pin on SITE 1, both, `register int pN __asm__(...) = K;` inits   2
 *   pin on SITE 1, both, statements in site 2's order                2
 *   pin on SITE 1, both, declared r0 before r1                    ** 0 **
 *   pin on SITE 1, both, `unsigned int`                           ** 0 **
 *   pin on SITE 1, both, as landed                                ** 0 **
 *   pin on BOTH sites                                             ** 0 **  (site 2 inert)
 *   pin on SITE 1 + `__asm__ volatile ("" : : "r" (p1))`          ** 0 **  (barrier inert)
 *
 * The bare pin is enough; the volatile barrier is measured INERT and is not in
 * the file, which is "Try the BARE register pin before the barrier" reading
 * the way that section predicts (the hard-register declaration removes the
 * pseudo, so there is nothing left for cse1 to common and no barrier needed).
 *
 * ------------------------------------------------------------------
 * NEW: with UNINITIALISED pins, DECLARATION ORDER STOPS MATTERING
 * ------------------------------------------------------------------
 * Grepped first as "declaration order", "pin declaration", "argument-setup
 * order" and "uninitialised pin"; the two governing sections are "Pin
 * DECLARATION order is argument order" and "An uninitialised pin moves its
 * assignment".  The second says the pin has two independent knobs -- the
 * declaration's position and the assignment's position -- and reaches for the
 * assignment "when the first cannot go late enough".  Measured here, with all
 * three assignments split into their own statements in the ROM's order:
 *
 *   declared r1 then r0   ** 0 **
 *   declared r0 then r1   ** 0 **
 *   statements reordered  2  (with EITHER declaration order)
 *
 * So the two knobs are not additive, they are exclusive: once every pin is
 * uninitialised and every value has its own assignment statement, the
 * assignment positions carry the whole ordering and the declaration order is
 * dead.  That is not a contradiction of the declaration-order rule -- that
 * rule's own exemplar (OvlFunc_932_2008c9c) uses INITIALISERS, where the
 * declaration IS the assignment.  Stated as a boundary: declaration order is
 * argument order ONLY for initialised pins.  Practical consequence: on an
 * uninitialised pin block, do not spend a screen permuting declarations.
 *
 * ------------------------------------------------------------------
 * NEW: "ZERO of 3235 generated .s build the same constant twice inside a
 * single block" is FALSE, and the counter-examples are how this was found
 * ------------------------------------------------------------------
 * Grepped by concept ("one basic block", "duplicate constant", "dominance",
 * "commoned-constant") before writing this.  The section "Identical constants
 * in ONE basic block: a controlled blocker" states, as its central control,
 * "Within ONE basic block -- never.  ZERO of 3235 generated `.s` files build
 * the same constant twice inside a single block", and concludes the shape "is
 * not reachable by any spelling of that constant".
 *
 * Re-run over the 3757 generated .s that have a sibling .c (scan kept beside
 * this file as scan_dupconst.py; block = run with no label definition and no
 * branch, `bl` deliberately NOT treated as a terminator), the count is 449
 * hits, not zero.  Two of them are worth naming:
 *
 *   asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_b.s -- `push {lr}`
 *      only, no branch anywhere, and `mov r1,#129 / lsl r1,#1` twice around an
 *      intervening call, once with `mov r0,#21` interleaved into the gap.
 *      That is this function's exact shape.
 *   asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a_b.s -- 0x100 built twice about
 *      forty instructions apart in one block, and 0x102 likewise.
 *
 * The claim is not merely stale, and the correction is not "the scan had a tab
 * bug".  BOTH counter-examples are `// fakematch` files: the shape exists in
 * the corpus only where somebody pinned it there.  So the ORIGINAL scan was
 * measuring gcc's behaviour on ordinary C and got the right answer for that
 * question; what it could not see is that the question has a documented escape.
 * Read together with the later "CORRECTED: a CALL-CLOBBERED PIN reaches this
 * class without any branch", the two sections already agree -- but the earlier
 * one still ends on "not reachable by any spelling", with no forward pointer,
 * and it is the one a reader hits first when grepping this shape.
 *
 * The operational lesson is the one that saved this function: **exclude
 * fakematches when asking what gcc does, and INCLUDE them when asking whether
 * a shape has ever been produced.** docs/elevation.md's "Reading generated
 * output as evidence: exclude fakematches first" states only the first half.
 *
 * ------------------------------------------------------------------
 * LANDING
 * ------------------------------------------------------------------
 * This is #1 of 5 and its sibling _200b060 is #2, so with both solved the two
 * travel together as ONE split piece and the .ld needs only two lines instead
 * of three.
 *
 * There is exactly ONE .ld line naming this .o on its FULL PATH, in
 * overlays/rom_7b0400/overlay.ld, with this content:
 *
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.o(.text)
 *
 * It sits between the lines whose content is
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_b.o(.text)
 * and
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_b.o(.text)
 * and becomes TWO lines in ROM order:
 *     src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_a.o(.text)   <- af18 + b060
 *     asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_b.o(.text)   <- the other 3
 *
 * `grep` also reports
 *     \t\tasm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_c.o(.text)
 * in overlays/rom_793768/overlay.ld.  That is a DIFFERENT full path -- a
 * same-named .o under a different overlay -- and must not be touched.
 *
 * FLAG GROUP: default.  objcmp/tryc's own `-fcall-used-r4` group is what both
 * functions match under, and function 3 of this .s (_200b1c0) uses r4 without
 * pushing it, confirming the TU is in that group.  No per-file rule is needed.
 *
 * FAKEMATCH: this file carries a call-clobbered register pin and must be added
 * to fakematch.txt as
 *     OvlFunc_925_200af18  src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_a.c
 * The sibling _200b060 in the same piece is NOT a fakematch -- its two 0x100
 * calls sit behind `if (i == 0x14)` and the plain basic-block lever reaches
 * them, which is the boundary this pair straddles: same constant, same callee,
 * same overlay, and a branch is worth an entry in fakematch.txt.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[8];
    short f18;
    unsigned char pad1a[0xe];
};

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(int *actor, int f);
extern void __Func_8092950(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct P *p);

void OvlFunc_925_200af18(void)
{
    struct P p;
    int *a;
    int *b;
    unsigned int i;
    int t;
    int x, y;
    int x2, y2;

    a = __MapActor_GetActor(0x16);
    b = __MapActor_GetActor(0x18);
    __PlaySound(0xbe);
    {
        register int p1 __asm__("r1");
        register int p0 __asm__("r0");
        p1 = 0x80;
        p0 = 0x16;
        p1 <<= 1;
        __Func_8092950(p0, p1);
    }
    __Func_8092950(0x18, 0x80 << 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t != 0) {
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0xfff00000;
            OvlFunc_common0_10c(x, y, a[4], 0, 0x80 << 11, 0, 0xd8 << 13, &p);
        } else {
            x2 = b[2] + ((__Random() * 24) >> 16 << 16);
            x2 += 0xfff40000;
            y2 = b[3] + ((__Random() << 5) >> 16 << 16);
            y2 += 0xfff00000;
            OvlFunc_common0_10c(x2, y2, b[4], 0, 0x80 << 11, 0, 0xd8 << 13, &p);
        }
        i++;
    } while (i <= 0x1f);
    __MapActor_SetPos(0x16, 0, 0);
    __MapActor_SetPos(0x18, 0, 0);
}

void OvlFunc_925_200b060(void)
{
    struct P p;
    int *a;
    int *b;
    unsigned int i;
    int t;
    int x, y;
    int x2, y2;
    int k;

    a = __MapActor_GetActor(0x16);
    b = __MapActor_GetActor(0x18);
    __PlaySound(0xbe);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    k = 0x80 << 1;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t != 0) {
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0x80 << 14;
            OvlFunc_common0_10c(x, y, a[4], 0, 0xfffc0000, 0, 0xd8 << 13, &p);
        } else {
            x2 = b[2] + ((__Random() * 24) >> 16 << 16);
            x2 += 0xfff40000;
            y2 = b[3] + ((__Random() << 5) >> 16 << 16);
            y2 += 0x80 << 14;
            OvlFunc_common0_10c(x2, y2, b[4], 0, 0xfffc0000, 0, 0xd8 << 13, &p);
        }
        if (i == 0x14) {
            __Func_8092950(0x16, k);
            __Func_8092950(0x18, k);
        }
        i++;
    } while (i <= 0x1f);
    __Func_8092950(0x16, 0);
    __Func_8092950(0x18, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 1);
}
