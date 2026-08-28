/* OvlFunc_944_2008468 -- 0x02008468, asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a.s
 *
 * 23 of 23 lines, TWO differing -- and they are adjacent, which makes this the
 * smallest instance of the argument-setup interleave in the tree.  Candidate:
 * scratch/L8468.c.
 *
 *      rom   mov r1, #0xa4 / ldr r2, =0x1410000 / mov r0, #0x0 / lsl r1, #0x10
 *      ours  mov r1, #0xa4 / ldr r2, =0x1410000 / lsl r1, #0x10 / mov r0, #0x0
 *
 * The ROM slots the independent `mov r0, #0` INTO the split build of r1,
 * between the mov and the lsl; ours completes the build first.  Both hoist the
 * pooled load to the same place, so the only question is which of the two
 * remaining instructions goes first.
 *
 * The documented interleave lever does not apply: it needs a preceding branch,
 * and this function has none (br == 0).  The three zeroes in the function are
 * materialised separately by the ROM, which rules out a shared named local --
 * that would have to live in a callee-saved register and the ROM pushes only lr.
 *
 * TRIED, all 2: SCHED2 (worse, 5), O1 (worse, 5), CSE, GCSE, ALIAS, STRENGTH,
 * FIXEDR7.  Every flag group in the tree is inert on it.
 *
 * Worth revisiting first if a lever for the interleave class is ever found --
 * at two adjacent instructions in a 23-line function it is the cleanest test
 * case available.
 */
