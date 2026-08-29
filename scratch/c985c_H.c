struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200985c(int slot, int a, int b)
{
    struct A *p;
    unsigned char *q;
    int x;
    int y;

    p = __MapActor_GetActor(slot);
    if (p != 0) {
        __Func_8092b08(slot, 3);
        q = &p->f23;
        *q |= 2;
        y = p->f10 >> 20;
        x = (p->f8 >> 20) - 1;
        __Func_8010704(a, b, 3, 1, x, y);
    }
}

void OvlFunc_946_20098b0(int slot, int a, int b)
{
    struct A *p;
    unsigned char *q;
    int x;
    int y;

    p = __MapActor_GetActor(slot);
    if (p != 0) {
        __Func_8092b08(slot, 3);
        q = &p->f23;
        *q |= 2;
        x = p->f8 >> 20;
        y = (p->f10 >> 20) - 1;
        __Func_8010704(a, b, 1, 3, x, y);
    }
}
