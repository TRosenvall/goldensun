/* Cluster OvlFunc_931_200811c..OvlFunc_931_200811c extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_a_a_b.s.
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
extern int _AREA_4b;
extern int _AREA_4c;
extern unsigned char gScript_930__02009730[];
extern unsigned char L19f4[] __asm__(".L19f4");
extern unsigned char L1724[] __asm__(".L1724");

unsigned char *OvlFunc_931_200811c(void)
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
    if (v == (int)(&_AREA_4b))
        return gScript_930__02009730;
    if (v == (int)(&_AREA_4c))
        return L19f4;
    return L1724;
}
