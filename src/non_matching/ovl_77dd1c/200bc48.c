/* OvlFunc_882_200bc48 -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c.s
 *
 * BLOCKER: CONSTANT CSE, STRAIGHT LINE (the documented-unreachable case)
 *
 * 41 of 53 differing, ours 54 lines -- exactly ONE instruction too many, and
 * the 41 is that one instruction shifting everything after it.
 *
 * This is the cleanest example of the class in the corpus and is worth keeping
 * as the reference specimen: 22 calls, ZERO labels, one repeated constant.
 * `__SetFlag(0xb3 << 1)` and `__ClearFlag(0xb3 << 1)` are 20 calls apart; the
 * ROM rebuilds `mov r0,#0xb3 / lsl r0,#1` at both sites, gcc hoists the value
 * into r5 and reloads it with `mov r0, r5`, paying a push for the privilege.
 *
 * The documented rule says the reload needs BOTH a control-flow boundary
 * between the uses AND -fno-rerun-cse-after-loop.  There is no boundary here
 * and there is no way to make one, so the rule predicts this is unreachable.
 * It is:
 *
 *   -fno-rerun-cse-after-loop            41 (54 lines)
 *   -fno-gcse                            41 (54)
 *   -fno-expensive-optimizations         41 (54)
 *   -fno-cse-follow-jumps                41 (54)
 *   -fno-force-mem                       41 (54)
 *   -fno-thread-jumps                    41 (54)
 *   -O1                                  37 (54)
 *
 * Every one of them leaves the instruction COUNT wrong, which is the tell that
 * none of them touched the hoist.
 *
 * See tools/script_candidates.py, which exists because of this function: it
 * ranks straight-line call scripts by repeated EXPENSIVE constant, so this
 * class can be avoided at selection time rather than discovered at screen time.
 */

/* ---- MERGED from src/non_matching/overlays/200bc48.c ----
 * That file was a second park for the same function, written later under the
 * src/non_matching/overlays/ naming while this one already existed.  Its
 * analysis is kept verbatim below; the duplicate file is removed.
 *
 OvlFunc_882_200bc48 -- 0x0200bc48,
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c.s
 *
 * 54 vs 53 lines, 41 differing.  Candidate at scratch/Lbc48.c.
 * A pure straight-line script: 22 calls, no memory operations, no branches.
 *
 * BLOCKER: the flag id `0xb3 << 1` is built twice by the ROM (once for
 * __SetFlag, once for __ClearFlag) and gcc commons the two into r5, adding a
 * push.
 *
 * This is the no-branch case recorded in docs/elevation.md.  Both naming levers
 * need a dominating block and there is none, so the flag group is the only tool
 * -- and CSE_CFLAGS does not fix it, nor -O1 (37 differing) nor --no-sched2
 * (37).  The commoning is the main -O2 CSE.
 *
 * Second instance of that shape after src/non_matching/overlays/20095bc.c, and
 * the two together give it a name: a straight-line script whose flag id is used
 * twice is out of reach.  `tools/pool.py` prints both facts (`br` and `flag2`),
 * so the combination is visible before writing any C.
 */
