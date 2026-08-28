struct A { unsigned char pad0[8]; int f8; unsigned char padc[4]; int f10; };

extern struct A *__MapActor_GetActor(int id);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a200(void)
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int x;

    a = __MapActor_GetActor(0xf)->f8 >> 20;
    b = __MapActor_GetActor(0xf)->f10 >> 20;
    c = __MapActor_GetActor(8)->f10 >> 20;
    d = __MapActor_GetActor(0xa)->f10 >> 20;
    if (a == 0x23) {
        if (d == 7) {
            OvlFunc_946_2009774(0xf, -0x10, 0);
        } else if (c == 7) {
            OvlFunc_946_2009774(0xf, -0x70, 0);
        } else {
            OvlFunc_946_2009774(0xf, -0x60, 0);
            OvlFunc_946_2009774(0xf, -0x50, 0);
        }
    } else if (a == 0x22) {
        if (d == 7)
            return;
        OvlFunc_946_2009774(0xf, -0x60, 0);
        OvlFunc_946_2009774(0xf, -0x40, 0);
    } else if (a == 0x21) {
        OvlFunc_946_2009774(0xf, -0x90, 0);
    } else if (a == 0x1f) {
        OvlFunc_946_2009774(0xf, -0x50, 0);
    } else if (a == 0x1e) {
        OvlFunc_946_2009774(0xf, -0x60, 0);
    } else if (a == 0x18) {
        return;
    }
    __WaitFrames(2);
    x = __MapActor_GetActor(0xf)->f8 >> 20;
    e = b - 1;
    __Func_8010704(a, e, 1, 3, x, e);
    __Func_8010704(0, 0, 1, 3, a, e);
}
