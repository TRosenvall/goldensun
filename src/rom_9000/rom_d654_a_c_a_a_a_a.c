/* Actor script VM: the opcode that calls a native predicate.
 *
 * Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_a_a.s -- one
 * function, so the ROM layout is preserved without splitting the translation
 * unit.
 */
#include "actor.h"

typedef s32 (*ScriptNative)(Actor *actor);

/* Treats the operand after the opcode as a function pointer and calls it with
 * the actor.
 *
 * A non-zero result means "not finished": this returns 0 so the same opcode
 * runs again next frame. On zero the cursor advances past the operand -- but
 * only if the callee did not move the cursor itself, so a predicate is free to
 * jump and have that jump stick.
 */
s32 ActorCmd_CallNative(Actor *actor)
{
    s16 cursor = (s16)actor->scriptPos;
    ScriptNative fn = ((ScriptNative *)actor->script)[cursor + 1];

    if (fn(actor) != 0)
        return 0;
    if ((s16)actor->scriptPos == cursor)
        actor->scriptPos += 2;
    return 1;
}
