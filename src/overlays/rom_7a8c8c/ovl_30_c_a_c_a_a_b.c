/* Cluster OvlFunc_922_20080f8..OvlFunc_922_20080f8 extracted from goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_a_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 7-way form: selects one of 7 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * The family sweeps in batches 08-15 capped at 4-way and reported the
 * families complete. Removing that cap found 25 more, at arities up to
 * twelve -- this one included.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_34;
extern int _AREA_3e;
extern int _AREA_3f;
extern int _AREA_40;
extern int _AREA_41;
extern int _AREA_43;
extern unsigned char L29bc[] __asm__(".L29bc");
extern unsigned char L29ec[] __asm__(".L29ec");
extern unsigned char L2a4c[] __asm__(".L2a4c");
extern unsigned char L2ac4[] __asm__(".L2ac4");
extern unsigned char L2b3c[] __asm__(".L2b3c");
extern unsigned char L2b9c[] __asm__(".L2b9c");
extern unsigned char L29a4[] __asm__(".L29a4");

unsigned char *OvlFunc_922_20080f8(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_AREA_34))
        return L29bc;
    if (v == (int)(&_AREA_3e))
        return L29ec;
    if (v == (int)(&_AREA_3f))
        return L2a4c;
    if (v == (int)(&_AREA_40))
        return L2ac4;
    if (v == (int)(&_AREA_41))
        return L2b3c;
    if (v == (int)(&_AREA_43))
        return L2b9c;
    return L29a4;
}
