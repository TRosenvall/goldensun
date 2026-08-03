/* Cluster OvlFunc_911_200816c..OvlFunc_911_200816c extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances, three-way form: selects one of three edge-transition tables
 * from a gState halfword, falling through to the last. One of nine.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_27;
extern int _ID_26;
extern unsigned char L2f80[] __asm__(".L2f80");
extern unsigned char gScript_913__0200afc8[];
extern unsigned char L2e60[] __asm__(".L2e60");

unsigned char *OvlFunc_911_200816c(void)
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
    if (v == (int)(&_ID_27))
        return L2f80;
    if (v == (int)(&_ID_26))
        return gScript_913__0200afc8;
    return L2e60;
}
