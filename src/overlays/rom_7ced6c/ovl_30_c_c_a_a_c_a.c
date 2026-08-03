/* Cluster OvlFunc_946_2008ec4..OvlFunc_946_2008ec4 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_a.s.
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
extern int _AREA_71;
extern int _AREA_72;
extern int _AREA_7b;
extern int _AREA_7c;
extern int _AREA_7d;
extern unsigned char L3904[] __asm__(".L3904");
extern unsigned char L38e0[] __asm__(".L38e0");
extern unsigned char L39f4[] __asm__(".L39f4");
extern unsigned char gScript_932__0200bd48[];
extern unsigned char L3d6c[] __asm__(".L3d6c");
extern unsigned char L3880[] __asm__(".L3880");

unsigned char *OvlFunc_946_2008ec4(void)
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
        return L3904;
    if (v == (int)(&_AREA_72))
        return L38e0;
    if (v == (int)(&_AREA_7b))
        return L39f4;
    if (v == (int)(&_AREA_7c))
        return gScript_932__0200bd48;
    if (v == (int)(&_AREA_7d))
        return L3d6c;
    return L3880;
}
