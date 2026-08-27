#include "gba/types.h"
#include "actor.h"

extern Actor *GetFieldActor(int slot);
extern void _Actor_SetScript(Actor *a, void *script);

void Func_8092a1c(int slot, int packed, void *script)
{
	Actor *a;
	Actor *t;

	a = GetFieldActor(slot);
	t = GetFieldActor(packed & 0xff);
	if (a != 0 && t != 0) {
		a->unk_68 = (u32)t;
		if ((packed & 0x10000) == 0) {
			a->goalFacing = 0x28;
			a->accel = t->accel * 2;
			a->speed = t->speed;
			a->interactFlags = 0;
		}
		_Actor_SetScript(a, script);
	}
}
