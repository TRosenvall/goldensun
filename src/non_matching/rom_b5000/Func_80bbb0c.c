/* Func_80bbb0c -- NON-MATCHING, NO CANDIDATE, NO FIGURE.  Honest triage park.
 * Unattempted before batch 313.  Reference asm/rom_b5000/rom_bbb0c_a_a_a.s.
 *
 * NO C WAS WRITTEN, SO THERE IS NO N-of-M AND I AM NOT QUOTING ONE.
 *
 * Verify with (once a candidate exists):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/Func_80bbb0c.c \
 *     asm/rom_b5000/rom_bbb0c_a_a_a.s --func Func_80bbb0c
 *
 * INSTALL SHAPE, CONFIRMED.  `split_s.py --dry-run` says the .s "holds only
 * Func_80bbb0c and no data; convert it directly, NO SPLIT NEEDED".
 * datacheck.py exits 0 with no output.  So a landing is a WHOLE-FILE conversion
 * to src/rom_b5000/rom_bbb0c_a_a_a.c with NO new exports and no linker-script
 * change -- the cheapest install shape of the four targets in this brief.
 * The .s annotation names it RunBattleMainLoop: 2,874 lines, the second largest
 * routine in the module.
 *
 * THE DISPATCH CENSUS, CORRECTED.  The brief's "102 jump tables" is an ENTRY
 * count.  `grep -cE '^\t(mov|ldr|add)\tpc'` gives THREE dispatch sites, and the
 * interesting thing about all three is the MINVAL FORM:
 *
 *   site  entries  minval   how the index is formed            default-filled
 *   1       13      -1      `add r3, #1`                             3
 *   2       22     0x17c    `ldr r1,=0xfffffe84 / add r3, r0, r1`    6
 *   3       67      3       `sub r3, #3`                            19
 *
 * A THIRD AND FOURTH MINVAL FORM THE RECIPE DOES NOT NAME.  The brief and the
 * doc both describe the index as `sub rN, #minval`, and a screen that looks
 * only for `sub` reads sites 1 and 2 as minval 0 and goes hunting for a lowest
 * case that is not there.  Site 1 uses `add r3, #1`, which means minval = -1;
 * site 2 adds a POOLED NEGATIVE (0xfffffe84 = -0x17c), which means minval =
 * 0x17c and is invisible to any immediate-based grep.  So the minval screen
 * needs four forms: bare (0), `sub #k`, `add #k` (negative minval), and
 * `add rD, rN, <pooled>`.
 *
 * SITE 1'S MINVAL IS -1 AND THAT IS A `case -1:` -- BUT NOT THE BRIEF'S.  The
 * doc's `case -1:` lever is an OUT-OF-RANGE case that flips a decision tree
 * into a table (the BufferString shape, worth 124 bytes there).  This is the
 * opposite: -1 is the table's own MINIMUM, inside the span, so it is an
 * ordinary lowest case and the lever has nothing to do with it.  Site 1's
 * entry at index 1 -- i.e. case 0 -- IS the default label (.Lbc666), so case 0
 * shares the default arm and must be written; and the selector is read from a
 * spill slot (`ldr r3,[sp,#0x18]`), so the -1 is a genuine source value and not
 * a biasing trick.
 *
 * NO CASE B SITE HERE.  All three sites are TABLES whose node counts clear the
 * threshold with margin 68, 139 and 404.  The doc's screen result stands: a
 * decision tree where the formula says table is the only CASE B tell, and this
 * function has no tree at all.  Nothing to find.
 *
 * FRAME, by the five greps.
 *   sub sp,#0x64 ....... 100 bytes, one site
 *   (add|sub) sp, rN ... 0
 *   mov rX, sp ......... 0      <- and this is why BOTH forms must be counted
 *   add rX, sp, #K ..... 1
 *   str rX,[sp] no load. 1
 * ONE stack-aggregate materialisation, and it uses the `add rX, sp, #K` form
 * with `mov rX, sp` absent entirely -- the mis-assignment the doc already
 * records, seen again.
 * SPILL-SLOT ACCESS-COUNT TABLE (store/load), the ranking instrument:
 *   0x4c: 1/22   0x10: 1/16   0x48: 1/15   0x50: 1/9    0x30: 1/7
 *   0x44: 1/7    0x3c: 8/6    0x0c: 2/8    0x14: 3/5    0x2c: 3/5
 *   0x24: 5/4    0x18: 2/5    0x38: 1/5    0x04: 2/4    0x40: 1/4
 *   0x08: 4/1    0x00: 1/0 (outgoing arg)
 * TWENTY-ONE distinct offsets in a 100-byte frame -- the heaviest spill map of
 * the four.  THE SHAPE OF IT IS THE READ: six slots are 1-store/many-load
 * (0x4c, 0x10, 0x48, 0x50, 0x30, 0x44 -- 76 loads between them against six
 * stores).  Those are PARAMETERS AND ONE-TIME DERIVED POINTERS set in the
 * prologue and never reassigned: the prologue indeed opens
 * `str r0,[sp,#0x50] / ... / str r3,[sp,#0x38] / ... / str r0,[sp,#0x10]`.
 * Per the doc's `update_equiv_regs` note the gate is REG_N_SETS, not
 * REG_N_REFS, so a one-set pseudo would normally be DENIED a slot -- these got
 * one anyway, which means they are live across too many calls to keep.  Expect
 * the declaration list to open with those six, in DESCENDING slot order, and
 * the aggregate at 0x3c (8 stores / 6 loads) to sit against them in REVERSED
 * order.
 *
 * LOOP CENSUS, BY BACKWARD EDGE -- and this is a bound worth as much as a
 * lever.  Only EIGHT backward edges in 2,476 instructions, two of them
 * unconditional `b`.  The six conditional closures: bne 3, bge 1, ble 1,
 * beq 1.  So four of six close on equality and two on signed compares.  THIS
 * FUNCTION IS NOT LOOP-SHAPED -- it is 2,476 instructions of straight-line
 * dispatch with eight loops in it, so check_dbra_loop and
 * duplicate_loop_exit_test have almost no surface here and the loop-form levers
 * are nearly worthless.  Spend the effort on the spill map instead.
 *
 * ONE `.call_via` SITE.  A RUNG-8 per-opcode histogram over the RAW reference
 * will under-count `mov` and `bx` by one each, because the macro expands to
 * `mov r12,pc` + `bx`.  Expand it before trusting those two columns.
 *
 * WHY NO CANDIDATE.  2,476 instructions with a 21-slot spill map, 130
 * high-register mentions and 272 labels is the largest reconstruction of the
 * four, and the brief's own instruction was depth on two over shallowness on
 * four.  The depth went to OvlFunc_968_200b068, which landed at 85.4% aligned
 * with an exact call multiset.  What this park buys the next agent is the
 * install shape (the cheapest of the four), the corrected minval forms, and the
 * six prologue slots that fix the head of the declaration list.
 */
