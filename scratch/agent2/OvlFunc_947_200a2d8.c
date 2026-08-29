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
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

int OvlFunc_947_200a2d8(struct A *src)
{
    struct P t;
    int v;
    int x;
    int y;
    int z;
    int n;

    v = iwram_3001e40 & 7;
    if (v != 0)
        return 0;
    t.f4 = 7;
    t.f8 = 0xb333;
    t.fc = 0xb333;
    x = src->f8 + (((__Random() * 17 >> 16) - 8) << 16);
    y = src->fc + ((__Random() * 17 >> 16) << 16);
    z = src->f10 + (((__Random() * 17 >> 16) - 8) << 16);
    n = (int)(((__Random() * 5 >> 16) << 16) + (0xc0 << 10)) / 10;
    OvlFunc_common0_10c(x, y, z, 0, n, v, 0x90001, &t);
    return 0;
}
