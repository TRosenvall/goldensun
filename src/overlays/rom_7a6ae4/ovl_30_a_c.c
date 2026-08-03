/* Cluster OvlFunc_920_2008040..OvlFunc_920_2008040 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_a_c.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances, four-way form: selects one of four edge-transition tables from
 * a gState halfword, falling through to the last. Head of a 24-member family.
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_31;
extern int _ID_30;
extern int _ID_2f;
extern unsigned char L9ec[] __asm__(".L9ec");
extern unsigned char La64[] __asm__(".La64");
extern unsigned char Lb24[] __asm__(".Lb24");
extern unsigned char L9bc[] __asm__(".L9bc");

unsigned char *OvlFunc_920_2008040(void)
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
    if (v == (int)(&_ID_31))
        return L9ec;
    if (v == (int)(&_ID_30))
        return La64;
    if (v == (int)(&_ID_2f))
        return Lb24;
    return L9bc;
}
