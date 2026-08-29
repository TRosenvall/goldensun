/* Func_8094428 -- 0x08094428, asm/rom_8a000/rom_93304_c_c_c.s
 *
 * 83 ROM lines against 80 of ours; the 48 differing are a three-line shift from
 * ONE construct, and everything else -- three flag-guarded arms sharing an id
 * register, the rotated while loop, the int return read off the epilogue --
 * reproduces.  Candidate: scratch/L4428.c.
 *
 * BLOCKER, and it is the exact mirror of the symbol-base case solved in
 * ovl_30_c_c_a_c_c_a_a... see common1_a_a_a_a_c_c_a_b.c:
 *
 *      rom   ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r6, [r3]
 *      ours  ldr r3, =gState+500 / ldr r6, [r3]
 *
 * The ROM materialises the symbol base and adds the offset at runtime; we fold
 * the offset into the relocation.  gState is 0x02000240 (wram.sym) and the
 * target 0x02000434 even has its own symbol, ewram_2000434, so the base is not
 * in doubt -- the ROM genuinely chose base-plus-register.
 *
 * WHY LAST ROUND'S REMEDY CANNOT APPLY.  There the fix was to declare the
 * symbol with the access's type and index it, which keeps the offset as an
 * addressing-mode DISPLACEMENT.  That only works while the offset fits the
 * displacement: 0x1e did, 0x1f4 does not -- thumb word loads cap at 124.  Once
 * the offset must live in a register anyway, gcc prefers folding it into the
 * pooled address, and no spelling tried reaches back.
 *
 * TRIED, all 48: `extern short gState[]` with `*(int *)&gState[0xfa]` -- the
 * spelling the ROM's `mov #0xfa / lsl #1` literally suggests, and it still
 * folds; GCSE, ALIAS, STRENGTH.  CSE (79 differing) and SCHED2 (50) are worse.
 *
 * Also worth recording from this function: the ROM COMMONS the -1 triple passed
 * to Func_80933f8 here (mov r2,#1 / neg / mov r0,r2 / mov r1,r2), which is
 * precisely what OvlFunc_965_2008eac was parked for NOT doing at the same
 * callee.  The same call is spelled both ways in the same ROM, so neither form
 * is a property of the callee.
 */
