/* Cluster OvlFunc_914_20089f8..OvlFunc_914_20089f8 extracted from goldensun/asm/overlays/rom_7a1ff0/ovl_30_c_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Near-twin of src/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_b.c -- the same
 * 24-byte struct passed by value, with no else arm. Read that header for why
 * the `ldmia`/`stmia` pair and the four register loads are the calling
 * convention for a 24-byte aggregate rather than anything the source spells
 * out.
 *
 * This one had been passed over twice as "complex struct handling" before the
 * twin made the shape obvious. The tell is `sub sp, #0x20` with a local at
 * sp+8 and an `ldmia`/`stmia` of exactly the remainder: that is an aggregate
 * argument, and the C for it is one line.
 */
#include "gba/types.h"

typedef struct { u32 w[6]; } S24;

extern int OvlFunc_914_2008474(S24 *p);
extern void OvlFunc_914_2008608(S24 s);

void OvlFunc_914_20089f8(void)
{
    S24 s;

    __CutsceneStart();
    if (OvlFunc_914_2008474(&s))
        OvlFunc_914_2008608(s);
    __CutsceneEnd();
}
