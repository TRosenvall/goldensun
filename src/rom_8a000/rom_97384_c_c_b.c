/* Movement: restart the idle script once an actor has stopped moving.
 *
 * Split out of asm/rom_8a000/rom_97384_c_c.s. The neighbours are the _a and
 * _c files beside it; the script data stays in _c, which now exports
 * `.La0128` so this can reference it. `.global` emits no bytes, and the
 * sibling `.La0108` was already exported the same way.
 */
#include "actor.h"

extern u8 Data_80a0128[] __asm__(".La0128");
extern void _Actor_SetScript(Actor *actor, void *script);

/* Runs only when all three move targets still hold the no-target sentinel --
 * that is, nothing is in flight on any axis.
 *
 * The ROM tests the three against each other rather than against the constant
 * three times, which is gcc's own CSE and needs no help: writing the
 * comparison out three times produces it.
 */
void Func_8097a54(Actor *actor)
{
    if (actor->targetX == (fx32)ACTOR_NO_TARGET
        && actor->targetY == (fx32)ACTOR_NO_TARGET
        && actor->targetZ == (fx32)ACTOR_NO_TARGET)
        _Actor_SetScript(actor, Data_80a0128);
}
