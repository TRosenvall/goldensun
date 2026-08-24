/* Func_809a44c @ 0x0809a44c -- asm/rom_8a000/rom_9a44c_a_a.s
 *
 * Source asm: goldensun/asm/rom_8a000/rom_9a44c_a_a_a.s
 *
 * Blocker class 5, SCHEDULING (see docs/elevation.md). 26 of 27 instructions
 * are identical; the actor load is hoisted one instruction:
 *
 *     rom    add r3, r2 / str r3, [r0, #0x1c] / ldr r1, [r0, #0x50]
 *     ours   ldr r1, [r0, #0x50] / add r3, r2 / str r3, [r0, #0x1c]
 *
 * Tried: the actor pointer as a local declared at the top; as a local in a
 * nested block at the point of use; and inlined into the expression with no
 * local at all. All three hoist it identically.
 *
 * BOTH SCHEDULER FLAGS WERE TRIED AND BOTH FAIL, DIFFERENTLY -- and that
 * corrects a claim made elsewhere in this tree.
 *
 * The Makefile comment on the O1_CFLAGS rules says those TUs match at -O1
 * "(equivalently -O2 -fno-schedule-insns2)". For this function the two are NOT
 * equivalent and neither one matches:
 *
 *   -O2                        26 of 27. Only the tail is wrong: the sprite
 *                              load is hoisted across the rotY store.
 *   -O2 -fno-schedule-insns2   the TAIL IS FIXED -- the load falls back below
 *                              the store, exactly as the ROM has it -- but four
 *                              earlier pairs now load in the wrong order. The
 *                              ROM reads the destination first in the FIRST
 *                              `x += y` pair and the addend first in the other
 *                              four, which is the scheduler doing something the
 *                              flag switches off wholesale.
 *   -O1                        diverges at instruction 4 of 27, worse than
 *                              either, because -O1 also changes register
 *                              allocation and expression ordering rather than
 *                              only the post-reload scheduler.
 *
 * So the ROM was built with the scheduler ON, and what is needed is a source
 * form that makes gcc keep that one load below that one store. No flag
 * substitutes for it. tools/tryc.py now takes --no-sched2 so the middle option
 * can be checked without hand-compiling.
 *
 * WORTH SEVEN FUNCTIONS, NOT ONE. This body appears verbatim in seven places --
 * six overlays plus this main-ROM copy -- so whatever form fixes it elevates
 * all seven. See tools/find_twins.py.
 *
 * Worth noting what this function establishes: +0x30 and +0x34 are added to
 * the ROTATIONS at +0x18 and +0x1C, not to the position. actor.h flags those
 * two fields as read two ways in the annotations -- as movement tuning and as
 * a scale pair -- and this is a third reading. They are named maxSpeed/accel
 * there; here they are plainly angular velocity.
 *
 * RE-ATTEMPTED, batch 41, from tools/rank_parks.py. Still 2 of 27.
 *
 * THIS IS A TWIN PAIR AND THE DIFF IS IDENTICAL in both members --
 * src/non_matching/ovl_7b4558/2008ab0.c (OvlFunc_927_2008ab0) and
 * src/non_matching/rom_8a000/rom_9a44c.c (Func_809a44c), one an overlay copy of
 * the other. Solving either solves both, which is the reason to spend on it.
 *
 *     rom    ... last add-assign ... / ldr r1,[r0,#0x50] / add r0,#0x64
 *     ours   ... ldr r1,[r0,#0x50] hoisted TWO INSTRUCTIONS EARLIER, into the
 *            middle of the last add-assign group ...
 *
 * gcc schedules the sprite load above a store, which means it proved the two
 * cannot alias. Everything tried was aimed at that:
 *
 *   the goalFacing read as an explicit pointer walk rather than a field   2
 *   the sprite read as *(struct Spr **)((unsigned char *)a + 0x50)        2
 *   the same with the address in its own local first                      2
 *   -fno-schedule-insns, -fno-gcse, -fno-strength-reduce, --no-rerun-cse  2
 *   -fno-schedule-insns2                                                  8 (worse)
 *   -O1                                                                   8 (worse)
 *
 * Note that the two flags which DO move it move it the wrong way, which says
 * the post-reload scheduler is not what places this load -- it is placed
 * earlier and the scheduler is what puts it back. A next attempt should look at
 * why gcc believes the load cannot alias the stores, rather than at scheduling.
 * The compiler source is in the build image; see docs/elevation.md.
 */
#include "actor.h"

struct DrawActor {
    u8 pad_00[0x1e];
    u16 angle;
};

/* Per-frame hook that integrates stored velocities: a straight-line move that
 * bypasses the seek logic entirely.
 */
void Func_809a44c(Actor *actor)
{
    actor->pos.x += actor->velX;
    actor->pos.y += actor->velY;
    actor->pos.z += actor->velZ;
    actor->rotX += actor->speed;
    actor->rotY += actor->accel;
    ((struct DrawActor *)actor->sprite)->angle += actor->goalFacing;
}
