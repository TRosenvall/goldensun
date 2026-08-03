/* Cluster OvlFunc_947_200a580..OvlFunc_947_200a580 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_2580_a_b.s.
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
extern int _AREA_73;
extern int _AREA_74;
extern int _AREA_77;
extern int _AREA_79;
extern int _AREA_7a;
extern unsigned char L33a8[] __asm__(".L33a8");
extern unsigned char L3438[] __asm__(".L3438");
extern unsigned char L3498[] __asm__(".L3498");
extern unsigned char L351c[] __asm__(".L351c");
extern unsigned char L3618[] __asm__(".L3618");
extern unsigned char L339c[] __asm__(".L339c");

unsigned char *OvlFunc_947_200a580(void)
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
    if (v == (int)(&_AREA_73))
        return L33a8;
    if (v == (int)(&_AREA_74))
        return L3438;
    if (v == (int)(&_AREA_77))
        return L3498;
    if (v == (int)(&_AREA_79))
        return L351c;
    if (v == (int)(&_AREA_7a))
        return L3618;
    return L339c;
}
