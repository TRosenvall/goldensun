/* Cluster OvlFunc_941_2008044..OvlFunc_941_2008044 extracted from goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_a.s.
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
extern int _ID_6a;
extern unsigned char L1cd8[] __asm__(".L1cd8");
extern unsigned char L1cc0[] __asm__(".L1cc0");

unsigned char *OvlFunc_941_2008044(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_6a))
        return L1cd8;
    return L1cc0;
}
