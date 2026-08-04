/* OvlFunc_924_2008ffc  [ovl_7ac2d8]
 *
 * Source asm: goldensun/asm/overlays/rom_7ac2d8/ovl_f84_a_a_c.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data.
 *
 * A cutscene beat: set speed, start a walk, hide the sprite, wait, reposition,
 * wait again. Thirty-six instructions against thirty-six, first five identical.
 *
 * Blocker: ARG-INTERLEAVE, on __MapActor_SetSpeed:
 *
 *     rom    ldr r2, =0x3333 / mov r0, #0x0  / ldr r1, =0x6666
 *     ours   ldr r2, =0x3333 / ldr r1, =0x6666 / mov r0, #0x0
 *
 * CORRECTED IN BATCH 26: the conclusion below is too strong. A middle-position
 * r0 IS sometimes reachable by declaring the callee -- see
 * src/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_b.c, where __Func_809228c has
 * the identical shape (`mov r2 / mov r0 / mov r1`) and one extern fixes it.
 * So `r0 not at either end` is not by itself a blocker. What separates that
 * case from this one is open; the difference on the surface is that the other
 * two arguments here come from POOL LOADS rather than plain movs, which is an
 * observation and not yet an explanation.
 *
 * THIS WIDENS THE CLASS. The two members already parked
 * (src/non_matching/ovl_794ac0/2008428.c and
 * src/non_matching/ovl_7f148c/2008078.c) both had r0 landing inside a single
 * argument's two-instruction construction, `mov rN, #imm ... lsl rN`. Here
 * there is no shift at all -- r0 sits between two INDEPENDENT pooled argument
 * loads. So the class is about r0's position within the argument block, not
 * about any particular instruction pair.
 *
 * TRIED, both byte-identical to the form below:
 *   1. `extern void __MapActor_SetSpeed(int, fx32, fx32);` -- declaring the
 *      mismatching callee, which is the lever that fixed four functions in the
 *      last two batches
 *   2. `extern void __PlaySound(int);` -- declaring the PRECEDING call so r0 is
 *      not held live across it
 *
 * THE FILTER WAS NOT WIDENED TO MATCH, deliberately. tools/pick_candidates.py
 * rejects the narrow `mov/lsl` shape only. Generalising it to "r0 written with
 * another argument register written both before and after" was implemented and
 * REJECTED THREE FUNCTIONS THAT ACTUALLY MATCHED -- OvlFunc_940_2008224,
 * OvlFunc_936_20083d8 and OvlFunc_922_20085b8 -- because r0-r3 are ordinary
 * scratch registers too and nothing in the instruction text separates argument
 * setup from a range check that happens to use r2 and r3.
 *
 * A filter that rejects good candidates is worse than one that lets bad ones
 * through: a bad candidate costs one screen, a good one that is never listed is
 * never seen again. So this shape stays undetected and costs a screen when it
 * comes up. The tell, once compiled, is r0 anywhere other than the front or the
 * back of the argument block.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_924_2008ffc(int x)
{
    fx32 v;

    __CutsceneStart();
    __PlaySound(0xe4);
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __Func_8092b08(0, 2);
    __Func_809228c(0, 0, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __CutsceneWait(8);
    v = (x << 19) + (0x80 << 12);
    __MapActor_SetPos(0, v, 0);
    __CutsceneWait(0x1e);
}
