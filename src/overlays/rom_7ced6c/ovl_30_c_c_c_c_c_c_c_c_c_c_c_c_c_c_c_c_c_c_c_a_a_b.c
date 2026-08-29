extern unsigned char *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void OvlFunc_946_2009b14(void);
extern void __Func_8010704(int sx, int sy, int w, int h, int dx, int dy);

void OvlFunc_946_2009bbc(void)
{
    int a;
    int b;
    int c;
    int d;
    int x;

    a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    d = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    if (b == 0x13) {
        if (c == 0x18) {
            OvlFunc_946_2009774(8, 0, -0x50);
        } else if (d == 0x18) {
            OvlFunc_946_2009774(8, 0, -0x70);
            OvlFunc_946_2009774(8, 0, -0x20);
        } else {
            OvlFunc_946_2009774(8, 0, -0x50);
            OvlFunc_946_2009774(8, 0, -0x70);
        }
    } else if (b == 0xe) {
        if (c == 0x18)
            return;
        if (d == 0x18)
            OvlFunc_946_2009774(8, 0, -0x40);
        else
            OvlFunc_946_2009774(8, 0, -0x70);
    } else if (b == 0xa) {
        if (d == 0x18)
            return;
        OvlFunc_946_2009774(8, 0, -0x30);
    } else {
        OvlFunc_946_2009b14();
        return;
    }
    __WaitFrames(2);
    x = a - 1;
    __Func_8010704(x, b, 3, 1, x, *(int *)(__MapActor_GetActor(8) + 0x10) >> 20);
    __Func_8010704(0, 0, 3, 1, x, b);
}
