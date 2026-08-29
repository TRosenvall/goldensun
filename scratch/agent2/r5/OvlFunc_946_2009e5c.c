struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009e5c(void)
{
    struct A *p;
    int x, y, n, t;

    p = __MapActor_GetActor(0xb);
    x = p->f8 >> 20;
    p = __MapActor_GetActor(0xb);
    y = p->f10 >> 20;
    if (x == 0x1e)
        return;
    if (x == 0x22) {
        p = __MapActor_GetActor(0xa);
        if ((p->f10 >> 20) == 0x12)
            return;
        n = 0x40;
        OvlFunc_946_2009774(0xb, -n, 0);
    } else if (x == 0x24) {
        p = __MapActor_GetActor(0xa);
        if ((p->f10 >> 20) == 0x12) {
            n = 0x20;
            OvlFunc_946_2009774(0xb, -n, 0);
        } else {
            OvlFunc_946_2009774(0xb, -0x60, 0);
        }
    }
    __WaitFrames(2);
    p = __MapActor_GetActor(0xb);
    t = p->f8 >> 20;
    y--;
    __Func_8010704(x, y, 1, 3, t, y);
    __Func_8010704(0, 0, 1, 3, x, y);
}
