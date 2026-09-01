struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern struct A *__MapActor_GetActor(int slot);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_947_20091c4(void)
{
    struct P t;
    struct A *src;
    int v;
    int x;
    int z;
    int n;

    src = __MapActor_GetActor(0);
    v = iwram_3001e40 & 3;
    if (v != 0)
        return;
    t.f4 = 0xa;
    t.f8 = 0xb333;
    t.fc = 0xb333;
    x = src->f8 + (((__Random() * 17 >> 16) - 8) << 16);
    z = src->f10 + (((__Random() * 17 >> 16) - 8) << 16);
    n = (int)(((__Random() * 5 >> 16) << 16) + (0xc0 << 10)) / 10;
    OvlFunc_common0_10c(x, src->fc, z, 0, n, v, 0x90001, &t);
}
