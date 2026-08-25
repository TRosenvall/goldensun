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
 * WHAT WOULD RETIRE ALL FOURTEEN AT ONCE: any construct that reaches the
 * straight-line case. Batch 42 concluded that is unreachable in plain C given
 * how update_equiv_regs is written, so the honest reading is that these need a
 * compiler-level answer, not a source-level one. They are the single largest
 * argument for reading gcc's source rather than generating more variants.
 *
 * No C body is given here because every member's C is trivial and correct --
 * what fails is where gcc places one `mov`.
 */
