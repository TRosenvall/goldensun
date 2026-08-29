extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_a4;
extern int _AREA_a5;

extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8091eb0(int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_960_2008b24(int a, int n)
{
    unsigned char *s;
    unsigned char *g;
    unsigned char *act;
    short *q;
    int area;
    int zero;

    s = iwram_3001ebc;
    q = (short *)(s + (0xc1 << 1));
    if (*q == 0x63) {
        zero = 0;
        *q = zero;
    }
    __ClearFlag(0x20f);
    {
        unsigned char *g0 = gState;
        area = *(short *)(g0 + (0xe0 << 1));
    }
    if (area == (int)(&_AREA_a4))
        __SetFlag(n + 0x2f9);
    else if (area == (int)(&_AREA_a5))
        __SetFlag(n + 0x309);
    __SetFlagByte(0x84 << 2, 0);
    __Func_8091eb0(0x62, 5);
    g = gState;
    g[0x22b] = 3;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_a5)) {
        if (n == 0xb) {
            __Func_8091eb0(0x62, 7);
        } else if (n == 0xc) {
            __Func_8091eb0(0x62, 6);
            __MapActor_SetIdle(0xc);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
    act = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    act[0x55] = 3;
}
