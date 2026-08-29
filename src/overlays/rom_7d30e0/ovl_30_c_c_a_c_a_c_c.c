/* OvlFunc_948_2009cf8
 *
 * Cut out of goldensun/asm//overlays/rom_7d30e0/ovl_30_c_c_a_c_a_c_c.s.
 *
 * Three position tests that pick which follow-up runs. Needs CSE_CFLAGS for the
 * doubled 0xc0 << 2 flag id.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_948_2009ca0(void);
extern void OvlFunc_948_2009ccc(void);
extern void OvlFunc_948_2009c6c(void);

void OvlFunc_948_2009cf8(void)
{
    unsigned char *a;
    int y;
    int x;
    int z;

    a = (unsigned char *)__MapActor_GetActor(0xa);
    y = *(int *)(a + 0xc) / 0x100000;
    x = *(int *)(a + 8) / 0x100000;
    z = *(int *)(a + 0x10) / 0x100000;
    if (__GetFlag(0xc0 << 2) == 0 && y <= 2) {
        OvlFunc_948_2009ca0();
        __SetFlag(0xc0 << 2);
    }
    if (z == 0x37) {
        if (x == 0x2a)
            OvlFunc_948_2009ccc();
        if (x == 0x26)
            OvlFunc_948_2009ca0();
    } else {
        OvlFunc_948_2009c6c();
    }
}
