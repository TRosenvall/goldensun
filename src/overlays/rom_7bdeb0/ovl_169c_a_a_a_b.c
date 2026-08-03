/* Cluster OvlFunc_934_200969c..OvlFunc_934_200969c extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_169c_a_a_a_b.s.
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
extern int _ID_5d;
extern int _ID_5e;
extern int _ID_5f;
extern unsigned char L2420[] __asm__(".L2420");
extern unsigned char L2450[] __asm__(".L2450");
extern unsigned char L2624[] __asm__(".L2624");
extern unsigned char L2414[] __asm__(".L2414");

unsigned char *OvlFunc_934_200969c(void)
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
    if (v == (int)(&_ID_5d))
        return L2420;
    if (v == (int)(&_ID_5e))
        return L2450;
    if (v == (int)(&_ID_5f))
        return L2624;
    return L2414;
}
