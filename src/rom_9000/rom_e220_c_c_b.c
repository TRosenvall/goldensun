/* Cluster ActorCmd_SetAttr..ActorCmd_SetAttr extracted from goldensun/asm/rom_9000/rom_e220_c_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/stage1.ld.
 *
 * Script opcode: set an actor attribute. Reads a field id from script[cursor+1]
 * and a value from script[cursor+2], looks the id up in the accessor table at
 * Data_80136e0, and calls it with operation 0. A null table entry is skipped
 * silently. Advances the cursor by 3 and returns 1 either way.
 *
 * HEAD OF A 3-MEMBER FAMILY. Its two siblings in the same .s are identical
 * except for the operation they pass:
 *
 *     ActorCmd_SetAttr   op 0
 *     ActorCmd_IncAttr   op 1
 *     ActorCmd_CmpAttr   op 2
 *
 * so one accessor table serves set, increment and compare, and the opcode
 * chooses which. That is worth knowing before naming the table's entries --
 * they are not setters, they are three-way accessors.
 */
#include "actor.h"

typedef void (*AttrAccessor)(Actor *actor, s32 op, s32 value);

extern AttrAccessor Data_80136e0[];

s32 ActorCmd_SetAttr(Actor *actor)
{
    s16 cursor = (s16)actor->scriptPos;
    s32 *operand = &((s32 *)actor->script)[cursor + 1];
    AttrAccessor fn = Data_80136e0[operand[0]];

    if (fn != 0)
        fn(actor, 0, operand[1]);
    actor->scriptPos += 3;
    return 1;
}
