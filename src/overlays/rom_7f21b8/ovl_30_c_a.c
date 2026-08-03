/* Cluster OvlFunc_967_200804c..OvlFunc_967_200804c extracted from goldensun/asm/overlays/rom_7f21b8/ovl_30_c_a.s.
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
extern int _ID_b3;
extern unsigned char gOvl_02009690[];
extern unsigned char L16b0[] __asm__(".L16b0");

unsigned char *OvlFunc_967_200804c(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_b3))
        return gOvl_02009690;
    return L16b0;
}
