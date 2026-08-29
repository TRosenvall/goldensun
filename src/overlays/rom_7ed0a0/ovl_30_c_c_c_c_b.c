/* Cluster OvlFunc_964_200a370..OvlFunc_964_200a370 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances for this map: picks one of two edge-transition tables from a
 * gState halfword. One of an 18-member family; see
 * src/overlays/rom_79aad8/ovl_314_a.c for the shape.
 *
 * tools/split_s.py REFUSED this one until `.L3c0c` and `.L3ef4` were declared
 * .global in the .s. Both are .incbin tables, so C cannot carry them into
 * this translation unit; they have to stay in assembly and be referenced
 * across the object boundary the split creates. Four sibling tables in the
 * same file were already exported for this function's already-elevated
 * neighbours -- these two simply had not been needed yet. A .global emits no
 * bytes, and `make compare` was verified green after the export and BEFORE
 * the split, so the two changes are separable.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_ac;
extern unsigned char L3c0c[] __asm__(".L3c0c");
extern unsigned char L3ef4[] __asm__(".L3ef4");

unsigned char *OvlFunc_964_200a370(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_ac))
        return L3c0c;
    return L3ef4;
}
