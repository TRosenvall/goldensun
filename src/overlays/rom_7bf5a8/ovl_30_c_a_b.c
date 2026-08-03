/* Cluster OvlFunc_935_200808c..OvlFunc_935_200808c extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_30_c_a_b.s.
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
extern int _ID_60;
extern int _ID_61;
extern int _ID_62;
extern unsigned char L1c80[] __asm__(".L1c80");
extern unsigned char L1cc0[] __asm__(".L1cc0");
extern unsigned char L1cfc[] __asm__(".L1cfc");
extern unsigned char L1c7c[] __asm__(".L1c7c");

unsigned char *OvlFunc_935_200808c(void)
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
    if (v == (int)(&_ID_60))
        return L1c80;
    if (v == (int)(&_ID_61))
        return L1cc0;
    if (v == (int)(&_ID_62))
        return L1cfc;
    return L1c7c;
}
