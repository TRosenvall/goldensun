/* Cluster OvlFunc_916_2008054..OvlFunc_916_2008054 extracted from goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_a.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in goldensun/overlays/rom_7a37f0/overlay.ld is unchanged.
 *
 * .L12c0 is a 4-byte .lcomm in ovl_30_c_c_c_c_c.s, read as a pointer here and
 * passed to two different overlay routines. The ROM keeps its ADDRESS in r5
 * across both calls and reloads the value each time, which is what reading
 * the same global twice compiles to.
 *
 * THE __Func_8010704 PROTOTYPE IS LOAD-BEARING -- see docs/elevation.md.
 * Without it the call is implicitly declared and gcc-2.96 fills r1 before r0;
 * the ROM fills r0 first. Declaring it is the whole difference between this
 * function matching and not.
 */
extern void *L12c0 __asm__(".L12c0");
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_916_2008054(void) {
    OvlFunc_916_2008c2c(L12c0);
    __Func_8010704(0, 0x40, 0x20, 0x20, 0, 0);
    OvlFunc_916_2008b3c(L12c0, 0xff);
    OvlFunc_916_2008150();
}
