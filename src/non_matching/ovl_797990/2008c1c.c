/* OvlFunc_901_2008c1c -- asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_c_a_a.s
 * OvlFunc_898_2009090 -- asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_c_a.s
 *
 * TWO-MEMBER FAMILY (cross-overlay duplicates).  2 of 75, same length.
 *
 * 73 of 75 exact, including four __CopyMapTiles sites sharing one carried `2`,
 * the stack-arg pair for __Func_8010704, and both bit-twiddles.  The residue
 * is one scheduling transposition:
 *
 *     rom  and r3, r2 / mov r2, r8      / strb r3, [r6]
 *     ours and r3, r2 / strb r3, [r6]   / mov r2, r8
 *
 * `mov r2, r8` is gcc copying the sprite pointer out of a high register for
 * the next statement's `ldrb [r2, #9]`.  The ROM issues it before the mask
 * store; gcc after.
 *
 * THE USEFUL FINDING IS THE TYPE ASYMMETRY, and it contradicts the sharpening
 * made the same day for `orr`:
 *
 *     orr, constant as destination   ->  needs `unsigned char m`
 *                                        (`int m` is folded, byte-identical
 *                                        to the plain literal)
 *     and, constant as destination   ->  needs `int m`
 *                                        (`unsigned char m` puts the LOADED
 *                                        value in the destination instead)
 *
 * Measured here: `unsigned char m = 0xfe;` gives 4 differing;
 * `int m = 0xfe;` gives 2 and fixes the operand roles.  Both spellings of
 * "make the constant the destination" (`*p = m & *p;` and
 * `m = m & *p; *p = m;`) are byte-identical to each other under either type,
 * so the type is the whole lever and the statement form is inert.
 *
 * ALSO MEASURED, all 2 differing at position 57:
 *   `p = a + 0x23;` moved before the __Func_8010704 call    2
 *   the two independent stores swapped in source           16 (much worse)
 *
 * Best C: scratch/J8c1c.c.
 */
