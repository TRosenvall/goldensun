/* Cluster OvlFunc_920_20080f4..OvlFunc_920_20080f4 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 4-way form: selects one of 4 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_31;
extern int _AREA_30;
extern int _AREA_2f;
extern unsigned char Lea8[] __asm__(".Lea8");
extern unsigned char Lefc[] __asm__(".Lefc");
extern unsigned char gOvl_02008f80[];
extern unsigned char Le9c[] __asm__(".Le9c");

unsigned char *OvlFunc_920_20080f4(void)
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
    if (v == (int)(&_AREA_31))
        return Lea8;
    if (v == (int)(&_AREA_30))
        return Lefc;
    if (v == (int)(&_AREA_2f))
        return gOvl_02008f80;
    return Le9c;
}
