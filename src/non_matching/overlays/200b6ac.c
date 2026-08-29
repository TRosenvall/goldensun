/* OvlFunc_924_200b6ac -- 0x0200b6ac,
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_a.s
 *
 * 87 of 87 lines, 15 differing.  Candidate at scratch/Lb6ac.c.
 *
 * SOLVED, and worth reusing:
 *   - All FOUR interleaved argument sites reproduce with the batch-127 lever
 *     (named locals at the top, the function's `bne` between them and the
 *     calls), including the four-argument one where r0, r1 and r2 are all split
 *     builds: `mov r0,#0xe8 / mov r1,#1 / mov r2,#0xa4 / lsl r2,#18 /
 *     mov r3,#1 / neg r1,r1 / lsl r0,#17`.
 *   - The area id is a SYMBOL.  The ROM pool-loads 0x36 where `cmp r2, #0x36`
 *     would do, which is the tell area.sym's header documents; `_AREA_36` was
 *     already defined.  Spelling the test `v == (int)(&_AREA_36)` took the
 *     screen from 78 differing to 20.
 *   - The two stack arguments of __CopyMapTiles must be NAMED LOCALS assigned
 *     adjacent to the call.  The ROM materialises both before storing either
 *     (`mov r3,#3 / mov r2,#2 / str r3,[sp] / str r2,[sp,#4]`); inline
 *     literals give store-then-rebuild.  20 -> 15.  Same note as the
 *     OvlFunc_959_200a308 template.
 *   - The value stored at iwram+0x1c0 is DERIVED from the offset
 *     (`add r3,r2 / add r2,#0x43 / str r2,[r3]`, 0x1c0 + 0x43 = 0x203) and
 *     reproduces from a mutated offset variable, as the batch-123 rule predicts.
 *
 * BLOCKER: register roles in the two address chains, plus one argument
 * evaluation order.
 *      rom   ldr r3,=gState / mov r1,#0xe0 / lsl r1,#1 / add r3,r1
 *      ours  ldr r2,=gState / mov r3,#0xe0 / lsl r3,#1 / add r2,r3
 * The ROM keeps the base in r3 and the offset in r1/r2; ours swaps them.  Same
 * swap in the iwram chain.  And at OvlFunc_common0_18 the ROM emits the memory
 * load LAST of the four arguments where gcc emits it first.
 *
 * A NEGATIVE WORTH RECORDING: giving each address chain its own pair of locals
 * -- which is the batch-124 rule, and which was exact on OvlFunc_923_200a370 --
 * makes this WORSE (84 lines, 86 differing).  The two chains here are in
 * different basic blocks and never live at once, so splitting them only adds
 * pseudos.  The rule holds for chains that overlap, not for chains that merely
 * both exist.
 *
 * ALSO TRIED: swapping the two scratch declarations; naming the loaded argument
 * as a local before the call.  Both 15.
 */
