/* Cluster OvlFunc_924_2008f30..OvlFunc_924_2008f30 extracted from goldensun/asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c.s.
 *
 * THE LAST MEMBER OF THE 24-FUNCTION FAMILY, and the only one that needed its
 * .s split by hand.
 *
 * That .s held one function and FOURTEEN .incbin tables. Converting the whole
 * file -- which is what tools/split_s.py advised at the time, and what its
 * "holds only this function" check has since been taught to refuse -- deleted
 * the data along with the assembly and broke the link with
 *
 *     undefined reference to `.L6c10'
 *
 * split_s.py cuts on FUNCTION boundaries, so it cannot separate a function
 * from data in the same file. Done by hand instead, in two separable steps,
 * each verified with `make compare` before the next:
 *
 *   1. Export the four tables this function selects between. Nine siblings in
 *      the same .data section were already exported exactly this way, so the
 *      practice is the file's own. A .global emits no bytes.
 *   2. Cut at .func_end into _b (this function) and _c (the .data section),
 *      listing both in overlay.ld where the original was. The function comes
 *      first in the file, so the order is preserved and the layout does not
 *      move.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_36;
extern int _ID_37;
extern int _ID_38;
extern unsigned char L6ad8[] __asm__(".L6ad8");
extern unsigned char L6c10[] __asm__(".L6c10");
extern unsigned char L6d60[] __asm__(".L6d60");
extern unsigned char L6ec8[] __asm__(".L6ec8");

unsigned char *OvlFunc_924_2008f30(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_ID_36))
        return L6ad8;
    if (v == (int)(&_ID_37))
        return L6c10;
    if (v == (int)(&_ID_38))
        return L6d60;
    return L6ec8;
}
