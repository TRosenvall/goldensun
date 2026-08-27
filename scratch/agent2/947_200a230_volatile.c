struct Cfg {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    short f20;
    short f22;
    void (*f24)(void);
};

extern volatile unsigned int iwram_3001e40;
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void OvlFunc_947_20093b0(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, void *h);

void OvlFunc_947_200a230(void)
{
    struct Cfg c;
    int flag;
    int x;
    int y;
    int m;

    flag = iwram_3001e40;
    flag &= 2;
    if (flag == 0) {
        if ((iwram_3001e40 & 7) == 0) {
            __PlaySound(0x88);
        }
        c.f4 = 0xa;
        c.f8 = 0x80 << 8;
        c.fc = 0x80 << 8;
        c.f10 = 0x19999;
        c.f14 = 0x19999;
        m = __Random();
        m &= 0xffff000;
        c.f20 = m;
        c.f24 = OvlFunc_947_20093b0;
        x = -(int)((((__Random() * 5) >> 16) << 16) + (0xc0 << 11)) / 2;
        y = -(int)((((__Random() * 5) >> 16) << 16) + (0xa0 << 11));
        OvlFunc_common0_10c(0xa2 << 17, 0xc0 << 14, 0xe4 << 16, x,
                            y, flag, 0x14d0000, &c);
    }
}
