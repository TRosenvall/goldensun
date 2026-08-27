extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern int __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern int __Func_809228c(int a, int b, int c);

void OvlFunc_948_2009838(void)
{
    char *a;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x1b333, 0xd999);
    __MapActor_SetSpeed(0xc, 0x1b333, 0xd999);
    __PlaySound(0xbc);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(0xc, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(0xc);
    __Func_809228c(0, 0, 0x18);
    __PlaySound(0xbc);
    __Func_809228c(0xc, 0, 0x10);
    __MapActor_WaitMovement(0);
    __MapActor_TravelTo(0xc, 0x9c << 1, 0xe8);
    __MapActor_WaitMovement(0xc);
    __CutsceneEnd();
    __ClearFlag(0x88 << 2);
}
