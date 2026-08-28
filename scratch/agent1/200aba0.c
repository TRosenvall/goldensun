struct E {
    unsigned char pad00[0xc];
    int f0c;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x1c];
    int f3c;
};

extern unsigned int gTimers[] __asm__(".L3b40");
extern struct E *gEnts[] __asm__(".L3b10");

void OvlFunc_897_200aba0(void)
{
    struct E *p;
    unsigned int i, n;

    for (i = 0; i <= 9; i++) {
        n = gTimers[i];
        if (n != 0) {
            p = gEnts[i];
            if (n <= 8) {
                p->f18 += -0x1ccc;
                p->f1c += 0x8000;
                p->f0c += 0x4ccc;
                p->f3c += 0x4ccc;
            } else {
                p->f0c += 0x140000;
                p->f3c += 0x140000;
            }
            gTimers[i] += 1;
            if (gTimers[i] > 0xe)
                gTimers[i] = 0;
        }
    }
}
