/* Cluster OvlFunc_957_2008a00..OvlFunc_957_2008a00 extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_b.s.
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
extern int _AREA_93;
extern int _AREA_95;
extern int _AREA_97;
extern unsigned char L41b0[] __asm__(".L41b0");
extern unsigned char L4270[] __asm__(".L4270");
extern unsigned char L4318[] __asm__(".L4318");
extern unsigned char L4198[] __asm__(".L4198");

unsigned char *OvlFunc_957_2008a00(void)
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
    if (v == (int)(&_AREA_93))
        return L41b0;
    if (v == (int)(&_AREA_95))
        return L4270;
    if (v == (int)(&_AREA_97))
        return L4318;
    return L4198;
}
