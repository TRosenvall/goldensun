/* Cluster OvlFunc_948_2008ee0..OvlFunc_948_2008ee0 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 4-way form, returning a named global from one arm. That
 * is why the family sweeps in batches 08-13 missed it -- they matched only on
 * `.L` returns, so fourteen members of the three families were invisible.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for the shape.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_75;
extern int _AREA_76;
extern int _AREA_78;
extern unsigned char L2bb4[] __asm__(".L2bb4");
extern unsigned char L2cb0[] __asm__(".L2cb0");
extern unsigned char gScript_953__0200adac[];
extern unsigned char L2ba8[] __asm__(".L2ba8");

unsigned char *OvlFunc_948_2008ee0(void)
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
    if (v == (int)(&_AREA_75))
        return L2bb4;
    if (v == (int)(&_AREA_76))
        return L2cb0;
    if (v == (int)(&_AREA_78))
        return gScript_953__0200adac;
    return L2ba8;
}
