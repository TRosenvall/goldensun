/* Cluster OvlFunc_947_200a53c..OvlFunc_947_200a53c extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_a.o and asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7d0e88/overlay.ld.
 *
 * Fills a 24-byte block from a helper and, if that succeeds, passes it BY VALUE
 * to another; otherwise runs a three-call fallback.
 *
 * A 24-BYTE STRUCT PASSED BY VALUE, which is worth recognising because the
 * assembly for it looks like hand-written pointer juggling:
 *
 *     mov r2, sp / add r3, sp, #0x18 / ldmia r3!, {r0, r1} / stmia r2!, {r0, r1}
 *     ldr r0, [r5] / ldr r1, [r5, #4] / ldr r2, [r5, #8] / ldr r3, [r5, #0xc]
 *     bl OvlFunc_947_20088ec
 *
 * That is the ARM calling convention for a 24-byte aggregate: the first sixteen
 * bytes go in r0-r3 and the remaining eight are copied to the top of the
 * outgoing stack area with an `ldmia`/`stmia` pair. Written as
 * `f(s)` with `s` a six-word struct, gcc emits exactly this. Written as a
 * pointer plus explicit copies it does not.
 *
 * `sub sp, #0x20` covers both the local (24 bytes at sp+8) and the 8-byte
 * outgoing slot at sp, which is why the local sits at an offset rather than at
 * sp itself -- that falls out of the struct being an argument, not something the
 * source has to arrange.
 *
 * Its near-twin src/overlays/rom_7a1ff0/ovl_30_c_c_a_a.c has the same shape with
 * no else arm.
 */
#include "gba/types.h"

typedef struct { u32 w[6]; } S24;

extern int OvlFunc_947_2008758(S24 *p);
extern void OvlFunc_947_20088ec(S24 s);

void OvlFunc_947_200a53c(void)
{
    S24 s;

    __CutsceneStart();
    if (OvlFunc_947_2008758(&s)) {
        OvlFunc_947_20088ec(s);
    } else {
        OvlFunc_947_200a498();
        OvlFunc_947_20083a8();
        OvlFunc_947_200a4cc();
    }
    __CutsceneEnd();
}
