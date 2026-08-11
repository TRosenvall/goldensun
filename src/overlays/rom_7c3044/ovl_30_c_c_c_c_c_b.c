/* Cluster OvlFunc_937_20081fc..OvlFunc_937_20081fc extracted from goldensun/asm/overlays/rom_7c3044/ovl_30_c_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7c3044/ovl_30_c_c_c_c_c_a.o and asm/overlays/rom_7c3044/ovl_30_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7c3044/overlay.ld.
 *
 * A sanctum attendant, in the SHIFTED-ADDITIVE variant of the facing test:
 *
 *     ldr r2, =0x5fff / add r3, r2 / ldr r2, =0x3ffe0000 / lsl r3, #16 / cmp
 *
 * 0x5fff is -0xa001 taken modulo 0x10000, so on a u16 it is the same range as
 * the subtractive form -- but written as an ADDITION the ROM pools a small
 * positive constant instead of 0xffff5fff, which is the visible difference.
 * `(u16)(f + 0x5fff) <= 0x3ffe` reproduces it.
 *
 * The pre-shifted 0x3ffe0000 is gcc's own doing: given a narrowing cast it
 * shifts once and compares against the constant moved to match, rather than
 * emitting lsl/lsr to narrow first. See
 * src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_b.c for the three variants of
 * this test side by side.
 *
 * Unlike the other members, the cutscene is opened INSIDE the else arm rather
 * than before the test, so nothing is live across a call and no local is needed
 * for the facing.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_937_20081fc(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    if ((u16)(f + 0x5fff) <= 0x3ffe) {
        __UI_Sanctum(8);
    } else {
        __CutsceneStart();
        __MessageID(0x1a8f);
        __ActorMessage(8, 0);
        __CutsceneEnd();
    }
}
