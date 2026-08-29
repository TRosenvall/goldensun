/* OvlFunc_900_20081e4 -- asm/overlays/rom_797740/ovl_30_c_c_c.s
 *
 * BLOCKER: orr OPERAND ORDER IN A DELIBERATELY MIXED FUNCTION
 *
 * 6 of 54 differing.
 *
 * Five orr sites and the ROM mixes their forms on purpose.  The else arm's first
 * two `orr r3, r5` need the plain literal `*q |= 0x14`; the ROM's THIRD is
 * `orr r5, r3`, and the area == 0xa arm is `mov r3,#0x14 / orr r3,r2` -- both
 * "constant is rd".  Naming the constant unsigned char reaches "constant is rd"
 * but lands it in r5/r3 where the ROM has r3/r2.
 * * MEASURED: all-plain 11 (with a cross-jump); all-named 22+ (cross-jumped);
 * named-in-both-arms-only 6 (the file kept); unsigned char m shared vs per-block
 * 6 / worse; char* instead of unsigned char* 6; separate pointer local for the
 * third site 6; m hoisted above the if 42; unsigned char m = 0x14 at the top of
 * the else with literals below adds r6, worse.  -fno-rerun-cse-after-loop 6.
 */
