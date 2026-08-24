/* RETIRED. All four members are out of this file.
 *
 * OvlFunc_969_2009280 was MATCHED in batch 37 by the basic-block lever -- assign
 * the pooled values to named locals in a different basic block from the call.
 * See reports/arg-interleave.md.
 *
 * OvlFunc_882_2008398, OvlFunc_882_20083cc and OvlFunc_882_2008400 are
 * straight-line, so that lever cannot reach them, and they are FAKEMATCHED in
 * batch 38 with a pinned register. They are on the worklist in
 * reports/fakematch-worklist.md, which carries the full negative result --
 * twenty formulations ruled out, and the finding that `volatile` produces the
 * right ordering in plain C and is unusable only because it also forces a stack
 * slot.
 *
 * This file is kept, empty of members, so that the class name still resolves
 * from the batch reports that cite it. Nothing here is built.
 */
