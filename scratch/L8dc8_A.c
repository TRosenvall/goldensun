extern unsigned int gState;
extern int _AREA_a5;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_808ee0c(void);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_960_2008ce4(void);

void OvlFunc_960_2008dc8(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned int q;
    unsigned int off;
    int z;
    int x, y;
    int m1, m2;
    int e1, f1, e2, f2;
    int pri;

    z = 0;
    x = 0xf8 << 16;
    y = 0xb2 << 18;
    m1 = -1;
    m2 = -1;
    e1 = 0xf;
    f1 = 0x2c;
    e2 = 0xc;
    f2 = 0x47;
    pri = 0xc8 << 4;
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    if (*(short *)q == (int)(&_AREA_a5)) {
        a = __MapActor_GetActor(0xe);
        a[0x23] = 2;
        b = __MapActor_GetActor(0xe);
        b[0x55] = z;
        __MapActor_SetPos(0xe, x, y);
        __Func_8010704(0x1f, 0x5f, 1, 1, e1, f1);
        __Func_808edac(0x64, m1, m2);
        __Func_808ee0c();
        __Func_8010704(0x7f, 0x7f, 1, 1, e2, f2);
        __StartTask(OvlFunc_960_2008ce4, pri);
    }
}
