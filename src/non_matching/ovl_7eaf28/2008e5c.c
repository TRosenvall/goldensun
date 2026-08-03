/* OvlFunc_960_2008e5c  [ovl_7eaf28]  --  0x02008e5c
 *
 * Source asm: goldensun/asm/overlays/rom_7eaf28/ovl_314_c_c.s
 *
 * Blocker: SPLIT REFUSED, not a codegen mismatch. The C below is believed
 * correct and has never been compiled against a reference, because the
 * function cannot be given a translation unit of its own yet.
 *
 * It is the last of the 18-member GetEntrances family; the other seventeen
 * are elevated. Same shape as src/overlays/rom_79aad8/ovl_314_a.c.
 *
 * WHY THE SPLIT IS REFUSED
 *
 *     REFUSING to split asm/overlays/rom_7eaf28/ovl_314_c_c.s:
 *     local labels would cross files.
 *
 * `.L` symbols do not survive into an object's symbol table, so a label
 * referenced from one part of a split and defined in another is invisible to
 * the linker. The guard exists because a split once broke the link silently.
 *
 * The same refusal on rom_7b7f1c and rom_7ed0a0 WAS cleared, by declaring the
 * two tables the function selects between `.global` -- a .global emits no
 * bytes, and it was already the established practice for sibling tables in
 * those files. That does not work here.
 *
 * This file holds NINE functions and 54 local labels, of which only 8 are
 * exported. Cutting at this function strands references belonging to the
 * OTHER functions, not to this one -- this function needs only .L17b4 and
 * .L19c4 (its two tables) plus .Le74/.Le76 (its own branch targets).
 *
 * So the fix is not two exports but dozens, restructuring nine functions'
 * worth of data to land one 15-instruction stub. Not worth it on its own.
 * It becomes cheap the moment anything else in this file is elevated, since
 * the boundary has to be worked out then anyway.
 *
 * WHAT WOULD ALSO WORK: splitting at a different point, so that this function
 * travels with the labels its neighbours reference. tools/split_s.py takes one
 * function and produces _a/_b/_c; a variant that cuts on a label-closed
 * boundary rather than a function boundary would clear this whole class.
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
