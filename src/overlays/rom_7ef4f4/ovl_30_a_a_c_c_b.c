/* Cluster OvlFunc_965_2008f58..OvlFunc_965_2008f58 extracted from goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_b.s.
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
extern int _AREA_b0;
extern int _AREA_af;
extern int _AREA_ae;
extern unsigned char L3270[] __asm__(".L3270");
extern unsigned char L3330[] __asm__(".L3330");
extern unsigned char L34f8[] __asm__(".L34f8");
extern unsigned char L3558[] __asm__(".L3558");

unsigned char *OvlFunc_965_2008f58(void)
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
    if (v == (int)(&_AREA_b0))
        return L3270;
    if (v == (int)(&_AREA_af))
        return L3330;
    if (v == (int)(&_AREA_ae))
        return L34f8;
    return L3558;
}
