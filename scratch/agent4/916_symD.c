extern int _CONST_3333A, _CONST_3333B;
extern unsigned short *L12c4 __asm__(".L12c4");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_916_20088b0(void)
{
    int nine;

    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(6);
    __PlaySound(0xef);
    { int s8 = 8; __MapActor_SetSpeed(s8, 0x80 << 8, (int)&_CONST_3333A); }
    __MapActor_SetAnim(8, 2);
    __MapActor_TravelTo(8, 0x68, 0xb0);
    __CutsceneWait(6);
    __MapActor_SetAnim(0, 2);
    { int s0 = 0; __MapActor_SetSpeed(s0, 0x4ccc, (int)&_CONST_3333B); }
    __Func_809228c(0, 8, 0);
    __CutsceneWait(0x18);
    __MapActor_SetAnim(0, 1);
    __MapActor_WaitMovement(8);
    __MapActor_SetAnim(8, 1);
    __PlaySound(0x90 << 1);
    __PlaySound(0xd5);
    nine = 9;
    __Func_8010704(5, 9, 1, 4, 4, nine);
    __Func_8010704(0, 0, 1, 4, 6, nine);
    *L12c4 = 0;
    __CutsceneEnd();
}
