struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char scramble[] __asm__(".L1314c");

void Func_800be70(unsigned char *a, int step)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int i;
    int n;
    int e;

    p = (unsigned char *)(gSpriteSlots[a[0x1c]].vramOffset + 0x6010000);
    n = a[0x20] * a[0x21] / 64;
    for (i = 0; i < n; i++) {
        if ((unsigned int)(step - 0x40) <= 0x3f) {
            e = scramble[(step + (i << 4)) & 0x3f];
            q = p + (e & 0x3e);
            if (e % 2 != 0)
                *(unsigned short *)q = *q;
            else
                *(unsigned short *)q = 0xff00 & *(unsigned short *)q;
        }
        p += 0x40;
        step++;
    }
}
