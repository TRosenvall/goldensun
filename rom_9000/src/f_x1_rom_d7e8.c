/* Func_d7e8 -- ScriptOp_SwitchToDefaultScript
 *
 * Entity script opcode.  Repoints the entity's VM at the default script
 * .L13240 and rewinds the cursor to the top of it, so execution continues
 * there rather than in whatever script was running.  Returning 0 stops the VM
 * for this entity this frame -- the new script starts on the next one.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_d7e8 rom_9000/src/f_x1_rom_d7e8.c \
 *         --rom-offset 0xd7e8 --rom-size 0x10
 *
 * The single r3 pin is a matching aid: the original reuses one register for
 * both the script address and the zero it writes to the cursor, which is why
 * `v` is assigned twice rather than written as two statements.
 */

#include "types.h"
#include "entity.h"

extern u8 L13240[];

int Func_d7e8(Entity *e)
{
    int v;

    v = (int)L13240;
    e->script = (void *)v;

    v = 0;
    e->scriptCursor = v;

    return 0;
}
