typedef unsigned char u8;

struct A {
    u8 pad00[0xc];
    int fc;
    u8 pad10[0x13];
    u8 f23;
    u8 pad24[0x31];
    u8 f55;
};

extern unsigned char gState[];
extern int _AREA_7e;
extern struct A *__MapActor_GetActor(int n);
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int n, int x, int y);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_8092b08(int n, int v);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009214(void)
{
    struct A *a;
    unsigned char *g;
    short *e;
    int m, n;

    a = __MapActor_GetActor(8);
    g = gState;
    e = (short *)(g + (0xe0 << 1));
    if (__GetFlag(*e + (0x8d2 - (int)&_AREA_7e)) != 0) {
        __MapActor_SetPos(8, 0x28a0000, 0xa80000);
        a->fc = 0xffe00000;
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Func_8092b08(8, 3);
        a->f55 = 0;
        a->f23 |= 2;
        m = 0xa;
        __Func_8010704(0x2a, 0xa, 1, 1, 0x28, m);
    } else {
        __MapActor_GetActor(8)->f55 = 0;
    }
}
