/* Actor script VM: the opcode that makes the script jump to a new base.
 *
 * Whole-file conversion of asm/rom_9000/rom_ca2c_a.s -- one function, so the
 * ROM layout is preserved without splitting the translation unit.
 */
#include "actor.h"

/* Script opcode handler, dispatched from Data_13624 by Func_a494.
 *
 * Folds the cursor at +0x04 into the script base at +0x00 -- base moves
 * forward by cursor*4 + 4 -- and resets the cursor, so the instruction after
 * the current one becomes the new script origin. Returning 1 keeps the VM
 * running this frame.
 *
 * The cursor is read SIGNED here (the ROM's `ldrsh`) even though it is only
 * ever a forward offset in practice.
 */
s32 ActorCmd_SetScript(Actor *actor)
{
    actor->script = (u8 *)actor->script + (s16)actor->scriptPos * 4 + 4;
    actor->scriptPos = 0;
    return 1;
}
