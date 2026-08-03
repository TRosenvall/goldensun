/* Cluster OvlFunc_960_2008e5c..OvlFunc_960_2008e5c extracted from goldensun/asm/overlays/rom_7eaf28/ovl_314_c_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_7eaf28/overlay.ld, so the ROM layout does
 * not move.
 *
 * The eighteenth and last member of the two-way GetEntrances family. It was
 * parked, and the park note was WRONG about why.
 *
 * split_s.py refused the cut on crossing local labels, and the note reasoned
 * from the file's totals -- nine functions, 54 labels, only 8 exported -- that
 * clearing it would take "not two exports but dozens, restructuring nine
 * functions' worth of data to land one 15-instruction stub", and left it.
 *
 * Computing what actually crosses THIS cut gives ONE label: .L1a00. Exporting
 * it cleared the refusal. The totals were never the right thing to count.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constant has to be a symbol.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_a6;
extern unsigned char L19c4[] __asm__(".L19c4");
extern unsigned char L17b4[] __asm__(".L17b4");

unsigned char *OvlFunc_960_2008e5c(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_ID_a6))
        return L19c4;
    return L17b4;
}
