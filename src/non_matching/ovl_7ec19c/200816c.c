/* OvlFunc_962_200816c  [ovl_7ec19c]  and its twin OvlFunc_967_2008234 [ovl_7f21b8]
 *
 * Source asm: goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_c_a.s
 *
 * NOT SPLIT. Both splits were made, both functions screened CLEAN, both failed
 * `make compare`, and both were reverted.
 *
 * A sanctum attendant with a QUADRANT facing test rather than the range test
 * the six elevated members use:
 *
 *     (u16)((facing + 0x2000) & ~0x3fff) == 0xc000
 *
 * THE C BELOW IS INSTRUCTION-FOR-INSTRUCTION CORRECT. tryc reports OK on 39
 * against 39 for both members. Two levers were needed to get there and both are
 * worth keeping:
 *
 *   1. the mask is a NAMED `int` local -- as a literal it narrows to a halfword
 *      pool entry where the ROM has `ldr r2, =0xffffc000`;
 *   2. the mask is assigned AFTER the addition -- assigned before it, gcc loads
 *      it into r2 early and builds 0x2000 in r1, where the ROM builds 0x2000 in
 *      r2 and then REUSES r2 for the mask. Four positions apart.
 *
 * Blocker: LITERAL POOL PLACEMENT, and it is invisible to the screen.
 *
 * The ROM keeps its pool INSIDE the function body, behind a `.pool_aligned`
 * between the two message arms. gcc puts the pool after the epilogue in a
 * single-function translation unit. The instruction streams are identical; the
 * BYTES differ in 36 places, every one of them a `ldr rN, [pc, #imm]` or a
 * branch displacement -- the pool sits at a different distance.
 *
 * tools/tryc.py normalises pool loads to `ldr rD, =value` on both sides, which
 * is what makes the pool-tell class readable at all, and it is exactly why this
 * compares equal. THIS IS THE THIRD FALSE-POSITIVE CLASS in that tool, after
 * dropped label definitions (batch 20) and the keyhole listing (batch 24). It
 * now WARNS when the reference has an inline pool; 336 of 4730 overlay .s files
 * do, so the warning is specific rather than constant noise.
 *
 * WHAT WOULD FIX IT is unknown and is the same open question as
 * src/non_matching/rom_15000/rom_1c154.c: nothing tried there moved gcc's pool
 * placement, and adding or removing code around the function did not either.
 * Splitting one function into its own TU may simply destroy the context that
 * put the pool mid-body -- but that hypothesis was tested on rom_1c154.c and
 * refuted, so it is not that either.
 *
 * Both members are recorded here rather than in two files because the C is the
 * same apart from three ids: OvlFunc_967_2008234 uses 0x9a7, 0x28fc and 0x26f6.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_962_200816c(int slot)
{
    Actor *a;
    u32 f;
    int m;

    a = __MapActor_GetActor(0);
    f = a->facing;
    f += 0x80 << 6;
    m = ~0x3fff;
    if ((u16)(f & m) == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x96f)) {
        __MessageID(0x262c);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25d5);
        __ActorMessage(slot, 0);
    }
}
