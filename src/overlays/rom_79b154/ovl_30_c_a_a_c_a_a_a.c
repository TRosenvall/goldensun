/* Cluster OvlFunc_907_200811c..OvlFunc_907_200811c extracted from goldensun/asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_a_a.s.
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
extern int _ID_1e;
extern int _ID_23;
extern int _ID_20;
extern unsigned char L1498[] __asm__(".L1498");
extern unsigned char L1600[] __asm__(".L1600");
extern unsigned char L16f0[] __asm__(".L16f0");
extern unsigned char gScript_944__02009480[];

unsigned char *OvlFunc_907_200811c(void)
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
    if (v == (int)(&_ID_1e))
        return L1498;
    if (v == (int)(&_ID_23))
        return L1600;
    if (v == (int)(&_ID_20))
        return L16f0;
    return gScript_944__02009480;
}
