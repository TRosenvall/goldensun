/* Func_809a44c @ 0x0809a44c -- asm/rom_8a000/rom_9a44c_a_a.s
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
 * Worth noting what this function establishes: +0x30 and +0x34 are added to
 * the ROTATIONS at +0x18 and +0x1C, not to the position. entity.h flags those
 * two fields as read two ways in the annotations -- as movement tuning and as
 * a scale pair -- and this is a third reading. They are named maxSpeed/accel
 * there; here they are plainly angular velocity.
 */
#include "entity.h"

struct DrawActor {
    u8 pad_00[0x1e];
    u16 angle;
};

/* Per-frame hook that integrates stored velocities: a straight-line move that
 * bypasses the seek logic entirely.
 */
void Func_809a44c(Entity *entity)
{
    entity->x += entity->velX;
    entity->y += entity->velY;
    entity->z += entity->velZ;
    entity->rotX += entity->maxSpeed;
    entity->rotY += entity->accel;
    ((struct DrawActor *)entity->actor)->angle += entity->goalFacing;
}
