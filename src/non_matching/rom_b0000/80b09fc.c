/* Func_80b09fc (InitListRecord) -- 0x080b09fc,
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s
 *
 * 16 vs 14 lines, 12 differing.  Candidate at scratch/Lb09fc.c.
 * Every store is in the ROM's order and to the right offset; what differs is
 * two things, both about the constant 0.
 *
 *   - The ROM loads the zero FROM THE POOL: `ldr r6, =0x0`, where ours emits
 *     `mov r5, #0x0`.  gcc-2.96 does sometimes pool a zero -- Func_80a3ddc
 *     reproduces `ldr r2, =0` from a plain `0` -- but nothing here provokes it.
 *   - Because the ROM has a pool it also has `b L0 / L0:` branching over it,
 *     which is the two-line deficit.
 *
 * So the whole difference is downstream of whether the TU emits an inline pool,
 * and that is decided by the zero's spelling, which I could not reach.  The
 * register roles (r5 for the list pointer, r4 for the loaded halfword) are
 * swapped with ours as a consequence.
 */
