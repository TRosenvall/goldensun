/* Cluster OvlFunc_895_2008030..OvlFunc_895_2008030 extracted from goldensun/asm/overlays/rom_78dee8/ovl_30_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the
 * .o keeps its name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances, three-way form: selects one of three edge-transition tables
 * from a gState halfword, falling through to the last. One of nine.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_13;
extern int _AREA_10;
extern unsigned char L1d04[] __asm__(".L1d04");
extern unsigned char L1d64[] __asm__(".L1d64");
extern unsigned char MapEntrance_ARRAY_895__02009cd4[];

unsigned char *OvlFunc_895_2008030(void)
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
    if (v == (int)(&_AREA_13))
        return L1d04;
    if (v == (int)(&_AREA_10))
        return L1d64;
    return MapEntrance_ARRAY_895__02009cd4;
}
