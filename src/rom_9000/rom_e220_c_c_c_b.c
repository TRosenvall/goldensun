/* Cluster ActorCmd_IncAttr..ActorCmd_IncAttr extracted from goldensun/asm/rom_9000/rom_e220_c_c.s.
 *
 * Split out of that .s; the sibling parts stay as assembly.
 *
 * Script opcode: increment an actor attribute. Identical to ActorCmd_SetAttr but
 * for the operation passed to the accessor -- see
 * src/rom_9000/rom_e220_c_c_b.c, which records that one table serves all
 * three.
 */
#include "actor.h"

typedef void (*AttrAccessor)(Actor *actor, s32 op, s32 value);

extern AttrAccessor Data_80136e0[];

s32 ActorCmd_IncAttr(Actor *actor)
{
    s16 cursor = (s16)actor->scriptPos;
    s32 *operand = &((s32 *)actor->script)[cursor + 1];
    AttrAccessor fn = Data_80136e0[operand[0]];

    if (fn != 0)
        fn(actor, 1, operand[1]);
    actor->scriptPos += 3;
    return 1;
}
