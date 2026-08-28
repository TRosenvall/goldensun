extern void OvlFunc_930_2009060(void);

struct Actor { unsigned char pad00[0x6c]; void *f6c; };

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void __Func_8092b08(int a, int b);

void OvlFunc_930_20090b8(void)
{
    unsigned char *p;
    int m;
    int z;
    int s;

    __CutsceneStart();
    z = 0;
    m = 0xfd;
    p = __MapActor_GetActor(0xe) + 0x23;
    *p = m & *p;
    p = __MapActor_GetActor(0xe) + 0x59;
    *p = m & *p;
    p = __MapActor_GetActor(0xe) + 0x55;
    *p = z;
    ((struct Actor *)__MapActor_GetActor(0xe))->f6c = OvlFunc_930_2009060;
    s = 0x12;
    __Func_8010704(0x37, 0x10, 1, 1, 0x38, s);
    __Func_8010704(0x37, 0x10, 1, 1, 0x14, s);
    __WaitFrames(1);
    __SetFlag(0x80 << 2);
    __Func_8092b08(0xe, 2);
    __CutsceneEnd();
}
