extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_DoAnim(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_8092b08(int slot, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_2008214(void)
{
    unsigned char *p;
    int e5, e6;
    int two;

    __CutsceneStart();
    __MapActor_SetPos(8, 0, 0);
    __SetFlag(0x883);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xf, 2);
    p = (unsigned char *)__MapActor_GetActor(0xf) + 0x55;
    *p = 0;
    p = (unsigned char *)__MapActor_GetActor(0xf) + 0x23;
    two = 2;
    *p = two | *p;
    __Func_8092b08(0xf, 2);
    e5 = 0x12;
    e6 = 0xe;
    __Func_8010704(0, 0, 1, 1, e5, e6);
    __CutsceneEnd();
}
