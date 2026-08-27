struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
};

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct A *a);
extern void __MapActor_Surprise(int slot, int n);
extern void __Func_8092708(int a, int b, int c);

void OvlFunc_956_2008b30(void)
{
    unsigned char *g;
    int *slot;
    struct A *a;
    int lim;
    int add;
    int mask;

    g = gState;
    slot = (int *)(g + (0xfa << 1));
    a = __MapActor_GetActor(*slot);
    lim = 0xa6 << 18;
    add = 0xc0 << 12;
    mask = 0xfff00000;
    if (a->f8 > lim)
        a->f8 = lim;
    a->f34 = 0x80 << 9;
    a->f30 = 0x80 << 10;
    __Actor_SetAnim(a, 5);
    __Actor_TravelTo(a, a->f8, a->fc, (a->f10 & mask) + add);
    __Actor_WaitMovement(a);
    __MapActor_Surprise(*slot, 0x81 << 1);
    __Func_8092708(*slot, 6, 0);
}
