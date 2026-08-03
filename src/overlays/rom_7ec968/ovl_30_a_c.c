/* Cluster OvlFunc_963_2008040..OvlFunc_963_2008040 extracted from goldensun/asm/overlays/rom_7ec968/ovl_30_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the
 * .o keeps its name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances, three-way form: selects one of three edge-transition tables
 * from a gState halfword, falling through to the last. One of nine.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_aa;
extern int _ID_ab;
extern unsigned char La40[] __asm__(".La40");
extern unsigned char Lad0[] __asm__(".Lad0");
extern unsigned char gOvl_02008998[];

unsigned char *OvlFunc_963_2008040(void)
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
    if (v == (int)(&_ID_aa))
        return La40;
    if (v == (int)(&_ID_ab))
        return Lad0;
    return gOvl_02008998;
}
