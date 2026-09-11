/* Func_80be18c -- 0x080be18c, asm/rom_b5000/rom_bbb0c_a_c_a_c_c.s
 *
 * NOT ATTEMPTED, DELIBERATELY. This is a GCC NESTED FUNCTION and writing a
 * standalone transcription now would produce a file the next round deletes.
 *
 * THE EVIDENCE. It saves r9 and then READS it (`mov r1, r9`, halfword 0x4649,
 * verified against baserom.gba), and uses it as a base with ONLY NEGATIVE
 * displacements. Six sites in Func_80be378 -- the 1658-line third function of
 * this same .s -- do
 *
 *     add rN, sp, #0x30 / mov r9, rN / bl Func_80be18c
 *
 * which is the recorded static-chain sequence, and that function's frame is
 * `sub sp, #0x30`. So r9 is a STATIC CHAIN and those negative offsets are the
 * ENCLOSING function's locals. A standalone function cannot express that.
 *
 * SECOND SPECIMEN OF THE DOCUMENTED CLASS, after Func_8016018 ->
 * Func_8015fb8, and the FIRST IN THE MAIN ROM -- six call sites and a
 * 236-instruction callee, both larger than the recorded pair.
 *
 * ORDER OF WORK, which is the actionable part:
 *      1. Func_80be378   the enclosing function (1658 lines)
 *      2. Func_80be18c   this, as a nested function inside it
 *      3. Func_80be0b4   the remaining function in the .s
 * Done in that order the whole .s lands with NO hand split. Done in any other
 * order it cannot land at all.
 *
 * The docs already say a standalone transcription of a nested function is
 * provisional and should be deleted once the parent is elevated. That is why
 * this file is a note and not a candidate.
 */
