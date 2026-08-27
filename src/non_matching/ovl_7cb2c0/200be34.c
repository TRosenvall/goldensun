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
 * CORRECTED: this is NOT unreachable.  I first measured "0 of 2987 generated
 * .s files contain mov rA,#K / mov rB,#K2 / lsl rA,#n" and concluded the class
 * was closed.  The detector was broken: generated .s files are gcc's own output
 * and use DECIMAL immediates and the THREE-operand shift (`lsl r2, r2, #1`),
 * while my pattern required the ROM's two-operand form.  It matched nothing.
 *
 * Re-measured with both forms accepted and a positive control:
 *     adjacent mov/lsl build (control) : 777 of 2987
 *     split mov / mov / lsl            :  51 of 2987
 *
 * gcc DOES emit this shape, 51 times.  So the spelling exists and I have not
 * found it.  This park is "not yet solved", not "cannot be solved".
 *
 * Best C: scratch/rbe34.c.
 */
