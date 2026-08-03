/* Cluster OvlFunc_935_20082e0..OvlFunc_935_20082e0 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_a_a.s.
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
extern int _AREA_60;
extern int _AREA_61;
extern int _AREA_62;
extern unsigned char L1f98[] __asm__(".L1f98");
extern unsigned char L2064[] __asm__(".L2064");
extern unsigned char L2190[] __asm__(".L2190");
extern unsigned char L1f8c[] __asm__(".L1f8c");

unsigned char *OvlFunc_935_20082e0(void)
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
    if (v == (int)(&_AREA_60))
        return L1f98;
    if (v == (int)(&_AREA_61))
        return L2064;
    if (v == (int)(&_AREA_62))
        return L2190;
    return L1f8c;
}
