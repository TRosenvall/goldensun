/* Cluster OvlFunc_927_200a4ac..OvlFunc_927_200a4ac extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_c_a_b.s.
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
extern int _ID_44;
extern int _ID_45;
extern int _ID_46;
extern unsigned char L3a48[] __asm__(".L3a48");
extern unsigned char L3b20[] __asm__(".L3b20");
extern unsigned char L3c1c[] __asm__(".L3c1c");
extern unsigned char L3d54[] __asm__(".L3d54");

unsigned char *OvlFunc_927_200a4ac(void)
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
    if (v == (int)(&_ID_44))
        return L3a48;
    if (v == (int)(&_ID_45))
        return L3b20;
    if (v == (int)(&_ID_46))
        return L3c1c;
    return L3d54;
}
