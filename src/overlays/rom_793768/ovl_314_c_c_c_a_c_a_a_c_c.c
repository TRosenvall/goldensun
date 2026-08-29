/* Cluster OvlFunc_898_2008f64..OvlFunc_898_2008f64 extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c.s.
 *
 * The remaining .s held ONLY this function and no data, so no further split was
 * needed.
 *
 * Twin of src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_b.c -- same shape,
 * different table label and four different constants. Read that header for the
 * declaration lever reordering the two shifts, which is what both of these
 * needed.
 *
 * Every substituted constant was checked against the ROM individually before
 * screening, per the twin-copy hazard recorded in batch 30.
 */
extern void OvlFunc_898_2008ef4(int a, int b, int c);
extern unsigned char L283e[] __asm__(".L283e");
extern void __Func_8010560(void *p, int a, int b);

void OvlFunc_898_2008f64(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L283e, 0x32, 0x12);
    OvlFunc_898_2008ef4(0x9c << 1, 0x98 << 1, 6);
}
