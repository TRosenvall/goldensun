/* Func_8092504 @ 0x08092504 -- asm/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_c_c.s
 *
 * A NEW failure mode: the ROM SPILLS a value to the stack that gcc keeps in a
 * register, so the ROM is five instructions LONGER than anything produced
 * here (31 vs 26).
 *
 *     rom    sub sp, #4 ... mov r7, sp / str r3, [r7] ... ldr r3, [r7]
 *     ours   the sampled byte stays in a register for the whole loop
 *
 * Every other blocker so far has been a difference in choice between two
 * equally-sized codegens. This one needs gcc to run OUT of registers where it
 * currently does not, which no rewriting of the C body will cause -- the
 * function simply is not under enough register pressure.
 *
 * Most likely the original had another live value here that this
 * reconstruction is missing, which would mean the reading below is incomplete
 * rather than merely unlucky. Worth revisiting with fresh eyes on what else
 * the loop might have been tracking.
 */
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void WaitFrames(s32 n);

/* Samples the sprite's current-animation byte and blocks until it changes,
 * giving up after 90 frames. Distinct from waiting for a SPECIFIC animation to
 * end: this waits for whatever is playing now to be replaced by anything.
 */
void Func_8092504(s32 slot)
{
    Actor *actor = GetFieldActor(slot);
    u8 *anim;
    s32 initial;
    s32 frames;

    if (actor == NULL)
        return;
    if (actor->drawKind != 1)
        return;

    anim = (u8 *)actor->sprite + 0x24;
    initial = *anim;
    for (frames = 0; frames <= 0x59; frames++) {
        WaitFrames(1);
        if (*anim != initial)
            break;
    }
}
