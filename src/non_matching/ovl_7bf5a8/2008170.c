/* OvlFunc_935_2008170 -- 0x02008170, asm/overlays/rom_7bf5a8/ovl_170_a.s
 *
 * 26 differing of 150, first diff at index 16, AT EXACT SIZE AND EXACT
 * INSTRUCTION COUNT WITH RELOCATIONS SILENT. Best candidate and the full sweep
 * are in scratch_elev/b255/a5/ (final/, NOTES.md, and the -da dumps under
 * dumps/). 123 of the 150 are already exact, including the ENTIRE control-flow
 * skeleton on the first screen.
 *
 * BLOCKER CLASS: register allocation / scheduling tie-break.
 *
 * THREE DISCRIMINATORS FIRED, AND TWO OF THEM ELIMINATE AXES OUTRIGHT:
 *
 *   1. Relocations SILENT at exact size and exact count. By the recorded
 *      relocation partition that is an ORDERING problem, not a CSE one.
 *   2. `-fno-schedule-insns2` REGRESSES, 77 -> 86. By the recorded sign rule
 *      sched2 is already producing the ROM's order.
 *   3. ALL SIXTY-FOUR one-member-union subsets over the six tail fields
 *      measure EXACTLY 26 -- not one encoding moved by any of them.
 *
 * So ALIAS IS ELIMINATED, twice over and exhaustively. Do not spend a round on
 * union spellings here. (The contrast worth knowing: the eighteen-copy
 * block-push park is the same diagnostic with the OPPOSITE sign, and there
 * alias set 0 was exactly the answer.)
 *
 * EVICTION PINS AS A SET, A THIRD INSTANCE: `hx` alone is 72 and `e` alone is
 * 71, and TOGETHER they are 26. A greedy one-at-a-time pass would have parked
 * this at 54 and reported both pins useless. The shipped candidate carries four
 * pins plus two `+=` accumulations, minimal at a fixpoint over two removal
 * passes.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_7bf5a8/ovl_170_a.c.
 * `asmfacts.py` says WHOLE -- convert directly. One function, no data, no
 * split, and NOT the WHOLE+DATA case. `makefile_flags()` is empty and no
 * wildcard reaches this stem. One `.ld` line, which MUST KEEP its asm/ path.
 * Both `.L` symbols are defined in a sibling object and both are already
 * `.global` -- grepped, nothing to export.
 *
 * PROCESS NOTE FROM THE ATTEMPT, worth heeding: a 96-way naming sweep was run
 * BEFORE the `-fno-schedule-insns2` diagnostic, against the recorded search
 * order. It cost a round and returned two encodings. Run the one-compile
 * diagnostic first; it is what eliminated alias here for free.
 */
