/* Cluster OvlFunc_930_200807c..OvlFunc_930_200807c extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_a_c_c.s.
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
extern int _ID_4a;
extern unsigned char L1844[] __asm__(".L1844");
extern unsigned char L17b4[] __asm__(".L17b4");

unsigned char *OvlFunc_930_200807c(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_4a))
        return L1844;
    return L17b4;
}
