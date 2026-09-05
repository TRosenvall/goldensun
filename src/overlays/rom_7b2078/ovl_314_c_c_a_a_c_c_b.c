/* OvlFunc_926_2008518  --  0x02008518
 * [asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c.s, FIRST of three functions]
 *
 * 130 instructions of straight-line cutscene script. Byte-exact: 320 bytes,
 * 134 encodings and 23 relocations identical.
 *
 * THIS IS A MEMBER OF THE JUST-REOPENED "ZERO INTERLEAVED INTO A SHIFTED
 * BUILD" CLASS, AND IT IS STRAIGHT-LINE AT EVERY SITE -- no conditional
 * branch anywhere in the body, so the dominating-block lever that the old
 * "out of reach" boundary was really about never applies. The general cure
 * from the re-derivation carried it: UNIFORM WHOLE-VALUE ASCENDING FILL,
 * one statement per argument, q0/q1/q2 in order, shifts written whole
 * (`0xc4 << 1`), never as a mov+lsl pair. The hand-placed-zero special case
 * is NOT needed here -- see the cascade note below, where it was needed at
 * an intermediate stage and measured INERT at the fixpoint.
 *
 * THE WHOLE FUNCTION TURNED ON ONE SHARED CONSTANT. Written with plain
 * literals and no pins the first screen was 134 encodings against 134 with
 * 41 differing -- and the entire tail, including all three eight-argument
 * OvlFunc_common0_10c calls with their four spilled words, was already
 * exact. The single defect was `cse_main` commoning `0xc4 << 1` (0x188),
 * which appears at __Func_80921c4 and again ~20 instructions later at
 * __MapActor_TravelTo, into r5 at the top of the function:
 *
 *     rom   bl __MapActor_GetActor / mov r6, r0 / bl __CutsceneStart
 *     ours  bl __MapActor_GetActor / mov r5, #0xc4 / mov r6, r0 / ...
 *                                    ^ hoisted, and lsl r5,#1 later
 *
 * One hoisted seed displaced every instruction after it by one slot and
 * produced 41 differing lines out of 134. Two uses across calls -> PIN THE
 * ARGUMENT REGISTER, exactly as recorded; a hard call-clobbered r2 is dead
 * across the next `bl`, so gcc must rematerialise at the second site.
 *
 * PIN THE FIRST USE, CONFIRMED FROM BOTH ENDS. Dropping q2 at the FIRST
 * site (__Func_80921c4) costs 40 of 41 -- the whole defect comes back.
 * Dropping the SECOND site's pins entirely costs 2. The first-use pin is
 * the destruction of the pseudo; the later site is a rounding error.
 *
 * *** NEW FINDING -- THE PIN SET COLLAPSES IN CALL ORDER, AND AN
 * OVER-PINNED SITE CAN MASQUERADE AS A DIFFERENT LEVER. ***
 * Grepped first: docs/elevation.md records "N PINS IS A SIZE, NOT A SET",
 * "ONE PIN AT THE FIRST USE COVERS THE LATER ONES", "SOMETIMES EVERY PIN IS
 * LOAD-BEARING", and (in the alias-set entry) "a pin can be a symptom of a
 * different defect" -- where the different defect is an ALIASING one. What
 * is not recorded is that the different defect can be ANOTHER PIN OF THE
 * SAME CLASS, and that the greedy sweep therefore has a direction.
 *
 * The measured chain, each step only reachable after the previous one:
 *
 *   a6  site1 = 3 pins, SetSpeed = 3 pins (hand-placed zero form),
 *       site4 = 3 pins, two named locals              -> OK
 *   e3  site4 drops q0                                -> OK   (3/3/3 -> 3/3/2)
 *   g5  site4 drops q2 as well, split build left BARE -> OK   (-> 3/3/1)
 *   h6  SetSpeed's HAND-PLACED ZERO now measures INERT: the plain uniform
 *       ascending fill matches                        -> OK
 *   k8  SetSpeed drops q2, second split build BARE    -> OK   (-> 3/2/1)
 *   m1..m7  fixpoint: every remaining pin and both named locals fail.
 *
 * The load-bearing part is that h6 was NOT available before g5. With site4
 * still carrying three pins, the uniform fill at __MapActor_SetSpeed
 * measured 3 differing and only the hand-placed-zero spelling matched --
 * i.e. the special case looked load-bearing while it was really absorbing
 * the scheduling pressure of an over-pinned site TWENTY INSTRUCTIONS LATER.
 * A greedy sweep that stops at "this drop regresses" would have shipped the
 * special-case spelling plus three inert pins and called them load-bearing.
 *
 * PRACTICAL RULE: sweep the sites in CALL ORDER, front to back, and after
 * any successful drop re-open every LATER site including its spelling, not
 * just its pin list. The first-use pin is the only one that must be whole.
 *
 * Final pin sizes for three sites of ONE class in ONE function: 3, 2, 1.
 * That extends the recorded "two sites of identical shape wanted 2 pins and
 * 1" to a monotone run, and the two sites sharing the 0x188 constant --
 * structurally identical, `f(single, single, split)` -- ended at 3 and 1.
 *
 * WHAT NEEDED NO LEVER AT ALL. Both 0x80 << 11 and 0x80 << 9 are used twice
 * (a store into the actor at +0x48, then as an argument), and the ROM
 * carries each in a high register (r9, r10) across the calls. Left as bare
 * literals, gcc's own CSE produces exactly that, high-register prologue
 * included -- "NAMING A VALUE gcc ALREADY CARRIES DESTROYS THE CARRY", the
 * opposite polarity to the 0x188 pair in the same function. The wide push
 * {r5,r6,lr} + r8/r9/r10 saves is therefore NOT a pin tell: read by content
 * it holds two commoned constants, the actor pointer, the stack-struct
 * address and a zero, all of which gcc reaches unaided.
 *
 * THE FRAME IS THE STRUCT. `sub sp, #0x38` = 0x10 outgoing argument area for
 * the eight-argument OvlFunc_common0_10c plus a 0x28 local. `struct P` must
 * be exactly 0x28 bytes; padding it to 0x40 costs 2 differing (the `sub sp`
 * and the `add sp`). Only p.f4 is ever written. Same reading as the
 * neighbour at src/overlays/rom_78ef88/ovl_314_c_c_c_c_c_b.c.
 *
 * TWO NAMED LOCALS FOR __Func_8010704'S STACK ARGUMENTS, the template
 * neighbour's lever unchanged: the ROM emits `mov r3,#0xa / mov r2,#0x16 /
 * str r3,[sp] / str r2,[sp,#4]`, both live at once. Bare literals reuse r3
 * and the anti-dependence blocks the hoist (3 differing). Naming only one of
 * the two is also 3 -- a partial fix is no fix here.
 *
 * MEASURED WORSE (all against ref.s under tools/objcmp.py, 134 encodings):
 *
 *   no pins anywhere, plain literals                          41
 *   site1 pinned on the two SINGLE-instruction args only,
 *     split build left bare (the class's default refinement)  41  fully inert
 *   site1 pinned on q2 only                                    3
 *   site1 q0+q2 (drop q1)                                      2
 *   site1 q1+q2 (drop q0)                                      3
 *   site1 q0+q1, split bare                                   40
 *   site1 dropped entirely                                    40
 *   site4 dropped entirely                                     2
 *   SetSpeed dropped entirely                                  3
 *   SetSpeed q1+q2 (drop q0)                                   3
 *   SetSpeed q0+q2 (drop q1)                                   2
 *   __Func_8010704 literals                                    3
 *   __Func_8010704 one named local                             3
 *   struct P padded to 0x40                                    2
 *
 * NOTE THE FIRST TWO ROWS TOGETHER. The class's stated refinement -- "pin the
 * single-instruction arguments and leave the split build bare" -- is exactly
 * WRONG at site1 and measures byte-identical to no pin at all. It is a
 * scheduling refinement and it cannot address a CSE defect: here the split
 * build IS the shared constant, so its own register is the only one whose
 * pin does anything. Where the two rules collide, the shared-constant rule
 * wins. At site4, where nothing is shared any more, the refinement holds and
 * the split build is bare.
 *
 * Every relocation is an R_ARM_THM_CALL; there is no R_ARM_ABS32, so all
 * pooled words (0x17b4, 0x892, 0xffff8000) are bare literals, not symbols.
 *
 * No Makefile wildcard reaches rom_7b2078; tree default -O2. objcmp agrees
 * against scratch ref.s and against the original asm/ path.
 *
 * LANDING: SPLIT. The .s holds three functions and NO data (no .word/.byte/
 * .align/section directive anywhere in it), and exactly ONE linker-script
 * line names the object -- overlays/rom_7b2078/overlay.ld:30
 *
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c.o(.text)
 *
 * with no matching (.data) entry. The target is the FIRST of the three, so
 * following the neighbour's convention there is no _a part: this file lands
 * as src/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_b.c and the remaining two
 * functions (0x2008658 and 0x200871c) as
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_c.s, and line 30 becomes
 *
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_b.o(.text)
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_c.o(.text)
 *
 * in that order.
 */
