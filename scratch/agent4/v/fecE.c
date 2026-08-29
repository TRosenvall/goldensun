struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x24 - 0x18];
    int f24;
    int f28;
    int f2c;
    unsigned char pad30[8];
    int f38;
    int f3c;
    int f40;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __WaitFrames(int n);
extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_883_2008fec(void)
{
    struct A *a;
    struct A *b;
    struct A *p;
    int v;
    int z;
    int k2;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(5);
    __CutsceneStart();
    b->f8 = a->f8;
    b->fc = a->fc;
    b->f10 = a->f10;
    v = 0x80 << 24;
    b->f38 = v;
    b->f3c = v;
    b->f40 = v;
    z = 0;
    b->f24 = z;
    b->f28 = z;
    b->f2c = z;
    b->f14 = a->fc;
    __WaitFrames(1);
    __MapActor_SetSpeed(5, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(5, 0x6e, 0x11b);
    __Func_8092848(0, 5, 2);
    k2 = 0x12f;
    __MessageID(0xf39);
    if (a->f8 < b->f8)
        __Func_8093040(0xa005, 0, 2);
    else
        __Func_8093040(0x8005, 0, 2);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(2);
    __MapActor_SetAnim(5, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(5, *(short *)((char *)p + 0xa), *(short *)((char *)p + 0x12));
    __MapActor_WaitMovement(5);
    __MapActor_SetPos(5, 0, 0);
    __Func_80921c4(0, 0x6e, k2);
    __CutsceneEnd();
}
