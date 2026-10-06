/* Func_80216b4  --  0x080216b4   [rom_15000]
 *
 * NON-MATCHING, 9 of 24 encodings, 24 against 24, 52 bytes against 52,
 * 2 relocations identical.  (Earlier bodies of this park stood at fourteen and
 * then at twelve differing; both of those numbers are dead -- see batch 330's
 * section at the end for what moved.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80216b4.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.s --func Func_80216b4
 *   (and --whole, which agrees at the same figure)
 *
 * SPLIT SHAPE: none.  asm/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.s holds ONE
 * `.thumb_func_start` (Func_80216b4) and no data; `tools/datacheck.py` exits 0
 * silently, so when this lands it is a WHOLE-FILE conversion to
 * src/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.c with no `split_s.py` step.
 * PINS: 0.  No flag group (generic `asm/%.o: src/%.c`).
 *
 * THE FUNCTION IS ONE BASIC BLOCK -- 24 encodings, no branch of any kind.  So
 * every ordering question here is ONE sched2 run and every register question is
 * LOCAL-ALLOC.  `.18.greg` prints `;; 0 regs to allocate:` for this body, which
 * is the free discriminator: global_alloc never runs, so no `allocno_compare`
 * arithmetic applies to anything below.
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S 14 WAS **FOUR** CAUSES, TWO OF WHICH ARE NOW CLOSED
 *
 * The park called the blocker "load ordering within an addition, plus the
 * register naming it drags along" and treated it as one thing.  Per-index it is
 * four, and they move independently:
 *
 *   1. x -> r2 AND tb -> r5.  *** CLOSED. ***  The park had the accumulator in
 *      r5 and the table in r4; the ROM has x in r2 and tb in r5.
 *   2. `b` REUSES r0.  *** CLOSED. ***  The ROM's index 13 is `ldr r0,[r0,#0]`:
 *      it overwrites the parameter, because `a` is dead at that point.  The park
 *      declared a separate `b` and got `ldr r2,[r0,#0]`.
 *   3. THE TWO `ldrb`s INSIDE THE ADD, indices 7/8 and 15/16.  STILL OPEN, 4 of
 *      the 12.
 *   4. THE SECOND `*w` READ AGAINST THE FIRST STORE, indices 10-13.  STILL OPEN,
 *      and it is NOT the same cause as 3 -- see the arithmetic below.
 *
 *   (A fifth, w/r4 against m/r1, appeared when 1 and 2 closed and then closed
 *   itself when the mask became a literal.  Its mechanism is recorded below
 *   because it is the cleanest local-alloc tie this bank has produced.)
 *
 * HOW 1 AND 2 CLOSED: *** REASSIGN THE PARAMETER, AND WRITE EACH HALF AS ONE
 * EXPRESSION. ***  `a = *(unsigned char **)a;` is what the ROM's `ldr r0,[r0]`
 * is, and letting gcc pick the accumulator (`a[0x14] = a[8] + tb[v];`) rather
 * than naming an `x` that lives across both halves is what puts it in r2.  The
 * park's attempt 2 -- "both loads through separate named locals, WORSE, 18 of
 * 22" -- was right about the symptom and wrong about the lesson: the cost is not
 * "naming subexpressions", it is giving the two halves a SHARED long-lived
 * local.  Re-measured on that layout, `x = a[8]; a[0x14] = x + tb[v];` was worse by
 * five and a full `x`-accumulator form worse by eight.
 *
 * HOW THE FIFTH CLOSED, AND THE ARITHMETIC IS EXACT.  With `m = 7` as a local,
 * `.17.lreg` gives `w` 3 refs across 24 insns and `m` 3 refs across 24 insns --
 * IDENTICAL.  local-alloc.c's QTY_CMP_PRI (line 1496) is
 *
 *     (int)((double)(floor_log2(n_refs) * n_refs * size) / (death - birth) * 10000)
 *
 * = 1*3*1/24*10000 = 1250 for BOTH, so `qty_compare_1` falls to its stated
 * tie-break, "sort by qty number", and `w` -- the earlier pseudo -- takes r1,
 * the earlier REG_ALLOC_ORDER entry.  Writing the mask as the literal `& 7`
 * makes the mask pseudo born at its first USE instead of at `m = 7`: 3 refs
 * across 20 insns, 1500, which beats w's 1250 outright.  m -> r1, w -> r4,
 * exactly the ROM.  14 -> 13 -> 12.
 *
 *   *** AND NOTE WHAT WAS INERT: ALL 24 PERMUTATIONS OF THE FOUR DECLARATIONS
 *   READ EXACTLY 13. ***  The declaration lever cannot move a local-alloc
 *   quantity-number tie, because the qty numbers follow first use in the insn
 *   stream, not the declaration list.  Assignment order is equally inert here:
 *   `m = 7` first merely moves `movs r4,#7` earlier in the stream and leaves it
 *   in r4 (13 against 14, same register).
 *
 *   NOTE ALSO, for docs/elevation.md: *** BOTH ALLOCATOR FORMULAE CARRY
 *   floor_log2. ***  The batch-321 correction that `local-alloc.c`'s
 *   `qty_compare` "does not" is wrong at the source: local-alloc.c:1496 has it,
 *   and global.c's `allocno_compare` has the same expression.  The REAL
 *   difference between the two decisions is the DENOMINATOR -- a quantity's
 *   `death - birth` (an insn span in one block, after quantities have been
 *   COMBINED across pseudos) against an allocno's `live_length` (REG_LIVE_LENGTH
 *   summed over blocks).  An empirical "verified twice" result attributing that
 *   to floor_log2 would behave exactly as reported while naming the wrong term.
 *
 * ---------------------------------------------------------------------------
 * THE TWO OPEN CAUSES, WITH THE DECIDING RUNG FOR EACH
 *
 * CAUSE 3 -- `ldrb r2,[r0,#8]` AGAINST `ldrb r3,[r5,r3]`.  From
 * `-fsched-verbose=5` on the 13-figure layout, insn 28 (the struct byte) and
 * insn 30 (the table byte) are TIED at prio 45.  CLASS does not separate them:
 * 30's link from the preceding `ands` has insn_cost 1, which takes the
 * `rank_for_schedule` escape to class 3 alongside independent 28.  THE DECIDER
 * IS THE DEPENDENT-COUNT RUNG, 6 AGAINST 4:
 *
 *     insn 30  ...  core : 67 66 55 41 35 32      <- 6 dependents
 *     insn 28  ...  core : 67 66 38 32            <- 4 dependents
 *
 * and more dependents wins.  The two extra are MEMORY dependences on the two
 * `strb`s.  Insn 28 does not have them because `[r0,#8]` and `[r0,#20]` share a
 * base with constant offsets and gcc PROVES them disjoint; insn 30's `[r5,r3]`
 * is variably indexed and cannot be.  So this is a DISAMBIGUATION result, not a
 * source-order one -- which is why reversing the operands to `tb[v] + a[8]` is
 * 16, worse, in all six preamble orders.
 *
 *   AND THE ALIAS ROUTE IS CLOSED HERE, WITH ITS REASON: both stores are BYTE
 *   stores through a `char`-typed lvalue, and C guarantees any object may be
 *   accessed through a character lvalue, so gcc gives them alias set 0.
 *   `DIFFERENT_ALIAS_SETS_P` requires both sets nonzero, so it can never fire
 *   against them.  Measured, not assumed: `const unsigned char` on the table and
 *   its pointer is EXACTLY INERT at 13; indexing `L37226[v]` directly is 15;
 *   both direct is 16.
 *
 * CAUSE 4 -- the second `ldr r3,[w]` against the first `strb`.  Insn 35 (store)
 * has prio 42 and insn 41 (the reload of `*w`) prio 40, so priority alone
 * decides for the store; the ROM takes the load.  Insn 35's 42 is 2 + 40 and the
 * 40 comes FROM insn 41: the store-to-load memory dependence is what lifts the
 * store above the load it feeds.  Break that dependence and the two tie at 40,
 * whereupon CLASS separates them the ROM's way (41 is independent of the `add`
 * just scheduled, class 3; 35 data-depends on it, class 1).  Same closed route:
 * breaking it needs the byte store disambiguated from an `unsigned int` global
 * read, and alias set 0 forbids it.  Moving the second read ABOVE the store in
 * source cannot be used either -- with no store between them, cse2 commons the
 * two `*w` loads into one and the instruction count drops to 22 (measured: 23 at
 * 22 instructions, COUNT and MEM both flagged).
 *
 * MEASURED AND INERT OR WORSE (figures at the layout each was tried on):
 *   24 declaration permutations                       13, all of them, exactly
 *   6 preamble-assignment orders                      13 / 14, no register change
 *   `tb[v] + a[8]` operand reversal, x6               16 / 17
 *   `const` on the table and its pointer              13 (inert)
 *   direct `iwram_3001800` read                       13 (inert) / 14
 *   direct `L37226[v]` index                          15;  both direct 16
 *   18 crossed subsets over {named x, accumulate
 *     through x, split v, split v 2nd, int v,
 *     v declared first} at depth 3                    12, ALL EXACTLY INERT
 *   struct type for `a` (declaration lever)           24 at 18 instructions --
 *     the typed members FOLD the offsets, COUNT and MEM both flagged.  This is
 *     the contraindicated case the batch-321 note predicted.
 *   `-fno-schedule-insns2` (instrument, not shipped)  12 -> 10; the ROM's block
 *     is NOT chain order (it hoists the first `*w` read and the second one), so
 *     the flag is not the answer and I am not proposing SCHED2_CFLAGS.
 *
 * WHAT IS EXACT: 24 of 24 instructions, both relocations, 52 bytes, the
 * parameter reuse, the accumulator register, the table register, the mask
 * register, the global register, and both halves' arithmetic.  What remains is
 * two sched2 decisions that both reduce to the same unprovable disjointness
 * between a `char`-lvalue store and a differently-typed access.
 *
 * WHAT WOULD CLOSE IT: evidence that the ROM's store is NOT a char-lvalue store
 * -- e.g. a typed field at 0x14 whose own alias set is nonzero while leaving the
 * `ldrb`/`strb` widths and the offsets 8, 0x14 and 0 intact.  The plain struct
 * above is not that, because it folds the offsets; a struct whose 0x14 member is
 * reached through a cast that keeps the member type is the untried shape.
 
 * ===========================================================================
 * BATCH 330 BRIEF D.  *** NINE of 24 now, device-free, counts and relocations
 * still equal.  THE PARK'S OWN OPERAND-REVERSAL NEGATIVE IS REFUTED. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80216b4.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.s --whole
 *   -> `XX Func_80216b4   9 of 24 differ (ours 24), first at index 7`
 *   (--func Func_80216b4 agrees: 24 against 24 encodings, no SIZE line.)
 *
 * WHAT CLOSED, AND IT IS ONE CHARACTER PAIR: *** THE TWO HALVES WANT OPPOSITE
 * OPERAND ORDERS. ***  The first half is `tb[v] + a[8]` and the second half is
 * `a[8] + tb[v]`.  The standing header recorded "operand reversal, x6, worse"
 * -- that measurement reversed BOTH halves (re-measured here: both reversed is
 * eleven differing).  Reversing ONLY THE FIRST closes the first half's whole
 * register rotation: the index pseudo moves off r2 onto r3, a[8] moves off r3
 * onto r2, and the `ldr`/`lsr`/`and` triple that the park counted as three
 * separate wrong encodings falls out together.  The two halves are NOT a
 * symmetric pair and must never be edited as one.
 *
 *   >> THE GENERAL LESSON, for docs/elevation.md: when a body has two textually
 *   >> identical statements, "vary the spelling" has to mean varying them
 *   >> INDEPENDENTLY.  Six prior probes on this park varied them together,
 *   >> which is a search over the diagonal of a square.  <<
 *
 * WHAT THE REMAINING NINE IS, PER INDEX
 *   4  the two `ldrb`s inside each add, still swapped in BOTH halves
 *      (the park's cause 3, unchanged)
 *   1  `add r2,r3` against our `add r3,r2` in the FIRST half only -- the
 *      accumulator is whichever operand is written first, so this one encoding
 *      is in direct tension with the four above: the order that fixes the
 *      register rotation names the table byte first, and the ROM's add
 *      accumulates into a[8]'s register
 *   4  the park's cause 4, the second global read against the first store
 *
 * ===========================================================================
 * THE ALIAS ROUTE IS REAL AND THE PARK'S REASON FOR CLOSING IT IS WRONG
 *
 * The standing header says the alias route "is closed here, with its reason:
 * both stores are BYTE stores through a `char`-typed lvalue ... so gcc gives
 * them alias set 0".  The first half of that is right and the conclusion is
 * wrong, because the lvalue's type is a SOURCE-LEVEL CHOICE.  The rule is
 * `lang_get_alias_set`, c-common.c:3347-3352:
 *
 *     if (TREE_CODE_CLASS (TREE_CODE (t)) == 'r'
 *         && TREE_CODE (TREE_TYPE (t)) == INTEGER_TYPE
 *         && TYPE_PRECISION (TREE_TYPE (t)) == TYPE_PRECISION (char_type_node))
 *       return 0;
 *
 * -- it tests the REFERENCE's type, and it requires INTEGER_TYPE.  A reference
 * whose type is an eight-bit BIT-FIELD declared `unsigned int` is an
 * INTEGER_TYPE of precision 32, so it escapes the test and gets its own
 * non-null set while still emitting `ldrb`/`strb`.  Measured, as an INSTRUMENT:
 * writing the store and the table read through two DISTINCT one-bit-field
 * struct types takes the first-half-reversed body to FIVE differing, closing
 * all four of the park's cause-3 encodings in both halves at once.
 *
 *   *** THAT FIVE IS A FIGURE ABOUT THE BLOCKER, NOT A PROPOSAL. ***  A byte
 *   table indexed through `((struct T *)(tb + v))->c` is not source anyone
 *   wrote; it is a device, and the shipped body above is the device-free one.
 *   What the five proves is that cause 3 is EXACTLY the disambiguation the park
 *   said it was, and that it is reachable if a genuine non-char type for either
 *   end is ever found.
 *
 * AND THE TWO HALVES OF THAT INSTRUMENT ARE MUTUALLY DEPENDENT, which is the
 * part worth keeping.  `true_dependence` (alias.c) is, in order:
 *     if (MEM_VOLATILE_P (x) && MEM_VOLATILE_P (mem))  return 1;
 *     if (DIFFERENT_ALIAS_SETS_P (x, mem))             return 0;
 *     ... if (mem_mode == QImode || GET_CODE (mem_addr) == AND) return 1;
 * so (a) a QImode store aliases EVERYTHING that reaches that late, which is the
 * real reason a `strb` is so hard to disambiguate, and (b) the moment the store
 * leaves alias set 0 the two reads of the global become provably independent of
 * it and *** cse2 COMMONS THEM, dropping the body to sixteen instructions. ***
 * Keeping two reads therefore needs a `volatile unsigned int *` for the global,
 * which is why the instrument carries one.  On the device-free body `volatile`
 * is EXACTLY INERT (nine, byte-identical), so it is not shipped.
 *
 * MEASURED THIS BATCH (all 24 against 24 instructions unless noted)
 *   first half reversed, second half not                   ** 9 **  <- shipped
 *   the same + `const` on the table and its pointer            9  (inert)
 *   the same + `volatile unsigned int *w`                      9  (inert)
 *   the same + the store through a one-bit-field struct        9  (inert)
 *   the same + the store through `q = a + 0x14`                9  (inert)
 *   the same + the table byte named in a local                 9  (inert)
 *   BOTH halves reversed                                      11
 *   neither half reversed (the standing body)                 12
 *   `volatile unsigned int *w` alone                          12  (inert)
 *   one-bit-field store + volatile global, neither reversed   12  (inert)
 *   one-bit-field store alone, no volatile                    16 instructions,
 *     the two global reads commoned -- COUNT, not a distance
 *   one-bit-field store and table + volatile, not reversed    10
 *   INSTRUMENT: the above + first half reversed                5
 *   the table byte named, both halves                         17
 *   the second global read moved above the first store        15
 *
 * NEXT, named: the one encoding at index 9.  The add accumulates into its
 * FIRST operand's register, so the first half cannot have both the ROM's
 * register rotation (which needs the table term written first) and the ROM's
 * accumulator (which needs a[8] written first) from one `+` expression.  The
 * untried shape is one that separates those two decisions -- an accumulator
 * whose register is fixed before either load, or a first half whose index
 * pseudo reaches r3 without being evaluated first.  Four probes that named the
 * sum or the table byte in a local did not separate them.
*/
extern unsigned int iwram_3001800;
extern unsigned char L37226[] __asm__(".L37226");

void Func_80216b4(unsigned char *a)
{
    unsigned int *w;
    unsigned char *tb;
    unsigned int v;

    w = &iwram_3001800;
    tb = L37226;
    v = (*w >> 2) & 7;
    a[0x14] = tb[v] + a[8];
    a = *(unsigned char **)a;
    v = (*w >> 2) & 7;
    a[0x14] = a[8] + tb[v];
}
