/* ovl_314_c_a_c_c_c_c_c_c_c_c_a_b.c  --  OvlFunc_969_200be9c  --  0x0200be9c
 *   [asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a.s, 3rd of FIVE]
 *
 * VERDICT:
 *
 *   OK OvlFunc_969_200be9c -- 928 bytes, 358 encodings and 95 relocations identical
 *
 * ...against BOTH a scratch single-function extract and the REAL asm/ path.
 *
 * 346 instructions of straight-line cutscene script -- 94 calls, ONE
 * `__Func_8091c7c` diamond whose two arms rejoin on a boolean, and two
 * `iwram_3001ebc[0xec << 1]` counter bumps, one in the `else` arm and one under
 * the boolean after the join.  No loop, no actor-struct block, no stack
 * argument, no mid-function pool.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT WAS CHECKED RATHER THAN ASSUMED.
 * `grep -n rom_7f6e64 Makefile` returns exactly ONE line, and it is an EXPLICIT
 * rule for a DIFFERENT file (`ovl_314_a_a_a_c.o`, on ALIAS_CFLAGS).  No wildcard
 * reaches this stem, so the generic cross-dir `asm/%.o: src/%.c` at Makefile:146
 * applies and the TU takes the tree default -O2.  objcmp printed no
 * "(built with: ...)" line from either reference path -- the check that catches
 * the wildcard trap.  Nothing to add to the Makefile.
 *
 * THE PROLOGUE BY CONTENT, NOT BY WIDTH.  `push {r5, r6, lr}` plus
 * `mov r6, r10 / mov r5, r8 / push {r5, r6}`: FOUR callee-saved values, a WIDE
 * push -- and NOT ONE OF THEM IS A NAMED LOCAL.  Counting `mov rN, r5..r11`
 * copies by DESTINATION reads them off directly:
 *   r5   the diamond's boolean, then 0x1001 (5 sites), then 0x2003 (3 sites)
 *   r6   0x80 << 6 (4 sites), then `add r6, #2` -> 0x2002 (2 sites)
 *   r8   0x80 << 7 (2 sites, after an earlier REBUILT use at site 8)
 *   r10  0xc0 << 8 (2 sites, after an earlier REBUILT use at site 7)
 * Every one of those is a repeated constant gcc commons UNAIDED, into exactly
 * those four registers, once the competing classes are pinned away.  The whole
 * body is bare literals plus one `int v`.
 *
 * THE LENGTH TELL POINTS THE WRONG WAY HERE.  Plain C is 944 bytes against the
 * ROM's 928 -- sixteen LONG, 331 of 358 encodings differing -- because gcc
 * commons SEVEN values where the ROM commons four and pays `push {r5,r6,r7,lr}`
 * + `push {r5,r6,r7}` + `push {r7}` for r5,r6,r7,r8,r9,r10,r11.  The three extra
 * classes are the whole problem; the diff TEXT (r8-r11 traffic) says so and the
 * size does not.
 *
 * THE PIN SET IS TWENTY-ONE, AND ITS NOMINATION IS THE INTERESTING PART.
 * Candidates: 39 sites by the recorded CSE rule (an argument list carrying a
 * constant the ROM builds more than once and that is not a bare `mov #imm8`;
 * `gen.py --info` prints it), 5 more by CALL-SITE FAMILY (same callee AND same
 * argument shape), and 2 that NEITHER rule reaches.  Minimised greedily under
 * objcmp, re-testing after every drop, to a fixpoint FROM BOTH ENDS: both
 * directions converge on the SAME 21 --
 *
 *   6, 7, 15, 16, 19, 20, 24, 27, 35, 36, 37, 40, 41, 46, 47, 50, 58, 67, 86,
 *   91, 94
 *
 * Of those, 17 are CSE-nominated, 2 are FAMILY-only (24 `__MapActor_Emote(1,
 * 0x105, 0x28)` and 91 `__Func_8092adc(0x15, 0xa0 << 7, 0x14)`, both carrying a
 * constant used ONCE), and 2 are nominated by neither -- see below.  The three
 * that fall out (44, 53, 82) are each the LATER use of a value whose earlier use
 * is pinned, the recorded "one pin at the first use covers the later ones".
 * They are INERT SCAFFOLDING AND DO NOT SHIP.
 *
 * THE PIN SET NEEDS A HOLE, AND THE HOLE'S SIGNATURE IS THE PROLOGUE.  22 of
 * the 39 CSE-nominated sites are the ones carrying the ROM's OWN four held
 * values, and they must be left BARE.  Pinning all 39 (plus 6/16/24/91) gives
 * 318 of 358 differing, TWELVE BYTES SHORT, and `push {r5, lr}` against the
 * ROM's `push {r5, r6, lr}` -- every carry destroyed at once.  That is the
 * recorded "a pin set can need a HOLE where the ROM itself commons", and the
 * narrowed prologue is a one-glance way to see it.
 *
 * `__Func_8092c40` WANTS THE DESCENDING FILL, again.  Site 6, `(2, 0)`: the ROM
 * emits `mov r1, #0 / mov r0, #2`.  Uniform ascending there is byte-identical to
 * leaving the site unpinned (2 encodings), which is the sharpest form of the
 * tell -- the ascending fill is not a weaker pin, it is no pin at all.
 *
 * UNIFORM ASCENDING IS RIGHT AT THE OTHER TWENTY.  One statement per argument,
 * whole value per statement, q0..q3 in order.  Transcribing the ROM's emitted
 * register order into every fill instead is 22 differing; only site 6 wants a
 * different order at all.  Whole-value literals (`0xc000`) and shifted spellings
 * (`0xc0 << 8`) TIE, so the shifted form ships because it reads like the ROM.
 *
 * TWO PINS THAT NEITHER NOMINATION RULE REACHES, AND THEY ARE BOTH ONE-MEMBER
 * FAMILIES (NEW).  Site 6 `__Func_8092c40` and site 16 `__Func_80921c4(1,
 * 0x141, 0xae)` are the ONLY call to their callee in this function.  The CSE
 * rule misses them because their expensive argument is used once; the family
 * rule misses them because a family of one has no sibling to compare a shape
 * against -- the coded rule grows a family only when it already intersects the
 * CSE set, so a singleton can never grow.  Both are required, each worth exactly
 * 2 encodings when dropped.  This is the neighbour of the recorded "a family
 * whose arguments are all CHEAP is nominated by neither rule": there the
 * arguments are cheap, here the FAMILY is a singleton and the argument is a
 * pooled 0x141.  Practical rule, cheap to apply: also nominate every site whose
 * CALLEE APPEARS ONCE in the function and whose argument list is not all bare
 * `mov #imm8`.  Here that nominates 5 (sites 6, 16, 53, 54, 55) and 2 survive --
 * the same 2-in-5 hit rate the family rule got.
 *
 * THE ORDERING BAND IS PER-PIN AND ADDITIVE, SO READ AGGREGATES ACCORDINGLY.
 * All 21 drops partition with nothing between: NINE ordering pins (6, 7, 16, 24,
 * 27, 46, 47, 91, 94) cost EXACTLY 2 encodings each with SIZE and RELOCATIONS
 * both silent, and TWELVE CSE pins (15, 19, 20, 35, 36, 37, 40, 41, 50, 58, 67,
 * 86) cost 67 to 332 with RELOCATIONS ALWAYS differing.  Nothing landed in 4-9.
 * Dropping ordering pins in SUBSETS measures 2, 4, 6, 8, 10 exactly -- so a
 * MULTI-SITE residue of 4, 6 or 8 with relocations silent is two, three or four
 * ordering pins, not "neither band".  The recorded 4-to-9 gap is a statement
 * about ONE dropped pin.
 *
 * NAMING THE FOUR HELD VALUES IS INERT -- BUT ONLY WITH THE ASM LABEL.
 * Measured on top of the exact candidate:
 *   bare literals                                        EXACT   <- ships
 *   `register int n __asm__("r5")` etc. for all four      EXACT   (inert)
 *   plain `int n, m, k, j`                               324 of 358, 4 SHORT
 * The recorded "naming a value gcc already CARRIES destroys the carry" is
 * measured only in the plain form.  The asm-labelled form does NOT destroy it:
 * it merely re-states the allocator's own answer and costs nothing, which is
 * why it is inert here rather than a tie-by-luck.  Since it measures inert, it
 * does not ship.
 *
 * 0x2002 IS A BARE LITERAL AND gcc DERIVES IT (NEW, AND IT REFUTES A "NEVER").
 * The ROM does `add r6, #2` off the held 0x2000.  The notebook says
 * `add rD, rN, #k` off a constant is a positive tell for a NAMED LOCAL, that
 * "two independent CONST_INTs can NEVER produce that add", and that deriving is
 * reachable only when the base has "a REAL USE" / has "been forced into a
 * register by a runtime use".  None of that applies here: both values are plain
 * integer literals in the argument lists of separate calls, there is no variable
 * and no runtime use anywhere, and gcc-2.96 STILL emits `add r6, r6, #2` and
 * pools nothing.  Isolated on a three-function probe (scratch_elev/b241/
 * f200be9c/probe.c):
 *
 *   g(0x2000) x4 then h(0x2002) x2   mov/lsl into r5, then `add r5, #2`, NO pool
 *   g(0x2000) x1 then h(0x2002) x2   0x2002 POOLED
 *   h(0x2002) x2 alone               0x2002 POOLED
 *
 * So the discriminator is only whether CSE has ALREADY put the base in a
 * register, and REPETITION ALONE is enough to do that -- no runtime consumer and
 * no mutated variable required.  Spelling it as a mutated local (`m = 0x80 << 6;
 * ... m += 2;`, with the asm label) is byte-identical, so the bare literal ships.
 *
 * MEASURED WORSE, everything tried on top of the exact candidate:
 *   pin all 39 CSE sites (no hole)          318/358, 12 bytes SHORT, push {r5,lr}
 *   plain `int` locals for the four values  324/358, 4 bytes SHORT
 *   ROM emitted order in every fill          22/358
 *   withhold `__Func_8092adc`'s prototype    27/358
 *   uniform ascending at site 6               2/358
 * and TIES (measured, not shipped): whole-value literals; asm-labelled locals
 * for any of the four held values; a per-instruction ROM-order fill at every
 * pinned site; withholding `OvlFunc_969_2008894`'s prototype (one argument, so
 * there is no order to fix -- prototypes are PER-SITE, and `__Func_8092adc`'s
 * three-argument sites prove it in the other direction).
 *
 * LANDING.  The `.s` holds FIVE functions -- OvlFunc_969_200b924, _200bbc8,
 * _200be9c (this one), _200c23c, _200c8d8 -- so it needs a THREE-WAY cut.  It is
 * clean: the file has NO `.section`, `.data`, `.rodata`, `.bss`, `.word` or
 * `.byte` directive anywhere (pure `.text`), there is NO intra-file `bl` between
 * the five, and every `.L` label is local to one function (.L3c3c in the 2nd,
 * .L3f08/.L3f26/.L3f40 in this one, .L4784/.L47d4/.L485a in the 4th; the only
 * external label references, `=.L6760` and `=.L6764`, are in the 4th).
 * Matched on FULL PATH, `asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a.o`
 * is named by EXACTLY ONE linker-script line:
 *
 *   overlays/rom_7f6e64/overlay.ld:57
 *       asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a.o(.text)
 *
 * There is no `.data`, `.rodata` or `.bss` line for this `.o` in the script --
 * its .data/.bss blocks (lines 71-79) name only `ovl_314_c_c_a/_b/_c`.
 * `overlays/rom_7f6e64/overlay.map` does list zero-size `.data` and `.bss` for
 * the object, but the map is a GENERATED artefact and not a linker input; it
 * needs no edit.  So the whole remap is: replace that one line with three, in
 * address order --
 *
 *       asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_a.o(.text)
 *       asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_b.o(.text)
 *       asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_c.o(.text)
 *
 * The `_a_a` / `_a_b` / `_a_c` names are FREE -- the script has no
 * `ovl_314_c_a_c_c_c_c_c_c_c_c_a_*` entry.  When cutting, take the `.include`
 * lines only and let each piece keep its own comment block; the head of the
 * current file describes the FIRST function, not this one.
 *
 * THE OTHER FOUR ARE NOT THE SAME SHAPE, so this is not a five-for-one.  They
 * carry stack arguments (`str rN, [sp]` into `__CopyMapTiles` / `__Func_8010704`
 * -- named locals PER CALL SITE), `__MapActor_GetActor` blocks with halfword
 * field stores, `__MapActor_SetBehavior` script-pointer symbols
 * (gScript_969__0200e088 and friends) and, in the 4th, two `=.L6*` label
 * references.  Each is its own job; none of this function's levers is enough.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_969_2008894(int a);
extern void OvlFunc_969_20088a8(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_200be9c(void)
{
    int v;

    __Func_80925cc(1, 1);
    __CutsceneWait(0x14);
    __Func_8093040(1, 0, 0x14);
    __Func_80925cc(2, 1);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 2;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_969_20088a8(0, 0x80 << 7);
    v = 0;
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(2, 4);
        v = 1;
    } else {
        __CutsceneWait(0x14);
        __Func_80925cc(2, 1);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    OvlFunc_969_2008894(2);
    if (v != 0) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    { PIN3; q0 = 1; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x141; q2 = 0xae;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_969_20088a8(1, 0x80 << 6);
    OvlFunc_969_2008894(0x1001);
    { PIN3; q0 = 0; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_969_20088a8(3, 0xc0 << 8);
    OvlFunc_969_20088a8(1, 0x80 << 7);
    OvlFunc_969_2008894(0x1001);
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(1, 0x80 << 6, 0x14);
    OvlFunc_969_2008894(0x1001);
    { PIN3; q0 = 1; q1 = 0xa0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8001, 0, 0x28);
    __Func_8092adc(1, 0x80 << 6, 0x14);
    OvlFunc_969_2008894(0x1001);
    __PlaySound(0x11);
    __MapActor_SetAnim(3, 4);
    OvlFunc_969_2008894(3);
    __Func_8092adc(1, 0x80 << 7, 0x50);
    { PIN3; q0 = 3; q1 = 0xc0 << 7; q2 = 0x50;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0, 2);
    OvlFunc_969_20088a8(2, 0xc0 << 8);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Jump(2, 4, 0x3c);
    OvlFunc_969_20088a8(1, 0x80 << 6);
    __MapActor_Emote(1, 0x101, 0x28);
    OvlFunc_969_2008894(0x1001);
    { PIN3; q0 = 1; q1 = 0xc0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(1, 4, 0x28);
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 0x15; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_TravelTo(0x15, 0xc8, 0xbc);
    __MapActor_TravelTo(6, 0xc8, 0xcc);
    __Func_80933d4(0x33333, 0x6666);
    __Func_80933f8(0xfc << 16, 0, 0xbe << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    __PlaySound(0x17);
    { PIN2; q0 = 0x15; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    OvlFunc_969_2008894(0x15);
    __Func_80925cc(0x15, 1);
    __CutsceneWait(0x14);
    OvlFunc_969_2008894(0x15);
    __MapActor_DoAnim(0x15, 4);
    OvlFunc_969_2008894(0x15);
    __Func_8092adc(0x15, 0xc0 << 6, 0x14);
    OvlFunc_969_2008894(0x15);
    { PIN2; q0 = 6; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    __Func_809259c(3, 2);
    OvlFunc_969_2008894(0x2003);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    OvlFunc_969_2008894(0x2002);
    OvlFunc_969_20088a8(0x15, 0xe0 << 8);
    __Func_80925cc(1, 1);
    OvlFunc_969_2008894(1);
    __Func_80925cc(0x15, 1);
    __Func_8093040(0x15, 0, 0x14);
    OvlFunc_969_2008894(0x2003);
    __MapActor_DoAnim(0x15, 4);
    OvlFunc_969_2008894(0x15);
    __MapActor_Surprise(2, 0x81 << 1);
    OvlFunc_969_2008894(0x2002);
    __MapActor_SetAnim(0x15, 3);
    OvlFunc_969_2008894(0x15);
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(1, 2);
    OvlFunc_969_2008894(1);
    __MapActor_SetAnim(0x15, 4);
    OvlFunc_969_2008894(0x15);
    { PIN3; q0 = 0x15; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Jump(3, 4, 0x14);
    __Func_8093040(0x2003, 0, 0x14);
    { PIN3; q0 = 0x15; q1 = 0x103; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
}
