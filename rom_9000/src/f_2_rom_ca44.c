/* Func_ca44 -- ScriptOp_SetDrawKindNone
 *
 * Entity script opcode.  Clears the draw kind so Func_c880 skips the entity
 * entirely, then advances the script cursor past this one-byte opcode.
 * Returning 1 keeps the VM running on the same entity this frame.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_ca44 rom_9000/src/f_2_rom_ca44.c \
 *         --rom-offset 0xca44 --rom-size 0x14
 *
 * The two register pins are matching aids.  The original keeps the drawKind
 * address in r2 across the store and reuses r3 as a scratch for both the
 * constant and the cursor; agbcc picks r1 for the cursor without the pin.
 */

#include "types.h"
#include "entity.h"

int Func_ca44(Entity *e)
{
    register u8 *kind asm("r2");
    register int t asm("r3");

    kind = &e->drawKind;
    t = 0;
    *kind = t;

    t = e->scriptCursor;
    t += 1;
    e->scriptCursor = t;

    return 1;
}
