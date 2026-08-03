/* Cluster OvlFunc_930_2009180..OvlFunc_930_2009180 extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script, so the ROM layout does not move.
 *
 * GetEntrances for this map: picks one of two edge-transition tables from a
 * gState halfword. One of an 18-member family; see
 * src/overlays/rom_79aad8/ovl_314_a.c for the shape.
 *
 * tools/split_s.py REFUSED this one until `.L1c9c` and `.L1b10` were declared
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
extern int _ID_4a;
extern unsigned char L1c9c[] __asm__(".L1c9c");
extern unsigned char L1b10[] __asm__(".L1b10");

unsigned char *OvlFunc_930_2009180(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_4a))
        return L1c9c;
    return L1b10;
}
