#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);
extern void _Actor_SetScript(struct Actor *a, void *script);

void Func_8092a1c(int slot, int packed, void *script)
{
    struct Actor *a;
    struct Actor *t;

    a = GetFieldActor(slot);
    t = GetFieldActor(packed & 0xff);
    if (a != 0 && t != 0) {
        a->unk_68 = (u32)t;
        if ((packed & (0x80 << 9)) == 0) {
            a->goalFacing = 0x28;
            a->accel = t->accel << 1;
            a->speed = t->speed;
            a->interactFlags = 0;
        }
        _Actor_SetScript(a, script);
    }
}
