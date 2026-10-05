/* HeightTile_6 -- 0x08011ddc  (asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_c.s)
 *
 * MATCHING, 0 differing encodings of 40, NO PINS, NO DEVICES  (MEASURED, batch 325 brief C).
 * Replaces the park src/non_matching/rom_9000/8011ddc.c, which measures 19 of 40.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_9000/rom_11ce0_a_c_c_a_a_a_b_c.c asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_c.s --func HeightTile_6
 *   -> OK HeightTile_6 -- 80 bytes, 40 encodings and 0 relocations identical
 *
 * SPLIT SHAPE: NONE NEEDED.  Batch 324's HeightTile_5 landing already split
 * rom_11ce0_a_c_c_a_a_a_b.s into _b_a / _b_b / _b_c and rewrote stage1.ld.
 *   datacheck.py asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_c.s -- CLEAN, no data
 *   section, no exports needed.
 * PINS: 0.  Flag groups: none.  Symbol-table entries: none.
 * The park's recipe pointed at the PRE-SPLIT path ..._a_a_a_b.s and was DEAD.
 *
 * Three-corner two-segment ramp along the anti-diagonal, the mirror of
 * HeightTile_5.  Divisor 16, so the sign correction is the `+0xf / asr #4`
 * sequence rather than a __divsi3 call.
 *
 * THE PARK READ 19 OF 40 (40 instructions against 40, so a real distance) AND
 * DIAGNOSED "REGISTER ASSIGNMENT of two live locals ... the residue is the
 * register-assignment class".  The observation was exactly right and the
 * verdict was wrong: it is ONE transposition, and it is reachable.
 *
 *     rom    sub r0, r2, r1 / mov r1, r0 / add r1, #0xf    t in r0, u in r1
 *     ours   sub r2, r1     / mov r0, r2 / add r0, #0xf    t in r2, u in r0
 *
 * The park even wrote that pair down, and then noted "ours is one instruction
 * cheaper at that point; the counts match because the difference is paid back
 * elsewhere".  The payback is the point: that one transposition produced ALL
 * FIVE runs of the diff --
 *
 *   1. our `sub r2, r1` is hoisted one slot early by sched2;
 *   2. and 3. every `cmp` against the index names r0 where the ROM names r1;
 *   4. the ROM's low arm needs `mov r0, r1` to fetch the index and we do not,
 *      because our index is already in r0;
 *   5. the ROM's high arm needs NO `mov` at all -- the raw delta is already in
 *      r0 -- while we emit `mov r0, r3 / mul r0, r2`.
 *
 * THE FIX IS HeightTile_4's, UNCHANGED: `a = t + 0xf`.  Writing the biased
 * index back into the parameter gives the parameter the extra reference that
 * wins it its own argument register r1, and leaves the raw delta `t` a fresh
 * register -- r0, which is exactly what the high arm's `mul r0, r3` requires.
 * See src/rom_9000/rom_11ce0_a_c_c_a_a_a_b_a.c for the allocator arithmetic;
 * the lever is the same and transferred with no adjustment.
 *
 * SCOPE NOTE ON THE FAMILY'S MULTIPLY RULE, measured here.
 *
 * The park recorded that "the two sites want opposite spellings, same as the
 * sibling" and kept the first multiply flipped and the second as written.
 * Measured against this body:
 *
 *     high arm `(C - B) * t`   ->  0        high arm `t * (C - B)`  ->  0
 *     low arm  `(B - A) * a`   ->  0        low arm  `a * (B - A)`  ->  2 of 40
 *
 * so **only the LOW arm's spelling is load-bearing, and the HIGH arm's is
 * exactly inert.**  The reason is in the pattern: `*thumb_mulsi3` (arm.md:1116)
 * emits `mov %0, %1 ; mul %0, %0, %2` for alternatives 0 and 1, but
 * alternative 2 constrains md operand 1 to `0` -- tied to the destination --
 * and emits `mul %0, %0, %2` alone.  Once `t` is in r0 it is ALREADY in the
 * destination, alternative 2 matches whichever way the source spells it, and
 * no `mov` is emitted either way.
 *
 * > **The multiply's operand order only decides anything at a site where md
 * > operand 1 is not already in the destination register.**  At such a site
 * > the ROM's `mov` names md operand 1, which is the SECOND C operand.
 *
 * Both arms are therefore written in the same `(height delta) * (index)` order
 * below, which is the form the low arm requires and the high arm is free to
 * take.
 *
 * ALSO NOTE: brief 325 predicted this function's index would spell
 * `a = b - a; a += 0xf;`.  Measured on HeightTile_4, that arrangement is three
 * instructions SHORT -- the writeback belongs on the BIAS, not on the
 * subtraction.  The park's own two recorded inert rows (`u` computed first with
 * `t` derived, and `t` computed before the third load) both reproduce as inert.
 */
int HeightTile_6(signed char *p, int a, int b)
{
    int A;
    int B;
    int C;
    int t;

    A = *p << 19;
    p++;
    B = *p << 19;
    C = p[1] << 19;
    t = b - a;
    a = t + 0xf;
    if (a == 0xf)
        return B;
    if ((unsigned int)a <= 0xe)
        return A + ((B - A) * a) / 16;
    return B + ((C - B) * t) / 16;
}
