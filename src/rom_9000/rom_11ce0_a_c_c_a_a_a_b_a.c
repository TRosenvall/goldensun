/* HeightTile_4 -- 0x08011d60  (asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_a.s)
 *
 * MATCHING, 0 differing encodings of 26, NO PINS, NO DEVICES  (MEASURED, batch 325 brief C).
 * Replaces TWO rival parks: src/non_matching/rom_9000/8011d60.c (19 of 26) and
 * src/non_matching/rom_9000/HeightTile_4.c (20 of 26, 4 instructions short).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_9000/rom_11ce0_a_c_c_a_a_a_b_a.c asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_a.s --func HeightTile_4
 *   -> OK HeightTile_4 -- 52 bytes, 26 encodings and 0 relocations identical
 *
 * SPLIT SHAPE: NONE NEEDED.  Batch 324's HeightTile_5 landing already split
 * rom_11ce0_a_c_c_a_a_a_b.s into _b_a / _b_b / _b_c, so this reference is
 * already a single-function .s and stage1.ld already names it.
 *   datacheck.py asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b_a.s -- CLEAN, no data
 *   section, no exports needed.
 * PINS: 0.  Flag groups: none.  Symbol-table entries: none.
 *
 * Both rival parks' recipes pointed at the PRE-SPLIT path
 * asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s and so were DEAD -- objcmp exits
 * FileNotFoundError on them.  Neither figure had been re-checked since the
 * split.
 *
 * WHAT THE 19 WAS, AND THE PAD TRAP IT WAS HIDING.
 *
 * objcmp reported "ref 26, ours 26" and printed no SIZE line, which reads as
 * length-exact.  It is not.  The reference is 25 real instructions = 50 bytes,
 * padded to 52 with one trailing `.short 0x0000` -> 26 encodings.  The park's
 * body is 26 real instructions = 52 bytes with NO pad -> also 26 encodings.
 * **We were one instruction LONG and both the count check and the size check
 * were blind to it**, so the 19 was mostly misalignment.  Aligned, the park's
 * body is 3.
 *
 * THE WHOLE RESIDUE WAS ONE PAIR OF TRANSPOSED PARAMETERS:
 *
 *     rom    mov r5, r2                    ... sub r3, r5, r1
 *     ours   mov r5, r1 / mov r1, r2       ... sub r3, r1, r5
 *
 * The max block writes the running maximum into r2, so whichever index
 * parameter sits in r2 must be relocated to the callee-saved r5.  The ROM
 * relocates `b` and leaves `a` in its own argument register; gcc relocated
 * `a` and moved `b` into r1, which costs the extra `mov` and swaps the
 * subtraction's operands.  Both prologue reads, the max, the `mov r1,r3` /
 * `add r1,#0xf` pair and the whole compare chain already matched.
 *
 * WHY THAT TRANSPOSITION HAPPENED, from the allocator rather than from a guess.
 * `.18.greg` on the park's body:
 *
 *     ;; 6 regs to allocate: 39 36 35 37 34 33
 *     ;; 33 preferences: 1        (33 = a, arrives in r1)
 *     ;; 34 preferences: 2        (34 = b, arrives in r2)
 *     dispositions: 33 in 5   34 in 1          <- transposed against the ROM
 *
 * and `.17.lreg`'s "used N times across L insns" feeds global.c:607's
 * floor_log2(n_refs) * n_refs / live_length:
 *
 *     39 u  3 refs /  3 = 1.000      37 m  3 refs /  7 = 0.429
 *     36 B  4 refs /  9 = 0.889      34 b  2 refs /  9 = 0.222
 *     35 A  4 refs / 11 = 0.727      33 a  2 refs / 10 = 0.200
 *
 * which reproduces the order `39 36 35 37 34 33` exactly.  `b` beats `a` BY
 * THE DENOMINATOR ALONE: both have two references, and `assign_parms` emits
 * the incoming copies in declaration order, so `a` is born one insn earlier
 * and live_length(a) = live_length(b) + 1 ALWAYS.  b is therefore allocated
 * first, finds r2 held by the maximum, takes r1, and `a` is pushed to r5.
 *
 * So no statement reordering can reach this -- the numerators are equal and
 * a's range is structurally the longer one.  The only reachable lever is
 * `n_refs`, and `floor_log2` makes 2 -> 3 a cliff: 0.200 -> 0.300, which clears
 * b's 0.222.  `a = t + 0xf` supplies exactly that one extra reference.
 *
 * AMENDMENT TO THE FAMILY'S RULE 1.  HeightTile_5's header says "write the
 * shape index back into a parameter".  True, and **WHICH STATEMENT you write it
 * back on is itself the lever, and it is not the subtraction:**
 *
 *     a = b - a;  u = a + 0xf;       22 encodings, 3 insns SHORT  (park's own negative)
 *     b -= a;     u = b + 0xf;       22 encodings, 3 insns SHORT
 *     t = b - a;  a = t; a += 0xf;   22 encodings, 3 insns SHORT
 *     t = b - a;  a = t + 0xf;       0 of 26   *** this body ***
 *     t = b - a;  b = t + 0xf;        9 of 26  (register rotation)
 *
 * Writing back on the SUBTRACTION kills the whole save pair and the `push {r5}`
 * with it, because a and b then both die at the sub and nothing needs a
 * callee-saved register -- that is the three-instruction collapse the park
 * measured twice and generalised from.  Writing back on the BIAS leaves both
 * parameters live across the max block, so r5 is still needed, while still
 * giving `a` the reference that wins it r1.  This also retires the prediction
 * in brief 325 that `_6`'s index spells `a = b - a; a += 0xf;` -- measured
 * here, that arrangement is three instructions short.
 *
 * A WRONG PROGRAM THAT PRODUCES THE RIGHT BYTES -- worth keeping as a trap.
 * Spelling the subtraction `t = a - b` measures the same 19, but its aligned
 * diff is 2 rather than 3 because it emits the ROM's `sub r3, r5, r1`
 * verbatim.  It is semantically WRONG.  Under gcc's map (a->r5, b->r1) that
 * encoding means pos2 - pos3; under the ROM's map (a->r1, b->r5) the same
 * encoding means pos3 - pos2.  **The operand-order error and the register
 * transposition cancel inside the encoding.**  Renaming the parameters does not
 * rescue it: allocation follows argument POSITION, not the name.
 *
 * MEASURED INERT, all at exactly 19 of 26 and all 26 instructions:
 *   u = b - a + 0xf as one expression          19
 *   unsigned int index local, HeightTile_3 way 19
 *   max written if/else rather than m=A + test 19
 *   m = B first, tested A >= B                 19
 *   index locals declared before A/B/m         19
 *   (unsigned int)b - (unsigned int)a          19
 * MEASURED WORSE:
 *   p[1] read before p[0]                      21  (transposes the two reads)
 */
int HeightTile_4(signed char *p, int a, int b)
{
    int A;
    int B;
    int m;
    int t;

    A = p[0] << 19;
    B = p[1] << 19;
    m = A;
    if (B > A)
        m = B;
    t = b - a;
    a = t + 0xf;
    if (a == 0xf)
        return m;
    if ((unsigned int)a <= 0xe)
        return A;
    return B;
}
