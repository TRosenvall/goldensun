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
    int x, zz;
    int zero;

    zero = 0;
    __CutsceneStart();
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x23;
    *p &= 0xfd;
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x55;
    *p = zero;
    x = ((struct A *)__MapActor_GetActor(0x14))->f8 >> 20;
    zz = ((struct A *)__MapActor_GetActor(0x14))->f10 >> 20;
    __Func_8010704(3, 0x11, 1, 1, x, zz);
    __StartTask(OvlFunc_928_2008324, 0xc8 << 4);
    __SetFlag(0x201);
    __Func_8092b08(0x14, 2);
    __CutsceneEnd();
}
