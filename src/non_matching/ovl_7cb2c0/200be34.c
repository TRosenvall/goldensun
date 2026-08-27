/* OvlFunc_945_200be34 -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: THE SPLIT SHIFTED BUILD -- unreachable, proved by corpus test
 *
 * 14 of 63, same length.  49 lines exact, including the four-argument
 * OvlFunc_945_200c8ac call with its interleaved lsl/neg/lsl, which matches
 * byte for byte.  The entire residue is one transposition:
 *
 *     rom  mov r1,#0x96 / mov r0,#0x10 / lsl r1,#0x10 / ldr r2,=0x24a0000
 *     ours mov r1,#0x96 / lsl r1,#0x10 / ldr r2,=0x24a0000 / mov r0,#0x10
 *
 * The ROM slots `mov r0, #0x10` between the two halves of the `0x96 << 16`
 * build; gcc completes the build first.  Structurally identical to the
 * `mov #K / mov #0 / neg / mov #0` family in ovl_7c460c/2008c74.c -- a
 * multi-instruction constant build split by an unrelated `mov`.
 *
 * CORPUS TEST -- UNREACHABLE, do not spend screens:
 *   0 of the 2987 GENERATED .s files (built from committed src/*.c) contain
 *
 *       mov rA, #K / mov rB, #K2 / lsl rA, #n
 *
 *   gcc-2.96 as configured here finishes a shifted constant build before
 *   touching another register, always.  No spelling reaches it.
 *
 * This is the same technique that closed the neg family, and it generalises:
 * when a residue is a fixed short instruction sequence, grep the GENERATED .s
 * for it first.  If the compiler has never emitted it anywhere in the corpus,
 * the difference is not a spelling problem.
 *
 * Best C: scratch/rbe34.c.
 */