#define PIN2 register int q0 __asm__("r0"); register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PINQ1 register int q1 __asm__("r1")

struct P {
    int f0;
    int f4;
    unsigned char pad8[0x20];
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_926_2008518(void)
{
    struct P p;
    int *a;
    int n, m;

    a = __MapActor_GetActor(9);
    __CutsceneStart();
    __MessageID(0x17b4);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q0 = 0; q1 = 0xa8; q2 = 0xc4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0xc0 << 8, 0x14);
    __PlaySound(0x84);
    __MapActor_GetActor(9)[0xa] = 0xa0 << 13;
    __MapActor_GetActor(9)[0x12] = 0x80 << 11;
    { PIN2; q0 = 9; q1 = 0xc0 << 10;
      __MapActor_SetSpeed(q0, q1, 0xc0 << 9); }
    { PINQ1; q1 = 0x98; __MapActor_TravelTo(9, q1, 0xc4 << 1); }
    __MapActor_WaitMovement(9);
    __MapActor_GetActor(9)[0x12] = 0x80 << 9;
    __Func_8092adc(9, 0, 0);
    __PlaySound(0x84);
    p.f4 = 7;
    OvlFunc_common0_10c(a[2], a[3], a[4] + (0x80 << 11), 0x80 << 8,
                        0, 0, 0x80 << 9, &p);
    OvlFunc_common0_10c(a[2], a[3], a[4] + (0x80 << 11), 0,
                        0, 0, 0x80 << 9, &p);
    OvlFunc_common0_10c(a[2], a[3], a[4] + (0x80 << 11), 0xffff8000,
                        0, 0, 0x80 << 9, &p);
    __CutsceneWait(0x1e);
    __Func_809202c();
    n = 0xa;
    m = 0x16;
    __Func_8010704(0xa, 0x18, 1, 1, n, m);
    __SetFlag(0x892);
    __CutsceneEnd();
}
