struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
    unsigned char pad38[0x55 - 0x38];
    unsigned char f55;
};

extern void __SetFlag(int id);
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Func_8093530(void);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct A *a);
extern void __CutsceneWait(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneEnd(void);

void OvlFunc_954_2008270(void)
{
    struct A *a;
    int z;

    __SetFlag(0x301);
    a = __MapActor_GetActor(0xd);
    __CutsceneStart();
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    __Func_80933f8(0x96 << 18, -1, 0xc8 << 16, 1);
    __Actor_SetAnim(a, 3);
    __Func_8093530();
    z = 0;
    a->f55 = z;
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x80 << 12, a->f10);
    a = __MapActor_GetActor(0xe);
    a->f55 = z;
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x80 << 14, a->f10);
    __Actor_WaitMovement(a);
    __CutsceneWait(0x2d);
    __Func_8010704(0x2b, 0xc, 1, 1, 0x29, 0xc);
    __CutsceneEnd();
}
