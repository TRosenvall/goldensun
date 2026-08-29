extern unsigned int gState;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8012078(int a, int x, int y, int t);

void OvlFunc_956_2008658(void)
{
    unsigned char *e;
    unsigned int r2;
    unsigned int r3;
    int x;
    int y;
    int t;

    r3 = (unsigned int)&gState;
    r2 = 0xfa;
    r2 <<= 1;
    r3 += r2;
    e = __MapActor_GetActor(*(int *)r3);
    x = *(int *)(e + 8) >> 20;
    t = 0x17;
    y = *(int *)(e + 0x10) >> 20;
    if (x == 0x51 && y == 0xc) {
        if (((0xe0 << 8) & *(unsigned short *)(e + 6)) == (0x80 << 7))
            t = 0xfd;
        __Func_8012078(0, x << 20, y << 20, t);
    }
}
