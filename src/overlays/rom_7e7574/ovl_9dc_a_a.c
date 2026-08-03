/* Cluster OvlFunc_959_20089dc..OvlFunc_959_20089dc extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the
 * .o keeps its name and its slot in the overlay's linker script is unchanged.
 * Confirmed with tools/split_s.py, which refuses this shortcut when the file
 * also carries .incbin tables the .c cannot take with it.
 *
 * GetEntrances, four-way form: selects one of four edge-transition tables from
 * a gState halfword, falling through to the last. One of a 24-member family.
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_a0;
extern int _ID_a1;
extern int _ID_a2;
extern unsigned char L62a4[] __asm__(".L62a4");
extern unsigned char L64b4[] __asm__(".L64b4");
extern unsigned char L6754[] __asm__(".L6754");
extern unsigned char L6814[] __asm__(".L6814");

unsigned char *OvlFunc_959_20089dc(void)
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
    if (v == (int)(&_ID_a0))
        return L62a4;
    if (v == (int)(&_ID_a1))
        return L64b4;
    if (v == (int)(&_ID_a2))
        return L6754;
    return L6814;
}
