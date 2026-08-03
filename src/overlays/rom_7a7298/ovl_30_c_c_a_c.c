/* Cluster OvlFunc_921_20081ec..OvlFunc_921_20081ec extracted from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_a_c.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in the overlay's linker script is unchanged.
 *
 * GetEntrances for this map: picks one of two edge-transition tables from a
 * gState halfword. One of an 18-member family; see
 * src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constant has to be a symbol.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_33;
extern unsigned char L2db8[] __asm__(".L2db8");
extern unsigned char L2c80[] __asm__(".L2c80");

unsigned char *OvlFunc_921_20081ec(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_33))
        return L2db8;
    return L2c80;
}
