extern void *GetEnemyInfo(int id);
extern void *GetPCBaseStats(int id);
extern unsigned char L88e38[] __asm__(".L88e38");

int Func_80797fc(int id, unsigned char *base, int *out)
{
    int i;
    int *o;
    unsigned char *p;
    unsigned char *row;
    unsigned char *q;
    unsigned int k;

    if (id > 7) {
        k = *((unsigned char *)GetEnemyInfo(id) + 0x34);
        if (k > 0x2b)
            k = 0;
        row = L88e38 + k * 24;
        o = out;
        p = row + 4;
        for (i = 0; i < 4; i++)
            *o++ = *p++ * 10;
    } else {
        o = out;
        p = base + 0x24;
        for (i = 0; i < 4; i++)
            *o++ = *p++ * 10;
    }
    if (id <= 7) {
        for (i = 0; i < 4; i++) {
            int off = i + 0x90;
            q = (unsigned char *)GetPCBaseStats(id) + 2;
            *out += q[off];
            out++;
        }
    }
    return 0;
}
