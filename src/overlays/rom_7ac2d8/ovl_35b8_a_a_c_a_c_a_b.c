/* OvlFunc_924_200b860  --  0x0200b860
 * OvlFunc_924_200b948  --  0x0200b948
 * [asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.s, FIRST TWO of FOUR functions.
 *  THE FILE DOES NOT LAND WHOLE -- see "LANDING" at the foot of this comment.]
 *
 * VERDICT: MATCH, both.
 *   OK OvlFunc_924_200b860 -- 232 bytes, 108 encodings and 7 relocations identical
 *   OK OvlFunc_924_200b948 -- 284 bytes, 131 encodings and 8 relocations identical
 * Each confirmed TWICE, and the second time from the FINAL combined text: once
 * against the single-function extracts ref_a.s / ref_b.s, and once against the
 * ORIGINAL four-function path asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.s --
 * same line both times, from single-function extracts final_a.c / final_b.c cut
 * out of this file.
 *
 * THE COMBINED OBJECT IS CONFIRMED BY ARITHMETIC, NOT BY A --func RUN, because
 * objcmp cannot isolate one function in a multi-function CANDIDATE: pointed at
 * this file it reports "ours 516 bytes" for both names and the recorded
 * `XX RELOCATIONS differ` artifact fires. The artifact is the confirmation once
 * it is read properly. The combined object is 516 bytes = 232 + 284 EXACTLY and
 * 239 encodings = 108 + 131 EXACTLY, and its fifteen relocations are the two
 * reference lists CONCATENATED, the second displaced by exactly 232 bytes
 * (0xe8): ref b948 has 0x1e/0x56/0x84/0x8a/0xba/0xc0/0xde/0xf4 and the combined
 * object has 0x106/0x13e/0x16c/0x172/0x1a2/0x1a8/0x1c6/0x1dc, every one of them
 * ref + 0xe8. Nothing crosses the function boundary -- no shared pool word, no
 * shifted literal -- so the pair is safe to ship in ONE translation unit.
 * DEFAULT FLAG SET. No flag group is involved -- the generic `asm/%.o: src/%.c`
 * rule at Makefile:146 with plain GCC296_CFLAGS is what these want, and no
 * Makefile edit is needed.
 *
 * Two passes (b860) and three passes (b948) of an eight-tick sparkle over a map
 * rectangle: paint the tiles, then on each odd tick spawn one particle from a
 * jittered point and wait a frame, repainting two rows at the end of each pass.
 * b948 alternates between two different spawn lines on ticks 1,5 and 3,7.
 *
 * ---------------------------------------------------------------- provenance
 *
 * tools/solved_twins.py: "solved shapes 2117, remaining functions 1508 ...
 * REMAINING FUNCTIONS WITH A SOLVED TWIN: 0 across 0 templates". No twin, as
 * expected; the family was reached by callee-set identity instead
 * (§PICK TARGETS BY TEMPLATE), grepping `OvlFunc_common0_10c` across solved
 * src/ in this same overlay.
 *
 * THE SCORED 1.00 WAS WORTH ITS STRUCT AND PROTOTYPE AND NOTHING ELSE, which is
 * exactly what templated.py's own note about a four-symbol 1.00 predicts.
 * src/overlays/rom_7ac2d8/ovl_1db4_a_b.c (OvlFunc_924_200a030) handed over
 * `struct P` verbatim, the eight-argument spawner prototype and the
 * `__Random() * N >> 16` fixed-point idiom -- all three correct first time. Its
 * THREE named levers all measured HARMFUL or INERT here (table below). The
 * leverage was the family, not the score.
 *
 * ------------------------------------------------- the one lever that matters
 *
 * BOTH FUNCTIONS MATCH WITH NO PIN AT ALL, AND EVERY PIN I ADDED WAS ME PAYING
 * FOR THE WRONG LOOP SHAPE. This is the sufficient-not-necessary rule arriving
 * from the direction that is hardest to see, so it is worth the space.
 *
 * The template's headline lever is "THE INNER LOOP IS A `goto` LOOP AND THE
 * OUTER ONE IS NOT, and that split is the whole function." I applied it. With a
 * `goto` inner loop b860 came out at 67 differing and 4 bytes long, because
 * `k` had taken a HIGH register (r8) and the hoisted `1` had taken r7 -- the
 * ROM has them the other way round. Naming `int one = 1;` and pinning it at the
 * `i & one` test plus the two in-loop `__CopyMapTiles` calls swapped them back
 * and took it to 8 differing with SIZE AND RELOCATIONS BOTH SILENT; sweeping
 * the setup statement order got it to 6. That is the recorded ORDERING-pin
 * signature, and it is a trap: a small silent-size residue reads like "one more
 * ordering tweak" when it is really the shape underneath being wrong. The
 * residue would not close, because the last two `mov rHIGH` copies were
 * permuted and no source order can flip them while one value is a source
 * assignment and the other is an LICM hoist.
 *
 * WRITING THE INNER LOOP AS AN ORDINARY `do/while` AND DELETING `one` MATCHED
 * ON THE NEXT TRY. Same for b948: an `int d = 0x90 << 12;` -- the template's own
 * second lever, transplanted -- gave 7 differing with size and relocations
 * silent, and deleting it matched. MECHANISM: with a real loop, LICM hoists
 * BOTH the `1` and the bias/`d` into callee-saved registers itself, and it
 * emits them in the order it DISCOVERS them in the loop body. In both ROMs the
 * discovery order is the ROM's emission order. Name one of the two and it stops
 * being a hoist and becomes a preheader assignment, which is emitted BEFORE the
 * hoisted one -- so naming a value here does not order the pair, it inverts it.
 * A hand-named value can never be re-ordered against an LICM hoist by moving its
 * statement, which is why the residue was closed and not shrinkable.
 *
 * SO: READ THE `goto`-LOOP LEVER'S OWN PRECONDITION, DO NOT COUNT ARMS. The
 * template's precondition is a STRENGTH-REDUCTION signature -- "the ROM rebuilds
 * `lsl r2, r7, #2` inside every arm". Both of these ROMs rebuild an
 * inner-loop-invariant shift inside the body (`lsl r1, r7, #0x4` in b860,
 * `lsl r2, r7, #0x13` in b948's first arm) and I read that as the signature. It
 * is NOT: gcc-2.96 declines to hoist an invariant used ONCE in the loop, because
 * rematerialising a shift is cheaper than a callee-saved register, so a
 * single-use invariant is rebuilt in a perfectly ordinary loop too. The
 * template's three arms each used its value, giving three uses and a real
 * hoist; one use looks identical to a disabled optimiser and is not.
 * b948 carries the discriminator IN THE ROM: r10 is a genuine giv, initialised
 * to 0xfffe0000 at the top of the outer body and stepped `add r10, r2` with
 * r2 = 0xfffe0000 each tick -- that is `-(i << 17)` strength-reduced, and
 * strength reduction is precisely what a `goto` loop turns off. A live giv in
 * the ROM is positive proof the loop is ORDINARY, and it outranks any number of
 * rebuilt invariants. b860 has no giv either way, so it was settled by
 * measurement: `goto` 6 differing, ordinary loop exact.
 *
 * ----------------------------------------------- NEW (grepped by concept first)
 *
 * A SINGLE-USE LOOP INVARIANT IS NOT A `goto`-LOOP TELL, AND A LIVE giv REFUTES
 * ONE. Grepped docs/elevation.md by concept before writing this up -- "loop
 * optimisation", "strength", "giv", "invariant", "rebuild", "hoist", "used
 * once" -- across §"`goto` loops disable loop optimisation ENTIRELY", its
 * "what actually separates it from its counter-example" table and the LICM
 * entries. The recorded tells are all about what the ROM REBUILDS. Nothing
 * records the converse test, that a strength-reduced induction variable in the
 * ROM PROVES the loop is ordinary and settles the question in one look, nor the
 * use-count bound that makes the rebuild tell unreliable below two uses. Both
 * halves are new and both are cheap to check before writing any C.
 *
 * ------------------------------------------------- what IS load-bearing, then
 *
 * `unsigned int i, k` -- 2 differing in EACH function if signed (`bhi` where the
 * ROM has `bls`, at index 76 and index 96). Same one-encoding tell the sibling
 * ovl_35b8_a_a_c_a_c_c_a_b.c records; check the terminating branch, not the shifts.
 *
 * `p.f4` FIRST, ascending after it -- 93 differing in b860 if the two 0x8000
 * stores are hoisted above it. The sibling records this exact defect at 13
 * differing on its own struct; it is worth 93 here. f0 is never written in
 * either function, again.
 *
 * THE TWO ARMS OF b948's `if` WANT OPPOSITE SHIFT GROUPINGS, and so do the two
 * FUNCTIONS. This is the template's "adjacency does not carry a spelling over"
 * confirmed twice more, once WITHIN one function:
 *     b948 arm A  0x22e0000 - (i << 17) - (k << 19)   shifts APART
 *                 grouped instead: 112 differing, +4 bytes
 *     b948 arm B  ((k * 4 + i) << 17) + 0xb70000      shifts TOGETHER
 *                 apart instead:   113 differing, +8 bytes
 *     b860        ((-i - (k << 4)) << 16) + 0x88 * 0x40000   TOGETHER
 *                 apart instead:   117 differing, +24 bytes
 * Only the ROM's own instruction order says which, every time.
 *
 * THE COUNTER INITIALISER MUST COME AFTER THE STRUCT STORES IN b860 -- and it
 * is INERT in b948. The template's "counter initialisation wants to come first"
 * lever, and its three recorded confirmations, are 2 differing here if obeyed
 * (`k = 0;` hoisted above `p.f4 = 5;`). In b948 the same move is exact either
 * way. One lever, two functions 232 bytes apart, three different verdicts across
 * this pair and the template. Re-measure it; never transplant it.
 *
 * -------------------------------------------------------- measured worse
 *
 *   b860                                                 enc   size
 *     goto inner loop + named `one` + best order          6    =      (dead end)
 *     goto inner loop, no pin                            67   +4
 *     naming the 0xffff3334 bias                         96   -4
 *     `int one` at the CopyMapTiles but not the test     87   -4
 *     p.f8/p.fc stores above p.f4                        93   =
 *     shifts kept apart in z                            117  +24
 *     signed i, k                                         2    =
 *     `k = 0;` above the struct stores                    2    =
 *   b948
 *     named `int d = 0x90 << 12;`                         7    =      (dead end)
 *     goto inner loop                                   130  -24
 *     arm A grouped                                     112   +4
 *     arm B apart                                       113   +8
 *     named `int zero`                                   69    =
 *     signed i, k                                         2    =
 *     `k = 0;` above the struct stores                    0    =      (inert)
 *
 * Naming the bias is the one that corrects a mechanism. The template says a
 * named constant KEEPS a callee-saved register ("gcc will ADD a callee-saved
 * register to share a constant", read backwards). Here naming it REMOVED one:
 * the push set drops to two high registers and `ldr r3, =0xffff3334` is
 * rematerialised twice inside the loop. The bound is the REMATERIALISATION COST
 * of the constant, not whether it has a name -- 0xffff3334 is one `ldr` from the
 * pool, so gcc prefers the pool to a register the moment the value has a pseudo
 * of its own; the template's 0x90 << 12 costs two instructions and does not.
 *
 * The frame is arithmetic, as always: sp = 0x38 is 0x10 of outgoing stack
 * arguments (the spawner takes eight) plus the 0x28-byte `struct P` at sp+0x10,
 * in both functions. `struct P` is the family's, unchanged.
 *
 * b860 pushes r8/r9/r10, b948 r8/r9/r10/r11, and both fall out unaided -- the
 * prologue is by CONTENT here and the content is right once the loop shape is.
 * r4 is absent from both push sets because `-fcall-used-r4` is in GCC296_CFLAGS.
 *
 * ------------------------------------------------------------------- LANDING
 *
 * THIS FILE CANNOT LAND WHOLE AND A LINKER EDIT IS REQUIRED. The .s holds FOUR
 * functions, not two: OvlFunc_924_200b860, OvlFunc_924_200b948, then
 * OvlFunc_924_200ba64 (83 instructions) and OvlFunc_924_200bb24, both still
 * unsolved. The two solved ones are the FIRST TWO and are contiguous, so this is
 * a clean two-way split at the .func_end of OvlFunc_924_200b948:
 *
 *   src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c   this file (functions 1-2)
 *   asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_c.s   functions 3-4, unchanged
 *
 * naming per the convention this same directory already uses: splitting
 * ovl_35b8_a_a_c_a_c_c_a.s gave _c_c_a_b (solved .c, leading piece) and
 * _c_c_a_c (trailing asm remainder), and both appear in the linker script in
 * that order.
 *
 * EXACTLY ONE LINKER LINE NAMES THE OLD .o, matched on full path and cited by
 * content, in overlays/rom_7ac2d8/overlay.ld (indented with TWO TABS,
 * shown here as <TAB> because this is a C comment):
 *
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.o(.text)
 *
 * It sits between the lines for ovl_35b8_a_a_c_a_b.o(.text) and
 * ovl_35b8_a_a_c_a_c_b.o(.text), and must be REPLACED BY TWO LINES IN THIS ORDER:
 *
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.o(.text)
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_c.o(.text)
 *
 * There is NO .data, .rodata or .bss line for this .o anywhere in the tree --
 * grepped the whole repo for the stem, and overlay.ld:92 is the only hit
 * outside asm/. Neither object has a section beyond .text: both functions reach
 * every constant through `ldr rX, =value`, which the ASSEMBLER pools into
 * .text, so there is nothing to remap. No Makefile rule is needed either; the
 * default-flag match means the generic pattern rule already covers the new .c.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_200b860(void)
{
    struct P p;
    unsigned int k, i;

    __CopyMapTiles(0x4a, 0x3a, 0x46, 0x22, 1, 1);
    p.f4 = 7;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    for (k = 0; k <= 1; k++) {
        for (i = 0; i <= 7; i++) {
            if (i & 1) {
                OvlFunc_common0_10c(0xd2 << 15, 0,
                                    ((-i - (k << 4)) << 16) + 0x88 * 0x40000,
                                    (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                                    0,
                                    (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                                    0x90 << 12, &p);
                __CutsceneWait(1);
            }
        }
        __CopyMapTiles(0x4a, 0x3b, 0x46, 0x22 - k, 1, 1);
        __CopyMapTiles(0x4a, 0x3a, 0x46, 0x21 - k, 1, 1);
    }
}

void OvlFunc_924_200b948(void)
{
    struct P p;
    unsigned int k, i;

    __CopyMapTiles(0x4c, 0x3d, 0x4a, 0x26, 1, 1);
    p.f4 = 5;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    for (k = 0; k <= 2; k++) {
        for (i = 1; i <= 7; i++) {
            if (i & 1) {
                if (i & 2)
                    OvlFunc_common0_10c((0x69 - (__Random() * 5 >> 16)) << 16, 0,
                                        0x22e0000 - (i << 17) - (k << 19), 0,
                                        0, -0x4000, 0x90 << 12, &p);
                else
                    OvlFunc_common0_10c(((k * 4 + i) << 17) + 0xb70000, 0,
                                        (0x26c - (__Random() * 5 >> 16)) << 16,
                                        0x80 << 7, 0, 0, 0x90 << 12, &p);
                __CutsceneWait(1);
            }
        }
        __CopyMapTiles(0x47, 0x3b, 0x46, 0x22 - k, 1, 1);
        __CopyMapTiles(0x47, 0x3b, k + 0x4b, 0x26, 1, 1);
    }
}
