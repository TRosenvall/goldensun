/* OvlFunc_901_20089f8 -- asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c.s
 * OvlFunc_898_2009238 -- asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c.s
 *
 * TWO-MEMBER FAMILY (cross-overlay duplicates, found by twin_families.py).
 * TWO INDEPENDENT BLOCKERS, both documented-unreachable.  16 of 51 at best.
 *
 * 1. CONSTANT CSE WITH NO BOUNDARY.  `0xcc << 1` is passed to __Func_80921c4
 *    twice, both uses inside the same guarded block with no branch between
 *    them.  gcc hoists it into r5 and pays a push the ROM does not
 *    (`push {r5, r14}` vs `push {r14}`).
 *      default flags                       51 differing, ours 53 lines
 *      -fno-rerun-cse-after-loop           16 differing, ours 52 lines
 *    The flag is needed regardless -- it fixes a SECOND hoist, of the 0x867
 *    flag id, whose two uses DO straddle the guard.  So this function needs
 *    CSE_CFLAGS and still does not match.
 *
 * 2. SPLIT SHIFTED BUILD, at two sites.  Perturbing the second `0xcc << 1` to
 *    `0xcd << 1` (so gcc cannot common it) gives 51 lines against 51 with 5
 *    differing -- one of which is the perturbed constant itself.  The other
 *    four are:
 *      rom  mov r1,#0x81 / mov r0,#0x15 / lsl r1,#0x1
 *      ours mov r1,#0x81 / lsl r1,#0x1  / mov r0,#0x15
 *    the cheap `mov r0` slotted between the two halves of the build.
 *
 * The perturb test is what makes this park worth writing: without it the
 * function reads as one blocker at 16 differing, and the CSE looks like the
 * whole story.  It is not -- even with the repeat removed the function is
 * four instructions from matching, on a shape that has resisted every
 * spelling tried across six other parks.
 *
 * Best C: scratch/I89f8.c; the perturbed probe is scratch/I89f8p.c.
 */
