extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009c84(void)
{
    int a;
    int b;
    int c;

    a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    if (b == 7) {
        if (c == 0x18) {
            OvlFunc_946_2009774(8, 0, 0x30);
        } else {
            OvlFunc_946_2009774(8, 0, 0x50);
            OvlFunc_946_2009774(8, 0, 0x70);
        }
    } else if (b == 0xa) {
        if (c == 0x18)
            return;
        OvlFunc_946_2009774(8, 0, 0x90);
    } else if (b == 0xe) {
        OvlFunc_946_2009774(8, 0, 0x50);
    } else {
        return;
    }
    __WaitFrames(2);
    c = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    a -= 1;
    __Func_8010704(a, b, 3, 1, a, c);
    __Func_8010704(0, 0, 3, 1, a, b);
}
