/* ovl_30_c_c_a_a_c_a.c  --  OvlFunc_891_2008098, the WHOLE of
 * asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_a.s (one `.thumb_func_start`), so NO
 * SPLIT is required and overlays/rom_78c76c/overlay.ld:21 stays VERBATIM as
 * `asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_a.o(.text)`.
 *
 *   OK OvlFunc_891_2008098 -- 184 bytes, 79 encodings and 12 relocations
 *   identical, by --func AND by --whole.  Pin-free; nothing goes in
 *   fakematch.txt.
 *
 * A cutscene step: swaps a tile rectangle in, flashes a second rectangle 22
 * times, lays down three more rectangles, rewrites the attribute block, sets a
 * save bit and tails into the next step.
 *
 * WHAT CLOSED IT, AND WHY THE PARK HAD THE PIECES BUT NOT THE COMBINATION.
 * The park's residue was one instruction: the ROM builds a FRESH 2 in a
 * call-clobbered scratch for the third-from-last call's two stack arguments
 * (`mov r3, #2 / str r3, [sp] / str r3, [sp, #4]`, the stores necessarily
 * BEFORE the register arguments because r3 is one of them), where we reused the
 * callee-saved register the loop already had a 2 in.  The park diagnosed that as
 * a constant the ROM rematerialises at a DOMINATED use -- the second instance of
 * a contradiction it records as unresolved -- and concluded this tree has no
 * symbol space to express it.  IT IS NOT A SYMBOL AND IT IS NOT A DOMINANCE
 * EXCEPTION.
 *
 * THE LOOP'S TWO STACK ARGUMENTS ARE LITERALS THAT LICM HOISTS.  Written as
 * literals inside a REAL loop, loop-invariant motion lifts them into the
 * preheader, and the pseudos it creates are born in `loop` -- which runs AFTER
 * cse and gcse.  `cse2` is the only constant-commoning pass left, it works per
 * extended basic block, and the post-loop block is not on the preheader's path,
 * so the later literal 2 is never commoned with them and gcc materialises the
 * ROM's fresh one.  Naming them as locals, which is what the park did, hands the
 * same job to a pass that runs EARLY and can common across the loop.
 *
 * THE PARK HAD ALREADY MEASURED THIS SPELLING AND BLAMED IT ON ITSELF.  Its
 * rejected list carries "literals in the loop body instead of named values" at
 * thirty-five differing, with the note that the `goto` lever disables the LICM
 * that would have hoisted them.  That observation is correct and it is the
 * reason the measurement is void: the two levers were tested TOGETHER, each
 * failing because of the other.  The `goto` was only ever needed to stop gcc
 * reversing the loop's induction variable -- the ROM counts UP -- and a plain
 * bottom-tested `do { ... } while (i != 0x16)` counts up on its own.  With the
 * loop restored, the literals hoist and both levers pay at once.
 *
 *   GENERAL FORM: WHEN TWO LEVERS ARE KNOWN TO INTERFERE, A MEASUREMENT THAT
 *   APPLIES BOTH REFUTES NEITHER.  This park's own prose named the interference
 *   and the rejected row stood for four batches anyway.
 *
 * WHAT WAS LOAD-BEARING AND IS NOW GONE: four of the park's seven locals.  The
 * loop pair, the post-loop pair and the park's whole "disjoint live ranges split
 * into separate variables" step all disappear; the two stack locals that remain
 * (`s1`, `s2`) are the ones the ROM really does keep in two scratch registers at
 * the first and last call sites, which is the landed idiom recorded in
 * src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.c -- and that file also records
 * the rule that a call site whose fifth and sixth arguments are EQUAL needs
 * nothing, because gcc's single register is what the ROM does there.  Both halves
 * of this function are that one file's findings, applied.
 *
 * The counter is dead outside the test and still counts up, which contradicts
 * the tell recorded in src/non_matching/rom_15000/801e3c8.c ("a count-up loop
 * whose counter is unused in the body means the counter was live in the
 * original").  THAT TELL IS WRONG: the direction is decided by whether loop
 * optimisation ran at all, and a bottom-tested `do/while` whose exit test is an
 * inequality against the limit is left alone.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void OvlFunc_891_20096dc(void);

void OvlFunc_891_2008098(void)
{
    int i;
    int s1;
    int s2;
    int c;
    int d;

    s1 = 2;
    s2 = 1;
    __CopyMapTiles(0, 0x1c, 0x11, 8, s1, s2);
    __PlaySound(0xc8);
    i = 0;
    do {
        __CopyMapTiles(0xa, 0x3d, 0x11, 0x28, 2, 1);
        __CutsceneWait(4);
        __CopyMapTiles(8, 0x3d, 0x11, 0x28, 2, 1);
        __CutsceneWait(4);
        i++;
    } while (i != 0x16);
    c = 4;
    d = 3;
    __CopyMapTiles(0, 0x3b, 0xf, 0x26, c, d);
    __CopyMapTiles(4, 0x3b, 0x11, 0x26, c, d);
    __CopyMapTiles(8, 0x3c, 0x11, 0x27, 2, 2);
    s1 = 0x11;
    s2 = 8;
    __Func_8010704(0, 0, 2, 1, s1, s2);
    __SetFlag(0x207);
    OvlFunc_891_20096dc();
}
