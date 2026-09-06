/* OvlFunc_918_20098b8
 *   [asm/overlays/rom_7a5214/ovl_17ec_c_c.s, lines 6-143.  The file holds
 *   EXACTLY ONE FUNCTION (`grep -c thumb_func_start` returns 1) *AND A REAL
 *   `.section .data`* at line 145: `.global .L1ca8` / `.L1ca8:` /
 *   `.incbin "overlays/rom_7a5214/orig.bin", 0x1ca8, (0x1da8-0x1ca8)`.
 *   A SPLIT IS THEREFORE UNAVOIDABLE even though there is only one function,
 *   and DELETING THE .s WOULD BREAK THE LINK: `.L1ca8` is `.global` and is
 *   loaded from a DIFFERENT file in the same overlay --
 *     asm/overlays/rom_7a5214/ovl_17ec_a.s:12  ldr r6, =.L1ca8
 *     asm/overlays/rom_7a5214/ovl_17ec_a.s:44  ldr r6, =.L1ca8
 *   (the identically named `.L1ca8` in rom_7f148c and rom_7c097c are unrelated
 *   -- a `.L####` name is an ADDRESS, not an identity.)
 *
 *   TWO .ld LINES NAME THIS .o AND BOTH ARE ON THE FULL PATH.  They are the
 *   ONLY two lines in ANY .ld in the tree that name `ovl_17ec_c_c.o` at all
 *   (`grep -rn "ovl_17ec_c_c\.o" --include=*.ld .` returns exactly these):
 *     overlays/rom_7a5214/overlay.ld:57  asm/overlays/rom_7a5214/ovl_17ec_c_c.o(.text)
 *     overlays/rom_7a5214/overlay.ld:64  asm/overlays/rom_7a5214/ovl_17ec_c_c.o(.data)
 *   The overlay's .bss list (lines 71-75) names only ovl_314_c_c_c_{a,b,c}.o,
 *   and there is no .rodata list, so no section named for this .o is left
 *   unaccounted.  LINE 64 IS THE ONE THAT MATTERS: an .ld entry matching no
 *   input section is NOT an error, so aiming it wrong drops the blob silently.
 *
 *   LANDING SHAPE -- let tools/split_s.py rewrite the script, do not hand-aim:
 *       python3 tools/split_s.py asm/overlays/rom_7a5214/ovl_17ec_c_c.s \
 *                                OvlFunc_918_20098b8
 *   takes the "one function plus strictly trailing data" path, writes
 *     asm/overlays/rom_7a5214/ovl_17ec_c_c_b.s   the function
 *     asm/overlays/rom_7a5214/ovl_17ec_c_c_c.s   the .data + .global .L1ca8
 *   and expands BOTH .ld lines into `_b.o(SECTION)` / `_c.o(SECTION)` pairs.
 *   Then THIS FILE becomes src/overlays/rom_7a5214/ovl_17ec_c_c_b.c, _b.s is
 *   deleted, and .ld:57's `_b` entry becomes `src/...`.  The leftover
 *   `_b.o(.data)` and `_c.o(.text)` entries match nothing and are harmless.
 *   Neither `ovl_17ec_c_c_b` nor `ovl_17ec_c_c_c` collides: asm/overlays/
 *   rom_7a5214 holds ovl_17ec_{a,b,c_b,c_c} only.  Run `make compare` AFTER
 *   the split and BEFORE this .c lands -- split_s is byte-neutral by
 *   construction, and a layout mistake and a bad decompilation look identical
 *   at the end.
 *
 *   THE TWO FUNCTIONS OF THIS BATCH DO **NOT** SHARE A .s.  The sibling
 *   OvlFunc_965_2008d4c lives in asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.s
 *   in a different overlay; see src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.c.]
 *
 * EXACT, measured as a single-function extract against the ORIGINAL asm/ path
 * (this .s holds one function, so --func isolates it on the reference side):
 *
 *   OK OvlFunc_918_20098b8 -- 344 bytes, 140 encodings and 28 relocations identical
 *
 * Reproduced on a re-run and against a scratch copy of the same .s.  objcmp
 * prints NO `(built with: ...)` line -- adjust=set(), the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP IS NEEDED and
 * none can arrive by accident: no Makefile rule, literal or `%`-patterned,
 * names asm/overlays/rom_7a5214/ovl_17ec_c_c*.o -- the only rom_7a5214 line in
 * the Makefile (683) names ovl_314_c_c_c_b.o.
 *
 * 135 instructions: a cutscene opener, a 17-iteration cos/sin ring loop, and a
 * fade-out tail.  ITS TWIN IS OvlFunc_965_2008d4c, which is the same script
 * with one extra `__CutsceneWait(0x1e)` and two different callees; the file
 * that solves it is a copy of this one with three lines changed.
 * `tools/solved_twins.py` reported ZERO twins for both -- correctly, because
 * neither was solved yet.  The FAMILY was found by grepping the corpus for two
 * idioms: `bl __cos` (36 .s files, 15 of them beside a solved .c) and the
 * literal `0x1090001` (3 .s files, two of which are this batch's targets).
 *
 * THE TEMPLATE'S HEADLINE LEVER DOES NOT APPLY, AND IT IS INERT NOT HARMFUL.
 * ovl_30_c_c_c_a_a_c_a_c.c's finding is "UNIFY A STRUCT TAG TO *ADD* A
 * DEPENDENCE", whose standing hazard is that the TU must never see
 * -fno-strict-aliasing (281 differing there).  Here -fno-strict-aliasing is
 * BYTE-IDENTICAL, and widening `struct Actor` to carry the neighbouring `f28`
 * word measures 0.  Nothing in this function turns on an alias set.  What IS
 * live is the OTHER two passes: -fno-schedule-insns2 is 45 differing and
 * -fno-gcse is 133 (the template's -fno-gcse was silent).  -fno-cse-follow-jumps,
 * -fno-rerun-cse-after-loop, -fno-expensive-optimizations and
 * -fno-schedule-insns are all byte-identical.
 *
 * ------------------------------------------------------------------- NEW ----
 * A REPEATED ACCESSOR CALL IS A SOURCE-LEVEL REPEAT, AND THE .s SAYS SO.
 * Grepped docs/elevation.md by concept first ("accessor", "re-calls",
 * "repeated call") -- the corpus records the ONE-SITE spellings ("A
 * POINTER-RETURNING CALL WHOSE RESULT DIES IMMEDIATELY MUST NOT BE NAMED", its
 * "TOO STRONG" correction, and "A COMPOUND ASSIGNMENT EVALUATES THE CALL ONCE")
 * and nothing about the N-SITE case, which runs the other way.
 *
 * EIGHT `bl __MapActor_GetActor` in 135 instructions, one per field store --
 * four in the opening block, one for `f55 = 3`, one whose result is genuinely
 * live (the loop's actor pointer, held in r10), and two in the tail.  That is
 * not an artefact to optimise around: the source calls the accessor eight
 * times and names the result exactly once.
 *
 *     __MapActor_GetActor(0)->f48 = ...   (written out at every store)   0
 *     p = __MapActor_GetActor(0); p->f48 = ...                         117 differing, 8 bytes SHORT
 *
 * And the choice is genuinely per-function, not per-callee: the near-identical
 * OvlFunc_968_200ca2c in src/overlays/rom_7f2f14/ovl_30_c_c_c_c_b.c opens with
 * the SAME five stores and DOES cache, holding the pointer in r7 across the
 * whole body.  Its .s shows two `bl __MapActor_GetActor` at the top and none
 * afterwards; ours shows one per store.  **Count the `bl`s between the stores
 * before choosing.**  The recognisers agree with each other: the cached form
 * is SHORT (it drops five calls' worth of `mov r0,#0`/`bl`), so a candidate
 * that is a handful of instructions short with the relocation list missing
 * accessor entries is this, not a constant problem.
 *
 * ------------------------------------------------------------------- NEW ----
 * PINNING r0 ALONE ORDERS A POOLED r1; THE MIRROR PIN DOES NOTHING.  The last
 * residue was two instructions, one adjacent transposition, size and
 * relocations both silent:
 *
 *     ROM   mov r0,#0      / ldr r1,=0x101
 *     ours  ldr r1,=0x101  / mov r0,#0
 *
 * This is the shape the recorded "A POOLED argument also needs naming in the
 * entry block" cures FROM THE POOL SIDE (OvlFunc_959_2008bec added
 * `e = 0xe666;` to its entry-block group).  Here the pool side is inert and the
 * CHEAP side is the whole lever:
 *
 *     PIN1, q0 = 0 only, 0x101 passed bare        0   <- the minimal cure
 *     PIN2, both filled ascending                 0   <- what ships (uniform fill)
 *     pin q1 only (the pooled one)                2   -- unchanged
 *     no pin                                      2
 *     PIN2 filled descending (q1 then q0)         2
 *
 * MECHANISM: the pool load has no seed instruction to order against, so naming
 * it fixes nothing; the `mov r0,#0` does, and a pin on it is a scheduling
 * anchor at the point where sched2 was choosing.  This also sharpens "A PARTIAL
 * PIN CAN BE WORSE THAN NO PIN AT ALL" (pinning only q0 at __Func_80933f8 on
 * OvlFunc_955_20099bc measured 20 against 8 for none): a partial pin is not
 * inherently a trap -- on a TWO-argument site where the unpinned argument is
 * POOLED there is no constant left for CSE to re-home, and the partial pin is
 * a complete cure.  Read what the unpinned argument costs.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * THE `neg` ORDER INSIDE AN ALL-(-1) FILL IS NOT A FILL-DIRECTION TELL.
 * "DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER" records that sched2 produces
 * several emitted orders from ONE spelling, for `mov`/`lsl`.  The same holds
 * for `mov`/`neg`, and this batch reads it out THREE WAYS from ONE source form:
 *
 *   this function        mov r0,r1,r2 #1 / neg r1 / neg r2 / mov r3,#0 / neg r0
 *   OvlFunc_965_2008d4c  identical to the above
 *   OvlFunc_965_2008eac  mov r0,r1,r2 #1 / neg r2 / neg r1 / mov r3,#0 / neg r0
 *   the template         mov r0,r1,r2 #1 / mov r3,#0 / neg r2 / neg r1 / neg r0
 *
 * All four are `{ PIN4; q0=-1; q1=-1; q2=-1; q3=0; __Func_80933f8(...); }`
 * written ascending, and all four are exact.  Writing the descending fill to
 * "match" the odd one out costs 4 here and 2 on 2008eac.  Do not read the neg
 * order.
 *
 * ------------------------------------------------------ CORRECTION ----------
 * THE __Func_80933f8(-1,-1,-1,0) "CONSTANT REUSE" BLOCKER IS FALSE, AND A PARK
 * RETIRES.  docs/elevation.md's "The same call is spelled both ways in the same
 * ROM" cites OvlFunc_965_2008eac by name as the function where "the ROM builds
 * -1 three separate times and our commoning of it is the blocker", and
 * src/non_matching/overlays/2008eac.c parks it on exactly that, saying "there
 * is no source spelling that separates them".  There is: the PIN4 ascending
 * fill, which post-dates the park.  OvlFunc_965_2008eac is now EXACT (see
 * src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.c) and src/non_matching/overlays/
 * 2008eac.c should be DELETED.  The entry's general claim -- that the reuse
 * verdict belongs to the TU and must not be carried between functions -- still
 * stands; only its example is wrong.  Dropping the pin here costs 126.
 *
 * PIN SET: FOUR SITES, ALL LOAD-BEARING, MINIMAL BY MEASUREMENT.  Every
 * pinnable constant-argument site was pinned, stripped greedily with a re-test
 * after each drop, then re-grown with an ADD pass, to a fixpoint from both
 * ends.  No site drops and no site is worth adding:
 *
 *   site  call                                        cost of dropping it
 *   ----  ------------------------------------------  -------------------
 *      0  __Func_80933f8(-1, -1, -1, 0)                     126
 *      2  __Func_8012330(0xa0<<11, 0xa0<<11, 0x80<<9)        31
 *      3  __Func_8012330(-1, -1, 0xe666)                     27
 *      1  __MapActor_Surprise(0, 0x101)                       2
 *
 * ADD PASS, all inert (tie at 0, so they do not ship): __MapActor_Surprise(0,
 * 0x80<<1) ascending AND descending, __MapActor_SetAnim(0, 0x16), __PlaySound,
 * __CutsceneWait.  That last group is the recorded "AN ALL-CHEAP CALL SITE
 * NEEDS NO ORDERING PIN".
 *
 * UNIFORM ASCENDING FILL, one statement per argument, whole value per
 * statement, at all four -- the tree's convention, and no site wants
 * descending (Surprise 2, 8012330 4, 80933f8 4).  Three of the four fills are
 * WIDER THAN THE MINIMUM and that is deliberate: PIN2 suffices at __Func_80933f8
 * and at __Func_8012330(-1,-1,0xe666), PIN1 at __MapActor_Surprise.  Only
 * __Func_8012330(0xa0<<11, ...) needs its full PIN3 -- PIN2 there is 38.
 *
 * WHAT NEEDED NOTHING.  The 17-iteration loop as a plain
 * `for (i = 0; i <= 0x10; i++)` over an `unsigned int` -- the ROM's `bls` is
 * what makes it unsigned, and `int` costs exactly 1 encoding (the branch);
 * `v[0] + v[0] / 2` written out, with gcc's own `lsr #31 / add / asr #1`
 * rounding falling out of the signed division by two; the four
 * `0x82<<16`-style two-instruction constants as plain shifted literals;
 * `0x1090001` and `0xcccc` inline; the two HIGH REGISTERS (r8 for `&s`, r10
 * for the actor pointer) chosen by gcc with no help at all -- r8/r10 are not a
 * wall and were never touched here.
 *
 * PROTOTYPES, 4 OF 12 LOAD-BEARING AND PER-SITE: dropping __Func_8012330 costs
 * 5, __MapActor_SetAnim 2, __MapActor_Surprise 2, __Func_8012350 2 (a VOID,
 * NO-ARGUMENT function -- the same reach the template records).  The other
 * eight are inert and stay declared because a complete prototype list is this
 * tree's convention.
 *
 * MEASURED WORSE (against 344 bytes / 140 encodings):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   cache `p = __MapActor_GetActor(0)` for the stores        117 (8 bytes short)
 *   `int v0, v1, v2;` instead of `int v[3]`                   124
 *   drop the __Func_80933f8 pin                               126
 *   PIN1 (q0 only) at __Func_80933f8                          126
 *   `a = __MapActor_GetActor(0)` moved after the s.* stores    95
 *   `s.f00 = 1;` added (the sibling's shape)                   96
 *   `v[0] = v[0] * 3 / 2` for `v[0] + v[0] / 2`                 83
 *   PIN2 instead of PIN3 at __Func_8012330(0xa0<<11, ...)       38
 *   one local `t` for both 0xa0<<11 arguments                   34
 *   drop the __Func_8012330(0xa0<<11,...) pin                   31
 *   drop the __Func_8012330(-1,-1,0xe666) pin                   27
 *   `s.f24 = ...` written after the two 0xcccc stores            6
 *   `v[1] = 0;` before `v[0] = __cos(ang);`                      6
 *   drop the __Func_8012330 prototype                            5
 *   descending fill at either __Func_8012330 or __Func_80933f8   4
 *   `int v[3];` declared before `struct Cfg s;`                  2
 *   drop the __MapActor_Surprise(0, 0x101) pin                   2
 *   pin only q1 (the pooled 0x101) at that site                  2
 *   loop counter typed `int`                                     1
 *
 *   INERT (tie at 0, so the plainer or more uniform form ships):
 *     `for (i = 0; i < 0x11; i++)`;  `v[0] += v[0] / 2`
 *     `ang` inlined into both calls;  no `z` local, literal 0 at both stores
 *     `0x50000` for `0xa0 << 11`;  `0x10000` for `0x80 << 9`
 *     the f55 stores through an explicit `unsigned char *` cast
 *     `int f24;` + a cast instead of the function-pointer field
 *     a wider `struct Actor` carrying the neighbouring `f28` word
 *     `int pad00;` instead of a named `f00` in struct Cfg
 *     PIN1/PIN2 reductions at three of the four pinned sites (see above)
 *     adding a pin at __MapActor_Surprise(0, 0x80<<1), __MapActor_SetAnim,
 *       __PlaySound or __CutsceneWait
 *
 * FLAGS: no flag group; -fno-schedule-insns2 45 and -fno-gcse 133 are the two
 * live passes, everything else byte-identical.  See the paragraph above.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x30];
    int f44;
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x14];
    void (*f24)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_918_200985c(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_918_20098b8(void)
{
    struct Cfg s;
    int v[3];
    struct Actor *a;
    unsigned int i;
    int ang;
    int z;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapActor_GetActor(0)->y = 0x82 << 16;
    __MapActor_GetActor(0)->f48 = 0x80 << 8;
    z = 0;
    __MapActor_GetActor(0)->f44 = z;
    __MapActor_GetActor(0)->f55 = z;
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xcc);
    __MapActor_GetActor(0)->f55 = 3;
    __CutsceneWait(0x18);
    a = __MapActor_GetActor(0);
    s.f04 = 7;
    s.f24 = OvlFunc_918_200985c;
    s.f08 = 0xcccc;
    s.f0c = 0xcccc;
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] + v[0] / 2;
        OvlFunc_common0_10c(a->x, a->y, a->z, v[0], v[1], v[2], 0x1090001, &s);
    }
    __PlaySound(0xbc);
    { PIN2; q0 = 0; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    __MapActor_SetAnim(0, 0x16);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __MapActor_Surprise(0, 0x80 << 1);
    __MapActor_GetActor(0)->f48 = 0x80 << 9;
    __MapActor_GetActor(0)->f44 = 0x80 << 7;
    __CutsceneEnd();
}
