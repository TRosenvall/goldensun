/* Func_80270ac  @  0x080270ac  [rom_15000]   *** DOES NOT LAND ALONE ***
 *
 * STILL NON-MATCHING.  THIS BODY'S OWN FIGURE: 18 differing encodings of 20 --
 * and that 18 is a MISALIGNMENT, NOT A DISTANCE.  Re-derived batch 328 brief G:
 *     objcmp --func: SIZE ref 44 bytes / ours 36
 *                    INSTRUCTION COUNT ref 18 / ours 13 (pool words 2/2)
 *                    ENCODINGS differ in 18 of 20, first at index 1
 *                    RELOCATIONS differ (a pure offset shift; symbols correct)
 * CORRECTION TO THE OLD HEADER'S ARITHMETIC: it accounted the deficit as "8
 * bytes = exactly the four r9 instructions".  We are FIVE instructions short,
 * not four -- `mov r5,r9 / push {r5}` + `pop {r3} / mov r9,r3` + `mov r3,r9` --
 * which is 10 bytes of code; the SIZE difference reads 8 only because our
 * shorter body picks up a 2-byte alignment pad.  That is the padding trap in a
 * standing header for the sixth recorded time.  THE FIGURE IS NOT A DISTANCE;
 * do not rank this park against figures that are.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80270ac.c asm/rom_15000/rom_23178_a_a_a_a_c_a_c_a.s --func Func_80270ac
 *
 * SPLIT SHAPE: none needed for this function (datacheck.py clean; the .s holds
 * exactly one thumb_func_start).  But the real unit of work is this function
 * INSIDE ITS PARENT, which lives in a different .s entirely -- see below.
 * PINS: 0 for this body.
 *
 * ===================== THE BLOCKER CLASS, NOW CONFIRMED =====================
 *
 * This is a gcc NESTED FUNCTION and its parent is proven:
 * asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s lines 212-214, inside Func_8027114
 * (RunMainMenuScreen, 2004 lines, the largest function in rom_15000):
 *     add r1, sp, #0x64 / mov r9, r1 / bl Func_80270ac
 * -- a static-chain setup immediately before the only call to this function
 * anywhere in the tree.  r9 is the Thumb STATIC_CHAIN_REGNUM.
 *
 * *** BATCH 328 BUILT THE NESTED STAND-IN (the old park's step 1) AND IT WORKS.
 *
 * A parent stub with `auto void Func_80270ac(void) __asm__("Func_80270ac")`
 * emits the five missing instructions, and the save/restore is EXACT:
 *     ours  push {r5,lr} / mov r5,r9 / push {r5}   ... reference: IDENTICAL
 *     ours  pop {r3} / mov r9,r3 / pop {r5} / pop {r0} / bx r0   ... IDENTICAL
 * Everything from `mov r0,r5` to `bx r0` was already exact in the standalone
 * body; the nested form adds precisely the prologue and epilogue that no
 * standalone body can emit.  SO THE CLASS IS NO LONGER A HYPOTHESIS.
 *
 * *** AND STEP 2, THE CHAIN+0 REFERENCE, IS SOLVED.
 *
 * The reference stores the chain pointer's own VALUE into the object's +4 word
 * (`mov r3, r9 / str r3, [sp, #4]`), i.e. a reference at chain+0 with NO
 * arithmetic.  Measured in the stand-in, with a parent holding `int frame[25]`:
 *     p->b = (int)&frame[0];    ->  mov r2,r9 / mov r3,r2 / sub r3,r3,#100
 *     p->b = (int)&frame[25];   ->  mov r2,r9 / mov r3,r2      (NO sub)
 * so the parent's locals sit BELOW the chain and chain+0 is ONE PAST THE END of
 * them -- the parent's frame base, which is what `add r1, sp, #0x64` after
 * `sub sp, #0x64` sets r9 to.  That is consistent with Func_8022a7c reaching
 * its parent's spilled first parameter at chain-4.
 *
 *   >> WHAT IS LEFT, AND IT IS SMALL: the stand-in still spills the chain to its
 *   >> own frame (`sub sp,#12 / add r3,sp,#8 / str r2,[r3] / mov r3,r2` where the
 *   >> reference has `sub sp,#8 / mov r3,r9`).  Four surplus instructions, all
 *   >> from the chain being materialised into a stack slot instead of read once.
 *   >> That is an artefact of the artificial parent, and it is the last thing to
 *   >> settle before step 3.
 *
 * THIS IS SHARED WITH TWO OTHER FUNCTIONS -- src/non_matching/rom_15000/8022a7c.c
 * (Func_8022a7c, chain-4, parent Func_8022b44) and the file-mate Func_80270d8 --
 * so the chain+0/chain-K source shape is worth solving once for all three.
 *
 * NEXT, IN ORDER:
 *   1. DONE (batch 328): the nested stand-in emits the r9 save/restore exactly.
 *   2. DONE (batch 328): chain+0 is one-past-the-end of the parent's locals.
 *   2b. Stop the chain being spilled -- read it once, as the reference does.
 *   3. Only then elevate Func_8027114 (2004 lines) with Func_80270ac, and
 *      probably Func_80270d8, nested inside it.  Per docs/elevation.md's
 *      static-chain class, DO NOT ship a non-nested transcription: it cannot
 *      emit the r9 save at all and can never pass `make compare`.
 *
 * ===================== STILL TRUE FROM THE OLD PARK =====================
 * RESIDUE 1 (the "halfword store becomes a word read-modify-write" class) is
 * REFUTED and stays refuted: the problem was the BASE REGISTER, not the struct
 * layout.  Thumb-1 has no SP-relative `strh`, so `s.a = 0xff` on a stack local
 * makes gcc insert into the SP-relative word; a named `struct S *p` makes the
 * address a register from the start and `strh` is immediately valid.  All four
 * of the old park's struct layouts failed identically, which is what that was
 * telling it.  Also still true: a named `unsigned short *h = &s.a` reaches the
 * ROM's exact 44 bytes but gets there by POOLING the 0xff as a HImode constant
 * and branching over the pool -- a wrong program shape at the right size.  Do
 * not read dsize 0 as progress here.
 *
 * The body below is the best STANDALONE body -- everything except the nested
 * prologue and epilogue -- so it is the right starting point for step 2b, not
 * something to install.
 */
struct S { unsigned short a; unsigned short pad; int b; };

extern void Func_802281c(struct S *s);
extern void _Func_80c10e8(struct S *s, int n);

void Func_80270ac(void)
{
    struct S s;
    struct S *p = &s;
    int u;

    p->b = u;
    p->a = 0xff;
    Func_802281c(p);
    _Func_80c10e8(p, 1);
}
