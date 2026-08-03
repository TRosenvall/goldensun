/* Cluster OvlFunc_924_2008e20..OvlFunc_924_2008e20 extracted from goldensun/asm/overlays/rom_7ac2d8/ovl_e20_a.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances, four-way form: selects one of four edge-transition tables from
 * a gState halfword, falling through to the last. One of a 24-member family.
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_39;
extern int _AREA_38;
extern int _AREA_37;
extern unsigned char L650c[] __asm__(".L650c");
extern unsigned char L635c[] __asm__(".L635c");
extern unsigned char L623c[] __asm__(".L623c");
extern unsigned char L60ec[] __asm__(".L60ec");

unsigned char *OvlFunc_924_2008e20(void)
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
    if (v == (int)(&_AREA_39))
        return L650c;
    if (v == (int)(&_AREA_38))
        return L635c;
    if (v == (int)(&_AREA_37))
        return L623c;
    return L60ec;
}
