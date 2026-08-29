/* Func_d8f4 -- ScriptOp_Skip
 *
 * Entity script opcode.  Advances the script cursor by two -- past this opcode
 * and the byte after it -- and returns 1 so the VM carries on.  The no-op with
 * an operand, in other words.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_d8f4 rom_9000/src/f_x3_rom_d8f4.c \
 *         --rom-offset 0xd8f4 --rom-size 0xc
 *
 * The r3 pin is a matching aid; agbcc picks r1 for the cursor without it and
 * is otherwise instruction-for-instruction identical.
 */

#include "types.h"
#include "entity.h"

int Func_d8f4(Entity *e)
{
    u16 cursor;

    cursor = e->scriptCursor;
    cursor += 2;
    e->scriptCursor = cursor;

    return 1;
}
