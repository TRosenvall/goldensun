extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a3c4(void)
{
    int a;
    int b;
    int d;
    int v;
    int t;

    a = *(int *)(__MapActor_GetActor(0x11) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    d = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    if (b == 0x13) {
        if ((unsigned int)(d - 3) <= 2)
            v = 0x10;
        else
            v = 0x40;
    } else if (b == 0x12) {
        if ((unsigned int)(d - 3) <= 2)
            return;
        v = 0x30;
    } else {
        if (b == 0xf)
            return;
        goto tail;
    }
    OvlFunc_946_2009774(0x11, 0, -v);
tail:
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    a -= 1;
    __Func_8010704(a, b, 3, 1, a, t);
    __Func_8010704(0, 0, 3, 1, a, b);
}
