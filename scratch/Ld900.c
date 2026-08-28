extern unsigned int gState;
extern unsigned char *iwram_3001edc;
extern void OvlFunc_924_200d158(int arg);
extern int __Random(void);

void OvlFunc_924_200d900(void)
{
    unsigned char *q;
    unsigned char *e;
    unsigned char *base;
    unsigned int r1;
    unsigned int r3;
    unsigned int r4;
    unsigned int off;
    int idx;
    int arg;
    int v;

    r3 = (unsigned int)&iwram_3001edc;
    q = *(unsigned char **)r3;
    r3 -= 0x20;
    e = *(unsigned char **)q;
    r1 = 0xfa;
    base = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r1 <<= 1;
    r4 += r1;
    idx = *(int *)r4;
    off = idx * 4;
    off += 0x14;
    arg = *(int *)(base + off);
    v = *(int *)(e + 8);
    if (v != 0) {
        v = v - 1;
    } else {
        OvlFunc_924_200d158(arg);
        v = ((unsigned int)(__Random() * 30) >> 16) + 0xa;
    }
    *(int *)(e + 8) = v;
}
