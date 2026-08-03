/* Cluster OvlFunc_920_20080a0..OvlFunc_920_20080a0 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_b.s.
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
extern int _ID_31;
extern int _ID_30;
extern int _ID_2f;
extern unsigned char Lc2c[] __asm__(".Lc2c");
extern unsigned char Lc5c[] __asm__(".Lc5c");
extern unsigned char Lcbc[] __asm__(".Lcbc");
extern unsigned char Lc14[] __asm__(".Lc14");

unsigned char *OvlFunc_920_20080a0(void)
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
    if (v == (int)(&_ID_31))
        return Lc2c;
    if (v == (int)(&_ID_30))
        return Lc5c;
    if (v == (int)(&_ID_2f))
        return Lcbc;
    return Lc14;
}
