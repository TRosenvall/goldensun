extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009d2c(void)
{
    int a;
    int b;
    int c;
    int d;
    int v;
    int s;
    int t;

    a = *(int *)(__MapActor_GetActor(0xa) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    d = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    if (b == 0x12) {
        if ((unsigned int)(d - 0x1f) <= 2) {
            v = 0x80;
        } else if ((unsigned int)(c - 0x1f) <= 2) {
            v = 0x80;
        } else {
            OvlFunc_946_2009774(0xa, 0, -0x70);
            v = 0x40;
        }
    } else if (b == 0xa) {
        if ((unsigned int)(d - 0x1f) <= 2)
            return;
        if ((unsigned int)(c - 0x1f) <= 2)
            return;
        v = 0x30;
    } else {
        if (b == 7)
            return;
        goto tail;
    }
    OvlFunc_946_2009774(0xa, 0, -v);
tail:
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    s = a - 1;
    __Func_8010704(s, b, 3, 1, s, t);
    __Func_8010704(0, 0, 3, 1, s, b);
}
