struct P {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern int iwram_3001e40;
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

int OvlFunc_933_2008344(struct A *src)
{
    struct P t;
    int v;
    int w;

    w = iwram_3001e40;
    if ((w & 7) == 0)
        __PlaySound(0x76);
    w = iwram_3001e40;
    v = w & 0xf;
    if (v != 0)
        return 0;
    t.f8 = 0xcccc;
    t.fc = 0xcccc;
    t.f22 = (__Random() << 12 >> 16) + (0xf8 << 8);
    OvlFunc_common0_10c(src->f8, src->fc, src->f10, 0, v, v, 0x880001, &t);
    return 0;
}
