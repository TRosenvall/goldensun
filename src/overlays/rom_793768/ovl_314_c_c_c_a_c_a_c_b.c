/* Cluster OvlFunc_898_200906c..OvlFunc_898_200906c extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_793768/overlay.ld, so the ROM layout does
 * not move.
 *
 * Plays a sound, runs a map-rect animation from the table at .L2896, then
 * hands three coordinates to an overlay-local routine. Identical in shape to
 * OvlFunc_901_2008bf8 (batch 10) with a different table and callee.
 */
extern void __PlaySound(int id);
extern void __Func_8010560(void *data, int a, int b);
extern void OvlFunc_898_2008ef4(int x, int y, int c);

extern unsigned char L2896[] __asm__(".L2896");

void OvlFunc_898_200906c(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L2896, 0x26, 6);
    OvlFunc_898_2008ef4(0x78, 0x90, 0xa);
}
