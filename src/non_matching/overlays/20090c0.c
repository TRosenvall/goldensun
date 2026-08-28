/* OvlFunc_924_20090c0 -- 0x020090c0,
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a.s
 *
 * 55 vs 56 lines, 22 differing.  Candidate at scratch/L90c0.c.
 *
 * SOLVED: the three-guard cascade, the three separately-named copies of
 * 0x80 << 9 for the first __Func_8012330, and the derived store
 * (`add r3, r2 / sub r2, #0xc0 / str r2, [r3]`, 0x1c0 - 0xc0 = 0x100) which
 * reproduces from a mutated offset variable as the batch-123 rule predicts.
 *
 * BLOCKER: register roles in the iwram address chain, plus the order of two
 * mov+neg builds.
 *      rom   mov r2,#0xe0 / ldr r3,[r3] / lsl r2,#1 / add r3,r2 / sub r2,#0xc0
 *      ours  ldr r2,[r3]  / mov r3,#0xe0 / lsl r3,#1 / add r2,r3 / sub r3,#0xc0
 * The ROM keeps the base in r3 and the offset in r2; ours swaps them, which
 * then swaps the store's operands.
 *
 * TRIED: swapping the declaration order of the base and offset locals;
 * reordering the two `m = 1; m = -m;` pairs so both movs precede both negs;
 * --no-rerun-cse.  All 22.
 *
 * Same wall as src/non_matching/overlays/200807c.c and
 * src/non_matching/rom_a1000/80a5fe0.c: the source can move an allocation but
 * cannot choose it.
 */
