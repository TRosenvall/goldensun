extern int *__MapActor_GetActor(int);
extern void __WaitFrames(int);
extern void OvlFunc_946_2009774(int, int, int);
extern void __Func_8010704(int, int, int, int, int, int);

void OvlFunc_946_200a16c(void)
{
    int x;
    int y;
    int z;

    x = __MapActor_GetActor(0xd)[8 / 4] >> 20;
    y = __MapActor_GetActor(0xd)[0x10 / 4] >> 20;
    __MapActor_GetActor(0xf);
    if (x == 0x19) {
        OvlFunc_946_2009774(0xd, 0x60, 0);
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (x == 0x1f) {
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (x == 0x22) {
        OvlFunc_946_2009774(0xd, 0x20, 0);
    } else if (x == 0x23) {
        OvlFunc_946_2009774(0xd, 0x10, 0);
    } else if (x == 0x24) {
        return;
    }
    __WaitFrames(2);
    z = __MapActor_GetActor(0xd)[8 / 4];
    y--;
    z >>= 20;
    __Func_8010704(x, y, 1, 3, z, y);
    __Func_8010704(0, 0, 1, 3, x, y);
}
