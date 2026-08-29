/* GetFlag (TestSaveBit) -- 0x08079338, asm/rom_77000/rom_79338_a.s
 *
 * 13 of 13 lines, THREE differing.  Candidate at scratch/Lgetflag.c.
 * 180 call sites -- the most-used accessor in the ROM.
 *
 * SOLVED, and this is the important half: gcc-2.96 does NOT generate its own
 * branchless "is non-zero" idiom from a comparison here.  `return v != 0;`,
 * `return !!v;`, and assigning to a local first ALL give a branch (16 lines,
 * 15 differing).  Writing the idiom out longhand --
 *
 *      r = -v;
 *      r |= v;
 *      return (unsigned int)r >> 31;
 *
 * -- produces the ROM's `neg r0,r3 / orr r0,r3 / lsr r0,#0x1f` and takes the
 * screen to 13 of 13.
 *
 * Note this CONTRADICTS what src/non_matching/rom_a1000/80a5fe0.c found, where
 * `1 - (v != 0)` did generate the idiom.  The difference is the arithmetic
 * around it: subtracting the comparison from a constant forces value mode,
 * while returning it alone lets gcc pick a branch.  Both spellings are worth
 * having.
 *
 * BLOCKER: which register the shift lands in.
 *      rom   lsl r3, r0, #0x14 / lsr r0, r3, #0x17   (three-operand, r3 temp)
 *      ours  lsl r0, #0x14 ... lsr r0, #0x17         (destructive on r0)
 * A named `t` intermediate does not separate them -- gcc coalesces t with the
 * result.  --no-sched2 and -O1 each get it to 2 differing but neither is exact,
 * and the remaining difference is the same register choice.
 *
 * TRIED: `int t` and `unsigned int t`; computing the shifts before the mask;
 * casting idx to unsigned before the shift; --no-sched2, -O1, --no-rerun-cse.
 */
