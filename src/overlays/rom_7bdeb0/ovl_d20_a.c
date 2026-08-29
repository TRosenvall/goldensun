/* Cluster OvlFunc_934_2008d20..OvlFunc_934_2008d20 extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_d20_a.s.
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
extern int _AREA_5d;
extern int _AREA_5e;
extern int _AREA_5f;
extern unsigned char L1f9c[] __asm__(".L1f9c");
extern unsigned char L2014[] __asm__(".L2014");
extern unsigned char L2134[] __asm__(".L2134");
extern unsigned char L1f6c[] __asm__(".L1f6c");

unsigned char *OvlFunc_934_2008d20(void)
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
    if (v == (int)(&_AREA_5d))
        return L1f9c;
    if (v == (int)(&_AREA_5e))
        return L2014;
    if (v == (int)(&_AREA_5f))
        return L2134;
    return L1f6c;
}
