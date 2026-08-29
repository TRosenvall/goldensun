/* OvlFunc_907_2008328 -- 0x02008328, asm/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c.s
 *
 * 97 ROM lines against 98 of ours, 60 differing, down from 78.
 * Candidate: scratch/N8328_D.c.
 *
 * The clear-every-actor loop (8..0x41 inclusive, unsigned), the two PlaySound
 * arms, the derived store of 0x100 through iwram_3001ebc, and both trailing
 * two-way branches all reproduce.
 *
 * SOLVED ON THE WAY, and reusable: the index is computed as
 *      t = *(unsigned short *)(w + (0xb6 << 1));  t -= 3;  k = (short)t;
 * Written as one expression -- `(short)(*(unsigned short *)... - 3)` -- the
 * arithmetic stays in unsigned-short and gcc emits `ldr r4, =0xfffd / add r3, r4`
 * where the ROM has a plain `sub r3, #3`.  Routing it through an int local gives
 * the sub.  78 differing -> 60, and one line shorter.  Same shape as the
 * halfword-mask finding in batch 136: a narrow type makes gcc pool a constant
 * that an int keeps as an immediate.
 *
 * BLOCKER, two things and both known walls:
 *
 *   1. REGISTER ROLES.  The ROM holds the iwram pointer in r7 and the loop's
 *      stored zero in r6; ours has them swapped, and that decides four lines
 *      plus everything downstream that names either register.  Both assignment
 *      orders were screened (zero named before and after the pointer) and both
 *      give 79 -- worse than leaving the zero a literal at 78.
 *
 *   2. TABLE-ACCESS ORDER.  __Func_8010560 takes a word from one table and two
 *      shorts from another at the same k*4 index.  The ROM computes the two
 *      shorts FIRST and the word into r0 last; gcc does r0 first.  Dropping the
 *      callee's prototype -- the documented way to push r0 to the end -- changes
 *      nothing here (60 either way).
 *
 * So the remaining 60 are one register-role decision and one argument-evaluation
 * order, neither of which has a lever.  Worth revisiting if the register-role
 * class ever yields the way the memory-typed half of it did in batch 136.
 */
