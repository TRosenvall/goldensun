#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(Actor *a, int n);
extern void __Func_8092158(int slot, int x, int z);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_927_2008d90(int slot, int x, int z, int w)
{
	Actor *a;

	a = __MapActor_GetActor(slot);
	__Func_8092b08(slot, 1);
	__MapActor_SetSpeed(slot, 0x30000, 0x18000);
	__PlaySound(0x98);
	a->motion.y = w;
	a->velY = 0x8000;
	a->velX = 0;
	__Actor_SetSpriteFlags(a, 0);
	__Func_8092158(slot, x, z);
	x <<= 16;
	z <<= 16;
	__MapActor_SetPos(slot, x, z);
	__Actor_SetSpriteFlags(a, 1);
	a->velY = 0x10000;
}
