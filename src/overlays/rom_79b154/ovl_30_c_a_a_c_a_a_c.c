/* Cluster OvlFunc_907_2008198..OvlFunc_907_2008198 extracted from goldensun/asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_a_c.s.
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
extern int _AREA_1e;
extern int _AREA_23;
extern int _AREA_20;
extern unsigned char L1744[] __asm__(".L1744");
extern unsigned char L1a2c[] __asm__(".L1a2c");
extern unsigned char L1bc4[] __asm__(".L1bc4");
extern unsigned char L1738[] __asm__(".L1738");

unsigned char *OvlFunc_907_2008198(void)
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
    if (v == (int)(&_AREA_1e))
        return L1744;
    if (v == (int)(&_AREA_23))
        return L1a2c;
    if (v == (int)(&_AREA_20))
        return L1bc4;
    return L1738;
}
