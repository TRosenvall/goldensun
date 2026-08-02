/* Entity script VM: the two conditional-jump opcodes.
 *
 * Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_c.s -- it holds both
 * of these and nothing else, so the ROM layout is preserved without splitting
 * the translation unit.
 */
#include "entity.h"

extern u16 Actor_FindScriptMarker(Entity *entity, void *label);

/* Jumps to the label operand when the condition byte at +0x57 is set,
 * otherwise steps past the opcode and its operand. The condition is whatever
 * the preceding event-flag opcode left there. Always returns 1, so the VM
 * keeps running this frame either way.
 */
s32 ActorCmd_GotoIfNZ(Entity *entity)
{
    void *label = ((void **)entity->script)[(s16)entity->scriptCursor + 1];

    if (entity->unk_57 != 0)
        entity->scriptCursor = Actor_FindScriptMarker(entity, label);
    else
        entity->scriptCursor += 2;
    return 1;
}

/* The inverse of ActorCmd_GotoIfNZ: jumps when the condition byte is clear.
 *
 * Written with the jump as the `if` body rather than the `else` in both, which
 * is what puts the call in the fall-through path the way the ROM has it.
 */
s32 ActorCmd_GotoIfZ(Entity *entity)
{
    void *label = ((void **)entity->script)[(s16)entity->scriptCursor + 1];

    if (entity->unk_57 == 0)
        entity->scriptCursor = Actor_FindScriptMarker(entity, label);
    else
        entity->scriptCursor += 2;
    return 1;
}
