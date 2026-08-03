/* Cluster OvlFunc_909_2008100..OvlFunc_909_2008100 extracted from goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_a_a.s.
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
extern int _AREA_21;
extern unsigned char L2ca8[] __asm__(".L2ca8");
extern unsigned char L2c9c[] __asm__(".L2c9c");

unsigned char *OvlFunc_909_2008100(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_21))
        return L2ca8;
    return L2c9c;
}
