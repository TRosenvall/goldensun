struct A { unsigned char pad00[0x10]; int f10; };

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void OvlFunc_954_200833c(int a, int b, int c);

void OvlFunc_954_200842c(void)
{
    unsigned char *g;
    struct A *a;
    int t;
    int y;
    int dir;
    int e;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    t = a->f10;
    y = t >> 20;
    dir = -0x30;
    if (y <= 8)
        dir = 0x30;
    e = 0x40;
    __Func_8010704(0x43, 8, 3, 1, e, y);
    OvlFunc_954_200833c(0x11, 0, dir);
    __Func_8010704(0x40, 0x18, 3, 1, e, __MapActor_GetActor(0x11)->f10 >> 20);
}
