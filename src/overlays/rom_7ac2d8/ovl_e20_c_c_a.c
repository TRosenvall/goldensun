/* Cluster OvlFunc_924_2008e80..OvlFunc_924_2008e80 extracted from goldensun/asm/overlays/rom_7ac2d8/ovl_e20_c_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 5-way form: selects one of 5 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * THE EARLIER FAMILY SWEEPS CAPPED AT 4-WAY. Batches 08-15 elevated the two-,
 * three- and four-way forms and reported the families complete; a sweep with
 * the arity unbounded finds 25 more at arities up to twelve. That is the
 * second self-imposed limit in the same sweep -- the first, corrected in batch
 * 15, was matching only `.L` returns.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_36;
extern int _AREA_37;
extern int _AREA_38;
extern int _AREA_39;
extern unsigned char L6700[] __asm__(".L6700");
extern unsigned char L67a8[] __asm__(".L67a8");
extern unsigned char L6838[] __asm__(".L6838");
extern unsigned char L6988[] __asm__(".L6988");
extern unsigned char L66e8[] __asm__(".L66e8");

unsigned char *OvlFunc_924_2008e80(void)
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
    if (v == (int)(&_AREA_36))
        return L6700;
    if (v == (int)(&_AREA_37))
        return L67a8;
    if (v == (int)(&_AREA_38))
        return L6838;
    if (v == (int)(&_AREA_39))
        return L6988;
    return L66e8;
}
