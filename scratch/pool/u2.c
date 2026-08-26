struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __StartTask(void *fn, int prio);
extern void OvlFunc_928_2008324(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int slot, int n);

void OvlFunc_928_2008968(void)
{
    unsigned char *p;
    unsigned char m;
    int x, z;

    __CutsceneStart();
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x23;
    m = 0xfd;
    *p = *p & m;
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x55;
    *p = 0;
    x = ((struct A *)__MapActor_GetActor(0x14))->f8 >> 20;
    z = ((struct A *)__MapActor_GetActor(0x14))->f10 >> 20;
    __Func_8010704(3, 0x11, 1, 1, x, z);
    __StartTask(OvlFunc_928_2008324, 0xc8 << 4);
    __SetFlag(0x201);
    __Func_8092b08(0x14, 2);
    __CutsceneEnd();
}
