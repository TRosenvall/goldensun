/* Cluster OvlFunc_948_2008a50..OvlFunc_948_2008a50 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 4-way form. Returns a named global from at least one arm,
 * which is why the family sweeps in batches 08-13 missed it -- they matched
 * only on `.L` returns.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_75;
extern int _AREA_76;
extern int _AREA_78;
extern unsigned char L29b0[] __asm__(".L29b0");
extern unsigned char L2a40[] __asm__(".L2a40");
extern unsigned char L2ad0[] __asm__(".L2ad0");
extern unsigned char gScript_884__0200a998[];

unsigned char *OvlFunc_948_2008a50(void)
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
    if (v == (int)(&_AREA_75))
        return L29b0;
    if (v == (int)(&_AREA_76))
        return L2a40;
    if (v == (int)(&_AREA_78))
        return L2ad0;
    return gScript_884__0200a998;
}
