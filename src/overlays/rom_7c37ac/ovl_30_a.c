/* Cluster OvlFunc_938_2008030..OvlFunc_938_2008030 extracted from goldensun/asm/overlays/rom_7c37ac/ovl_30_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first
 * arm rather than a local `.L` table, which is why the earlier family sweep
 * missed it -- that sweep matched only on `.L` returns.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constant has to be a symbol.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_67;
extern unsigned char gScript_887__02009c04[];
extern unsigned char L1bd4[] __asm__(".L1bd4");

unsigned char *OvlFunc_938_2008030(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_67))
        return gScript_887__02009c04;
    return L1bd4;
}
