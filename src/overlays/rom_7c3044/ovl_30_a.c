/* Cluster OvlFunc_937_2008030..OvlFunc_937_2008030 extracted from goldensun/asm/overlays/rom_7c3044/ovl_30_a.s.
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
extern int _ID_64;
extern int _ID_65;
extern unsigned char L4d0[] __asm__(".L4d0");
extern unsigned char L6c8[] __asm__(".L6c8");
extern unsigned char MapEntrance_ARRAY_937__020084a0[];

unsigned char *OvlFunc_937_2008030(void)
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
    if (v == (int)(&_ID_64))
        return L4d0;
    if (v == (int)(&_ID_65))
        return L6c8;
    return MapEntrance_ARRAY_937__020084a0;
}
