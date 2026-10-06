/* p2 -- HeightTile_A -- PARK at 2 differing encodings of 36.  PIN-FREE, DEVICE-FREE.
 *
 * Figure I measured (not inherited): 2 differing encodings of 36 (ref 36, ours 36),
 * first differing index 26.  `--whole` agrees: "2 of 36 differ (ours 36), first at
 * index 26".  72 bytes against 72; 35 real instructions plus one 2-byte pad on
 * BOTH streams.  PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011e88.c asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s --func HeightTile_A
 *
 * SPLIT SHAPE: the split is already DONE (batch 324's HeightTile_B landing did
 * it).  datacheck.py on the reference is CLEAN, no data section, no exports; the
 * install path if it ever lands is src/rom_9000/rom_11ce0_a_c_c_a_c_c_a.c with NO
 * further split_s.py run.
 *
 * THE RESIDUE is indices 26 and 27, the high arm's multiply pair: the ROM reads
 * md operand 1 out of the bias register and we read it out of the sample
 * register.  One run, one cause.
 *
 * THE PARK'S BOUND IS REFUTED ON LENGTH.  It said "the flip is NECESSARY and
 * costs +1 instruction.  That is the whole bound."  A flipped body at the ROM's
 * exact length exists: write the bias as its own statement placed BEFORE the
 * third sample read and the flip compiles to 35 instructions with no pad
 * absorption, reading 24 of 36.  The +1 was an artifact of statement placement.
 *
 * THE RESIDUE IS NOW TWO DIFFERENT MECHANISMS, one per statement order, both read
 * in `.18.greg`:
 *   ROM order (bias after the sample): `;; 34 conflicts: ... 0 3 13` -- the sample
 *     difference is NOT barred from r2 -- but `;; 34 preferences: 1`, and
 *     find_reg's trailing preference loop (global.c:1097-1110) overrides best_reg
 *     with r1.  36 instructions.  This is the park's mechanism and it reproduces.
 *   bias early: `;; 34 conflicts: ... 0 2 3 13`.  The sample difference now ALSO
 *     conflicts with hard r2, so only r1 and r4 upward remain and r1 is lowest --
 *     the preference is not even the decisive term any more.  NEW.
 *
 * WHY THE TWO ARE COUPLED, and this is the bound stated as a mechanism:
 * arm.h:989-991 gives REG_ALLOC_ORDER = { 3, 2, 1, 0, 12, 14, 4, 5, ... }, so
 * local_alloc hands the sign-extend temp r3 first.  With the bias early its local
 * range overlaps that temp, so the bias takes the next in order, r2, and the
 * sample difference inherits the hard-r2 conflict.  With the ROM's ordering the
 * bias gets r3 -- exactly the ROM's copy-then-subtract pair -- and the sample
 * difference is free for r2.  The flip at the ROM's length requires the bias
 * early, and the bias early costs the sample difference the one register it needs.
 *
 * ALTERNATIVE BODIES AT THE FIGURE, new and exactly inert at 2 of 36 / 35 insns:
 * write the product back into the bias local, either operand order.
 *
 * MEASURED THIS ROUND, 20 bodies.  EXACTLY INERT at 24 of 36 / 35 insns against
 * the early-bias flip: unsigned bias; negated bias spelling; bias declared first;
 * copy-then-subtract bias; unsigned shape-index parameter; low arm flipped too;
 * non-compound subtract.  WORSE: bias between sample and subtract 31 at 36;
 * sample via one expression 15 at 36 with an extra push; bias hoisted above the
 * branch 23 at 36 with an extra push; inline flip 32 at 36; named bias late 32 at
 * 36; one-member union bias 32 at 36; sample via a temp 32 at 36.
 * PINS ARE NOT AN ISOLATION HERE -- they perturb from index 1 and all read worse
 * (25, 25, 32 at 34 insns, 26), so the brief's warning holds.
 *
 * LANDING THIS WOULD HAVE CLOSED A 16-MEMBER FAMILY.  It does not close.  The
 * family stays at 15 landed of 16.
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
