/* HeightTile_A -- 0x08011e88  (asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s)
 *
 * NON-MATCHING, 2 of 36 encodings, NO PINS  (MEASURED, batch 323 brief J).
 * This REPLACES the installed park body, which measures 4 of 36.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011e88.c \
 *     asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s --func HeightTile_A
 *
 * (--whole agrees: 2 of 36, first at index 26.  No relocation difference.)
 *
 * SPLIT SHAPE, for whenever this lands:
 *   datacheck.py asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s -- CLEAN, no data section
 *   split_s.py --dry-run ... HeightTile_A ->
 *     rom_11ce0_a_c_c_a_c_c_b.s (HeightTile_A, 45 lines)
 *     rom_11ce0_a_c_c_a_c_c_c.s (HeightTile_B, 45 lines)
 *   so the eventual install path is src/rom_9000/rom_11ce0_a_c_c_a_c_c_b.c.
 *   HeightTile_B is itself a park (src/non_matching/rom_9000/8011ed0.c), so the
 *   split is needed either way.
 *
 * WHERE THIS BODY CAME FROM, AND WHY THE PARK SAT AT 4.
 *
 * The park's own trailing comment (batch 271) already said "Best candidate
 * scratch_elev/b271/regalloc/c7.c, 39 lines against 39, 2 encodings differing,
 * NO PINS" -- and THAT BODY WAS NEVER INSTALLED.  Measured in batch 323 it is
 * exactly 2 of 36.  This is the fourth instance of the "a park's own header
 * names a better body that was never installed" hazard; here the better body
 * was sitting in a scratch directory, intact, for 52 batches.
 *
 * The body below IS that body, verbatim.  Its two levers over the installed
 * park are:
 *
 *   1. REUSE THE EXISTING GLOBAL `a` FOR THE THIRD SAMPLE instead of a fresh
 *      local `c`.  local-alloc's combine_regs returns 0 when
 *      reg_qty[sreg] == -1, so a fresh local lets `*thumb_ashlsi3` tie the
 *      shift result into the dying ldrsb temp and the `minus` then ties into
 *      THAT -- one six-reference quantity that no two-reference quantity in a
 *      five-insn block can out-prioritise.  Reusing the global breaks the tie.
 *   2. WRITE THE SUBTRACTION AS THE ACCUMULATOR `a -= b`, so the destructive
 *      two-operand `sub r2, r4` survives.
 *
 * WHAT THE RESIDUE IS: two encodings, indices 26 and 27, and they are a REAL
 * INSTRUCTION PAIR, not pool words:
 *
 *     rom   mov r0, r3 / mul r0, r2       (0x1c18 then 0x4350)
 *     ours  mov r0, r2 / mul r0, r3       (0x1c10 then 0x4358)
 *
 * `*thumb_mulsi3` (arm.md:1116) ties the destination to operand 1, and a C
 * `X * Y` becomes `(mult Y X)`, so operand 1 is the SECOND C operand.  The ROM
 * wants operand 1 = `t - 8`, which is only reachable by spelling the multiply
 * `a * (t - 8)`.
 *
 * WHY THAT SPELLING COSTS 32 RATHER THAN 2 -- and this is now read out of the
 * COMPILER, not inferred.  global.c's set_preference starts
 *
 *     if (GET_RTX_FORMAT (GET_CODE (src))[0] == 'e') src = XEXP (src, 0), copy = 0;
 *
 * so for `(set prod (mult X Y))` the preference is taken from X, i.e. operand 1.
 * With operand 1 = the global `a`, the preference binds the product to `a` and
 * the globals land ROM-correct (a->r2, b->r4, t->r1).  With operand 1 = the
 * local `t-8`, `a` is left with a hard-register preference it should not have,
 * and `a` -- allocated BEFORE `t` -- takes r1, the argument register `t` needs.
 * `t` is then pushed to r4 and the whole assignment rotates: 32 of 36.
 *
 * THE RANKING, verified against the formula in local-alloc.c:1496 /
 * global.c:607 rather than quoted as a number.  Allocation order in
 * .18.greg is `32 47 54 34 35 33`, so `a`(34) precedes `t`(33).  Priority is
 * floor_log2(n_refs) * n_refs * size / live_length.  `a` has 7 refs
 * (set, minus, plus, set, minus-use, minus-set, mult) and `t` has 4
 * (set, cmp, mult, plus-8).  floor_log2(7)*7 = 14 against floor_log2(4)*4 = 8.
 * Flipping the order needs `a` down to 4 refs or `t` up to 6, and EVERY
 * reference in this function is an emitted instruction -- so neither is
 * reachable without changing the instruction stream.  That is the stop.
 *
 * MEASURED THIS ROUND (all 36 instructions against 36, so these are distances
 * and not misalignment; crossfire.py, depth 2 over 8 edits, 37 subsets):
 *   this body                                              2
 *   + `u = t - 8` named, `u * a`                           2   (exactly inert)
 *   + `a = a - b` instead of `a -= b`                      2   (exactly inert)
 *   + guard `(unsigned int)t < 8` instead of `<= 7`        2   (exactly inert)
 *       -- simplify_comparison folds them, as docs say
 *   + `a * u` (the operand flip the ROM wants)            32
 *   + `a * u` and `u` moved before `a -= b`               31
 *   + `a * u` and `u` moved into block 0                  23
 *   + `u` before `a -= b` alone                            4
 *   + `u` before the third load alone                     14
 *   + `u` hoisted into block 0 alone                      24
 *   + `(u * a) / 8 + b` instead of `b + (u * a) / 8`      29
 * No crossing of the flip with any inert or any rejected edit goes below 23.
 * PINS ARE STRICTLY WORSE (park's own measurement, batch 271):
 * `register int a __asm__("r2")` is 28 at 40 lines, both pins also 28.
 */
int HeightTile_A(signed char *p, int t)
{
    int a;
    int b;

    a = *p << 19;
    p++;
    b = *p << 19;
    p++;
    if ((unsigned int)t <= 7)
        return a + ((b - a) * t) / 8;
    a = *p << 19;
    a -= b;
    return b + ((t - 8) * a) / 8;
}
