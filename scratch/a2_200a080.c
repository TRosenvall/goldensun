extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200a080(void)
{
    int a;
    int w;
    int b;
    int c;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0xd) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    if (a == 0x24) {
        if (c == 0x22) {
            OvlFunc_946_2009774(0xd, -0x10, 0);
        } else if (b == 7) {
            OvlFunc_946_2009774(0xd, -0x20, 0);
        } else if (c == 0x1e) {
            OvlFunc_946_2009774(0xd, -0x50, 0);
        } else {
            OvlFunc_946_2009774(0xd, -0x60, 0);
            OvlFunc_946_2009774(0xd, -0x50, 0);
        }
    } else if (a == 0x23) {
        if (c == 0x22)
            return;
        if (b == 7)
            OvlFunc_946_2009774(0xd, -0x10, 0);
        else if (c == 0x1e)
            OvlFunc_946_2009774(0xd, -0x40, 0);
        else {
            OvlFunc_946_2009774(0xd, -0x50, 0);
            OvlFunc_946_2009774(0xd, -0x50, 0);
        }
    } else if (a == 0x22) {
        if (b == 7)
            return;
        if (c == 0x1e)
            OvlFunc_946_2009774(0xd, -0x30, 0);
        else
            OvlFunc_946_2009774(0xd, -0x90, 0);
    } else if (a == 0x1f) {
        if (c == 0x1e)
            return;
        OvlFunc_946_2009774(0xd, -0x60, 0);
    } else if (a == 0x19) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
