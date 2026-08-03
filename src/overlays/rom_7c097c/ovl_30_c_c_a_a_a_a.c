/* Cluster OvlFunc_936_2008180..OvlFunc_936_2008180 extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_a_a_a_a.s.
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
extern int _AREA_63;
extern int _AREA_66;
extern int _AREA_99;
extern int _AREA_9c;
extern unsigned char L4768[] __asm__(".L4768");
extern unsigned char L4a20[] __asm__(".L4a20");
extern unsigned char L4a80[] __asm__(".L4a80");
extern unsigned char L4b58[] __asm__(".L4b58");
extern unsigned char gScript_926__0200c750[];

unsigned char *OvlFunc_936_2008180(void)
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
    if (v == (int)(&_AREA_63))
        return L4768;
    if (v == (int)(&_AREA_66))
        return L4a20;
    if (v == (int)(&_AREA_99))
        return L4a80;
    if (v == (int)(&_AREA_9c))
        return L4b58;
    return gScript_926__0200c750;
}
