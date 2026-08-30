extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_2009f78(void)
{
    int a;
    int w;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0xc) + 0x10) >> 20;
    if (a == 0x24) {
        OvlFunc_946_2009774(0xc, -0x60, 0);
        OvlFunc_946_2009774(0xc, -0x60, 0);
    } else if (a == 0x22) {
        OvlFunc_946_2009774(0xc, -0x60, 0);
        OvlFunc_946_2009774(0xc, -0x40, 0);
    } else if (a == 0x18) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
