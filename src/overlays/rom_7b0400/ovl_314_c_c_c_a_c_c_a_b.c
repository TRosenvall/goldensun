extern char *iwram_3001e70;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__Func_8093554(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_925_200b324(void);
extern void __SetFlag(int id);

void OvlFunc_925_200addc(void)
{
    char *p;
    int z;
    int v;
    int s0, s1;

    p = iwram_3001e70 + (0xb2 << 1);
    __CutsceneStart();
    *(int *)(p + 0xc) = 0xe0 << 18;
    __Func_800fe9c();
    __WaitFrames(1);
    z = 0;
    __MapActor_GetActor(9)[0x55] = z;
    __MapActor_SetPos(9, 0xd0 << 15, 0x84 << 17);
    v = 0xffe00000;
    *(int *)(__MapActor_GetActor(9) + 0xc) = v;
    *(int *)(__MapActor_GetActor(9) + 0x3c) = v;
    __Func_8093554()[0x55] = z;
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x80 << 16, -1, 0xb8 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x1e);
    s0 = 5;
    s1 = 4;
    __CopyMapTiles(0x1d, 0x4a, 4, 0x4a, s0, s1);
    __Func_8092b08(0x11, 0);
    __Func_8092b08(0x12, 0);
    OvlFunc_925_200b324();
    __Func_8092b08(0x11, 1);
    __Func_8092b08(0x12, 1);
    __CutsceneWait(0x14);
    __SetFlag(0x251);
    __CutsceneEnd();
}
