/* OvlFunc_947_2008f58 -- 0x02008f58,
 * asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_c.s
 *
 * 55 of 55 lines, EIGHT differing, and all eight are two held values in swapped
 * registers.  Candidate at scratch/N8f58b_best.c.
 *
 *      rom   ldr r4, [r6, #0x10] / ldr r5, [r6, #0x8] ... add r1, r4 / add r0, r5
 *      ours  ldr r4, [r6, #0x8]  / ldr r5, [r6, #0x10] ... add r0, r4 / add r1, r5
 *
 * The two struct fields feed both an addition and a stack slot each, so they are
 * named locals by the batch-149 reading -- and that part is right: the
 * `str r5, [sp] / str r4, [sp, #4]` pair reproduces.  Only which field gets
 * which register differs.
 *
 * SCREENED AND INERT, all still 8: the two assignments in either order, the two
 * declarations in either order, both together, and the two sums computed into
 * named locals in the ROM's evaluation order to force which addition happens
 * first.  This is the coin flip of src/non_matching/ovl_7ced6c/2009c84.c,
 * .../200a16c.c and .../20096a8.c -- four functions now, and the source has no
 * handle on it in any of them.
 *
 * SOLVED and not to be re-derived: the 0x30-byte frame is eight bytes of
 * outgoing stack-argument area, four address-taken int locals, and a 0x18-byte
 * struct also passed by address to OvlFunc_947_2008ddc; the six-argument call
 * takes its two stack slots from the SAME registers the additions use, so they
 * are shared locals rather than a fresh pair; and the trailing `e[0x23] &= 0xfd`
 * gives the ROM's `mov r3, #0xfd / and r3, r2 / strb`.
 *
 * WHY IT IS HERE: five calls, where the candidate filter required eight until
 * this round.  Like OvlFunc_951_20096a8 it is a fair test of the relaxed floor
 * and it failed on a wall that has nothing to do with call density.
 */
