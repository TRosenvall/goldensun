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
