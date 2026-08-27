#include "gba/types.h"
#include "actor.h"

extern void __Func_80929d8(Actor *a, int n);
extern void __Actor_SetSpriteFlags(Actor *a, int n);

void OvlFunc_921_2009704(Actor *a)
{
	u8 *s;

	a->interactFlag = 0;
	a->goalFacing = 0;
	a->flags &= 0xfe;
	s = (u8 *)a->sprite;
	s[9] = (s[9] & ~0xd) | 4;
	__Func_80929d8(a, 9);
	__Actor_SetSpriteFlags(a, 0);
	a->rotX = 0x8000;
	a->rotY = 0x8000;
}
