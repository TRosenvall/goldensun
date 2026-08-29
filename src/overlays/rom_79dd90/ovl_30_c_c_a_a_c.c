/* Cluster OvlFunc_910_2008154..OvlFunc_910_2008154 extracted from goldensun/asm/overlays/rom_79dd90/ovl_30_c_c_a_a_c.s.
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
extern int _AREA_22;
extern unsigned char Ld30[] __asm__(".Ld30");
extern unsigned char Ld24[] __asm__(".Ld24");

unsigned char *OvlFunc_910_2008154(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_22))
        return Ld30;
    return Ld24;
}
