#include "gba/types.h"
#include "actor.h"

extern void __CutsceneStart(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneEnd(void);

void OvlFunc_936_2008504(void)
{
	int n;

	__CutsceneStart();
	__PlaySound(0xbc);
	n = 2;
	__CopyMapTiles(0x24, 0x17, 0x2b, 0xc, n, n);
	__WaitFrames(5);
	__CopyMapTiles(0x27, 0x17, 0x2b, 0xc, n, n);
	__WaitFrames(5);
	__MapActor_SetSpeed(0, 0x8000, 0x4000);
	__MapActor_GetActor(0)->interactFlag = 0;
	__MapActor_SetAnim(0, 2);
	__Func_809228c(0, 0, -8);
	__CutsceneWait(0xa);
	__Func_8091e9c(2);
	__MapTransitionOut();
	__WaitMapTransition();
	__CutsceneEnd();
}
