/* Cluster OvlFunc_965_2008fdc..OvlFunc_965_2008fdc extracted from goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_c_a_b.s.
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
extern int _ID_b0;
extern int _ID_af;
extern int _ID_ae;
extern unsigned char L3694[] __asm__(".L3694");
extern unsigned char L3754[] __asm__(".L3754");
extern unsigned char L3784[] __asm__(".L3784");
extern unsigned char L388c[] __asm__(".L388c");

unsigned char *OvlFunc_965_2008fdc(void)
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
    if (v == (int)(&_ID_b0))
        return L3694;
    if (v == (int)(&_ID_af))
        return L3754;
    if (v == (int)(&_ID_ae))
        return L3784;
    return L388c;
}
