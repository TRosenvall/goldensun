struct Actor {
    unsigned char pad0[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
    unsigned char pad38[0x1d];
    unsigned char f55;
};

extern unsigned char gState[];
extern void __MapActor_SetAnim(int id, int anim);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __ClearFlag(int id);

void OvlFunc_956_2008404(void)
{
    unsigned char *g;
    struct Actor *a;
    int e1, f1;

    g = gState;
    __MapActor_SetAnim(*(int *)(g + 0x1f4), 1);
    a = __MapActor_GetActor(0xb);
    a->f55 = 0;
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x40000, a->f10);
    a = __MapActor_GetActor(0xa);
    a->f55 = 0;
    a->f34 = 0x6666;
    a->f30 = 0xcccc;
    __Actor_TravelTo(a, a->f8, 0x200000, a->f10);
    __MapActor_WaitMovement(0xa);
    e1 = 9;
    f1 = 0xc;
    __Func_8010704(0, 0x19, 1, 1, e1, f1);
    __WaitFrames(2);
    __ClearFlag(0x367);
}
