/* Cluster OvlFunc_931_2008030..OvlFunc_931_2008030 extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_a.s.
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
extern int _AREA_4b;
extern int _AREA_4c;
extern unsigned char L1120[] __asm__(".L1120");
extern unsigned char L1288[] __asm__(".L1288");
extern unsigned char L10f0[] __asm__(".L10f0");

unsigned char *OvlFunc_931_2008030(void)
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
    if (v == (int)(&_AREA_4b))
        return L1120;
    if (v == (int)(&_AREA_4c))
        return L1288;
    return L10f0;
}
