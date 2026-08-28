/* OvlFunc_882_200bc48 -- 0x0200bc48,
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
