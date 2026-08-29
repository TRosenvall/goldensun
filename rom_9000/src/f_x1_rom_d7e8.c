/* Func_d7e8 -- point an entity's script at L13240
 *
 * STATUS: the CODE matches exactly. The only differing bytes are the literal
 * pool entry -- 0x08013244 where the ROM has 0x08013240 -- because L13240 has
 * been displaced 4 bytes by Func_b684 still compiling to the wrong size.
 * Expect this to resolve itself when b684 does; it is not a fault in this
 * function.
 */
#include "types.h"
#include "entity.h"

extern u8 L13240[];

int Func_d7e8(Entity *e)
{
    e->script = L13240;
    e->scriptCursor = 0;
    return 0;
}
