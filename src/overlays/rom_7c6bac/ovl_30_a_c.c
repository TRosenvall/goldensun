/* Cluster OvlFunc_942_2008040..OvlFunc_942_2008040 extracted from goldensun/asm/overlays/rom_7c6bac/ovl_30_a_c.s.
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
extern int _ID_6b;
extern int _ID_70;
extern int _ID_6c;
extern unsigned char L1738[] __asm__(".L1738");
extern unsigned char L17c8[] __asm__(".L17c8");
extern unsigned char L1840[] __asm__(".L1840");
extern unsigned char L1708[] __asm__(".L1708");

unsigned char *OvlFunc_942_2008040(void)
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
    if (v == (int)(&_ID_6b))
        return L1738;
    if (v == (int)(&_ID_70))
        return L17c8;
    if (v == (int)(&_ID_6c))
        return L1840;
    return L1708;
}
