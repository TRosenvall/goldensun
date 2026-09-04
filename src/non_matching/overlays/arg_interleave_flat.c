/* THE LARGEST REMAINING SHAPE GROUP, AND IT IS ONE BLOCKER -- 14 functions.
 *
 * Not a single function's park: a CLASS note, like pool_load_first.c.
 *
 * HOW IT WAS FOUND. Batch 49 elevated nine functions by enumerating a shape
 * already solved rather than reading the ranked candidate list. Running the
 * same idea over the whole unelevated corpus -- group every function under 30
 * instructions by its set of opcodes, rank the groups by size -- puts this
 * group first, at fourteen members:
 *
 *     flat (no branches), opcodes = {bl, bx, ldr, lsl, mov, pop, push}
 *
 * A LARGE GROUP IS NOT NECESSARILY A RICH VEIN. It is large precisely BECAUSE
 * it is blocked: these are the functions the candidate ranker has been
 * correctly refusing to offer for many rounds, accumulating.
 *
 * THE SHAPE. Every member ends with a call whose arguments interleave:
 *
 *     mov r1, #0xcb / mov r0, #0 / lsl r1, #1 / ldr r2, =0x2d7 / bl __Func_809218c
 *
 * r0 is written BETWEEN `mov r1` and its `lsl r1`. gcc emits r0 before or after
 * the whole block and never inside it.
 *
 * WHY THE LEVER DOES NOT REACH. Batch 43's basic-block lever moves an argument
 * constant by assigning it to a local in a block that dominates the call and
 * contains none of the uses. That needs a BRANCH. Every member of this group is
 * straight-line -- that is what the "flat" in the signature means -- so
 * REG_BASIC_BLOCK (regno) < 0 in update_equiv_regs can never hold. See batch 42
 * for the reading of local-alloc.c.
 *
 * SCREENED TO CONFIRM RATHER THAN ASSUMED. OvlFunc_883_2008e54:
 *
 *     default                       3 of 16, `mov r0, #0` three positions late
 *     callee's prototype withheld    3 of 16 (identical)
 *     the OTHER callee's withheld    6 of 16 (worse)
 *
 * MEMBERS (address order within each overlay):
 *
 *   OvlFunc_881_200a81c   ovl_77a7c8    OvlFunc_899_2008428   ovl_794ac0
 *   OvlFunc_883_2008dc0   ovl_780898    OvlFunc_921_20099bc   ovl_7a7298
 *   OvlFunc_883_2008e54   ovl_780898    OvlFunc_942_2008144   ovl_7c6bac
 *   OvlFunc_883_2008e84   ovl_780898    OvlFunc_944_2008468   ovl_7ca63c
 *   OvlFunc_883_2008f5c   ovl_780898    OvlFunc_945_200bdec   ovl_7cb2c0
 *   OvlFunc_883_2008f8c   ovl_780898    OvlFunc_959_2008bac   ovl_7e7574
 *   OvlFunc_884_200881c   ovl_784360
 *   OvlFunc_884_20088ac   ovl_784360
 *
 * OvlFunc_899_2008428 was already characterised individually in
 * tools/pick_candidates.py's r0-mid table as a "no"; it is the same defect.
 *
 * ===================== RESOLVED, BATCH 198 =====================
 *
 * THE CLASS IS REACHABLE. Everything above is an accurate description of the
 * shape and of why the levers available when it was written could not touch it.
 * Its conclusion -- "these need a compiler-level answer, not a source-level
 * one" -- is wrong, and it is the most expensive wrong conclusion in this
 * directory, because it told the candidate ranker to keep refusing fourteen
 * functions.
 *
 * A REGISTER PIN CLOSES THE SHAPE. Assigning the argument registers through
 * `register int qN __asm__("rN")` in the ROM's own order places the displaced
 * `mov` inside the other register's build:
 *
 *     q1 = 0xcb;  q0 = 0;  q1 <<= 1;  q2 = 0x2d7;
 *     __Func_809218c(q0, q1, q2);
 *
 * The reasoning above is about the BASIC-BLOCK lever, which moves a value by
 * putting it in a block that dominates the call, and which therefore needs a
 * branch. A pin does not go through basic blocks at all -- it names the hard
 * register, so the placement is decided at the assignment rather than by
 * liveness. `REG_BASIC_BLOCK (regno) < 0` never becomes true and never needs
 * to.
 *
 * CONFIRMED, not assumed:
 *
 *     OvlFunc_945_200bdec   elevated in batch 194, before this note was
 *                           re-read -- the class was already broken and nobody
 *                           had connected it back here
 *     OvlFunc_883_2008dc0   matched on the FIRST screen, batch 198
 *     OvlFunc_883_2008e54   matched on the FIRST screen, batch 198
 *
 * EIGHT OF FOURTEEN ARE NOW DONE, every one on its FIRST screen: 200bdec
 * (batch 194), 2008dc0 / 2008e54 / 2008e84 (batch 198), and 2008f5c / 2008f8c /
 * 200881c / 20088ac / 20099bc (batch 199).
 *
 * SIX REMAIN, each a short job -- the bodies are trivial and the only work is
 * reading each call's argument order off the ROM:
 *
 *   OvlFunc_881_200a81c   ovl_77a7c8    OvlFunc_942_2008144   ovl_7c6bac
 *   OvlFunc_899_2008428   ovl_794ac0    OvlFunc_944_2008468   ovl_7ca63c
 *   OvlFunc_959_2008bac   ovl_7e7574
 *
 * (That is five names; the fourteenth, OvlFunc_883_2008e84, is done. The
 * original list is preserved above in the MEMBERS block.)
 *
 * ONE MEMBER IS NOT LIKE THE OTHERS AND IT DID NOT MATTER. OvlFunc_921_20099bc
 * opens a cutscene and runs a map transition rather than playing a sound and
 * blitting a table; its body has nothing in common with the rest. The class was
 * defined by its OPCODE SET, not by what the functions do, and the blocker was
 * identical anyway. Shape grouping found a real class even though it says
 * nothing about behaviour.
 *
 * THE LESSON IS THE ONE BATCHES 193-197 KEPT FINDING, and this is its largest
 * instance: a conclusion drawn from one lever's mechanism was recorded as a
 * fact about C. The note even says "SCREENED TO CONFIRM RATHER THAN ASSUMED"
 * -- and it was, but every screen it ran varied the SOURCE, which is exactly
 * what the mechanism it had identified rules out. Screening more variants of
 * the thing you have proved cannot work is not confirmation.
 *
 * This file is kept rather than deleted because the shape description and the
 * member list are still the fastest way to work the rest.
 */
