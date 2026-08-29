/* Cluster OvlFunc_959_2008af8..OvlFunc_959_2008af8 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances, four-way form: selects one of four edge-transition tables from
 * a gState halfword, falling through to the last. One of a 24-member family.
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_a0;
extern int _AREA_a1;
extern int _AREA_a2;
extern unsigned char L6ff4[] __asm__(".L6ff4");
extern unsigned char L7258[] __asm__(".L7258");
extern unsigned char L7528[] __asm__(".L7528");
extern unsigned char L763c[] __asm__(".L763c");

unsigned char *OvlFunc_959_2008af8(void)
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
    if (v == (int)(&_AREA_a0))
        return L6ff4;
    if (v == (int)(&_AREA_a1))
        return L7258;
    if (v == (int)(&_AREA_a2))
        return L7528;
    return L763c;
}
