/* Func_ca58 -- ScriptOp_SetDrawKindSingle
 *
 * Entity script opcode.  Sets the draw kind to 1 so Func_c880 submits the single actor
 * at +0x50, then advances the script cursor past this one-byte opcode.
 * Returning 1 keeps the VM running on the same entity this frame.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_ca58 rom_9000/src/f_3_rom_ca58.c \
 *         --rom-offset 0xca58 --rom-size 0x14
 *
 * The two register pins are matching aids.  The original keeps the drawKind
 * address in r2 across the store and reuses r3 as a scratch for both the
 * constant and the cursor; agbcc picks r1 for the cursor without the pin.
 */

#include "types.h"
#include "entity.h"

int Func_ca58(Entity *e)
{
    u8 *kind;
    int t;

    kind = &e->drawKind;
    t = 1;
    *kind = t;

    t = e->scriptCursor;
    t += 1;
    e->scriptCursor = t;

    return 1;
}
