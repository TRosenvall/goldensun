/* Cluster OvlFunc_935_2008030..OvlFunc_935_2008030 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_30_a.s.
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
extern int _ID_60;
extern int _ID_61;
extern int _ID_62;
extern unsigned char L18cc[] __asm__(".L18cc");
extern unsigned char L1a34[] __asm__(".L1a34");
extern unsigned char L1b9c[] __asm__(".L1b9c");
extern unsigned char L189c[] __asm__(".L189c");

unsigned char *OvlFunc_935_2008030(void)
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
    if (v == (int)(&_ID_60))
        return L18cc;
    if (v == (int)(&_ID_61))
        return L1a34;
    if (v == (int)(&_ID_62))
        return L1b9c;
    return L189c;
}
