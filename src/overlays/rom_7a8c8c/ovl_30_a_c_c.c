/* Cluster OvlFunc_922_2008050..OvlFunc_922_2008050 extracted from goldensun/asm/overlays/rom_7a8c8c/ovl_30_a_c_c.s.
 *
 * The .s held ONLY this function and no data -- confirmed with
 * tools/asmfacts.py, not inferred from the function count.
 *
 * GetEntrances, 8-way form: selects one of 8 per-area tables from
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
extern int _AREA_34;
extern int _AREA_3e;
extern int _AREA_3f;
extern int _AREA_40;
extern int _AREA_41;
extern int _AREA_42;
extern int _AREA_43;
extern unsigned char L24bc[] __asm__(".L24bc");
extern unsigned char L2504[] __asm__(".L2504");
extern unsigned char L25f4[] __asm__(".L25f4");
extern unsigned char L263c[] __asm__(".L263c");
extern unsigned char L26cc[] __asm__(".L26cc");
extern unsigned char L2744[] __asm__(".L2744");
extern unsigned char L27bc[] __asm__(".L27bc");
extern unsigned char L248c[] __asm__(".L248c");

unsigned char *OvlFunc_922_2008050(void)
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
    if (v == (int)(&_AREA_34))
        return L24bc;
    if (v == (int)(&_AREA_3e))
        return L2504;
    if (v == (int)(&_AREA_3f))
        return L25f4;
    if (v == (int)(&_AREA_40))
        return L263c;
    if (v == (int)(&_AREA_41))
        return L26cc;
    if (v == (int)(&_AREA_42))
        return L2744;
    if (v == (int)(&_AREA_43))
        return L27bc;
    return L248c;
}
