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
 * ================= CLOSED, BATCH 200 -- ALL FOURTEEN =================
 *
 * Everything above the RESOLVED banner is an accurate description of the shape
 * and of why the levers available when it was written could not touch it. Its
 * conclusion -- "these need a compiler-level answer, not a source-level one" --
 * was wrong, and it was the most expensive wrong conclusion in this directory,
 * because it told the candidate ranker to keep refusing fourteen functions.
 *
 * ALL FOURTEEN ARE NOW ELEVATED, EVERY ONE ON ITS FIRST SCREEN:
 *
 *     batch 194   OvlFunc_945_200bdec   (before this note was re-read)
 *     batch 198   OvlFunc_883_2008dc0, 2008e54, 2008e84
 *     batch 199   OvlFunc_883_2008f5c, 2008f8c
 *                 OvlFunc_884_200881c, 20088ac
 *                 OvlFunc_921_20099bc
 *     batch 200   OvlFunc_899_2008428, OvlFunc_942_2008144,
 *                 OvlFunc_944_2008468, OvlFunc_959_2008bac,
 *                 OvlFunc_881_200a81c
 *
 * THE FIX, for the record: pin the argument registers and assign them in the
 * ROM's own order. A pin names the hard register, so the placement is decided
 * at the assignment. The basic-block lever the note reasoned from needs a block
 * that DOMINATES the call, which these branchless functions cannot provide --
 * that reasoning was correct about that lever and is simply not the only route.
 *
 * WHAT THE CLASS TAUGHT, beyond the fourteen functions:
 *
 *   - SHAPE GROUPING BY OPCODE SET WORKS EVEN THOUGH IT IGNORES BEHAVIOUR.
 *     OvlFunc_921_20099bc opens a cutscene and runs a map transition where the
 *     rest play a sound and blit a table; its body has nothing in common with
 *     them, and it was grouped correctly anyway, because the blocker is a
 *     property of the instruction sequence anit is not of what the sequence is for.
 *
 *   - "SCREENED TO CONFIRM RATHER THAN ASSUMED" IS NOT ENOUGH. The note did
 *     screen, and reported real numbers. Every variant it ran changed the
 *     SOURCE, which is exactly what the mechanism it had correctly identified
 *     rules out. Measuring from inside the closed set produces false
 *     confidence; the question to ask is whether the measurements can reach
 *     outside the thing being tested.
 *
 *   - THE INTERLEAVED NEIGHBOUR CAN BE A LOAD. OvlFunc_944_2008468's is
 *     `ldr r2, =0x1410000`. The shape does not care.
 *
 *   - ONE MEMBER NEEDED A SECOND LEVER. OvlFunc_942_2008144 also carried a
 *     precompute_register_parameters bind, and two independent POOL LOADS turn
 *     out to order the same way two independent movs do.
 *
 * This file is kept rather than deleted: the shape description, the discovery
 * method and the member list are the record of how a fourteen-function class
 * was found, closed for the wrong reason, and reopened.
 */
