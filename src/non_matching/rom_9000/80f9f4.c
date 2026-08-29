/* DecodeMetatileset -- 0x0800f9f4, asm/rom_9000/rom_f9cc_a_c_b.s
 *
 * FAKEMATCH.  The instruction stream is exact -- tryc.py reports OK, 78 lines --
 * and the .text is FOUR BYTES LARGER than the reference: 0x9c against 0x98.
 * That shifts everything after it in the translation unit, so `make compare`
 * fails on the ROM sha1 even though every instruction agrees.
 *
 * Candidate kept at scratch/hold_f9f4.c.  The difference is pool padding; the
 * reference keeps its literal pool inside the function body.
 *
 * HOW IT GOT THROUGH, which is the part worth keeping: the screen that produced
 * it used a FOUR-FUNCTION .s as the reference, so tryc printed
 * "[size check skipped: ref has 4 functions]" and only compared instructions.
 * Re-screening the same .c against a single-function reference reported the
 * size mismatch immediately.
 *
 *   Any screen carrying "size check skipped" has NOT been size-checked.
 *   Extract a single-function .s and re-run before wiring it into the build.
 *
 * Worth keeping from the attempt (it was otherwise a correct decompilation):
 * the function loads the same symbol twice, once before its `switch` and again
 * inside case 1, and gcc CSEs the two if both are written as the symbol (46
 * differing).  Writing the second as a bare address literal,
 * `(unsigned char *)0x2010002`, restores the second `ldr` and the ROM's
 * register assignment.  Also: moving `count = (n-1)/2;` from before the two
 * pointer initialisations to after them took it 15 -> exact, where swapping the
 * declarations gave 15 and swapping the assignments gave 23.
 */
