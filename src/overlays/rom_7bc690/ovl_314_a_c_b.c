/* Cluster OvlFunc_933_20083ac..OvlFunc_933_20083ac extracted from goldensun/asm/overlays/rom_7bc690/ovl_314_a_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 5-way form: selects one of 5 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_59;
extern int _AREA_5a;
extern int _AREA_5b;
extern int _AREA_5c;
extern unsigned char L2174[] __asm__(".L2174");
extern unsigned char L21d4[] __asm__(".L21d4");
extern unsigned char L2234[] __asm__(".L2234");
extern unsigned char L22dc[] __asm__(".L22dc");
extern unsigned char L212c[] __asm__(".L212c");

unsigned char *OvlFunc_933_20083ac(void)
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
    if (v == (int)(&_AREA_59))
        return L2174;
    if (v == (int)(&_AREA_5a))
        return L21d4;
    if (v == (int)(&_AREA_5b))
        return L2234;
    if (v == (int)(&_AREA_5c))
        return L22dc;
    return L212c;
}
