/* Func_801c154 (ReleaseScreenTiles) -- 0x0801c154,
 * asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s
 *
 * 15 vs 13 lines, 10 differing.  Candidate at scratch/L1c154.c.
 *
 * SOLVED: the mask must be applied through an `int` intermediate.  Written
 * `(v & 0xfffffe00)` with v an `unsigned short`, the OPERATION narrows to
 * HImode and gcc emits `mov r4,#0xfe / lsl r4,#8` -- masking with 0xfe00
 * instead of the ROM's pooled 0xfffffe00.  An `int v` gives the pool load.
 * Same rule as batch 79's Func_800c5b4 and the Func_8020a60 template.
 *
 * BLOCKER, two parts:
 *   - The two ANDs have OPPOSITE destination conventions in the ROM:
 *     `and r1, r3` puts the result in the VALUE register (a &= 0x1ff) while
 *     `and r3, r4` puts it in the CONSTANT's register (0xfffffe00 &= v).
 *     Spelling them that way in the source (`a &= 0x1ff;` and
 *     `n = 0xfffffe00; n &= v;`) changed nothing -- gcc canonicalises both.
 *   - The ROM ends `bl Func_8003dec / b L0 / L0:` -- a branch over an inline
 *     literal pool.  Ours has no pool to branch over, so it is two instructions
 *     short.  That is the `.pool_aligned` shape and it is downstream of the
 *     pool existing at all.
 */
