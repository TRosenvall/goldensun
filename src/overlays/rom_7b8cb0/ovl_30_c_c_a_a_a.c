/* OvlFunc_931_200807c -- the whole of
 * goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_a_a_a.s, so no split was needed
 * and the overlay's linker script is unchanged. The file holds one function and
 * no data section.
 *
 * Chooses this map's entrance table by area id and patches a few of its entries
 * according to which progress flags are set.
 *
 * THE LEVER IS THE SAME ONE THAT UNPARKED ITS TWO SIBLINGS THIS BATCH: A LOCAL
 * THAT ONLY HOLDS AN ADDRESS COSTS THE ORDERING -- DELETE IT. The park carried
 * the first block's two stores through named pointers `p` and `q`; indexing the
 * table symbol directly matches. Reordering or re-typing them does nothing.
 *
 * MEASURED (rom 62 lines):
 *   the park's `p`/`q` locals                            6 aligned
 *   `q = L140c; p = q;` and the reverse                  6 each
 *   the two stores swapped in source                     5
 *   `p`/`q` deleted, the table indexed directly          MATCH
 *
 * The later blocks keep their pointer locals because there the ROM recomputes
 * the base each time and the local is what produces that.
 */
extern unsigned int gState;
extern int _AREA_4b;
extern int _AREA_4c;
extern unsigned char L140c[] __asm__(".L140c");
extern unsigned char L15bc[] __asm__(".L15bc");
extern unsigned char L13f4[] __asm__(".L13f4");
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_931_200807c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int g;
    unsigned int off;
    int v;
    int z;

    g = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    g += off;
    v = *(short *)g;
    if (v == (int)(&_AREA_4b)) {
        if (__GetFlag(0x909) != 0) {
            z = 0;
            L140c[0x8e] = z;
            L140c[0xa6] = z;
        }
        return L140c;
    }
    if (v == (int)(&_AREA_4c)) {
        if (__GetFlag(0x8fd) != 0) {
            p = L15bc;
            p += 0x2e;
            z = 1;
            *p = z;
        }
        if (__GetFlag(0x8fe) != 0 || __GetFlag(0x907) != 0) {
            p = L15bc;
            p += 0x5e;
            z = 1;
            *p = z;
        }
        q = L15bc;
        __Func_808b868(q);
        return q;
    }
    return L13f4;
}
