extern int *__MapActor_GetActor(int);
extern void __WaitFrames(int);
extern void OvlFunc_946_2009774(int, int, int);
extern void __Func_8010704(int, int, int, int, int, int);

void OvlFunc_946_200a16c(void)
{
    int *act;
    int x;
    int y;
    int z;

    act = __MapActor_GetActor(0xd);
    x = act[8 / 4] >> 20;
    act = __MapActor_GetActor(0xd);
    y = act[0x10 / 4];
    y >>= 20;
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
    act = __MapActor_GetActor(0xd);
    z = act[8 / 4];
    y--;
    z >>= 20;
    __Func_8010704(x, y, 1, 3, z, y);
    __Func_8010704(0, 0, 1, 3, x, y);
}
