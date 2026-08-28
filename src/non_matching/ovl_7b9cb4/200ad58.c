/* OvlFunc_932_200ad58 -- asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_c_c_c_a.s
 *
 * BLOCKER: SPLIT SHIFTED BUILD -- 2 of 69, same length, 67 lines exact
 *
 *     rom  mov r2,#0xf9 / mov r1,#0x0 / lsl r2,#0x10
 *     ours mov r2,#0xf9 / lsl r2,#0x10 / mov r1,#0x0
 *
 * The ROM slots the cheap `mov r1, #0` between the two halves of the
 * `0xf9 << 16` build; gcc finishes the build first.  Note this shape IS
 * reachable -- 51 of the 2987 generated .s files contain it -- so this is
 * "spelling not found", not "cannot be done".  (An earlier claim that it was
 * unreachable was retracted in batch 119; the detector behind it was broken.)
 *
 * MEASURED (all 69 lines):
 *   plain literals                                            2
 *   `n = 0;` assigned immediately before the call, passed as arg2   2
 *   __Func_80933f8 declared `int` (return-type lever)         4  (worse)
 *
 * GETTING HERE WAS THE INTERESTING PART, and the lesson is reusable:
 * the first draft was 72 of 69 at 74 lines -- five instructions too many and
 * every register renamed -- purely from having too many long-lived locals.
 * The ROM spends r5, r6 and r8; I had r5, r6, r8 and r10.  Removing TWO locals
 * that each named a value used once (a `char *q` for the dereferenced global,
 * and an `int z` for a literal zero stored through a byte pointer) took it
 * from 72 differing to 2.
 *
 * The global's ADDRESS does want a name (`char **pp = &iwram_3001ebc;` gives
 * the ROM's single `ldr r6, =iwram_3001ebc` with two `ldr r2, [r6]` reloads),
 * and the byte offset does too (`k = 0xe0 << 1` held in r8, reused by both
 * stores).  The dereferenced VALUE does not.  Naming one level too many is
 * what cost the extra callee-saved register.
 *
 * Best C: scratch/vad58.c.
 */
