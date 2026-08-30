/* OvlFunc_926_2008658 -- 0x02008658,
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_a_c.s
 *
 * 73 of 73 lines, SIX differing -- three call sites, two lines each, the same
 * shape at every one.  Candidate at scratch/N8658_best.c.
 *
 * BLOCKER: argument-construction interleave.
 *      rom   mov r1, #0x80 / mov r2, #0x14 / mov r0, #0x0 / lsl r1, #0x8
 *      ours  mov r1, #0x80 / mov r2, #0x14 / lsl r1, #0x8 / mov r0, #0x0
 * The three sites are __Func_8092adc(0, 0x80 << 8, 0x14), __MapActor_Emote(9,
 * 0x80 << 1, 0x50) and __Func_8092adc(9, 0xd0 << 8, 0x14).  Everything else,
 * including the named pair of stack arguments at the closing __Func_8010704, is
 * exact.
 *
 * WHAT MAKES THIS ONE INFORMATIVE ABOUT THE CLASS: the ROM's prologue is
 * `push {lr}` ALONE.  It holds nothing in a callee-saved register, so the
 * interleave here cannot be rematerialisation of a spilled pseudo -- which is
 * the mechanism behind the dominating-block naming lever that produces this
 * shape on OvlFunc_926_200a484, two functions over in the same overlay.  gcc
 * emits the interleave here from nothing at all, and naming is actively wrong:
 * one named constant costs two lines (75 vs 73), three cost nine (82).
 *
 * So the naming lever is not the general explanation of the interleave.  It is
 * one way to reach a shape gcc will also produce unprompted, and on a function
 * with no register pressure there is no handle on it.
 *
 * SCREENED AND UNCHANGED AT 6: the shifted constants written as plain literals
 * (0x8000, 0x100, 0xd000); `0x80 << 8` respelled `0x40 << 9`; the third
 * argument named instead of the second; and the per-declaration greedy sweep
 * over every callee (tools/protolever.py -- dropping __Func_8010704's prototype
 * makes it 10, everything else leaves it at 6).
 *
 * Same class and same failure as src/non_matching/ovl_77a7c8/200b57c.c and
 * src/non_matching/ovl_7b4558/2009d04.c, both parked this round.
 */
