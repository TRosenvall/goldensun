/* Cluster OvlFunc_963_20080e4..OvlFunc_963_20080e4 extracted from goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances, three-way form: selects one of three edge-transition tables
 * from a gState halfword, falling through to the last. One of nine.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_aa;
extern int _AREA_ab;
extern unsigned char Lddc[] __asm__(".Lddc");
extern unsigned char Le54[] __asm__(".Le54");
extern unsigned char Ld10[] __asm__(".Ld10");

unsigned char *OvlFunc_963_20080e4(void)
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
    if (v == (int)(&_AREA_aa))
        return Lddc;
    if (v == (int)(&_AREA_ab))
        return Le54;
    return Ld10;
}
