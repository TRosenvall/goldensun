/* Func_80a5fe0 -- 0x080a5fe0, asm/rom_a1000/rom_a5534_c_a_a.s
 *
 * Best screen: 34 of 34 lines, 7 differing, all in the last eight instructions.
 * Candidate at scratch/La5fe0.c.
 *
 * SOLVED, and the useful half: the ROM ends with gcc's branchless
 * "is it nonzero" idiom --
 *
 *      eor r3, r2 / neg r0, r3 / orr r0, r3 / lsr r0, #0x1f
 *      mov r3, #1 / sub r0, r3, r0
 *
 * Writing the obvious `return rec[0] == 2;` gives a BRANCH (31 lines, 11
 * differing).  Writing the comparison as a VALUE in an arithmetic expression --
 *
 *      v = rec[0] ^ 2;
 *      return 1 - (v != 0);
 *
 * -- produces the whole branchless sequence and takes it to 34 of 34.  Worth
 * remembering generally: when the ROM has neg/orr/lsr #31, the source did not
 * compare, it computed.
 *
 * BLOCKER: register roles in that tail.
 *      rom   ldrb r3,[r5] / mov r2,#2 ... neg r0,r3 / mov r3,#1 / sub r0,r3,r0
 *      ours  ldrb r2,[r5] / mov r3,#2 ... neg r2,r3 / mov r0,#1 / sub r0,r2
 * The ROM keeps the value in r3 and the 1 in r3-after-reuse, and subtracts with
 * THREE operands; ours swaps which register holds the value and the constant,
 * and subtracts destructively.
 *
 * A NEGATIVE RESULT WORTH RECORDING.  The obvious lever -- naming the 1 as a
 * local, which is what the constant-as-destination note in this document
 * prescribes -- does not merely fail here, it UNDOES the match.  `one = 1;
 * return one - (v != 0);` goes back to 31 lines and a branch, in all three
 * placements tried (before the eor, after it, and with `v ^= 2` split out).
 * Naming the constant makes gcc treat the expression as control flow again.
 *
 * So the constant-as-destination lever and the branchless-idiom shape are in
 * conflict: the first needs the constant in a named local, the second needs the
 * whole return to stay one arithmetic expression.
 *
 * ALSO TRIED: `v = rec[0]; v ^= 2;` (8 differing, worse); `v = 2 ^ rec[0];` (7,
 * identical).
 */
