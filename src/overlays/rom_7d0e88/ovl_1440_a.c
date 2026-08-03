/* Cluster OvlFunc_947_2009440..OvlFunc_947_2009440 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1440_a.s.
 *
 * The .s held ONLY this function and no data -- confirmed with
 * tools/asmfacts.py, not inferred from the function count.
 *
 * GetEntrances, 6-way form: selects one of 6 per-area tables from
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
extern int _AREA_73;
extern int _AREA_74;
extern int _AREA_77;
extern int _AREA_79;
extern int _AREA_7a;
extern unsigned char L2eac[] __asm__(".L2eac");
extern unsigned char L2ef4[] __asm__(".L2ef4");
extern unsigned char L2f3c[] __asm__(".L2f3c");
extern unsigned char L2f84[] __asm__(".L2f84");
extern unsigned char L2fcc[] __asm__(".L2fcc");
extern unsigned char L2e7c[] __asm__(".L2e7c");

unsigned char *OvlFunc_947_2009440(void)
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
    if (v == (int)(&_AREA_73))
        return L2eac;
    if (v == (int)(&_AREA_74))
        return L2ef4;
    if (v == (int)(&_AREA_77))
        return L2f3c;
    if (v == (int)(&_AREA_79))
        return L2f84;
    if (v == (int)(&_AREA_7a))
        return L2fcc;
    return L2e7c;
}
