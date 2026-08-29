/* Overlay 887: two map edits applied back to back.
 *
 * Split out of the seventeen-part chain at
 * asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c.s; the
 * neighbouring parts stay as assembly and are listed around this one in
 * overlays/rom_787e04/overlay.ld, so the ROM layout is unchanged.
 *
 * Both calls pass the SAME value for their two stack arguments, so the ROM
 * reuses one register for each pair of stores and gcc does too. Where the two
 * differ, the ROM builds them into separate registers first and gcc does not
 * -- the "stack-arg-pair" blocker, which gates fifty functions.
 */

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_887_20093b4(void)
{
    __CopyMapTiles(0x16, 0x55, 0x19, 0x55, 2, 2);
    __Func_8010704(0x19, 0xf, 2, 2, 0x19, 0x19);
}
