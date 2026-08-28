/* Func_80167ac -- asm/rom_15000/rom_15e8c_a_c_c_c_c_c.s
 *
 * BLOCKER: FORMED POINTER vs REGISTER-OFFSET STORE -- ours 13 of the ROM's 15
 *
 * A 15-instruction leaf that copies three halfwords into a global buffer at
 * offsets 0xeae, 0xeac and 0xea8.  The ROM keeps the offset in r4 and builds a
 * fresh destination POINTER for each store:
 *
 *     rom  add r3, r2, r4 / strh r1, [r3, #0x0]
 *     ours strh r2, [r1, r3]
 *
 * The register-offset store is one instruction shorter, so we come out two
 * short overall.  This is the INVERSE of the usual lever: normally the work is
 * to stop gcc forming a pointer and get the `[rA, rB]` form, and there is no
 * documented way to push it the other way.
 *
 * MEASURED:
 *   *(unsigned short *)(p + o) = ...       14 differing, ours 13 lines
 *   a named `char *d = p + o;` per store    14, ours 13 (gcc folds the local
 *                                           straight back into the store)
 *
 * Note the offsets are themselves informative and are reproduced: 0xeae is
 * pooled, 0xeac is derived from it with `sub r4, #2`, and 0xea8 is a SECOND
 * pool entry rather than another derivation -- so the source names the first
 * offset, mutates it once, and writes the third as its own constant.
 *
 * Best C: scratch/D167ac.c.
 */
