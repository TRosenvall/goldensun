/* Func_80df9d0 -- 0x080df9d0.  *** MATCHING ***  (was parked at 2 of 36.)
 *
 * Batch 321, brief D.  The park's figure was correct and its DIAGNOSIS was
 * wrong in the most useful way: it said "NEXT: nothing source-level. This is
 * where gcc chooses to materialise a hoisted loop bound, and the four spellings
 * above do not reach it."  There was nothing wrong with where gcc materialises
 * the bound.  THE PARK'S OWN HAND-HOISTED LOCALS WERE THE ARTEFACT.
 *
 * VERIFIED (tools/objcmp.py, THE AUTHORITY, inside the container):
 *   OK Func_80df9d0 -- 72 bytes, 36 encodings and 0 relocations identical
 * ZERO PINS, zero devices, no per-file Makefile rule.
 *
 * Verify with (the INSTALLED path, after the text/data split below):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_c_b.c \
 *     asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_c_b.s --func Func_80df9d0
 *
 * SPLIT SHAPE: TEXT/DATA, and it needs NO new export.
 *   tools/datacheck.py asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_c.s
 *     data sections : .rodata
 *     functions     : Func_80df9d0
 *     Func_80df9d0  reads no data label -> split needs NO new export
 *   tools/split_s.py ... Func_80df9d0 --dry-run
 *     holds Func_80df9d0 plus 8 blob(s) and 8 label(s), ALL of it after the code
 *     _b = the function (45 lines), _c = the .rodata (27 lines); stage1.ld rewritten
 * So --whole on the UNSPLIT .s reads "SIZE ref 185, ours 72" and that is the
 * 113 bytes of .incrom rodata, not a distance: the per-function line is
 * `ok Func_80df9d0  36 encodings`.
 *
 * WHAT THE RESIDUE ACTUALLY WAS, and it is a mechanism worth the file.
 *
 * The park's body hand-hoisted five locals out of the loop nest -- `srcoff`,
 * `base`, `p`, `idx`, `v` -- and then spent its rounds trying to reorder the
 * two setup instructions that came out swapped:
 *
 *     rom    mov r7,#0 / mov r8,r3 / mov r5,#0      outer=0 ; bound ; srcoff=0
 *     ours   mov r7,#0 / mov r5,#0 / mov r8,r3
 *
 * Both differing encodings are REAL INSTRUCTIONS, not pool words: ref 0x4698 is
 * `mov r8,r3` (format 5, hi-reg move) and ours 0x2500 is `mov r5,#0` (format 3).
 * This function has no pool at all.
 *
 * The entry block IS scheduled -- `mov r3,#144 / lsl r3,r3,#1` sits AHEAD of the
 * three parameter copies in both streams, which only sched2 can do (that chain
 * has priority 2 where every other insn in the block has priority 0).  The three
 * zero-priority movs therefore come out in RTL (LUID) ORDER, and `mov r8,r3` is
 * reload's materialisation of the 288 that loop.c HOISTED TO THE PREHEADER.  So
 * the question the park was asking -- "which spelling moves the bound?" -- was
 * unanswerable, because the bound was already exactly where gcc puts it.  The
 * thing that had to move was `srcoff = 0`, from BEFORE the loop (where any
 * explicit `srcoff = 0;` statement must sit, ahead of NOTE_INSN_LOOP_BEG and so
 * ahead of every invariant hoist) to AFTER the hoist.
 *
 * ONLY ONE THING PUTS AN INITIALISATION THERE: being a DERIVED INDUCTION
 * VARIABLE.  loop.c emits giv initialisations into the preheader AFTER it has
 * emitted the invariant hoists, so a giv init gets the HIGHER LUID and sched2's
 * tie-break then lands it last.  Writing the offset as `y * stride` instead of
 * accumulating it by hand makes it a giv and fixes the order outright -- and the
 * emitted increment is still the ROM's `add r5, r12` at the loop bottom, because
 * that is the giv increment.
 *
 * The park's figure sat at 2 because it had hand-done BOTH of gcc's jobs: the
 * loop-invariant hoist of `off / 2` out of the inner loop, and the strength
 * reduction of `src[off + x]` into a walking pointer.  Deleting all five locals
 * and writing the nest as the two-dimensional copy it is leaves gcc to do both,
 * and it does them the ROM's way.
 *
 * THE MEASURED LADDER (tools/sweep_variants.py, which imports objcmp):
 *
 *   park's body (five hand-hoisted locals)                      2 differing
 *     + `srcoff = 0;` before `outer = 0;`                        3  worse
 *     + `int srcoff = 0;` as a declaration initialiser           3  worse
 *     + declaration order swap, outer<->srcoff                   2  inert
 *     + `for (outer = 0, srcoff = 0; ...)`                       2  inert
 *     + `while (1) { ...; if (...) break; }`                     2  inert
 *     + `p = src + srcoff` instead of the (int) cast             2  inert
 *     + inner loop written as `for`                             10  worse
 *   park's body with `srcoff = outer * stride` (giv)             7
 *       ^ index 9 NOW MATCHES: order fixed, residue is a pure
 *         r0<->r5 swap between `srcoff` and `base`
 *     + eleven allocation levers on top (decl order, operand
 *       order, `stride * outer`, inlining `idx`, `*p++`,
 *       two uses of `outer*stride`, for-form, ...)               7  all inert
 *     + `p = ...` moved BEFORE `base = ...`                      5
 *       (that fixes the ALLOCATION -- base r0, srcoff r5, the
 *        whole prologue exact -- and breaks the outer block's
 *        order instead, because both chains stage through r3 and
 *        the WAW/WAR on r3 makes their order unswappable)
 *   NO hand-hoisted locals at all, `off = y * stride`            0  <-- here
 *
 * The two shapes that reach 0 are this one and the same body with both loops
 * written as `do { } while` (also 0).  `0x120` spelled as a literal is
 * byte-identical to `0x90 << 1`, as the park already recorded.
 *
 * THE GENERAL LESSON, and it is the brief's lever class arriving from the other
 * side.  docs/humanization.md Sec.3 says a pin is evidence about a DECLARATION.
 * Here there was no pin -- but there were SEVEN DECLARATIONS, and five of them
 * were transcriptions of gcc's own output.  A park whose locals are each the
 * destination of one of gcc's loop optimisations has already lost the thing the
 * allocator is reading, and no amount of reordering those locals can give it
 * back.  The discriminator is cheap: if a local's only job is to hold a
 * loop-invariant subexpression or a strength-reduced address, DELETE IT and let
 * loop.c have it.  Two of this function's three differing-encoding neighbours in
 * the frontier have the same signature.
 */
void Func_80df9d0(unsigned char *src, unsigned char *dst, int stride)
{
    int y;
    int x;
    int off;

    for (y = 0; y != (0x90 << 1); y++) {
        off = y * stride;
        for (x = 0; x != 0x28; x++)
            dst[off / 2 + x / 2] = src[off + x];
    }
}
