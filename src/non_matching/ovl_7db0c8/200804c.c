/* OvlFunc_954_200804c -- 0x0200804c,
 * asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_a.s (SINGLE-function file)
 *
 * 108 of 108 lines, 41 differing, and the whole residue is that TWO CASE ARMS
 * ARE LAID OUT IN THE OPPOSITE ORDER.  Candidate at scratch/N804c_best.c.
 *
 * SOLVED: the dispatch itself.  Four cases {0, 6, 0x3c, 0x42} with 6 and 0x42
 * sharing a body, and gcc's sorted balanced tree -- `cmp #6 / beq / cmp #6 /
 * bhi / cmp #0 / beq` and a right subtree for 0x3c and 0x42 -- reproduces
 * exactly.  `bhi` says the switch value is UNSIGNED, so the read of the global
 * is cast.  The two named locals follow the batch-149 rule: `a = 0x32` is named
 * in the two arms where the ROM holds it in r6 across two calls and written as
 * a literal in the arm where it is materialised once and stored at once.
 *
 * BLOCKER: THE TREE SENSE AND THE ARM ORDER ARE COUPLED, and the ROM wants one
 * of each.
 *
 *   source order 0x3c, 6/0x42, 0   -- arms laid out the ROM's way, but the tree
 *                                     ends `beq case; b default` where the ROM
 *                                     has `bne default; b case`.  109 lines, 43.
 *   source order 6/0x42, 0x3c, 0   -- tree EXACT, and the two arms swap.
 *                                     108 lines, 41.   <- best
 *
 * There is no third arrangement.  SCREENED: `case 0x42:` before `case 6:` (no
 * change); an explicit `default: break;` on both orderings (no change); the
 * shared body duplicated into two separate arms so the tree sees two leaves and
 * cross-jumping merges them (109 lines, 69 differing -- it does not merge); and
 * a switch whose arms are nothing but `goto`s to labels placed in the ROM's
 * order, which is the batch-148 block-order lever and fails here because gcc
 * folds the gotos back into the switch (109, 43).
 *
 * So this is a layout decision inside gcc's own case expansion, and the source
 * has exactly one bit of influence over it -- the order the arms are written --
 * which is already spent buying the correct tree.
 */
