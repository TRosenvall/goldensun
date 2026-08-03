/* Particles: move one along a decaying ballistic arc.
 *
 * Split out of asm/rom_8a000/rom_925e0_a_a_a.s; the _a and _c parts stay as
 * assembly and are listed around this one in stage1.ld, so the ROM layout is
 * unchanged.
 */
#include "actor.h"

/* Installed as the per-frame hook at +0x6C.
 *
 * This repurposes the movement-tuning pair at +0x30/+0x34 as its OWN x and z
 * velocity -- which is the clearest evidence yet for that reading of those two
 * fields (see the caution in actor.h). It writes the new position into BOTH
 * the position words and the target words, so the normal seek logic finds
 * nothing left to do and stays out of the way.
 *
 * Each frame y advances by 0x400 while x and z advance by their velocities;
 * then vx loses a eighteenth of itself and vz a sixteenth, giving the drift a
 * soft ease-out. The sixteenth is a signed shift, so it rounds toward zero and
 * a negative velocity decays at the same rate as a positive one.
 *
 * The z velocity is deliberately read AFTER the x position is stored rather
 * than with its partner at the top: hoisting it costs the match.
 */
void Func_80925e0(Actor *actor)
{
    s32 vx = actor->speed;
    s32 vz;
    s32 v;

    v = actor->pos.x + vx;
    actor->pos.x = v;
    actor->targetX = v;

    vz = actor->accel;
    v = actor->pos.z + vz;
    actor->pos.z = v;
    actor->targetZ = v;

    v = actor->pos.y + 0x400;
    actor->pos.y = v;
    actor->targetY = v;

    actor->speed = vx - vx / 18;
    actor->accel = vz - vz / 16;
}
