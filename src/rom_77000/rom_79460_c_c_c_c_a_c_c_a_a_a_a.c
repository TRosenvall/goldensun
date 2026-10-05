/* Func_8079bf8 -- 0x08079bf8, asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a_a_a.s
 * (1 function, tools/datacheck.py prints nothing -- no data section -- and
 * `grep -c thumb_func_start` is 1, so the landing needs NO split; the whole .s
 * is this function).
 *
 * MATCHED.  0 differing encodings of 26.  56 bytes against 56, 26 encodings and
 * 1 relocation identical, and --whole is green too.  PINS: 0.  DEVICES: 0.
 * FLAGS: none.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a_a_a.c asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a_a_a.s --whole
 *
 * ---------------------------------------------------------------------------
 * THE PARK SAID "TWO SEPARATE THINGS, AND NEITHER IS REACHABLE".  BOTH ARE
 * REACHABLE, AND THEY ARE REACHABLE ONLY TOGETHER.
 *
 * The park's own words, kept here because the refutation is the finding:
 *
 *     "The ROM copies then subtracts in place; gcc uses the three-operand form.
 *      `x = a; x -= b;` -- the usual way to ask for that -- is byte-identical,
 *      because gcc folds the copy and the subtract back together."
 *     "The ROM moves argument 3 into r0 for the upcoming call BEFORE the clamp
 *      test; gcc does it after. Nothing in C orders an argument load against an
 *      unrelated comparison."
 *
 * Both observations are correct.  The verdict is wrong on both counts, and the
 * second one is wrong for the same reason as the first.
 *
 * DECOMPOSITION, from tryc --full on the park's body (27 lines against 27, so
 * the 15 was a distance and not a length defect).  TWO runs:
 *
 *   RUN A, indices 1-7 -- the opening block.
 *       ROM   mov r5,r0 / sub r5,r1 / mov r6,r2 / mov r0,r3 / cmp r5,#0
 *       ours  sub r5,r0,r1 / mov r6,r2 / cmp r5,#0  ... mov r0,r3 after L0
 *   RUN B, indices after the `mul` -- the signed divide by 512.
 *       ROM   cmp r0,#0 / bge / ldr r3,=0x1ff / add r0,r3 / asr r0,#9
 *       ours  mov r3,r0 / cmp r0,#0 / bge / ldr r2,=0x1ff / add r3,r0,r2
 *             / asr r0,r3,#9
 *
 * CAUSE OF RUN A: the clamped value is the PARAMETER, not a fresh local.
 * `a -= b; if (a < 0) a = 0;` instead of `x = a - b; if (x < 0) x = 0;`.
 * The dumps say it plainly -- in `.00.rtl`, the park's body sets a fresh
 * pseudo 36 at insns 18 and 23 while the parameter's pseudo 32 stays
 * SINGLE-SET; writing through the parameter makes insns 18 and 23 both set
 * pseudo 32, so 32 is set three times.  A multiply-set parameter pseudo cannot
 * be tied to its incoming argument register, so `assign_parms`' entry copy
 * SURVIVES as `mov r5,r0` and the subtract becomes destructive on r5.  That is
 * the park's "gcc folds the copy and the subtract back together" explained:
 * the fold is coalescing, and it is coalescing of a SINGLE-SET pseudo.  Ask
 * with a second variable and gcc folds; ask by reusing the parameter and there
 * is nothing to fold.
 *
 * AND THAT ALSO ANSWERS THE PARK'S SECOND "UNREACHABLE".  Nothing in C orders
 * an argument load against an unrelated comparison, and nothing needs to: once
 * the parameter pseudos stop being coalesced away, `mov r0,r3` is not an
 * argument load at all -- it is the entry copy for `d`, emitted by
 * `assign_parms` with the other entry copies at the top of the function, ahead
 * of the compare because that is where the prologue is.  Run A closes
 * entirely: with `a -= b` alone the first thirteen lines are exact, INCLUDING
 * `mov r0,r3`.  TWO OBSERVED DEFECTS, ONE CAUSE.
 *
 * CAUSE OF RUN B: the quotient needs its OWN variable.  `q = r / 512;` instead
 * of `r = r / 512;`.  `expand_divmod`'s power-of-two signed path
 * (expmed.c:3236-3249, the `abs_d != 2 && BRANCH_COST < 3` arm) ALWAYS emits
 * the copy -- `t1 = copy_to_mode_reg (compute_mode, op0)`, and
 * `copy_to_mode_reg` (explow.c:777-793) unconditionally allocates a fresh
 * pseudo at line 781 and reaches `emit_move_insn` at 791, since `x != temp`
 * always holds.  So `mov r3,r0` is generated in BOTH bodies; the question is
 * only whether it survives.  The shift's destination is the `tquotient`
 * target, i.e. the pseudo of the assignment's LHS.  Write `r = r / 512` and
 * that target IS `r`, so `r` is still set after `t1 = r` is read and the two
 * pseudos conflict -- `mov r3,r0` has to stay and the correction is done out
 * of place.  Write `q = r / 512` and `r` dies at the copy, `t1` ties to it, the
 * copy goes, and the `add`/`asr` are destructive on r0 exactly as the ROM has
 * them.  (The pool register follows for free: `ldr r3` rather than `ldr r2`.)
 *
 * ---------------------------------------------------------------------------
 * NEITHER HALF SURVIVES ONE-AT-A-TIME SCREENING.  Measured, each against the
 * park's own body (tools/sweep_variants.py, one container):
 *
 *     base (the park's body)         15 of 26, 27/27 lines, RELOCDIFF
 *     `a -= b` alone                 12 of 26, but 28 lines against 27
 *     `a = a - b` alone              12 of 26, 28 lines  (identical to above)
 *     `q = r / 512` alone            25 of 26, 26 lines against 27  WORSE
 *     BOTH                            0 of 26, exact
 *     `x` reused as the result       27 of 26, 28 lines           WORSE
 *     a separate `q` with `x`        25 of 26                     WORSE
 *
 * `q = r / 512` alone reads 25 of 26 -- it fixes RUN B BIT-FOR-BIT and the
 * figure gets WORSE, because dropping `mov r3,r0` shortens the body to 26
 * lines and every index after the first shifts.  That row is the project's
 * standing trap in its purest form: a park measuring it one at a time records
 * "MEASURED: worse" for the edit that is HALF THE ANSWER.  Cross with
 * tools/crossfire.py.
 *
 * KEPT FROM THE PARK, ALL CONFIRMED BY THE MATCH:
 *   - `r / 512` on a signed int, not a shift -- the `cmp #0 / bge / add #0x1ff
 *     / asr #9` round-toward-zero correction is the division's own expansion.
 *   - the multiply spelled `r * (x + c * 2)` with the call's result FIRST,
 *     because `*thumb_mulsi3` ties the destination to the first operand.  The
 *     batch-267 reading of the two elevated siblings Func_8079c30 and
 *     Func_8079c5c stands.
 *   - the clamp as an `if`, and `Func_8079b24` declared `int`-returning.
 */
extern int Func_8079b24(int a, int b);

int Func_8079bf8(int a, int b, int c, int d)
{
    int r;
    int q;

    a -= b;
    if (a < 0)
        a = 0;
    r = Func_8079b24(d, 1);
    r = r * (a + c * 2);
    q = r / 512;
    if (q < 0)
        q = 0;
    return q;
}
