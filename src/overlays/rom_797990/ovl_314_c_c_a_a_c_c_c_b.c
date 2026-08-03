/* Cluster OvlFunc_901_2008bf8..OvlFunc_901_2008bf8 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_797990/overlay.ld, so the ROM layout does
 * not move.
 *
 * Plays a sound, runs a map-rect animation from the table at .L17ae, then
 * hands three coordinates to an overlay-local routine.
 *
 * Every callee is declared with its return type. That is load-bearing here,
 * not tidiness -- see docs/elevation.md. Written with the calls implicitly
 * declared, gcc-2.96 keeps r0 live across each one and fills the next call's
 * r0 last, where the ROM fills it first.
 */
extern void __PlaySound(int id);
extern void __Func_8010560(void *data, int a, int b);
extern void OvlFunc_901_2008a80(int x, int y, int c);

extern unsigned char L17ae[] __asm__(".L17ae");

void OvlFunc_901_2008bf8(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L17ae, 0x26, 6);
    OvlFunc_901_2008a80(0x78, 0x90, 0xa);
}
