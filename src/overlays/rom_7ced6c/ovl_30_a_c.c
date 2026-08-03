/* Cluster OvlFunc_946_2008cc4..OvlFunc_946_2008cc4 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 6-way form: selects one of 6 per-area tables from
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
extern int _AREA_71;
extern int _AREA_72;
extern int _AREA_7b;
extern int _AREA_7c;
extern int _AREA_7d;
extern unsigned char L3310[] __asm__(".L3310");
extern unsigned char L3358[] __asm__(".L3358");
extern unsigned char L33a0[] __asm__(".L33a0");
extern unsigned char L3400[] __asm__(".L3400");
extern unsigned char L3448[] __asm__(".L3448");
extern unsigned char L3478[] __asm__(".L3478");

unsigned char *OvlFunc_946_2008cc4(void)
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
    if (v == (int)(&_AREA_71))
        return L3310;
    if (v == (int)(&_AREA_72))
        return L3358;
    if (v == (int)(&_AREA_7b))
        return L33a0;
    if (v == (int)(&_AREA_7c))
        return L3400;
    if (v == (int)(&_AREA_7d))
        return L3448;
    return L3478;
}
