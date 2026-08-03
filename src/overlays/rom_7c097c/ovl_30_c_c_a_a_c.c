/* Cluster OvlFunc_936_2008240..OvlFunc_936_2008240 extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_a_a_c.s.
 *
 * The .s held ONLY this function and no data -- confirmed with
 * tools/asmfacts.py, not inferred from the function count.
 *
 * GetEntrances, 7-way form: selects one of 7 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * The family sweeps in batches 08-15 capped at 4-way and reported the
 * families complete; removing that cap found 25 more at arities up to twelve.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_63;
extern int _AREA_66;
extern int _AREA_99;
extern int _AREA_9a;
extern int _AREA_9b;
extern int _AREA_9c;
extern unsigned char L4bf4[] __asm__(".L4bf4");
extern unsigned char gScript_882__0200ce88[];
extern unsigned char gScript_882__0200cedc[];
extern unsigned char L4f24[] __asm__(".L4f24");
extern unsigned char L4f54[] __asm__(".L4f54");
extern unsigned char L4f9c[] __asm__(".L4f9c");
extern unsigned char L4be8[] __asm__(".L4be8");

unsigned char *OvlFunc_936_2008240(void)
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
        return L4bf4;
    if (v == (int)(&_AREA_66))
        return gScript_882__0200ce88;
    if (v == (int)(&_AREA_99))
        return gScript_882__0200cedc;
    if (v == (int)(&_AREA_9a))
        return L4f24;
    if (v == (int)(&_AREA_9b))
        return L4f54;
    if (v == (int)(&_AREA_9c))
        return L4f9c;
    return L4be8;
}
