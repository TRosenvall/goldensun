#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_898_200973c(int slot, int a, int b);
extern void OvlFunc_898_2009724(int slot, int n);
extern void __WaitFrames(int n);
extern void __CutsceneEnd(void);

void OvlFunc_898_200885c(void)
{
	Actor *a;
	u16 *g;
	int f;

	a = __MapActor_GetActor(0xf);
	f = *(short *)&a->facing;
	g = &a->goalFacing;
	*g |= 2;
	__CutsceneStart();
	__MessageID(0x122d);
	__MapActor_SetAnim(0xf, 0);
	OvlFunc_898_200973c(0xf, 0, 2);
	OvlFunc_898_2009724(0xf, 0xa);
	a->facing = f;
	__WaitFrames(1);
	__CutsceneEnd();
	*g &= 1;
}
