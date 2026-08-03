/* Cluster OvlFunc_927_2008ee0..OvlFunc_927_2008ee0 extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_a_c_c_c.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances, four-way form: selects one of four edge-transition tables from
 * a gState halfword, falling through to the last. One of a 24-member family.
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_44;
extern int _AREA_45;
extern int _AREA_46;
extern unsigned char L30f4[] __asm__(".L30f4");
extern unsigned char L31e4[] __asm__(".L31e4");
extern unsigned char L3334[] __asm__(".L3334");
extern unsigned char L34b4[] __asm__(".L34b4");

unsigned char *OvlFunc_927_2008ee0(void)
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
    if (v == (int)(&_AREA_44))
        return L30f4;
    if (v == (int)(&_AREA_45))
        return L31e4;
    if (v == (int)(&_AREA_46))
        return L3334;
    return L34b4;
}
