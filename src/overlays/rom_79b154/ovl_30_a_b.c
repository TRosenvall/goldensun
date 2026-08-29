/* Cluster OvlFunc_907_2008088..OvlFunc_907_2008088 extracted from goldensun/asm/overlays/rom_79b154/ovl_30_a_b.s.
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
extern int _AREA_1e;
extern int _AREA_23;
extern int _AREA_20;
extern unsigned char L11ec[] __asm__(".L11ec");
extern unsigned char L130c[] __asm__(".L130c");
extern unsigned char L136c[] __asm__(".L136c");
extern unsigned char L11d4[] __asm__(".L11d4");

unsigned char *OvlFunc_907_2008088(void)
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
    if (v == (int)(&_AREA_1e))
        return L11ec;
    if (v == (int)(&_AREA_23))
        return L130c;
    if (v == (int)(&_AREA_20))
        return L136c;
    return L11d4;
}
