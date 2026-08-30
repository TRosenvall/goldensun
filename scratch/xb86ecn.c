struct O {
    unsigned char pad00[0x36];
    unsigned short f36;
};

struct P {
    unsigned char pad00[0x14];
    int f14;
};

extern void *iwram_3001e80[];
extern unsigned int gKeyHeld;
extern void Func_80c0a24(int a, int b, int c, int d, int e);

void Func_80b86ec(void)
{
    struct O *o;
    struct P *p;
    int k;

    o = iwram_3001e80[0];
    p = iwram_3001e80[0x20];
    k = 0x80 << 2;
    if ((gKeyHeld & k) != 0)
        o->f36 += k;
    if ((gKeyHeld & (0x80 << 1)) != 0)
        o->f36 += 0xfffffe00;
    if (p->f14 == 0)
        Func_80c0a24(0xf0 << 15, 0xf0 << 15, 0, 0, 0x80 << 9);
}
