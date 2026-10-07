/* Func_80f0614 -- 0x080f0614 -- asm/rom_f0000/rom_f0254_c.s
 *
 * NON-MATCHING, 38 of 45 encodings  (RE-MEASURED, batch 333 brief E; unchanged).
 *   COUNT DIFFERS (instructions ref 40, ours 39) -- both STREAMS read 45 only
 *   because the reference's spare halfword is pool and ours is pad, so this
 *   positional figure counts PHASE.  Aligned-equal 44.4 per cent, nine hunks.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f0000/80f0614.c \
 *     asm/rom_f0000/rom_f0254_c_c_a.s --func Func_80f0614
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * SCREENED ONCE, 40 of 44, one line short. Filed at the depth actually
 * reached; the structure is understood and the divergence is identified, but
 * only one spelling was tried.
 *
 * Returns early if a status halfword is non-zero; divides two signed halfwords
 * by 8 and returns early if they agree; otherwise copies one to the other,
 * indexes a table, and stores the result of Func_80f07f0.
 *
 * ONE THING READ CORRECTLY: ewram_2004c00 is loaded BOTH signed and unsigned
 * from the same address -- `ldrsh r2, [r3, r6]` for the division and
 * `ldrh r4, [r3]` for the copy -- which is the documented `short` rule: the
 * sign matters to the divide and cannot reach a halfword store, so one `short`
 * declaration gives both. That part reproduces.
 *
 * THE DIVERGENCE is inside gcc's signed-divide-by-8 expansion:
 *
 *     rom    mov r3, r2 / cmp r2, #0 / bge / add r3, r2, #7 / asr r1, r3, #3
 *     ours   cmp r2, #0 / bge / add r2, #7 / asr r0, r2, #3
 *
 * The ROM copies the loaded value into a second register before biasing it and
 * shifts the COPY; ours biases the loaded register in place. Both are correct
 * expansions of `v / 8` and the ROM's costs one more instruction, which is the
 * one-line shortfall -- and the extra live value is why it pushes r6 where we
 * push only r5.
 *
 * That is a copy of a value that is DEAD afterwards, so by the boundary in
 * docs/elevation.md it is not reachable by naming a second local -- gcc would
 * coalesce it. It is also not the source's choice: the copy is emitted by the
 * division expansion itself, below the level any C spelling addresses.
 *
 * THE PARK'S PROPOSED NEXT STEP IS NOW MEASURED, AND IT IS WORSE.  Writing the
 * division as an explicit `(v < 0 ? v + 7 : v) >> 3` makes the body ONE
 * INSTRUCTION LONGER than the reference instead of one shorter -- forty-one
 * against forty -- and drops the aligned share to 35.6 per cent.  The conditional
 * expression is a two-armed select, not the divide expander's single-sided bias,
 * so it pays for the arm the expander gets free.
 *
 * WHERE THE MISSING COPY COMES FROM, AND WHERE IT DIES.  The park called it an
 * artefact of the division expansion below the level any C spelling addresses.
 * The first half is right and the line is nameable: `expand_divmod`'s
 * signed-power-of-two branch form opens with `t1 = copy_to_mode_reg (op0)` at
 * expmed.c:3243, guarded by the branch-cost test at :3239, and compares t1 at
 * :3244.  The dump confirms it: at `.00.rtl` the copy is a real insn and the
 * compare reads the COPY, not the loaded value -- so the loaded value has no
 * other use and the copy is dead-ended from the start.  IT IS GONE BY `.03.cse`,
 * which rewrites the copy into the other direction and leaves it unused, and by
 * `.17.lreg` the insn is not in the stream at all.  So the copy is NOT lost to
 * coalescing in the allocator, as the park's "gcc would coalesce it" says; it is
 * lost to cse, three passes earlier, and no allocator-level lever can reach it.
 *
 * WHAT WOULD KEEP IT: a SECOND USE of the sign-extended value, so that cse
 * cannot make the copy dead.  This function has none to give.  The halfword that
 * reaches the store at +0x4c08 is loaded UNSIGNED from the same address
 * (`ldrh`), which is a different rtx and is not commoned with the signed load --
 * that is the documented `short` rule working correctly, and it is also why
 * routing the store through the signed value loses the second load and stays one
 * instruction short by a different route.
 *
 * THE EXTRA PUSHED REGISTER FOLLOWS THE COPY, IT DOES NOT CAUSE IT.  The
 * reference pushes one more callee-saved register than we do and spends it on
 * the zero that the register-offset `ldrsh` needs.  That value never crosses the
 * call, so it does not need to be callee-saved; it is there because the copy
 * adds a live value at that point.  Fix the copy and the push mask should come
 * with it -- the push mask is a symptom here, not a second blocker.
 *
 * NOTE ON READING THIS FUNCTION'S REGISTERS: r4 is CALL-USED in this tree's
 * build, so the reference writing r4 without pushing it is not an anomaly and
 * says nothing about the source.
 *
 * MEASURED, aligned-equal as a percentage of the reference stream, instructions
 * in brackets against the reference's forty:
 *
 *     the body below                                        44.4  [39]
 *     explicit `(v < 0 ? v + 7 : v) >> 3`                   35.6  [41]
 *     the dividend named in an `int` local                  44.4  [39]
 *     the dividend named in a `short` local, store from it   42.2  [39]
 *     the second division computed first                     35.6  [39]
 *     the unsigned halfword named in its own local           44.4  [39]
 *     the tail wrapped in a positive `if` instead of two
 *       early returns                                        44.4  [39]
 *     both loads named and divided in place                  35.6  [41]
 *
 * NEXT: nothing at the expression level, for the reason named above rather than
 * the one the park gave.  If this is ever to land it needs a second consumer of
 * the signed halfword, which would have to come from evidence outside this
 * function.
 */
extern short ewram_2004c00;
extern short ewram_2004c04;
extern short ewram_2004c08;
extern int Lf1220[] __asm__(".Lf1220");
extern short Func_80f07f0(int a, int b, int c);

void Func_80f0614(void)
{
    int a;
    int b;
    int t;

    if (ewram_2004c04 != 0)
        return;
    a = ewram_2004c00 / 8;
    b = ewram_2004c08 / 8;
    if (a == b)
        return;
    ewram_2004c08 = ewram_2004c00;
    t = (a + 0x10) & 0x1f;
    ewram_2004c04 = Func_80f07f0(Lf1220[a], t * 24, 1);
}
