/* Overlay 910: play a sound, run a map edit, record it happened.
 *
 * Split out of asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_79dd90/overlay.ld, so the ROM layout is unchanged.
 *
 * The map-edit table is reached with an asm label -- `.Lbd4` is already
 * .global in the sibling ovl_30_c_c_c_c_c.s, so no assembly changes.
 */

extern void __PlaySound(int id);
extern void __Func_8010560(void *data, int a, int b);
extern void __SetFlag(int flag);

extern unsigned char Data_bd4[] __asm__(".Lbd4");

void OvlFunc_910_20088e8(void)
{
    __PlaySound(0xbc);
    __Func_8010560(Data_bd4, 0x34, 0xb);
    __SetFlag(0x80 << 2);
}
