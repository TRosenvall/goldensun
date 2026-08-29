/* Cluster OvlFunc_964_2009270..OvlFunc_964_2009270 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_b.s.
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
extern int _AREA_ac;
extern int _AREA_ad;
extern unsigned char L3474[] __asm__(".L3474");
extern unsigned char L3654[] __asm__(".L3654");
extern unsigned char L342c[] __asm__(".L342c");

unsigned char *OvlFunc_964_2009270(void)
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
    if (v == (int)(&_AREA_ac))
        return L3474;
    if (v == (int)(&_AREA_ad))
        return L3654;
    return L342c;
}
