extern unsigned char gState[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_948_20095f0(void)
{
    unsigned char *g;
    unsigned char *e;

    g = gState;
    if (*(short *)(g + 0x24a) != 0xa) {
        __CutsceneStart();
        __MapActor_SetSpeed(0, 0x1b333, 0xd999);
        __MapActor_SetSpeed(0xa, 0x1b333, 0xd999);
        __PlaySound(0xbc);
        e = __MapActor_GetActor(0);
        if (e != 0)
            __MapActor_TravelTo(0xa, *(short *)(e + 0xa), *(short *)(e + 0x12));
        __MapActor_WaitMovement(0xa);
        __Func_809228c(0, 0, 0x18);
        __CutsceneWait(4);
        __PlaySound(0xbc);
        __Func_809228c(0xa, 0, 0x10);
        __MapActor_WaitMovement(0);
        __MapActor_TravelTo(0xa, 0x84 << 1, 0xb4 << 1);
        __MapActor_WaitMovement(0xa);
        __CutsceneWait(0xa);
        __CutsceneEnd();
    }
}
