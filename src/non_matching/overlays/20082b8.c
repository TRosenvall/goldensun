/* OvlFunc_921_20082b8 -- 0x020082b8,
 * asm/overlays/rom_7a7298/ovl_30_c_c_c_a_a_c_a.s
 *
 * 74 of 74 lines, TWO differing.  Candidate at scratch/L82b8.c.
 *
 *      rom   mov r1, #0x0 / mov r0, #0xa / bl __Func_8093054
 *      ours  mov r0, #0xa / mov r1, #0x0 / bl __Func_8093054
 *
 * Argument-setup order at ONE call out of twenty-three.  Every other call in
 * the function -- including several with the identical `(0xa, 0)` shape -- is
 * exact, and gcc's r0-then-r1 there matches the ROM.  Only this one is
 * reversed, and neither argument is a split build, so the batch-127 lever has
 * nothing to work with: there is no two-instruction sequence for the other
 * argument to be emitted inside.
 *
 * TRIED: naming the zero as a local in the dominating block; naming the slot
 * constant instead; --no-rerun-cse.  All 2.
 *
 * Worth recording as the boundary of the interleave lever.  It moves arguments
 * around a SPLIT BUILD; where every argument is one instruction, gcc's order is
 * fixed and the source cannot reach it.  Compare
 * src/non_matching/overlays/20099a4.c, which is the same class with the same
 * result for the opposite reason (a split build exists but no dominating block).
 */
