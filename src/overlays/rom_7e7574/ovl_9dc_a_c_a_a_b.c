/* Cluster OvlFunc_959_2008a80..OvlFunc_959_2008a80 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_a_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 6-way form: selects one of 6 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_6a;
extern int _AREA_a2;
extern int _AREA_a1;
extern int _AREA_a0;
extern int _AREA_a3;
extern unsigned char L69d0[] __asm__(".L69d0");
extern unsigned char L6e08[] __asm__(".L6e08");
extern unsigned char L6c28[] __asm__(".L6c28");
extern unsigned char L6ac0[] __asm__(".L6ac0");
extern unsigned char L6e98[] __asm__(".L6e98");
extern unsigned char L69b8[] __asm__(".L69b8");

unsigned char *OvlFunc_959_2008a80(void)
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
    if (v == (int)(&_AREA_6a))
        return L69d0;
    if (v == (int)(&_AREA_a2))
        return L6e08;
    if (v == (int)(&_AREA_a1))
        return L6c28;
    if (v == (int)(&_AREA_a0))
        return L6ac0;
    if (v == (int)(&_AREA_a3))
        return L6e98;
    return L69b8;
}
