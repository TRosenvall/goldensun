struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x6c - 0x14];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_964_2008ec8(void);

void OvlFunc_964_2009a10(void)
{
    unsigned char *p;
    unsigned char mask;

    __CutsceneStart();
    __Func_8092b08(9, 1);
    __MapActor_SetAnim(9, 1);
    __Func_8092950(9, 0);
    __MapActor_SetAnim(9, 2);
    p = (unsigned char *)__MapActor_GetActor(9) + 0x23;
    mask = 0xfd;
    *p = *p & mask;
    __SetFlag(0x81 << 2);
    __Func_8010704(0x1a, 8, 1, 1,
                   __MapActor_GetActor(9)->f8 >> 20,
                   __MapActor_GetActor(9)->f10 >> 20);
    __MapActor_GetActor(9)->f6c = OvlFunc_964_2008ec8;
    __MapActor_GetActor(8)->f6c = OvlFunc_964_2008ec8;
    __CutsceneEnd();
}
