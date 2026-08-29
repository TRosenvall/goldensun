extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a450(void)
{
    int x;
    int y;
    int z;
    int w;

    x = *(int *)(__MapActor_GetActor(0x11) + 8) >> 20;
    y = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    if (y == 0xf) {
        OvlFunc_946_2009774(0x11, 0, 0x40);
    } else if (y == 0x12) {
        OvlFunc_946_2009774(0x11, 0, 0x10);
    } else if (y == 0x13) {
        return;
    }
    __WaitFrames(2);
    z = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    w = x - 1;
    __Func_8010704(w, y, 3, 1, w, z);
    __Func_8010704(0, 0, 3, 1, w, y);
}
