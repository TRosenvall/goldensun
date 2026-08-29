/* Cluster OvlFunc_907_2008240..OvlFunc_907_2008240 extracted from goldensun/asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A conversation that bumps a counter the second time through. Built with
 * CSE_CFLAGS: the flag id 0x301 is read at the top and written at the bottom
 * with two calls between, which at -O2 costs a push, a pop and two moves --
 * 27 instructions against 25.
 *
 * THE SIBLING RELATIONSHIP IS WORTH NOTING. This is the same shape as
 * OvlFunc_909_200828c, elevated in batch 25 with the same flag, and the park
 * note on that one recorded this function by address as "same shape". The two
 * were parked together and are now elevated together.
 *
 * The counter increment builds its offset at runtime (`mov r3, #0xec /
 * lsl r3, #1 / add r2, r3`), so it is written as separate statements over a
 * byte pointer rather than as a folded index -- the statement-form lever from
 * GetEntrances. Folded, 0x1d8 becomes part of the addressing mode and the `add`
 * disappears.
 */
#include "gba/types.h"

extern u32 iwram_3001ebc;
extern void __ActorMessage(int actor, int b);

void OvlFunc_907_2008240(void)
{
    u8 *p;
    u32 off;

    __CutsceneStart();
    __MessageID(0x13ae);
    if (__GetFlag(0x301)) {
        p = (u8 *)iwram_3001ebc;
        off = 0xec;
        off <<= 1;
        p += off;
        *(u16 *)p = *(u16 *)p + 1;
    }
    __ActorMessage(9, 0);
    __SetFlag(0x301);
    __CutsceneEnd();
}
