/* OvlFunc_926_2008bf4  --  0x02008bf4
 * OvlFunc_926_2008cd4  --  0x02008cd4
 *
 * THE TWO FUNCTIONS OF THIS BATCH **DO** SHARE A .s, AND IT HOLDS NOTHING ELSE.
 *   asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_a.s
 *   `grep -c thumb_func_start` returns 2, at lines 9 and 108, and the file is
 *   201 lines with `grep -c '\.section\|\.incbin\|\.global'` returning 0 -- pure
 *   .text, no data blob, no exported label.  So this lands as ONE .c with NO
 *   SPLIT, no tools/split_s.py run, and NO LINKER EDIT AT ALL.
 *
 *   EVERY .ld LINE NAMING THIS .o, MATCHED ON THE FULL PATH.  `grep -rn
 *   "ovl_314_c_c_a_c_c_c_a_a_a_a_a\.o" --include=*.ld .` over all 60-odd .ld
 *   files in the tree returns EXACTLY ONE line, quoted by content:
 *     overlays/rom_7b2078/overlay.ld:36  asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_a.o(.text)
 *   That script has only two section lists -- `.text :` at line 15 and
 *   `.data :` at line 73; there is no .rodata and no .bss list -- and the .data
 *   list (lines 74-79) names only common0_c.o and ovl_314_c_c_c_c_c_c_c_{a,b,
 *   c_b,c_c}.o.  NO SECTION NAMED FOR THIS .o IS LEFT UNACCOUNTED, and there is
 *   no stale `.o(.data)` entry to re-aim.  LINE 36 MUST KEEP ITS `asm/` PATH:
 *   the Makefile's cross-dir rule `asm/%.o: src/%.c` (line 147) builds
 *   asm/<bank>/X.o from src/<bank>/X.c, exactly as the already-elevated sibling
 *   at overlay.ld:37 does.  Landing is: drop this file at
 *   src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_a.c, delete the .s,
 *   touch nothing else.
 *
 * EXACT, both measured as SINGLE-FUNCTION EXTRACTS of this exact file against
 * the ORIGINAL asm/ path (the .s holds two functions, so --func isolates each
 * on the reference side; objcmp does NOT isolate the candidate side, so the
 * two-function file must not be handed to it whole):
 *
 *   OK OvlFunc_926_2008bf4 -- 224 bytes, 97 encodings and 8 relocations identical
 *   OK OvlFunc_926_2008cd4 -- 224 bytes, 98 encodings and 8 relocations identical
 *
 * And, stronger than either, the WHOLE TU was checked: this file compiled at the
 * tree default and assembled gives a .text of 448 bytes byte-for-byte identical
 * to the assembled ROM .s, pools included.  The extracts were also confirmed not
 * to interact -- concat(extract1.bin, extract2.bin) == whole-TU .bin, 448 = 224
 * + 224.  Both OK lines reproduced on re-runs.
 *
 * NO FLAG GROUP, AND NONE CAN ARRIVE BY ACCIDENT.  objcmp prints no
 * `(built with: ...)` line, so adjust=set() and this is the tree default
 * `-O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 * -ffreestanding -fcall-used-r4`.  `grep -n rom_7b2078 Makefile` returns
 * NOTHING -- no rule, literal or `%`-patterned, reaches this bank, so neither
 * ALIAS_CFLAGS nor GCSE_CFLAGS nor the -O1 rules can capture it.  Probed on both
 * functions: -fno-strict-aliasing, -fno-gcse, -fno-schedule-insns,
 * -fno-cse-follow-jumps, -fno-rerun-cse-after-loop and -fno-strength-reduce are
 * all BYTE-IDENTICAL.  Only two passes are live: -fno-schedule-insns2 (101 bytes
 * differing on 2008bf4, 93 on 2008cd4, size unchanged) and
 * -fno-expensive-optimizations (195/203, and it changes the size to 232/220).
 *
 * WHAT THEY ARE.  Two mirror-image cutscene beats on actor 0x13.  Each writes a
 * descending angle into the actor's sub-object, waits a shrinking number of
 * frames (8, 6, 4, 2, 0), and walks the actor around a circle by 6*cos / 6*sin
 * -- 2008bf4 forward over a descending sweep, 2008cd4 backward over an ascending
 * one -- then plants the height, plays the same thud, and fires three particle
 * bursts whose constants are each other's mirror (2008bf4's r3 arguments are
 * -0x3333, -0xcccc, -0x10000 against 2008cd4's +0x10000, +0xcccc, +0x3333, and
 * their fifth arguments run 0x6666, 0x4ccc, 0x3333 against 0x3333, 0x4ccc,
 * 0x6666).  2008cd4 additionally clears f18 to -0xcccc.
 *
 * TEMPLATE PROVENANCE, AND THE SCORE CAVEATED.  tools/solved_twins.py reports
 * ZERO twins tree-wide, so there was no twin to transplant.  templated.py's
 * 1.00 against src/overlays/rom_7a5214/ovl_17ec_c_c_b.c rests on six shared
 * symbols, which that tool's own note calls close to coincidence -- and the
 * score was NOT what carried this.  What carried it was a file templated.py did
 * not point at: src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_b.c, the
 * LITERALLY ADJACENT sibling in the same bank, cut from the same parent .s
 * (ovl_314_c_c_a_c_c_c_a_a_a_a.s split into _a/_b/_c; we are _a, it is _b).  It
 * supplied the whole skeleton: the eight-argument OvlFunc_common0_10c signature
 * with its r0/r1/r2 = f8/fc/f10 mapping and four stack words, `struct B` with
 * the halfword at 0x1e, the `__MapActor_GetActor(0x13)` cache in r7, and the
 * unsigned counter.  BOTH FUNCTIONS MATCHED ON THE FIRST CANDIDATE.  Look for
 * the same-bank neighbour before the high-scoring far one.
 *
 * ------------------------------------------------------- TEMPLATE CORRECTION -
 * THE 1.00 NEIGHBOUR'S HEADLINE LEVER IS ACTIVELY WRONG HERE, AND ITS OWN TEST
 * SAYS SO.  ovl_17ec_c_c_b.c's finding is "A REPEATED ACCESSOR CALL IS A
 * SOURCE-LEVEL REPEAT": there, eight `bl __MapActor_GetActor` in 135
 * instructions, one per field store, and caching costs 117 differing and 8 bytes
 * SHORT.  It also gives the test -- COUNT THE `bl`s BETWEEN THE STORES -- and
 * the count here is ONE, for eleven field accesses across both functions.  So
 * these cache, and transplanting the repeat is the single most expensive thing
 * available:
 *
 *     __MapActor_GetActor(0x13)-> at every access   2008bf4  128 differing, 96 bytes LONG
 *                                                   2008cd4  139 differing, 112 bytes LONG
 *
 * The recognisers agree with each other in the direction that entry predicts:
 * the repeat form is LONG here exactly as the cached form was SHORT there.  The
 * prologue said it too -- `push {r5,r6,r7,lr}` plus `mov r7,r8 / push {r7}`
 * keeps four callee-saved registers across calls, which is a function that
 * intends to hold things; a `push {lr}` would have ruled caching out on sight.
 * PROLOGUE BY CONTENT: `pop {r0} / bx r0` at both ends is the void return.
 *
 * ---------------------------------------------------------------------- NEW --
 * A COUNTER-DERIVED VALUE CARRIED IN A REGISTER IS NOT A SECOND SOURCE
 * VARIABLE; A RECOMPUTED ONE PROVES THERE ISN'T ONE.  Grepped docs/elevation.md
 * by concept first, in the corpus's vocabulary and not mine -- "induction",
 * "giv", "strength-reduce", "second counter", "derived", "loop counter",
 * "stride", "one materialisation", "two roles".  The near entries are "A LIVE
 * GIV IN THE ROM PROVES THE LOOP IS ORDINARY" (a giv rules out the `goto` loop),
 * "THE `goto` LOOP DOES NOT HAVE TO COST THE INDUCTION VARIABLES" (when gcc
 * won't build the giv, write the second counter by hand), and "ONE
 * MATERIALISATION IS NOT ONE VARIABLE" (one `mov rN,#0` can be two variables).
 * None of them answers the question this .s poses, and the .s is unusually good
 * at posing it because it holds BOTH SHAPES OF THE SAME LOOP:
 *
 *   2008bf4   mov r6,#8 / mov r8,r6 ... mov r0,r8 / mov r3,#2 / neg r3 / add r8,r3
 *   2008cd4   mov r6,#8 ...            mov r0,#0xc / sub r0,r6 / lsl r0,#1
 *
 * The first reads exactly like a hand-written `n = 8; ... n -= 2;` -- a copy of
 * the counter's own materialisation, stepped once per iteration.  It is not
 * evidence of one.  Measured, each against its own function:
 *
 *   function   ROM shape          inline `(i-4)*2` / `(0xc-i)*2`   hand-written `n`
 *   --------   ----------------   ------------------------------   ----------------
 *   2008bf4    carried in r8      0  (EXACT)                       0  (EXACT) -- tie
 *   2008cd4    recomputed inline  0  (EXACT)                       91, 4 bytes LONG
 *
 * MECHANISM, and it is an asymmetry rather than a rule about direction.  The
 * hand-written counter ALWAYS becomes the carried register: v_t2_sepN's loop
 * comes out instruction-for-instruction identical to 2008bf4's, `mov r8,r6` and
 * the `mov r3,#2 / neg r3 / add r8,r3` step included, in a function whose ROM
 * has neither.  The inline expression, by contrast, leaves gcc free to choose,
 * and it chooses differently in the two bodies -- carried in 2008bf4,
 * recomputed in 2008cd4.  So the inline spelling can produce EITHER shape and
 * the named counter can produce only ONE.
 *
 * I tried and REJECTED the obvious explanation, that the sign of the derived
 * value's coefficient decides it (2008bf4's `n` moves WITH its descending
 * counter, 2008cd4's moves AGAINST its ascending one).  A four-way control --
 * same skeleton, both counter directions crossed with both coefficient signs --
 * has gcc build the carried register in ALL FOUR, `ctrl_against` included.  The
 * choice is a property of the surrounding body, not of the expression, and I am
 * not naming the pass: -fno-strength-reduce is byte-identical on both functions,
 * so whatever manufactures it is not that.
 *
 * THE RULE TO CARRY, which needs none of that:
 *   - ROM RECOMPUTES the derived value in the body  ->  DECISIVE.  There is no
 *     named counter in the source; a named one is excluded, and here costs 91.
 *   - ROM CARRIES it in a register                  ->  UNDECIDABLE from the .s.
 *     Both spellings tie.  Write it INLINE anyway: it is the only spelling that
 *     reproduces both shapes, so it is the one to reach for first and the one
 *     that cannot be wrong, and it removes a hazard -- see below.
 * Both loops here therefore ship inline, which also makes the pair read alike.
 *
 * AND THE HAZARD THE INLINE FORM REMOVES IS REAL.  The adjacent sibling
 * ovl_314_c_c_a_c_c_c_a_a_a_a_b.c records "ASSIGNMENT ORDER IS THE LEVER" and
 * measures its named-counter loop at 6 differing when the two initialisers are
 * swapped.  The same swap here is 83, because both initialisers are the SAME
 * VALUE 8 and gcc materialises it once and copies (`mov r6,#8 / mov r8,r6`), so
 * reversing the order reverses which of r6/r8 is the low register and every use
 * of the counter pays for it.  Writing the derived value inline deletes that
 * whole failure mode.
 *
 * ------------------------------------------------------------ CONFIRMATIONS --
 * NAMING A VALUE gcc ALREADY CARRIES DESTROYS THE CARRY, with the size tell.
 * The ROM RE-LOADS `ldr r2,[r7,#0x10]` before each of the three burst calls --
 * it must, since OvlFunc_common0_10c may write through the actor -- and adds a
 * carried `0x80<<12` held in r6.  Hoisting it into one local hoists the LOAD
 * too and comes out SHORT: 95 differing / 4 bytes short on 2008bf4, 56 / 12
 * bytes short on 2008cd4.  Three source expressions, written out.
 *
 * The same rule from the other side on 2008cd4: 0x3333 appears twice, as the
 * fifth argument of the first call and the fourth of the third, and gcc's own
 * CSE parks it in r8 across all three (`mov r8,r4` ... `mov r3,r8`).  Naming it
 * costs 51.  The high registers are gcc's here and were never touched -- r8 is
 * the only one either function uses, and it is a WALL IN NEITHER: it holds a
 * loop value in 2008bf4 and a CSE'd constant in 2008cd4, both unaided.
 *
 * STATEMENT ORDER IS STORE ORDER; gcc-2.96 does not schedule the pairs past each
 * other.  Every reordering costs, and the costs locate the statements exactly:
 * swapping the `fc`/`f3c` pair is 2 on both; 2008cd4's `f18` store sits strictly
 * between `f3c` and `__PlaySound` (7 differing before the pair, 5 after the
 * sound); swapping the cos/sin updates is 4 on both; moving `__WaitFrames` ahead
 * of the halfword store is 5 on 2008bf4 and 7 on 2008cd4.
 *
 * `bhi` / `bls` on the trip count make both counters unsigned; `int` costs
 * exactly 1 encoding, the branch, on each -- the sibling's recorded tell.
 *
 * ------------------------------------------------------------ MEASURED WORSE -
 * 2008bf4, against 224 bytes / 97 encodings:
 *
 *   spelling                                                    differing
 *   ---------------------------------------------------------  ---------
 *   `__MapActor_GetActor(0x13)->` at every access                 128 (96 bytes LONG)
 *   name `a->f10 + 0x80000` in one local                           95 (4 bytes short)
 *   named counter with the initialisers swapped (`n = 8, i = 8`)   83
 *   `__WaitFrames` moved ahead of the `f1e` store                    5
 *   sin update written before cos                                    4
 *   `a->f3c` written before `a->fc`                                  2
 *   counter typed `int`                                              1
 *
 * 2008cd4, against 224 bytes / 98 encodings:
 *
 *   spelling                                                    differing
 *   ---------------------------------------------------------  ---------
 *   `__MapActor_GetActor(0x13)->` at every access                 139 (112 bytes LONG)
 *   hand-written `n` for the frame count                            91 (4 bytes LONG)
 *   name `a->f10 + 0x80000` in one local                            56 (12 bytes short)
 *   name the twice-used 0x3333                                      51
 *   `a->f18` written before the `fc`/`f3c` pair                       7
 *   `__WaitFrames` moved ahead of the `f1e` store                     7
 *   `a->f18` written after `__PlaySound`                              5
 *   sin update written before cos                                     4
 *   `a->f3c` written before `a->fc`                                   2
 *   counter typed `int`                                               1
 *
 * INERT -- tie at 0, so the plainer or more uniform form ships:
 *   the named counter on 2008bf4 (see NEW above; inline ships)
 *   `i >= 4` for `i > 3`;  `i < 0xd` for `i <= 0xc`
 *   `(i - 4) << 1` for `(i - 4) * 2`
 *   `6 * __cos(ang)` and `__cos(ang) * 3 * 2` for `__cos(ang) * 6`
 *   `ang` inlined into all three uses (it is named for legibility only)
 *   `short f1e` for `unsigned short`;  a `struct St *` last parameter for `void *`
 *   dropping the unused `f18` field from the struct (2008bf4 alone)
 *   PIN ADD PASS, all inert and therefore NOT SHIPPED: PIN4 across the first
 *     OvlFunc_common0_10c, PIN1 at __PlaySound.  Every constant-argument site
 *     here is single-argument -- the recorded "AN ALL-CHEAP CALL SITE NEEDS NO
 *     ORDERING PIN" -- and the eight-argument site's r0/r1/r2 are all loads.
 *     No pin is load-bearing in either function; no scaffolding ships.
 *
 * PROTOTYPES, 1 OF 6 LOAD-BEARING AND THE SAME ONE IN BOTH: dropping
 * OvlFunc_common0_10c costs 4 on 2008bf4 and 2 on 2008cd4 (the implicit
 * declaration loses the `void` return and the pointer eighth argument).
 * __WaitFrames, __PlaySound, __cos and __sin are inert; __MapActor_GetActor
 * cannot be dropped at all, since the pointer return is what the whole body is
 * written against.  The inert five stay declared -- a complete prototype list is
 * this tree's convention.
 */
