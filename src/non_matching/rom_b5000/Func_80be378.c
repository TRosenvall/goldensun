/* Func_80be378 -- NON-MATCHING, NO CANDIDATE, NO FIGURE.  Honest triage park.
 * Unattempted before batch 313.  Reference asm/rom_b5000/rom_bbb0c_a_c_a_c_c_c.s.
 *
 * NO C WAS WRITTEN, SO THERE IS NO N-of-M AND I AM NOT QUOTING ONE.  What this
 * park carries is the measured install shape, the corrected census, and the one
 * PROVEN SOURCE FACT that the next agent must not re-derive.
 *
 * Verify with (once a candidate exists):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/Func_80be378.c \
 *     asm/rom_b5000/rom_bbb0c_a_c_a_c_c_c.s --func Func_80be378
 *
 * INSTALL SHAPE.  The .s holds TWO functions (Func_80be18c at line 9 and
 * Func_80be378 at line 270) and NO data -- datacheck.py exits 0 with no output,
 * zero `.section` / `.incbin` / `.lcomm`.  `split_s.py --dry-run` would cut it
 * into rom_bbb0c_a_c_a_c_c_c_a.s (Func_80be18c, stays assembly) and
 * rom_bbb0c_a_c_a_c_c_c_b.s (the target), and rewrite stage1.ld.  FUNCTION SPLIT
 * ONLY -- no data half, NO NEW EXPORTS.
 *
 * THE DISPATCH CENSUS, CORRECTED.  The brief's "111 jump tables" is an ENTRY
 * count.  `grep -cE '^\t(mov|ldr|add)\tpc'` gives THREE dispatch sites:
 *
 *   site   entries  minval  default-filled  real cases  nodes  formula
 *   1       100      0            90            10        10   100 >= 99
 *   2         6      0             1             5         5    50 >= 5
 *   3         5     0x24           1             4         4    40 >= 3
 *
 * TWO RESULTS FROM THAT TABLE, AND THEY ARE THE REASON TO READ THIS PARK.
 *
 * (A) SITE 3 PROVES A CASE SHARES THE DEFAULT ARM, AND THE TELL IS THE TABLE'S
 *     EXISTENCE, NOT A `bcc`.  `expand_end_case` takes the decision-tree path
 *     when `count < 5 || range > 10*count`, and case_values_threshold() is 5.
 *     Site 3 has FOUR distinct case bodies over a span of four, so count = 4
 *     and the formula says TREE -- yet the ROM emits a TABLE.  Reading the
 *     region settles it: the fifth entry and the out-of-range target are THE
 *     SAME LABEL (.Lbf084), whose body is `mov r5,#3`:
 *         .word .Lbf074 / .Lbf078 / .Lbf07c / .Lbf080 / .Lbf084
 *       .Lbf074: mov r5,#0x3f   .Lbf078: mov r5,#0x1f
 *       .Lbf07c: mov r5,#0xf    .Lbf080: mov r5,#7
 *       .Lbf084: mov r5,#3      <- also the bhi target
 *     So the source is `case 0x28: default: n = 3; break;` and count is 5, not
 *     4.  WRITE FOUR CASES AND A BARE DEFAULT AND YOU GET A DECISION TREE AND
 *     NO TABLE AT ALL.
 *     TWO CORRECTIONS TO THE BRIEF FALL OUT: the default-sharing case is at the
 *     TOP of the range here (0x28), not the "fourth, LOWEST" case; and there is
 *     NO `bcc` anywhere in this function -- the entry tests are `bls`, `bhi`,
 *     `bhi`.  THE ABSENCE OF A `sub` (sites 1 and 2) is what says minval == 0;
 *     site 3 has `sub r3,#0x24` and a hidden case anyway, so the two signals
 *     are INDEPENDENT and the `sub`-absence tell does not find this one.
 *     The robust tell is arithmetic: compute the node count the table implies
 *     and compare it against 5 and against range/10.
 *
 * (B) SITE 1 IS A TABLE WITH A MARGIN OF ONE.  100 entries, minval 0, and
 *     NINETY of the hundred entries are the default (.Lbee00, confirmed as the
 *     `bl .Lbee00` out-of-range target).  Ten case nodes against a range of 99:
 *     10*10 = 100 >= 99 holds BY ONE.  Write nine case nodes instead of ten and
 *     `range > 10*count` flips true, the table vanishes, and a 100-entry
 *     dispatch becomes a decision tree.  This is the tightest source constraint
 *     measured in the batch and it is a CHECK, not a lever: if a candidate
 *     emits a tree at site 1, it has too few cases, and nothing else about the
 *     candidate needs looking at until that is fixed.
 *     Site 1's entry test is `cmp r3,#0x63 / bls` on `ldrsh r3,[r3,r0]` with no
 *     `sub`, so minval is 0 and case 0 is REAL (entry 0 is .Lbe76c, a body of
 *     its own, not the default).
 *
 * FRAME, by the five greps (the recipe needs FIVE, see the LuckyDiceMain park:
 * `add rX, sp` 2-operand is a sixth form this function does not use).
 *   sub sp,#0x30 ....... 48 bytes, one site
 *   (add|sub) sp, rN ... 0
 *   mov rX, sp ......... 2
 *   add rX, sp, #K ..... 9
 *   str rX,[sp] no load. 3   <- and these PAIR, see below
 * SPILL-SLOT ACCESS-COUNT TABLE (store/load), which is the ranking instrument
 * for a spilling function rather than the aligned figure:
 *   0x08: 2/35   0x0c: 2/16   0x00: 3/2   0x04: 2/3
 *   0x14: 2/2    0x10: 1/1    0x28: 1/0
 * SEVEN distinct [sp,#K] offsets only.  0x08 and 0x0c carry 35 and 16 loads
 * against two stores each -- two long-lived values set once and read
 * constantly, which is where the eleven stack-aggregate materialisations and
 * the 131 high-register mentions are competing.  0x28 is store-only inside a
 * 0x30 frame, so it is written through one of the `add rX,sp,#K` bases, NOT a
 * dead slot -- do not call it a hole.
 *
 * LOOP CENSUS, PER FUNCTION AND BY BACKWARD EDGE (the raw mnemonic census is
 * the wrong instrument; it counts comparisons, not loops).  36 backward edges,
 * of which TWENTY-FOUR are unconditional `b` -- do-while bottoms or goto loops,
 * which care about loop FORM (check_dbra_loop, duplicate_loop_exit_test) and
 * not about signedness.  The twelve conditional closures: bne 4, blt 3,
 * bge 2, ble 1, bcs 1, beq 1.  So this function is MIXED -- five unsigned-ish
 * (bne/beq/bcs) against six signed (blt/bge/ble) -- and a tree-wide `!=`
 * rewrite would corrupt six sites.  Measure per edge, not per function.
 *
 * No `.call_via` sites, so a RUNG-8 per-opcode histogram over the raw reference
 * is trustworthy here without expanding veneers.
 *
 * FAMILY.  Same original TU as Func_80bbb0c (both from rom_bbb0c) and the
 * shapes do rhyme: three table dispatches each, both with a default-filled
 * table, both with a non-obvious minval.  But they are NOT twins -- 80bbb0c has
 * 21 spill offsets against this function's 7, one `.call_via` site against zero,
 * and 8 backward edges against 36.  Read them together for the switch shape,
 * not for the allocation.
 */
