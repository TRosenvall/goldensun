/* Cluster OvlFunc_921_2008130..OvlFunc_921_2008130 extracted from goldensun/asm/overlays/rom_7a7298/ovl_30_a.s.
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
extern int _ID_33;
extern unsigned char L28a0[] __asm__(".L28a0");
extern unsigned char L2798[] __asm__(".L2798");

unsigned char *OvlFunc_921_2008130(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_33))
        return L28a0;
    return L2798;
}
