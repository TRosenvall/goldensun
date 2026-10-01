/* OvlFunc_959_200b054  --  0x0200b054
 *
 * NOT RECONSTRUCTED, NO objcmp FIGURE EXISTS.  No candidate in this file, so no
 * "N of M" line to carry.  What follows is measurement of the REFERENCE, and it
 * carries the most actionable finding of brief D: THIS FUNCTION'S DOMINANT
 * LEVER IS NOT THE COMMONING BLOCKER, and the brief's difficulty ordering is
 * inverted for it.
 *
 * Reference sliced to ref_OvlFunc_959_200b054.s in this directory, from
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s, where it is the fourth and
 * last function (2,123 instructions, offsets 964-2151 of that file).  Note the
 * same directory already holds src/non_matching/ovl_7e7574/200a7b0.c -- the
 * 810-instruction Anim_Ramses park from batch 307 -- so the overlay is known
 * ground and the directory is derived from the REFERENCE's bank, not the address.
 *
 * Once a candidate exists:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200b054.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s --func OvlFunc_959_200b054
 *
 * ============ IT IS A SOURCE-BASE FUNCTION, NOT A COMMONING ONE ==============
 *
 * Brief D named this the hardest of the five: 2,123 instructions, ZERO
 * high-register mentions, `push {r5,lr}`.  Measured, it is the most tractable,
 * because the thing r5 holds is not a commoned constant -- it is a MESSAGE-ID
 * BASE, and brief D's own lever for that is already costed at +56/+6 -> +4/+3.
 *
 * r5 has exactly THREE definitions in 2,123 instructions:
 *     ldr r5, =0x2481     (line 119 of the slice)
 *     ldr r5, =0x248e     (line 740)
 *     ldr r5, =0x24a6     (line 1249)
 * and it is consumed 75 times as a base plus a literal offset, feeding
 * __MessageID.  BOTH ARMS of brief D's Thumb immediate-width tell are present,
 * which is what makes the reading safe rather than a guess:
 *     21 x  `add r0, r5, #k`        for k <= 7   (3-register form)
 *     54 x  `mov r0, r5 / add r0, #k`  for k > 7 (immediate too wide)
 * That split cannot be produced by constant commoning, because cse's
 * `use_related_value` is symbol-based and never rewrites one CONST_INT as
 * another plus an offset.  It can only come from a base the SOURCE named.
 *
 * SO THE DISCRIMINATION BRIEF D ASKED FOR, DONE ON ALL FIVE.  Brief D said the
 * named-base reading and the commoning reading are converses, both real, and to
 * discriminate by counting pooled constants on both sides.  Counting the
 * base-plus-offset uses directly is sharper and is one pass:
 *
 *   reference            insns  saved-reg  arg     add rX,rB,#k  mov+add   reading
 *                                   defs  copies      (k<=7)      (k>7)
 *   OvlFunc_888_200888c   1524       6       8          0           0     commoning
 *   OvlFunc_924_200bd20   1210      14       5          1           0     commoning
 *   OvlFunc_925_2009af0   1876       6       2          0           0     commoning
 *   OvlFunc_964_200a59c    993      21      10          0           0     commoning
 *   OvlFunc_959_200b054   2123       3      57         21          54     SOURCE BASE
 *
 * 959 is the ONLY one of the five with any base-plus-offset uses at all, and it
 * has 75.  Three saved-register definitions against 57 argument copies is the
 * signature: a value defined three times and used sixty is a NAME, not a
 * commoned constant.  Compare 964, which needs 21 definitions for 10 copies.
 *
 * WHY THE 57 COPIES ARE NOT EVIDENCE OF THE BLOCKER, stated explicitly because
 * the raw number is the largest in the batch and looks alarming: `mov rARG,
 * rSAVED` is a good screen for commoning (repro_commoning.c validates it on
 * 964, 46 -> 33 -> ref 10), and ON THIS FUNCTION IT FIRES FALSELY.  The screen
 * cannot tell a commoned constant from a named base entering argument position.
 * The discriminator is the pair of columns to its right: if a saved register is
 * reached with `add rX, rSAVED, #k` or `mov`+`add`, it is a base.  Run both
 * columns together or the screen will send you after the wrong mechanism on
 * exactly the function where the right one is cheapest.
 *
 * WHAT ELSE IS HERE.  A cutscene: 636 calls across 34 distinct callees, 53 pool
 * loads of 16 distinct values, `push {r5,lr}` and `sub sp,#8` with TWO
 * `str rX,[sp,#K]` and ZERO `ldr rX,[sp,...]` -- so both frame words are
 * outgoing-argument staging for one 5-or-6-argument call (`__Func_80105d4` in
 * the opening sequence) and there is no spill slot.  That is a fifth
 * confirmation of brief D's frame finding.
 *
 * ELEVEN branches, of which SEVEN are pool skips and none is backward: FOUR real
 * branches in 2,123 instructions.  This is the genuinely straight-line extreme
 * of the population, and the control flow is not the work.
 *
 * 116 wide-constant builds of 40 distinct values, 97 of them rebuilds, and ZERO
 * built into a callee-saved register.  Those are the `__MapActor_SetPos` and
 * `__Func_809218c` coordinate pairs (`mov r1,#0xe4 / lsl r1,#17` and so on), and
 * they are where the commoning blocker WILL bite once the bases are named --
 * expect the same ours-short-never-over deficit in the immediate multiset that
 * OvlFunc_964_200a59c showed, every entry a `mov` base or an `lsl` shift.
 *
 * RECOMMENDED ORDER OF WORK, with the reasons:
 *   1. Name the three message-ID bases as locals and write every __MessageID
 *      argument as base + literal.  This is the +56/+6 -> +4/+3 lever and it is
 *      75 call sites, i.e. most of the function's argument setup.
 *   2. Check the constant set with one `grep | sort | uniq -c` against the
 *      candidate's `.word` list -- 16 distinct values and 34 callees settle in
 *      one command (band doc section 5).
 *   3. Only then look at the coordinate pairs, and expect the irreducible
 *      remainder described in repro_commoning.c.
 * Do NOT start from the commoning mechanism on this one.  It is the last thing
 * that matters here, not the first.
 
 *
 * *** RECIPE REFERENCE PATH CORRECTED ON INSTALL. *** This park's `Verify with:` line named
 * scratch_elev/b311d/ref_<NAME>.s -- the agent's own workspace copy of the reference. That
 * directory is GITIGNORED, so the recipe would have become unrunnable the moment the workspace
 * was cleaned, and parkcheck reported TOOLING on all five parks of this brief.  A recipe must
 * name a DURABLE path on both sides: the installed .c and the tracked asm/ reference.  This is a
 * third flavour of the same defect, after a literal `<this file>` placeholder (six parks) and a
 * recipe naming a file that does not exist -- all three make a park unverifiable, which means its
 * figure can never be caught lying.
 */
