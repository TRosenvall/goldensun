/* Cluster OvlFunc_927_2008f40..OvlFunc_927_2008f40 extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_a_b.s.
 *
 * Split out of that .s; the sibling part stays as assembly and keeps its slot
 * in the overlay's linker script, so the ROM layout does not move.
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
extern unsigned char L36a0[] __asm__(".L36a0");
extern unsigned char L3790[] __asm__(".L3790");
extern unsigned char L38b0[] __asm__(".L38b0");
extern unsigned char L3a30[] __asm__(".L3a30");

unsigned char *OvlFunc_927_2008f40(void)
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
        return L36a0;
    if (v == (int)(&_ID_45))
        return L3790;
    if (v == (int)(&_ID_46))
        return L38b0;
    return L3a30;
}
