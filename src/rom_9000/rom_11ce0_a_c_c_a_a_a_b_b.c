/* HeightTile_5 @ 0x08011d94  [asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s]
 *
 * MATCHES. 0 of 34 encodings, 72 bytes against 72, 2 relocations identical.
 * Pin-free, device-free, no symbol-table entry, no per-file flag.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_11ce0_a_c_c_a_a_a_b_b.c \
 *     asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_b.s --func HeightTile_5
 *
 * SPLIT SHAPE (tools/split_s.py asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s
 * HeightTile_5 --dry-run):
 *     _b_a.s  HeightTile_4   34 lines
 *     _b_b.s  HeightTile_5   44 lines   <- becomes this .c
 *     _b_c.s  HeightTile_6   50 lines
 * datacheck.py reports nothing: no data labels, no exports needed.
 *
 * Two-segment ramp along the anti-diagonal over three corner bytes. For
 * x + z <= 14 it interpolates corner 0 -> corner 1, past 15 corner 1 -> corner
 * 2, and at exactly 15 it returns corner 1. The divisor is 15, so it is a real
 * __divsi3 call rather than the +0xF/>>4 rounding its mirror HeightTile_6 uses.
 *
 * THE PARK READ 27 OF 34 AND ITS DIAGNOSIS ("register assignment and
 * scheduling in the three-read prologue") WAS A SYMPTOM, NOT A CAUSE. The 27
 * was FOUR independent causes, none of them allocator pressure:
 *
 * 1. `a += b`, NOT `t = a + b`. This is the root. Writing the sum back into the
 *    parameter is what makes local-alloc tie the sum's quantity to a's, giving
 *    the ROM's two-address `adds r1, r1, r2`. A fresh `t` instead takes r0 --
 *    the register `p` has just died in -- and r0 is the return register, so the
 *    result then has to live in r5 and be copied out in the epilogue. That one
 *    choice accounted for 17 of the 27 differing encodings downstream.
 *
 * 2. THREE `return`s, not a result variable with `goto out`. With a result
 *    variable gcc coalesces it with h1 and emits `adds r0, r5, #0` at the very
 *    END; the ROM emits that copy at index 12, BEFORE the compare chain, which
 *    is what a plain `return h1` in the equality arm produces. The park's
 *    single-exit shape was not wrong about the control flow, only about where
 *    the result wants to live. 24 -> 21.
 *
 * 3. `(unsigned int)a <= 0xe` AS THE THEN ARM. Spelling the test as `> 0xe`
 *    emits the arms in the opposite order with the mirror `bls`, and costs two
 *    extra instructions and four bytes. This is exactly the rule the landed
 *    HeightTile_9 header states for its own `cmp r2,#7 / bhi`: the arm you want
 *    in the fall-through is the one you write as the then-branch. 21 -> 16.
 *    (The CAST is load-bearing separately: signed `a <= 0xe` reads 1 of 34,
 *    `bgt` for `bhi` -- the branch-suffix rule from HeightTile_3's header.)
 *
 * 4. MULTIPLY OPERAND ORDER: `(h1 - h0) * a`, not `a * (h1 - h0)`. Inside
 *    `base + (x) * (y) / 15` gcc evaluates the RIGHT operand of the multiply
 *    into the destination, so `diff * a` gives the ROM's `adds r0, r1, #0 /
 *    muls r0, r3` and `a * diff` gives the swapped pair. Worth exactly 2 per
 *    arm, and it was the last 4. Note the SENSE is the opposite of what the
 *    landed HeightTile_7 header records ("gcc writes the result into whichever
 *    operand it evaluated first") -- in a three-term expression the multiply's
 *    second operand is the one evaluated first, so both headers describe the
 *    same rule from different sides. Try BOTH spellings before calling a
 *    one-instruction register difference on a `mul` allocation.
 *
 * 5. `a -= 0xf;` as a STATEMENT, not `(a - 0xf)` inline. Inline costs +4 bytes
 *    and reads 14; the statement gives the ROM's destructive `subs r1, #15`.
 *    Same parameter-writeback mechanism as (1).
 *
 * The park's "merge the index into the `a` parameter gives 23 but grows the
 * function to 38 lines" observation was RIGHT about the mechanism and wrong to
 * reject it: the growth came from the single-exit result variable, not from the
 * merge, and removing the variable removes the growth.
 *
 * Measured inert / worse, for the record:
 *   t = b + a .......................... 27 (inert; commutative, folded)
 *   t computed before the reads ........ 27, +4 bytes
 *   unsigned t with casts at each use .. 27 (inert)
 *   if/else nest instead of gotos ...... 27 (inert)
 *   one scratch int reused for all 3 ... 30, +4 bytes (worse)
 *   *p++ instead of p += 1 ............. 27 (inert)
 *   p[(unsigned int)0] index casts ..... 27 (inert -- the park's own device)
 *   read p[1] before p[0] .............. 24 (fixes the prologue schedule, but
 *                                          transposes the two reads; made
 *                                          redundant once (1) and (2) land)
 *   (a - 0xf) inline in the high arm ... 14, +4 bytes
 */
int HeightTile_5(signed char *p, int a, int b)
{
    int h0;
    int h1;
    int h2;

    h0 = p[0] << 19;
    p += 1;
    h1 = p[0] << 19;
    h2 = p[1] << 19;
    a += b;
    if (a == 0xf)
        return h1;
    if ((unsigned int)a <= 0xe)
        return h0 + (h1 - h0) * a / 0xf;
    a -= 0xf;
    return h1 + (h2 - h1) * a / 0xf;
}
