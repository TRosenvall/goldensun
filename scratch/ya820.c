struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_965_200a820(void)
{
    struct Actor *a;
    struct Actor *b;
    int s;
    int x1, y1, x2, y2;

    a = __MapActor_GetActor(8);
    __Func_8092b08(8, 1);
    __Func_8092b08(9, 1);
    s = 0x13;
    __Func_8010704(0x45, 0x13, 3, 3, 5, s);
    __Func_8010704(0x45, 0x13, 3, 3, 0x11, s);
    x1 = a->f8 >> 20;
    y1 = a->f10 >> 20;
    __Func_8010704(3, 3, 1, 1, x1, y1);
    b = __MapActor_GetActor(9);
    x2 = b->f8 >> 20;
    y2 = b->f10 >> 20;
    __Func_8010704(3, 3, 1, 1, x2, y2);
}
