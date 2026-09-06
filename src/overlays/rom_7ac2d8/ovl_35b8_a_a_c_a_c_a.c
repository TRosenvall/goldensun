/* OvlFunc_924_200b860  --  0x0200b860
 * OvlFunc_924_200b948  --  0x0200b948
 * OvlFunc_924_200ba64  --  0x0200ba64
 * OvlFunc_924_200bb24  --  0x0200bb24
 * [asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.s -- ALL FOUR FUNCTIONS.
 *  THIS FILE COLLAPSES THE SPLIT MADE IN BATCH 240; see "LANDING" at the foot.]
 *
 * VERDICT: MATCH, all four, and the split is dead.
 *   OK OvlFunc_924_200b860 -- 232 bytes, 108 encodings and 7 relocations identical
 *   OK OvlFunc_924_200b948 -- 284 bytes, 131 encodings and 8 relocations identical
 *   OK OvlFunc_924_200ba64 -- 192 bytes, 87 encodings and 7 relocations identical
 *   OK OvlFunc_924_200bb24 -- 176 bytes, 81 encodings and 5 relocations identical
 * DEFAULT FLAG SET. NO flag group is involved in any of the four: the generic
 * `asm/%.o: src/%.c` rule with plain GCC296_CFLAGS is what they want, objcmp
 * printed no "(built with: ...)" line, and no Makefile edit is needed.
 *
 * ba64 and bb24 BOTH MATCHED ON THE FIRST CANDIDATE, with no pin, no named
 * constant and no flag. Every line of the table below was measured AFTERWARDS,
 * by perturbing a known-exact file -- which is the only way any of it is
 * trustworthy, and is much cheaper than perturbing a broken one.
 *
 * Each of the four is confirmed TWICE and the second time FROM THIS COMBINED
 * TEXT: once from a standalone single-function candidate, and once from a
 * single-function extract cut out of this file (preamble + one function), both
 * against the recovered pre-split reference
 * `git show a3477517^:asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.s`. ba64 and
 * bb24 additionally verify against the post-split path they live on today,
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_c.s -- same OK line every time.
 * THE RE-SCREEN AFTER THE MERGE WAS NOT A FORMALITY AND MUST NOT BE SKIPPED:
 * merging added `struct A` and two externs above b860 and b948, and a
 * recombination is not a concatenation. It happened to be inert here (see
 * "THE MERGE COST NOTHING" below), but that is a measurement, not an assumption.
 *
 * THE COMBINED OBJECT IS CONFIRMED BY ARITHMETIC, NOT BY A --func RUN, because
 * objcmp cannot isolate one function in a multi-function CANDIDATE: pointed at
 * this file with any --func it reports "ours 884 bytes" and fires
 * `XX RELOCATIONS differ`. Read properly, that artifact IS the confirmation.
 *   size    884 = 232 + 284 + 192 + 176   EXACTLY
 *   enc     407 = 108 + 131 +  87 +  81   EXACTLY
 *   relocs   27 =   7 +   8 +   7 +   5   EXACTLY
 *   nm      b860 @ 0x000, b948 @ 0x0e8 (=232), ba64 @ 0x204 (=516), bb24 @ 0x2c4 (=708)
 * and every relocation is its own function's list displaced by that function's
 * start. ba64's seven are 0x0e/0x2c/0x46/0x58/0x6c/0x9e/0xb4 standalone and
 * 0x212/0x230/0x24a/0x25c/0x270/0x2a2/0x2b8 combined -- each one + 0x204, symbol
 * for symbol. bb24's five are 0x2e/0x48/0x5c/0x8c/0xa4 and
 * 0x2f2/0x30c/0x320/0x350/0x368 -- each one + 0x2c4. b860's and b948's are
 * unchanged from batch 240's recorded lists. NOTHING CROSSES A FUNCTION
 * BOUNDARY: the R_ARM_ABS32 on `iwram_3001e40` appears TWICE, once inside
 * ba64's own range and once inside bb24's, so the two functions do not share a
 * pool word; nor do the two 0xffff3334 words, nor the two 0xb333 words. The
 * exact size and encoding sums are the same statement from the other side.
 *
 * ba64 and bb24 are the same STRAIGHT-LINE CALL SCRIPT twice: bail unless the
 * global frame counter is a multiple of four, pick a 7-or-5 particle kind on a
 * coin flip, fill the parameter block, and spawn one particle with two
 * independent 0..7 jitters scaled by 0x3333 and biased by -0xcccc. bb24 takes
 * its spawn point as three parameters; ba64 fetches actor 0 and takes the point
 * from the actor, adding a third 0..3 random to the y coordinate.
 *
 * --------------------------------------------------------------- provenance
 *
 * tools/solved_twins.py: "solved shapes 2125, remaining functions 1500 ...
 * REMAINING FUNCTIONS WITH A SOLVED TWIN: 0 across 0 templates". No twin, as
 * expected -- and as batch 240 already found for the first half of this same
 * file. The family was reached the way the notebook says to reach it when the
 * twin tool is empty, and TWO DIFFERENT AXES BOTH PAID:
 *   - the ROM-ADJACENT solved file, src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c,
 *     handed over `struct P`, the eight-argument spawner prototype, the
 *     `__Random() * N >> 16` fixed-point idiom and the `* 0x3333 - 0xcccc`
 *     scale -- all four correct first time;
 *   - an IDIOM-GREP for `iwram_3001e40` across the PARKED corpus landed
 *     src/non_matching/ovl_7f2f14/200c968.c, which carries this exact preamble
 *     (`v = iwram_3001e40 & 3; if (v != 0) return 0;` with `v` then passed as
 *     the sixth argument) and its `struct A { pad00[8]; f8; fc; f10; }`.
 * §"a park blocked on something unrelated is still a correct idiom template"
 * behaving as written. 200c968 is parked on SCRATCH-REGISTER SELECTION, and its
 * park note is worth reading for the contrast: its ROM builds the block in r2
 * and holds it in r10 (`add r2, sp, #0x10 / mov r10, r2`) and keeps the
 * global's ADDRESS live for two derefs. BOTH of these ROMs do the opposite --
 * `add r7, sp, #0x10` straight into the register that is used, and ONE deref of
 * the global -- which is exactly the shape 200c968's note records as "ours".
 * Reading a park's residue tells you which sibling shape is FREE.
 *
 * --------------------------------------------- the levers that actually paid
 *
 * THE ORDER OF THE TWO ENTRY READS IS THE BIGGEST SINGLE LEVER IN ba64, AT 79
 * OF 87. The ROM opens `mov r0, #0 / sub sp, #0x38 / bl __MapActor_GetActor`
 * and only then loads the global. Reading the global first --
 * `t = iwram_3001e40 & 3; a = __MapActor_GetActor(0);` -- is 79 differing with
 * SIZE SILENT and the first divergence at index 5. Nothing is added or removed;
 * the pool load simply moves above the call and every high-register assignment
 * behind it re-shuffles. This is §"POINTER BIRTH ORDER decides which register
 * each pointer gets" and §"Register allocation follows ASSIGNMENT position" in
 * their most brutal form: two adjacent statements with no data dependence
 * between them, and 91% of the function riding on which one is written first.
 * Note this is the FIRST thing to get right and the LAST thing a residue points
 * at -- the guard reads like scaffolding, so it gets written in whichever order
 * feels natural. Read the ROM's first three instructions instead.
 *
 * THE REDUNDANT `& 1` IS SOURCE, NOT CODEGEN -- 66 of 87 in ba64 and 74 of 81
 * in bb24, and it makes the function LONGER. `__Random() * 2 >> 16` is already
 * 0 or 1, so `((__Random() * 2 >> 16) & 1) == 0` and `(__Random() * 2 >> 16) == 0`
 * are the same predicate to a reader. They are not the same to gcc-2.96, which
 * cannot prove `__Random`'s range across the call: the mask survives as
 * `mov r3, #1 / and r0, r3` and the ROM has both instructions. Dropping it is
 * -4 bytes in ba64 and -12 in bb24. See "NEW" below.
 *
 * THE PARAMETER BLOCK'S f4 STORE MUST COME FIRST, a FOURTH and FIFTH
 * confirmation, and it costs 14 in each function. `p.f4 = 7;`, then the coin
 * flip that overrides it with 5, THEN the two 0xb333 stores. Writing the two
 * 0xb333 stores first -- the ascending-offset spelling that looks tidier -- is
 * 14 differing in BOTH functions with SIZE SILENT, first divergence
 * `ref 2307 (mov r3, #7)` against `ours 4b24 (ldr r3, [pc, ...])`. The two
 * siblings record this same defect at 13 (ovl_35b8_a_a_c_a_c_c_a_b.c) and 93
 * (ovl_35b8_a_a_c_a_c_a_b.c) on their own struct. Five functions in this family
 * now, five times f4 first. f0 is never written in any of the five.
 *
 * `a->fc + (expr containing a call)` IS NOT `(expr containing a call) + a->fc`
 * -- 42 of 87, SIZE SILENT. See "NEW" below; this is a boundary on a recorded
 * NEGATIVE, so it is the kind of thing that stops a round if it is not known.
 *
 * ------------------------------------------ NEW (both grepped by concept first)
 *
 * (1) A REDUNDANT MASK ON A FIXED-POINT EXTRACT IS A SOURCE TELL. Grepped
 * docs/elevation.md by concept before writing this: "redundant mask", "& 1",
 * "and r0, r3", "provably", "0 or 1", "narrowed", "Random", "fixed-point",
 * across §"A mask applied to a byte gets NARROWED unless it is named",
 * §"A value that is provably constant inside its branch is NOT evidence" and
 * the HImode-literal entries. What is recorded is the opposite direction --
 * masks that gcc NARROWS or REWRITES (§1230's `(x & (1 << bit)) != 0` becoming
 * a shift), and constants gcc proves. Nothing records the plain case:
 * **`and rX, #1` sitting between a `lsr #16` extract and a `cmp #0` means the
 * SOURCE wrote the mask, and it is worth two instructions and most of the
 * function.** The reading rule is cheap and general: an instruction that a
 * range-aware reader would call dead is dead to the READER, not to gcc-2.96,
 * whenever the value came back from a call. Do not "simplify" it out.
 * (2) THE COMMUTATIVE-OPERAND BOUNDARY LIFTS WHEN ONE OPERAND HAS A SIDE
 * EFFECT. §"The source-order lever does not reach commutative operands" says
 * `a | b` and `a + b` are interchangeable in the RTL before allocation, so
 * "there is nothing left in the source for the ordering to attach to", and
 * closes with "do not spend a round on this shape". That is stated for two
 * INDEPENDENT VALUES differing only in which register they land in, and it is
 * right there. It does NOT hold when one operand is a call: writing
 * `((__Random() * 4 >> 16) << 16) + a->fc` for the ROM's
 * `a->fc + ((__Random() * 4 >> 16) << 16)` is **42 of 87 differing with SIZE
 * SILENT**, because the operand order now decides whether the `ldr r2, [r2, #0xc]`
 * is scheduled before or after the `bl`, which is a SEQUENCING question and not
 * a register-role one. Grepped "commutative", "side effect", "evaluation
 * order", "left-to-right" first; §14644's "argument evaluation order is not
 * something the source can state" is about ARGUMENTS of one call, a different
 * claim. **Test for the boundary by asking whether either operand can trap,
 * call or store; if one can, the source order is load-bearing and worth one
 * screen.**
 *
 * ------------------------------------------------------- INERT, and worth it
 *
 * NINE spellings measured BYTE-IDENTICAL. These are recorded because six of
 * them are levers some sibling in this family needed, and shipping an inert
 * lever is scaffolding that the next reader has to disprove:
 *   `int t` vs `unsigned int t`         -- INERT in both. The siblings' single
 *       most reliable tell (`unsigned int i, k`, worth exactly 2 encodings in
 *       b860, b948 and ovl_35b8_a_a_c_a_c_c_a_b.c) has NOTHING to attach to
 *       here: it lives in the loop's terminating `bls`, and these two functions
 *       have no loop. Only `cmp #0 / bne`, which is signedness-blind.
 *   `if (t != 0) return;` vs `if (t == 0) { ... }`  -- INERT in both, despite
 *       §"An early `return 0` guard is not the same shape as a tail `return 0`"
 *       and §"Early return versus else-return is worth four instructions".
 *       Those are about a RETURN VALUE; a void guard with nothing after it
 *       collapses to the same CFG either way.
 *   named `int d = 0x90 << 12;`         -- INERT in ba64. THIRD VERDICT for one
 *       lever inside one family: LOAD-BEARING in ovl_1db4_a_b.c (it orders two
 *       `mov rHIGH` copies, 8 differing without it), HARMFUL in b948 (7
 *       differing WITH it), INERT here. Re-measure it; never transplant it.
 *   `struct A *a; a->f8`  ==  `unsigned char *a; *(int *)(a + 8)`
 *                         ==  `int *a; a[2]`     -- all three INERT. This is
 *       NOT a counter-example to §"Symbol base: index a typed array, don't
 *       cast-and-offset" or to the subscript-as-declared rule: those govern a
 *       DECLARED ARRAY OBJECT with a known element type, where `g[K]` and
 *       `*(g + K)` genuinely differ. An opaque pointer returned from a call has
 *       no declared extent, the offset is constant, and all three spellings
 *       reduce to one `ldr rD, [rB, #K]`. Pick the one that reads best --
 *       `struct A` here, borrowed intact from the 200c968 park.
 *   `t` vs a literal `0` at the sixth argument  -- INERT in bb24. The ROM emits
 *       `mov r3, r8 / str r3, [sp, #4]` from a callee-saved register, which
 *       LOOKS like proof the source named a variable. It is not: gcc has proven
 *       `t == 0` inside the guarded arm and CSEs a written `0` back onto the
 *       register that already holds it. §"A value that is provably constant
 *       inside its branch is NOT evidence" and §"A value in a callee-saved
 *       register is NOT evidence the source named it", both confirmed here.
 *   explicit `struct P *tp = &p;` with every field written through it -- INERT
 *       in bb24. This is the shape 200c968's park tried and measured EQUAL on
 *       its own residue; equal here too, so it is not a lever in either
 *       direction and does not need shipping.
 *   `* 0x10000` vs `<< 16`, `int` vs `unsigned int` on the `iwram_3001e40`
 *       extern, and declaring `t` before `a` -- all INERT.
 *
 * -------------------------------------------------------- measured worse
 *
 *   ba64 (ref 192 bytes, 87 encodings, 7 relocations)      enc   size
 *     iwram read placed before __MapActor_GetActor          79    =
 *     naming the -0xcccc bias                               71   -4
 *     dropping the redundant `& 1`                          66   -4
 *     `(rnd << 16) + a->fc` instead of `a->fc + (rnd << 16)` 42    =
 *     p.f8/p.fc stores above p.f4                           14    =
 *   bb24 (ref 176 bytes, 81 encodings, 5 relocations)
 *     naming the -0xcccc bias                               79  -12
 *     dropping the redundant `& 1`                          74  -12
 *     p.f8/p.fc stores above p.f4                           14    =
 *
 * NAMING THE BIAS REMOVES A CALLEE-SAVED REGISTER, CONFIRMED A SECOND TIME AND
 * MORE CHEAPLY. b860's note recorded this at -4 bytes; here it is -4 in ba64
 * and **-12 in bb24**, six whole instructions. Written twice inline,
 * `- 0xcccc` is CSEd into `ldr r6, =0xffff3334` plus two `add`s, which is what
 * both ROMs have. Given a name it acquires a pseudo, and gcc-2.96 then prefers
 * to rematerialise a single pooled `ldr` at each use over paying for a register
 * -- so the push set SHRINKS. The bound is the REMATERIALISATION COST of the
 * constant, not whether it has a name: 0xffff3334 is one pooled `ldr` and loses;
 * 0x90 << 12 is a two-instruction mov/lsl build and does not, which is why
 * naming `d` is merely inert. §"Name CHEAP constants; leave POOL constants
 * inline", from the direction where it costs you six instructions.
 *
 * NOTE ON READING THE RESIDUES ABOVE AGAINST THE DROP-PARTITION RULE. §"THE
 * RESIDUE'S RELOCATION LINE SORTS THE TWO PIN JOBS, WITH NO MIDDLE" is stated
 * for DROPPING A REQUIRED PIN, and within that domain it held all round. It
 * does not extend to statement-order defects and should not be read as if it
 * did: the f8/f4 reorder is a pure ordering defect with SIZE silent, yet it
 * lands at 14 encodings with one relocation displaced by 6 bytes -- the
 * ordering band is 2-3 with relocations silent, and the CSE band is 10+. The
 * reason is mechanical and does not contradict the rule: the reordered
 * statements STRADDLE a `bl`, so moving them moves that call's offset even
 * though nothing was gained or lost. Apply the partition to pin drops; for a
 * moved statement, read the first-divergence index instead.
 *
 * --------------------------------------------------- THE MERGE COST NOTHING
 *
 * A recombination is not a concatenation, and the two hazards were both checked
 * rather than assumed:
 *   RETURN TYPE   no conflict. All four are `void`; ba64 takes no arguments and
 *                 bb24 takes three `int`. Nothing forced a retype, so nothing
 *                 had to be re-spelled in b860 or b948.
 *   STRUCT        no conflict. All four use the family's `struct P` unchanged
 *                 (f0/f4/f8/fc + pad to 0x28) at identical offsets. The new
 *                 `struct A` is a SEPARATE type used only by ba64. NO UNION WAS
 *                 NEEDED OR CONSIDERED: nothing here reads one field at two
 *                 signednesses, which is the case where a union would have
 *                 silently grown the struct to a 4-aligned size and moved
 *                 `sub sp`.
 *   FRAME         both new functions are `sub sp, #0x38` = 0x10 of outgoing
 *                 stack arguments (the spawner takes eight) + the 0x28-byte
 *                 `struct P` at sp+0x10, the same arithmetic as the other two.
 *                 Four functions, one frame decomposition.
 * Confirmed by measurement, not by inspection: b860 and b948 re-screen EXACT
 * from this combined text, and the combined object's first fifteen relocations
 * are byte-for-byte the two lists batch 240 recorded.
 *
 * The push sets fall out unaided, as they did for the first two: ba64 pushes
 * r8/r9/r10 (actor pointer, guard value, the biased y), bb24 pushes r8..r11
 * (guard value plus its three spilled parameters). r4 is absent from all four
 * because `-fcall-used-r4` is in GCC296_CFLAGS.
 *
 * ------------------------------------------------------------------- LANDING
 *
 * THIS FILE LANDS WHOLE AND UNDOES BATCH 240'S SPLIT. It is the third split
 * this project has collapsed. The batch-240 landing note in
 * src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c is superseded in full.
 *
 * Files, all paths absolute from the repo root:
 *   ADD     src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.c   (this file)
 *   DELETE  src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c (functions 1-2)
 *   DELETE  asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_c.s (functions 3-4)
 *   also remove the generated asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.s
 *   left behind by earlier builds; it is compiler output and must never be
 *   committed.
 *
 * EXACTLY TWO LINKER LINES NAME THE SPLIT OBJECTS, matched on FULL PATH and
 * cited by CONTENT, adjacent, in overlays/rom_7ac2d8/overlay.ld at lines 92 and
 * 93 (indented with TWO TABS, shown here as <TAB> because this is a C comment):
 *
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.o(.text)
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_c.o(.text)
 *
 * They sit between the lines for ovl_35b8_a_a_c_a_b.o(.text) and
 * ovl_35b8_a_a_c_a_c_b.o(.text), and the PAIR IS REPLACED BY THE SINGLE LINE
 * IT REPLACED IN BATCH 240:
 *
 *     <TAB><TAB>asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.o(.text)
 *
 * There is NO .data, .rodata or .bss line for either object anywhere in the
 * tree -- grepped the whole repo for both stems and overlay.ld:92-93 are the
 * only hits outside asm/ and reports/. Neither piece has a section beyond
 * .text: all four functions reach every constant through `ldr rX, =value`,
 * which the ASSEMBLER pools into .text, so there is nothing to remap. No
 * Makefile rule is needed -- the default-flag match means the generic
 * `asm/%.o: src/%.c` pattern rule already covers the recombined .c, and the two
 * per-file rules that batch 240 did not need are still not needed.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern unsigned int iwram_3001e40;
extern struct A *__MapActor_GetActor(int slot);
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

void OvlFunc_924_200ba64(void)
{
    struct P p;
    struct A *a;
    unsigned int t;

    a = __MapActor_GetActor(0);
    t = iwram_3001e40 & 3;
    if (t == 0) {
        p.f4 = 7;
        if (((__Random() * 2 >> 16) & 1) == 0)
            p.f4 = 5;
        p.f8 = 0xb333;
        p.fc = 0xb333;
        OvlFunc_common0_10c(a->f8,
                            a->fc + ((__Random() * 4 >> 16) << 16),
                            a->f10,
                            (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                            (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                            t, 0x90 << 12, &p);
    }
}

void OvlFunc_924_200bb24(int x, int y, int z)
{
    struct P p;
    unsigned int t;

    t = iwram_3001e40 & 3;
    if (t == 0) {
        p.f4 = 7;
        if (((__Random() * 2 >> 16) & 1) == 0)
            p.f4 = 5;
        p.f8 = 0xb333;
        p.fc = 0xb333;
        OvlFunc_common0_10c(x, y, z,
                            (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                            (__Random() * 8 >> 16) * 0x3333 - 0xcccc,
                            t, 0x90 << 12, &p);
    }
}
