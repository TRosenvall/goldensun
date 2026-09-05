/* OvlFunc_968_200a2c8  --  0x0200a2c8      EXACT
 *
 * From asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.s.  115 instructions, 118
 * encodings, 268 bytes.  A two-actor cutscene beat: fetch map slots 8 and 9,
 * set both walking speeds, optionally cue a sound, walk each actor to the
 * point its facing index selects out of the table at `.L5148`, wait for both
 * to arrive, commit the new facing into each actor, optionally cue a second
 * sound, then sweep slots 8..12 and repaint the map under any actor whose
 * height has dropped into (-30, 0).
 *
 * VERDICT
 *   OK OvlFunc_968_200a2c8 -- 268 bytes, 118 encodings and 14 relocations identical
 * from tools/objcmp.py, against BOTH the scratch ref.s and the original
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.s path (the two agree, so no
 * Makefile pattern rule is biting).  Reproduced on three consecutive runs.
 * tryc: `OK OvlFunc_968_200a2c8  (120 lines)`.
 *
 * ------------------------------------------------------------------ LEVERS
 *
 * 1.  THE SPLIT BUILD'S OWN PIN IS LOAD-BEARING HERE, AND THE SINGLE-
 *     INSTRUCTION ARGUMENT'S PIN IS INERT -- THE EXACT INVERSE OF THE
 *     RECORDED REFINEMENT.  docs/elevation.md, "Re-derived: the wall hid 230
 *     functions", says "pin the single-instruction arguments and leave the
 *     split build bare; dropping the split build's own pin measured inert
 *     everywhere".  On this function the shipped pin set is r1 and r2 -- the
 *     two split builds -- at the FIRST `__MapActor_SetSpeed`, and adding r0
 *     (the slot, a single `mov`) measures EXACTLY ZERO.  Dropping r1 costs 7,
 *     dropping r2 costs 6, dropping both costs 10.
 *
 *     MECHANISM, and it is what reconciles the two readings.  A pinned fill at
 *     a `SetSpeed` call can be doing either of two jobs, and they need
 *     different registers:
 *       - buying the INTERLEAVE (the ROM lands a single-instruction argument
 *         inside another argument's mov/lsl pair) -- then the pin belongs on
 *         the interleaved argument;
 *       - defeating CONSTANT CSE of a value repeated at a later site -- then
 *         the pin belongs on the REPEATED value.
 *     Here `0x80 << 8` and `0x80 << 7` are passed to BOTH SetSpeed calls, and
 *     the ROM rebuilds them at each.  Site 1's ROM order is the ordinary one
 *     (r1, r2 built, then `mov r0, #8` last), so there is no interleave to buy
 *     at all; the pins exist only to kill the pseudo that cse_main would
 *     otherwise hand to site 2.  Site 2 IS a textbook interleave
 *     (`mov r1 / mov r2 / mov r0, #9 / lsl r1 / lsl r2`) and needs NO PIN OF
 *     ITS OWN -- it falls out for free once site 1 has no pseudo to share.
 *     READ WHICH JOB THE PIN IS DOING BEFORE CHOOSING ITS REGISTERS.
 *
 * 2.  PIN THE FIRST USE, confirmed from both ends.  Site 1 pinned + site 2
 *     bare is exact; site 1 bare + site 2 pinned is 10 -- identical to no pins
 *     at all.  Matches "ONE PIN AT THE FIRST USE COVERS THE LATER ONES".
 *
 * 3.  NAMED LOCALS DO NOT SUBSTITUTE FOR THE PIN HERE, AND NEITHER DOES A
 *     FLAG.  The uniform whole-value ascending fill written as ordinary locals
 *     (`s1/u1`, `s2/u2`, one per site, per the batch-129 `OvlFunc_953_200a820`
 *     recipe) is 10 -- byte-identical to the bare literals.
 *     `-fno-rerun-cse-after-loop` is 7 and `-fno-gcse` is inert.  This is the
 *     "commoned constant has TWO remedies and they are not interchangeable"
 *     entry with a THIRD outcome: on this function neither remedy works and
 *     only the hard call-clobbered destination does.  Consistent with "THE
 *     REFERENCE-COUNT CURE CANNOT SPLIT A CONSTANT SHARED ACROSS SITES".
 *
 * 4.  THE DIVISION IS WRITTEN TWICE, NOT NAMED ONCE.  The ROM's signed
 *     `/ 0x10000` keeps THREE registers:
 *
 *         ldr r3, [r0, #0xc]   / mov r2, r3 / cmp r3, #0 / bge .L
 *         ldr r1, =0xffff      / add r2, r3, r1
 *     .L: asr r2, #0x10
 *
 *     -- dividend in r3, bias temp in r2, and the QUOTIENT coalesced into the
 *     TEMP's register.  Every single-assignment spelling collapses it, and the
 *     failures point in two different directions (table below): naming the
 *     quotient once makes the whole chain one register and the body one
 *     instruction SHORT, while `v = c->fc; v = v / 0x10000;` keeps the copy but
 *     gives the quotient the DIVIDEND's register.  Pinning the quotient to r2
 *     and/or the dividend to r3 deletes the copy instead.
 *
 *     The cure is to NOT name the quotient and to write the division once per
 *     comparison:
 *
 *         if (c->fc / 0x10000 < 0) {
 *             if (c->fc / 0x10000 > -0x1e) {
 *
 *     cse commons the two divisions into one pseudo whose live range begins at
 *     the bias temp, so the quotient inherits r2 while the load keeps r3.
 *
 *     THIS IS NOT NEW MACHINERY -- it is "A 'redundant' register copy is
 *     usually a SECOND READ gcc has CSEd" (and its diagnostic: a body ONE
 *     INSTRUCTION SHORT with a `mov rA, rB` in the ROM's missing line).  What
 *     is new is the SUB-SHAPE: that entry is written about a MEMORY READ
 *     through a pointer, and the thing duplicated here is a compiler-generated
 *     DIVISION TEMP.  Naming only the load (`n = c->fc; v = n / 0x10000;`) is
 *     NOT enough -- the division expression itself has to appear twice.
 *
 * 5.  TWO IDENTICALLY-SHAPED `__Actor_TravelTo` CALLS BOTH WANT THE TABLE
 *     INDEX NAMED, AND NOT THE TABLE VALUE.  `d = actor->f64;` as its own
 *     statement, with `L5148[d]` left inline in the argument list, is exact at
 *     both sites.  Inlining the index costs 15 at site 1 and 7 at site 2;
 *     naming the looked-up VALUE instead (`t2 = L5148[b->f64]`) costs 17 at
 *     site 2, though it is also exact at site 1.  Naming the index is what
 *     hoists `mov r6, r10 / add r6, #0x64` -- the SECOND actor's `&f64`
 *     address -- above the FIRST call, which is where the ROM computes it.
 *
 * 6.  Ordinary levers that were also required: `x`/`y` named per stack-argument
 *     site at `__Func_8010704` (inlining them costs 5); the two range tests
 *     written as NESTED `if`s rather than one `&&` (fusing costs 7); the
 *     `.L5148` table reached with gcc's asm-label extension, which is safe
 *     because the label is high-numbered -- gcc's own labels in this TU are
 *     .L3, .L4, .L7, .L8, .L10, .L15, .L16, so nothing is captured.
 *
 * ------------------------------------------------- MEASURED-WORSE TABLE
 * Metric: `tryc --align` "instruction(s) in disagreeing regions, of 120",
 * every row a single change against the source below.
 *
 *   spelling                                                    differing
 *   ------------------------------------------------------------  -------
 *   SHIPPED                                                             0
 *   site 1: add a PIN0 on the slot argument                     0 (INERT)
 *   site 1: mov+lsl fill (`q1 = 0x80; q1 <<= 8;`) not whole      0 (INERT)
 *   TravelTo #1: name the VALUE, not the index                   0 (INERT)
 *   site 1: drop PIN2, keep PIN1                                        6
 *   site 1: drop PIN1, keep PIN2                                        7
 *   site 1 and 2 both bare                                             10
 *   site 1 bare, site 2 pinned instead                                 10
 *   both sites: whole-value NAMED locals, no pins                      10
 *   FINAL + -fno-rerun-cse-after-loop               7, and 119 lines (short)
 *   FINAL + -fno-gcse                                            0 (INERT)
 *   FINAL at -O1                                     48, and 125 lines
 *   TravelTo #1: index inlined                                         15
 *   TravelTo #2: index inlined                                          7
 *   TravelTo #2: name the VALUE, not the index                         17
 *   Func_8010704: x, y inlined                                          5
 *   div: `v = c->fc / 0x10000;`                      7, and 119 lines (short)
 *   div: `v = c->fc; v = v / 0x10000;`                                  5
 *   div: `n = c->fc; v = n / 0x10000;`               7, and 119 lines (short)
 *   div: bias branch written out by hand             7, and 119 lines (short)
 *   div: quotient pinned to r2                       8, and 119 lines (short)
 *   guard fused as `if (v < 0 && v > -0x1e)`         7, and 119 lines (short)
 *
 * Three inert rows are listed because they are the ones a reader would expect
 * to be load-bearing.  None of them ships.
 *
 * -------------------------------------------------------------- LANDING
 * NO SPLIT.  The `.s` holds exactly ONE function (.thumb_func_start on line 11,
 * .func_end on line 132) and NO data: it contains no .section, .data, .word,
 * .byte, .hword, .incbin, .space, .lcomm or .global directive.  (The overlay's
 * .map lists a zero-length .data for the object, which is a claim to check and
 * not evidence -- checked, and the .s carries none.)
 *
 * ONE linker-script line names the object, matched on the FULL PATH:
 *     overlays/rom_7f2f14/overlay.ld:76
 *         asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.o(.text)
 * No other .ld in the tree refers to it; the overlay's .data section names only
 * asm/overlays/rom_7f2f14/ovl_30_c_c_c_c.o.  Landing is this file at
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.c, delete the .s, and swap that
 * line's `asm/` for `src/`.
 *
 * FLAGS: the DEFAULT -O2 rule (`asm/%.o: src/%.c`, Makefile line 146).  No
 * pattern rule in the Makefile matches this stem -- the two rom_7f2f14
 * wildcards are `ovl_30_c_a_c_a_c_a%` and `ovl_30_c_a_c_a_c_c%` -- and
 * tryc.makefile_flags() on the landing path returns the empty set.  No flag
 * group is needed and none may be added: -O1 is 48 differing.
 *
 * `.L5148` lives in asm/overlays/rom_7f2f14/ovl_30_c_c_c_c.s, which already
 * declares it `.global`, so the asm-label extern below needs no change there.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50];
    short f64;
};

extern int L5148[] __asm__(".L5148");

extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);

#define PIN1  register int q1 __asm__("r1")
#define PIN2  register int q2 __asm__("r2")

void OvlFunc_968_200a2c8(int arg)
{
    struct A *a;
    struct A *b;
    struct A *c;
    unsigned int i;
    int d1;
    int d2;
    int x;
    int y;

    a = __MapActor_GetActor(8);
    b = __MapActor_GetActor(9);
    { PIN1; PIN2; q1 = 0x80 << 8; q2 = 0x80 << 7; __MapActor_SetSpeed(8, q1, q2); }
    __MapActor_SetSpeed(9, 0x80 << 8, 0x80 << 7);
    if (arg != 0) {
        __PlaySound(0xb4);
    }
    d1 = a->f64;
    __Actor_TravelTo(a, a->f8, L5148[d1], a->f10);
    d2 = b->f64;
    __Actor_TravelTo(b, b->f8, L5148[d2], b->f10);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    a->fc = L5148[a->f64];
    b->fc = L5148[b->f64];
    if (arg != 0) {
        __PlaySound(0x121);
    }
    for (i = 0; i <= 4; i++) {
        c = __MapActor_GetActor(i + 8);
        if (c->fc / 0x10000 < 0) {
            if (c->fc / 0x10000 > -0x1e) {
                x = c->f8 >> 20;
                y = c->f10 >> 20;
                __Func_8010704(4, 0x13, 1, 1, x, y);
            }
        }
    }
    __CutsceneWait(arg);
}
