extern int _AREA_a5;
extern unsigned char gState[];
extern unsigned short L1a00[] __asm__(".L1a00");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __StopTask(void *f);
extern void OvlFunc_960_2008ce4(void);

void OvlFunc_960_2008d24(void)
{
    unsigned char *g;
    int p0, p1;
    int q0, q1;
    unsigned short t;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_a5)) {
        __MapActor_GetActor(0xe)[0x23] = 2;
        __MapActor_GetActor(0xe)[0x55] = 3;
        __MapActor_SetPos(0xe, 0, 0);
        p0 = 0xf;
        p1 = 0x2c;
        __Func_8010704(0x10, 0x2c, 1, 1, p0, p1);
        __Func_808edac(0x64, 0, 0);
        __Func_8010704(0xc, 0x47, 1, 1, 0x7f, 0x7f);
        q0 = 0xc;
        q1 = 0x47;
        __Func_8010704(0xb, 0x47, 1, 1, q0, q1);
        __StopTask(OvlFunc_960_2008ce4);
        t = L1a00[0];
        *(unsigned short *)0x500019e = t;
    }
}
