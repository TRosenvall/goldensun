/* OvlFunc_881_2009a98 -- 0x02009a98,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a.s
 *
 * 67 lines against the ROM's 69, and the whole gap is the FIRST CALL.
 * Candidate at scratch/N9a98_best.c.
 *
 * BLOCKER: the `-1` TRIPLE, `__Func_80933f8(-1, -1, -1, 0)`.
 *      rom   mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0 / neg r1 / neg r2 / neg r0
 *      ours  mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2
 * gcc builds one -1 and copies it; the ROM materialises three independently.
 * Two instructions shorter, and every line after it is shifted, which is where
 * the 63 differing comes from -- the body itself is right.
 *
 * This is the class in src/non_matching/overlays/constant_reuse.c, and this
 * function is a CLEAN instance of it: unlike OvlFunc_955_2009424, which also
 * calls __Func_80933f8 elsewhere and so has the constant CSE'd across two call
 * sites and into a callee-saved register, here there is exactly ONE call and
 * nothing else in the function touches -1.  So the reuse is not cross-site CSE;
 * gcc simply will not materialise the same constant three times for three
 * argument registers.  That narrows the class usefully: whatever the original
 * build did differently, it is inside the expansion of a single call.
 *
 * SCREENED, all identical at 63 differing: three separate `int n1, n2, n3 = -1`
 * locals assigned immediately before the call; deleting the callee's prototype
 * so the arguments go through default promotion; and declaring the parameters
 * with three different types (`int, long, short`) so the constants would have
 * different modes at expansion -- they are folded to the same CONST_INT before
 * that and CSE sees one value either way.
 *
 * The eleven-flag sweep in constant_reuse.c was not repeated here.
 *
 * Everything else in the function is confirmed by the shifted-but-identical
 * tail: the mutated `e += 0x64` pointer that is both stored through and then
 * polled with `ldrsh`, the do-while around __WaitFrames, and the two-script
 * branch on OvlFunc_881_200b41c() == 0xb.
 */
