/* Cluster OvlFunc_940_2008224..OvlFunc_940_2008224 extracted from goldensun/asm/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7c5974/overlay.ld is
 * unchanged.
 *
 * Near-twin of src/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_a.c two files over
 * in the same overlay: the same facing range check, the same save-flag branch,
 * the same two-line dialogue split. Four constants differ -- the actor id, the
 * two message ids, and what the in-range case calls -- and nothing else does.
 *
 * Both levers carry over unchanged from the twin:
 *
 *  * `f - 0xa001 <= 0x3ffe` on a u32 for the folded range test, with no cast,
 *    so no shift is emitted. (Contrast
 *    src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_a.c, where the original DID
 *    narrow and the ROM shows a `lsl #16` against a pre-shifted constant.)
 *  * `extern void __ActorMessage(int, int);` -- one declaration for the one
 *    call whose r0 lands in the wrong position, with everything else left
 *    implicit.
 *
 * Matched on the first screen. Worth noting that the twin took two attempts
 * only because the __ActorMessage declaration had to be found; with it already
 * known this one was a straight transcription, which is the usual shape of a
 * second family member.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_940_2008224(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    if (f - 0xa001 <= 0x3ffe) {
        __Func_80b0278(0x19, 0x10);
    } else if (__GetFlag(0x941) != 0) {
        __CutsceneStart();
        __MessageID(0x24f9);
        __ActorMessage(0x10, 0);
        __CutsceneEnd();
    } else {
        __CutsceneStart();
        __MessageID(0x1bcf);
        __ActorMessage(0x10, 0);
        __CutsceneEnd();
    }
}
