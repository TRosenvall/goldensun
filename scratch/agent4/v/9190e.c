extern int L6668[] __asm__(".L6668");

struct A {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    unsigned char padc[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b, int c);
extern int OvlFunc_945_2009280(int d);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Func_809228c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void OvlFunc_945_200c880(int slot, int n);

void OvlFunc_945_2009190(int slot)
{
    struct A *a;
    struct A *b;
    struct A *p;
    int ok, mask, d, w, vy;
    unsigned char one;

    a = __MapActor_GetActor(0);
    ok = 1;
    __Func_8092b08(slot, 2, 1);
    b = __MapActor_GetActor(slot);
    one = 1;
    *((unsigned char *)b + 0x23) = one | *((unsigned char *)b + 0x23);
    mask = 0xf0 << 8;
    d = ((a->f6 + (0x80 << 7)) & mask) >> 12;
    if (OvlFunc_945_2009280(d) != 0)
        ok = 0;
    if (ok != 0) {
        d = ((a->f6 - 0x4000) & mask) >> 12;
        if (OvlFunc_945_2009280(d) != 0)
            ok = 0;
        if (ok != 0)
            d = ((a->f6 + (0x80 << 8)) & mask) >> 12;
    }
    vy = 0xcccc;
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(slot, p->f8, p->f10);
    __MapActor_SetSpeed(slot, 0x19999, vy);
    __MapActor_SetAnim(slot, 2);
    w = *(int *)((unsigned char *)L6668 + (d << 2));
    __Func_809228c(slot, w >> 16, (short)w);
    __MapActor_WaitMovement(slot);
    __MapActor_SetAnim(slot, 1);
    OvlFunc_945_200c880(slot, a->f6);
}
