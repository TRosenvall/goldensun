/* Cluster OvlFunc_948_20089f0..OvlFunc_948_20089f0 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_a_c.s.
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
extern unsigned char L2898[] __asm__(".L2898");
extern unsigned char L28e0[] __asm__(".L28e0");
extern unsigned char gOvl_0200a928[];
extern unsigned char L2868[] __asm__(".L2868");

unsigned char *OvlFunc_948_20089f0(void)
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
        return L2898;
    if (v == (int)(&_AREA_76))
        return L28e0;
    if (v == (int)(&_AREA_78))
        return gOvl_0200a928;
    return L2868;
}
