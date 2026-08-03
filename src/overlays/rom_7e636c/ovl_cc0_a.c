/* Cluster OvlFunc_958_2008cc0..OvlFunc_958_2008cc0 extracted from goldensun/asm/overlays/rom_7e636c/ovl_cc0_a.s.
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
extern int _ID_98;
extern int _ID_9d;
extern int _ID_9e;
extern unsigned char L17b4[] __asm__(".L17b4");
extern unsigned char L17fc[] __asm__(".L17fc");
extern unsigned char L1874[] __asm__(".L1874");
extern unsigned char L1784[] __asm__(".L1784");

unsigned char *OvlFunc_958_2008cc0(void)
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
    if (v == (int)(&_ID_98))
        return L17b4;
    if (v == (int)(&_ID_9d))
        return L17fc;
    if (v == (int)(&_ID_9e))
        return L1874;
    return L1784;
}
