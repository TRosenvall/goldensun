struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern char *iwram_3001f30;
extern struct A *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_957_2008d90(void)
{
    struct A *a;
    signed char v;
    char *q;
    int e, f;

    q = iwram_3001f30;
    a = __MapActor_GetActor(0xb);
    v = *(signed char *)(q + 0x35);
    if (v == 0) {
        e = 0x49;
        f = 0x11;
        __Func_8010704(0x4c, 0x10, 1, 1, e, f);
        if (a != 0) {
            a->f55 = 2;
            a->f23 = v;
        }
        __SetFlag(0x211);
    }
}
