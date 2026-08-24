/* Cluster OvlFunc_957_2008eac..OvlFunc_957_2008eac extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c.s.
 *
 * Preserves the original ROM layout in goldensun/overlays/rom_7e3e08/overlay.ld.
 *
 * Third and fourth member of the 24-BYTE STRUCT-BY-VALUE family, byte-identical
 * to src/overlays/rom_7a1ff0/ovl_30_c_c_a_a.c apart from the two callee names.
 * Read that header, and
 * src/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_b.c, for why the ldmia/stmia pair
 * and the four register loads are the ARM calling convention for a 24-byte
 * aggregate rather than anything the source spells out.
 *
 * Found by sweeping for the shape rather than by reading candidates: 53
 * functions in asm/ still have `sub sp, #N` with an ldmia/stmia pair, and the
 * two smallest were these. Recognising a calling convention is worth more than
 * any single lever -- the C is one line and the assembly looks like machinery.
 */
#include "gba/types.h"

typedef struct { u32 w[6]; } S24;

extern int OvlFunc_957_2008474(S24 *p);
extern void OvlFunc_957_2008608(S24 s);

void OvlFunc_957_2008eac(void)
{
    S24 s;

    __CutsceneStart();
    if (OvlFunc_957_2008474(&s))
        OvlFunc_957_2008608(s);
    __CutsceneEnd();
}
