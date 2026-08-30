extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a16c(void)
{
    int b;
    int c;
    int d;

    b = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 0x10) >> 20;
    __MapActor_GetActor(0xf);
    if (b == 0x19) {
        OvlFunc_946_2009774(0xd, 0x60, 0);
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x1f) {
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x22) {
        OvlFunc_946_2009774(0xd, 0x20, 0);
    } else if (b == 0x23) {
        OvlFunc_946_2009774(0xd, 0x10, 0);
    } else if (b == 0x24) {
        return;
    }
    __WaitFrames(2);
    d = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    c -= 1;
    __Func_8010704(b, c, 1, 3, d, c);
    __Func_8010704(0, 0, 1, 3, b, c);
}
