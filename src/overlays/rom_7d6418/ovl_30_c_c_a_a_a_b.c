/* Cluster OvlFunc_951_2008044..OvlFunc_951_2008044 extracted from goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_a_a_a.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances for this map: picks one of two edge-transition tables from a
 * gState halfword. One of an 18-member family; see
 * src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constant has to be a symbol.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_bd;
extern unsigned char L1aec[] __asm__(".L1aec");
extern unsigned char L1cfc[] __asm__(".L1cfc");

unsigned char *OvlFunc_951_2008044(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_bd))
        return L1aec;
    return L1cfc;
}
