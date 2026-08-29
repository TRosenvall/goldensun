/* Cluster ActorCmd_CmpAttr..ActorCmd_CmpAttr extracted from goldensun/asm/rom_9000/rom_e220_c_c.s.
 *
 * The remaining part of that .s held only this function and no data.
 *
 * Script opcode: compare an actor attribute. Identical to ActorCmd_SetAttr but
 * for the operation passed to the accessor -- see
 * src/rom_9000/rom_e220_c_c_b.c, which records that one table serves all three.
 */
#include "actor.h"

typedef void (*AttrAccessor)(Actor *actor, s32 op, s32 value);

extern AttrAccessor Data_80136e0[];

s32 ActorCmd_CmpAttr(Actor *actor)
{
    s16 cursor = (s16)actor->scriptPos;
    s32 *operand = &((s32 *)actor->script)[cursor + 1];
    AttrAccessor fn = Data_80136e0[operand[0]];

    if (fn != 0)
        fn(actor, 2, operand[1]);
    actor->scriptPos += 3;
    return 1;
}
