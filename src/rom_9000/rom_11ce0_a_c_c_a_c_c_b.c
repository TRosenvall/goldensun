/* HeightTile_B -- 0x08011ed0  (asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s)
 *
 * MATCHING, 0 of 34 encodings, NO PINS  (MEASURED, batch 324 brief H).
 * The installed park (src/non_matching/rom_9000/8011ed0.c) measures 31 of 34.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_11ce0_a_c_c_a_c_c_b.c \
 *     asm/rom_9000/rom_11ce0_a_c_c_a_c_c_b.s --func HeightTile_B
 *   -> OK HeightTile_B -- 68 bytes, 34 encodings and 0 relocations identical
 *
 * (Pre-split, against the two-function rom_11ce0_a_c_c_a_c_c.s, the same body
 * reads OK on --func HeightTile_B.  SIZE 68 against 68; 33 real instructions
 * plus the trailing `.short 0x0000` pad.)
 *
 * SPLIT SHAPE.
 *   datacheck.py asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s  -- CLEAN, no data section
 *   split_s.py --dry-run asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s HeightTile_B ->
 *     asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s   (HeightTile_A, 45 lines)
 *     asm/rom_9000/rom_11ce0_a_c_c_a_c_c_b.s   (HeightTile_B, 45 lines)
 *     stage1.ld rewritten
 *   so the install path is src/rom_9000/rom_11ce0_a_c_c_a_c_c_b.c.
 *   NOTE: 8011e88.c's header predicts `_b` for HeightTile_A and `_c` for
 *   HeightTile_B.  That is WRONG -- the tool names them `_a` and `_b`.
 *   HeightTile_A stays a park and its asm moves to `..._a.s`, so that park's
 *   recipe must be repointed when this lands.
 *
 * PINS: none.  datacheck/split exports: none needed.
 *
 * WHAT THE PARK HAD WRONG.
 *
 * The park read 31 of 34 and diagnosed "one unreachable register COPY ... the
 * whole count is the cascade from a single `mov r4, r2`", then recorded that an
 * explicit `i = t;` local is byte-identical at 34 and called the two-line
 * opening difference provably unfixable.  The observation was right; the verdict
 * was wrong, and the answer was two edits.
 *
 * EDIT 1 -- HeightTile_A's two levers, ported verbatim: reuse the existing
 * local `a` for the third sample instead of a fresh `c`, and write the
 * subtraction as the accumulator `a -= b`.  That alone leaves the figure at 31
 * but makes block 2 structurally the ROM's (`mov r3, t` + a destructive
 * `sub`), i.e. the whole residue becomes a pure renaming.  This is the
 * family's fourth transfer of those levers; 8011ddc.c records the second.
 *
 * EDIT 2 -- and this is the one that matters -- WRITE THE SECOND MULTIPLY
 * `a * (t - 8)`, NOT `(t - 8) * a`.
 *
 * WHY, AND WHY HeightTile_A's PARK SAYS THE OPPOSITE.
 *
 * `*thumb_mulsi3` (arm.md:1116) ties the destination to md operand 1, and a C
 * `X * Y` expands to `(mult Y X)`, so md operand 1 is the SECOND C operand.
 * `global.c:1585 set_preference` opens
 *
 *     if (GET_RTX_FORMAT (GET_CODE (src))[0] == 'e') src = XEXP (src, 0), copy = 0;
 *
 * and `mult`'s format is "ee", so the product's hard-register preference is
 * taken from md operand 1 -- the second C operand.  Spelling the multiply
 * therefore chooses which variable is dragged toward r0, and through
 * `prune_preferences`/`regs_someone_prefers` it chooses which registers the
 * OTHER allocnos are kept out of in find_reg's pass 0.
 *
 * HeightTile_A's park measured this same flip at 32 of 36 and stopped there.
 * That measurement is correct and TU-LOCAL: the flip ROTATES the register
 * assignment, and A's ROM wants the UNROTATED one (its index arrives in r1 and
 * stays there).  HeightTile_B's ROM wants the ROTATED one -- its index arrives
 * in r2 as the third parameter and the ROM spends `mov r4, r2` to evict it so
 * that `a` can have r2.  Same lever, opposite sign, because the two functions'
 * indices arrive in different argument registers.
 *
 * The allocator detail, from the unflipped body's `.18.greg`:
 *
 *     ;; 6 regs to allocate: 32 43 49 35 36 34
 *     ;; 35 conflicts: 32 34 35 36 38 40 42 43 46 1 2 3 13   (35 = a, no prefs)
 *     ;; 36 conflicts: 32 34 35 36 44 46 48 49 0 3 13        (36 = b, no prefs)
 *     ;; 34 preferences: 0 2                                 (34 = t, arrives r2)
 *     dispositions: 35 in 1, 36 in 4, 34 in 2
 *
 * find_reg builds pass 0's candidate set as
 * `used1 | ~regs_used_so_far | regs_someone_prefers` (global.c:1014-1016).
 * `a` is allocated before `t`; r0 and r3 are in its conflicts and r2 is in its
 * `regs_someone_prefers` BECAUSE `t` PREFERS ITS OWN ARGUMENT REGISTER -- so
 * `a` takes r1, `t` keeps r2, and no `mov r4, r2` is ever needed.  Flipping the
 * multiply moves the preference onto `a`; r1 stops being the free one, `a`
 * lands in r2, `b` in r1, `t` is evicted to r4, and the `mov r4, r2` appears.
 *
 * FAMILY RULE, reusable on HeightTile_4 / 5 / 6 and on any numbered family:
 * look at whether the ROM keeps the interpolation index in its INCOMING
 * ARGUMENT REGISTER.
 *   index stays in its argument register -> spell it `(t - k) * a`
 *   index copied out with a `mov`        -> spell it `a * (t - k)`
 * The opening `mov` is the DIAGNOSTIC, not the problem.
 */
int HeightTile_B(unsigned char *p, int unused, int t)
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
    return b + (a * (t - 8)) / 8;
}
