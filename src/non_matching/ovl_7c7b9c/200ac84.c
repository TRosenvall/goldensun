/* OvlFunc_943_200ac84 -- 0x0200ac84,
 * asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_c.s
 *
 * 57 differing of 530 AT EXACT LENGTH, and 45 of the 57 are one-for-one
 * register swaps. Candidate in scratch_elev/b257/a4/final/.
 *
 * BLOCKER CLASS: find_reg's conflict/preference pass -- the recorded
 * "parameter pointer one register too low" class.
 *
 * WHY IT IS FORCED, and this is the useful part: FOUR low-register values cross
 * loop 1's call, but only r5/r6/r7 are callee-saved low, because r4 is
 * call-clobbered by the tree's own `-fcall-used-r4`. So one of the four MUST
 * lose its register. The ROM's loser is the `id` base; ours is `n`.
 *
 * AND IT IS NOT THE PRIORITY FORMULA. By floor_log2(refs)*refs/live_length,
 * `n` (45 refs) outranks the id base (30) and should keep r7 -- which it does.
 * So the ROM's choice is not what the documented priority predicts, and the
 * discriminator to read is `;; 12 regs to allocate:` in .18.greg rather than a
 * refs count.
 *
 * AXIS CLOSED: `-fno-schedule-insns2` regresses, so sched2 is already right and
 * alias is the wrong axis.
 *
 * INERT: `unsigned n`; declaration order; `register int n`; `++i` in the test;
 * an explicit peel; an explicit temp.
 * WORSE: a pointer local 67; split indices 122 and 138;
 * -fno-rerun-cse-after-loop 66; -fno-strength-reduce 85.
 *
 * FOUR THINGS THAT DID WORK and are already in the candidate, worth keeping
 * whoever picks this up:
 *   A BACKWARD `goto` HIDES A LOOP FROM loop.c -- worth 197 -> 114 across nine
 *   search loops, and NOT the same as -fno-strength-reduce, which breaks the
 *   one loop the ROM does reduce.
 *   THE SEARCHED-FOR CONSTANT IS A LOCAL, NOT A LITERAL -- another 197 -> 114.
 *   cse folds it in the one-predecessor peeled test and cannot in the loop,
 *   which is exactly the ROM's `cmp r3, #0x17` / `cmp r3, r1` pair.
 *   LOCAL ARRAYS LAY OUT IN REVERSE DECLARATION ORDER.
 *   Moving `n = 0` across a loop CHANGES THE FRAME SIZE -- 0x30 with no slot
 *   against the ROM's 0x34 plus a caller-save pair.
 *
 * LANDING IF CLOSED: the .s also holds OvlFunc_943_200ab7c, parked separately at
 * 3 of 97, so closing both would land the file WHOLE. The overlay.ld line MUST
 * KEEP its asm/ path. makefile_flags() is empty.
 */
