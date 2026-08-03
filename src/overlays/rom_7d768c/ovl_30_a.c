/* Cluster OvlFunc_952_2008030..OvlFunc_952_2008030 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_a.s.
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
extern int _AREA_8b;
extern unsigned char L4a1c[] __asm__(".L4a1c");
extern unsigned char L4614[] __asm__(".L4614");

unsigned char *OvlFunc_952_2008030(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_8b))
        return L4a1c;
    return L4614;
}
