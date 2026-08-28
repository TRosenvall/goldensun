extern unsigned char *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __Func_8010704(int sx, int sy, int w, int h, int dx, int dy);

void OvlFunc_946_200aa98(void)
{
    int a;
    int b;
    int c;
    int d;
    int x;

    a = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0x13) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    d = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    if (a == 3) {
        if (c == 0xf) {
            OvlFunc_946_2009774(0x13, 0x20, 0);
        } else if (d == 0xf) {
            OvlFunc_946_2009774(0x13, 0x50, 0);
        } else {
            OvlFunc_946_2009774(0x13, 0x70, 0);
            OvlFunc_946_2009774(0x13, 0x30, 0);
        }
    } else if (a == 5) {
        if (c == 0xf)
            return;
        if (d == 0xf)
            OvlFunc_946_2009774(0x13, 0x30, 0);
        else
            OvlFunc_946_2009774(0x13, 0x80, 0);
    } else if (a == 6) {
        if (d == 0xf)
            OvlFunc_946_2009774(0x13, 0x20, 0);
        else
            OvlFunc_946_2009774(0x13, 0x70, 0);
    } else if (a == 8) {
        if (d == 0xf)
            return;
        OvlFunc_946_2009774(0x13, 0x50, 0);
    } else if (a == 9) {
        OvlFunc_946_2009774(0x13, 0x40, 0);
    } else if (a == 0xc) {
        OvlFunc_946_2009774(0x13, 0x10, 0);
    }
    __WaitFrames(2);
    x = b - 1;
    __Func_8010704(a, x, 1, 3, *(int *)(__MapActor_GetActor(0x13) + 8) >> 20, x);
    __Func_8010704(0, 0, 1, 3, a, x);
}
