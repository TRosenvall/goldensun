extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a004(void)
{
    int x;
    int y;
    int z;

    x = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    y = *(int *)(__MapActor_GetActor(0xc) + 0x10) >> 20;
    if (x == 0x18) {
        OvlFunc_946_2009774(0xc, 0x60, 0);
        OvlFunc_946_2009774(0xc, 0x60, 0);
    } else if (x == 0x22) {
        OvlFunc_946_2009774(0xc, 0x20, 0);
    } else if (x == 0x24) {
        return;
    }
    __WaitFrames(2);
    z = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    y--;
    __Func_8010704(x, y, 1, 3, z, y);
    __Func_8010704(0, 0, 1, 3, x, y);
}
