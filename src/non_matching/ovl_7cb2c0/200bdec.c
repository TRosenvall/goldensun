/* OvlFunc_945_200bdec  [overlays/rom_7cb2c0]
 *
 * Source asm: goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a.s
 *
 * BLOCKER CLASS: arg-interleave in a straight-line function. TWO instructions
 * of 26, one adjacent pair:
 *
 *     rom    mov r1, #0xa0 / mov r2, #0x28 / mov r0, #0x8  / lsl r1, #0x7
 *     ours   mov r1, #0xa0 / mov r2, #0x28 / lsl r1, #0x7  / mov r0, #0x8
 *
 * The ROM splits the build of 0x5000 around the first argument; gcc completes
 * it and then sets r0. Everything else is exact -- all seven calls, every
 * argument, prologue and interwork epilogue, 26 lines against 26.
 *
 * THIS IS A SECOND CONFIRMED INSTANCE OF A DOCUMENTED BOUNDARY, and it is
 * filed as confirmation rather than as a new problem. docs/elevation.md's
 * basic-block lever retires this class by assigning the constant in a
 * DOMINATING basic block, which forces gcc to rematerialise it split at the
 * call. That needs a branch to cross, and the doc is explicit that a call does
 * not create one.
 *
 * This function has NO branches: seven calls in sequence, no condition, no
 * loop, no early return. One basic block, nowhere to put the assignment. It
 * behaves exactly as src/non_matching/ovl_7cb2c0/200dca4.c predicted a
 * straight-line cutscene script would.
 *
 * MEASURED, all 2 differing:
 *   `0xa0 << 7` at the call site                              2
 *   `0x5000` written out                                      2
 *   the constant named in a local in the same basic block     2
 *
 * The third is the batch-153 inverse lever, and its failing here is the
 * expected result rather than a surprise: that lever makes gcc build a
 * constant CONTIGUOUSLY, and contiguous is what we already have. The ROM wants
 * it split. There is no third direction to try.
 */
extern void __CutsceneStart(void);
extern void __MessageID(int id);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200bdec(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 1, 1);
    __Func_8092adc(8, 0xa0 << 7, 0x28);
    __Func_809259c(8, 2);
    __MessageID(0x1e3d);
    __Func_8093040(8, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xb, 0);
}
