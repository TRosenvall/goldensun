extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009de0(void)
{
    int x;
    int y;
    int z;

    x = *(int *)(__MapActor_GetActor(0xa) + 8) >> 20;
    y = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    if (y == 0x12)
        return;
    if (y == 0xa) {
        OvlFunc_946_2009774(0xa, 0, 0x80);
    } else {
        OvlFunc_946_2009774(0xa, 0, 0x70);
        OvlFunc_946_2009774(0xa, 0, 0x40);
    }
    __WaitFrames(2);
    z = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    x--;
    __Func_8010704(x, y, 3, 1, x, z);
    __Func_8010704(0, 0, 3, 1, x, y);
}
