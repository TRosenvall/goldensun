/* Cluster OvlFunc_960_20083ac..OvlFunc_960_20083ac extracted from goldensun/asm/overlays/rom_7eaf28/ovl_314_c_a_c_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 4-way form: selects one of 4 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_a4;
extern int _AREA_a5;
extern int _AREA_a6;
extern unsigned char L1610[] __asm__(".L1610");
extern unsigned char gScript_930__020096b8[];
extern unsigned char L1790[] __asm__(".L1790");
extern unsigned char L15f8[] __asm__(".L15f8");

unsigned char *OvlFunc_960_20083ac(void)
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
    if (v == (int)(&_AREA_a4))
        return L1610;
    if (v == (int)(&_AREA_a5))
        return gScript_930__020096b8;
    if (v == (int)(&_AREA_a6))
        return L1790;
    return L15f8;
}
