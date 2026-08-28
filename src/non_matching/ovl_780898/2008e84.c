/* OvlFunc_883_2008e84 -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_a.s
 * OvlFunc_883_2008f5c -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_c.s
 *
 * BLOCKER: SPLIT SHIFTED BUILD -- 3 of 16, same length, both siblings
 *
 * Two 16-instruction functions, identical apart from five constants.  Thirteen
 * of the sixteen lines match; the residue is one transposition at the single
 * __Func_809218c call:
 *
 *     rom  mov r1,#0x83 / mov r0,#0x0 / lsl r1,#0x1 / ldr r2,=0x325
 *     ours mov r1,#0x83 / lsl r1,#0x1 / ldr r2,=0x325 / mov r0,#0x0
 *
 * The ROM slots the cheap `mov r0, #0` between the two halves of the shifted
 * build; gcc finishes the build first.  Both functions are straight line, so
 * the basic-block lever has nothing to bite on.
 *
 * MEASURED (both functions, all 16 lines, all 3 differing at position 8):
 *   plain literals                                      3
 *   __Func_809218c declared `int` (return-type lever)   3
 *   prototype removed entirely                          3
 *   `int z = 0;` assigned before the call               3
 *
 * The shape IS producible -- 51 of the 2987 generated .s files contain it --
 * so this is "spelling not found", not "cannot be done".  These two are the
 * smallest known instances (16 instructions), which makes them the cheapest
 * place to test any future idea about the class.
 *
 * Best C: scratch/D8e84.c and scratch/D8f5c.c.
 */
