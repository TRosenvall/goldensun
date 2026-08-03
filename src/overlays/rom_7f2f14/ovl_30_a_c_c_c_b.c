/* Cluster OvlFunc_968_2008e04..OvlFunc_968_2008e04 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_c_c_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 6-way form: selects one of 6 per-area tables from
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
extern int _AREA_b5;
extern int _AREA_b7;
extern int _AREA_b8;
extern int _AREA_b9;
extern int _AREA_ba;
extern unsigned char L5d68[] __asm__(".L5d68");
extern unsigned char L6020[] __asm__(".L6020");
extern unsigned char L6230[] __asm__(".L6230");
extern unsigned char L6350[] __asm__(".L6350");
extern unsigned char L6548[] __asm__(".L6548");
extern unsigned char L5dc8[] __asm__(".L5dc8");

unsigned char *OvlFunc_968_2008e04(void)
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
    if (v == (int)(&_AREA_b5))
        return L5d68;
    if (v == (int)(&_AREA_b7))
        return L6020;
    if (v == (int)(&_AREA_b8))
        return L6230;
    if (v == (int)(&_AREA_b9))
        return L6350;
    if (v == (int)(&_AREA_ba))
        return L6548;
    return L5dc8;
}
