/* OvlFunc_965_2009158  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_c_c_a_c.s
 * Best screen: 6 instructions in disagreeing regions, of 17 (rom 17, ours 15).
 *
 * BLOCKER CLASS: constant-CSE on a repeated -1 argument.
 *
 * The ROM builds -1 THREE TIMES for one call, materialising all four constants
 * and then negating three of them:
 *
 *      mov r0, #1 / mov r1, #1 / mov r2, #1 / mov r3, #0
 *      neg r0, r0 / neg r1, r1 / neg r2, r2
 *
 * gcc builds it once and copies, so our stream is two instructions shorter.
 *
 * WHAT WAS TRIED
 *   1. Three literal `-1` arguments (kept below). 6 of 17.
 *   2. Three separate named locals, each assigned 1 and negated separately in
 *      the ROM's order. BYTE-IDENTICAL.
 *
 * This CONFIRMS AT THREE OPERANDS what src/non_matching/ovl_7b7f1c/20088a8.c
 * established at two: separate locals holding the same value are CSEd because
 * there is nothing to tell them apart, and the basic-block lever cannot apply
 * because argument setup is straight-line code with no branch to put the second
 * constant behind. That is the lever's clause (b) and it is a hard requirement.
 *
 * The remaining three calls match exactly.
 */
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092708(int a, int b, int c);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_965_2009158(void)
{
    __Func_80933f8(-1, -1, -1, 0);
    __Func_8092708(0, 6, 0);
    __MapTransitionOut();
    __WaitMapTransition();
}