struct B {
    unsigned char pad00[0x1e];
    unsigned short f1e;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    unsigned char pad1c[0x3c - 0x1c];
    int f3c;
    unsigned char pad40[0x50 - 0x40];
    struct B *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, void *h);

void OvlFunc_926_2008bf4(void)
{
    struct Actor *a;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(0x13);
    for (i = 8; i > 3; i--) {
        ang = i << 12;
        a->f50->f1e = ang;
        __WaitFrames((i - 4) * 2);
        a->f8 += __cos(ang) * 6;
        a->f10 += __sin(ang) * 6;
    }
    a->fc = 0x120000;
    a->f3c = 0x120000;
    __PlaySound(0xe3);
    OvlFunc_common0_10c(a->f8 - 0xc0000, a->fc, a->f10 + 0x80000, 0xffffcccd, 0x6666, 0, 0, 0);
    OvlFunc_common0_10c(a->f8,           a->fc, a->f10 + 0x80000, 0xffff3334, 0x4ccc, 0, 0, 0);
    OvlFunc_common0_10c(a->f8 + 0xa0000, a->fc, a->f10 + 0x80000, 0xffff0000, 0x3333, 0, 0, 0);
}

void OvlFunc_926_2008cd4(void)
{
    struct Actor *a;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(0x13);
    for (i = 8; i <= 0xc; i++) {
        ang = i << 12;
        a->f50->f1e = ang;
        __WaitFrames((0xc - i) * 2);
        a->f8 -= __cos(ang) * 6;
        a->f10 -= __sin(ang) * 6;
    }
    a->fc = 0x120000;
    a->f3c = 0x120000;
    a->f18 = 0xffff3334;
    __PlaySound(0xe3);
    OvlFunc_common0_10c(a->f8 - 0xc0000, a->fc, a->f10 + 0x80000, 0x10000, 0x3333, 0, 0, 0);
    OvlFunc_common0_10c(a->f8,           a->fc, a->f10 + 0x80000, 0xcccc,  0x4ccc, 0, 0, 0);
    OvlFunc_common0_10c(a->f8 + 0xa0000, a->fc, a->f10 + 0x80000, 0x3333,  0x6666, 0, 0, 0);
}
