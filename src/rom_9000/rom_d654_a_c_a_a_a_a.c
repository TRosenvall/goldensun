/* Entity script VM: the opcode that calls a native predicate.
 *
 * Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_a_a.s -- one
 * function, so the ROM layout is preserved without splitting the translation
 * unit.
 */
#include "entity.h"

typedef s32 (*ScriptNative)(Entity *entity);

/* Treats the operand after the opcode as a function pointer and calls it with
 * the entity.
 *
 * A non-zero result means "not finished": this returns 0 so the same opcode
 * runs again next frame. On zero the cursor advances past the operand -- but
 * only if the callee did not move the cursor itself, so a predicate is free to
 * jump and have that jump stick.
 */
s32 ActorCmd_CallNative(Entity *entity)
{
    s16 cursor = (s16)entity->scriptCursor;
    ScriptNative fn = ((ScriptNative *)entity->script)[cursor + 1];

    if (fn(entity) != 0)
        return 0;
    if ((s16)entity->scriptCursor == cursor)
        entity->scriptCursor += 2;
    return 1;
}
