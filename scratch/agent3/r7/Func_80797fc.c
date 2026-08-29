extern void *GetEnemyInfo(int id);
extern void *GetPCBaseStats(int id);
extern unsigned char L88e38[] __asm__(".L88e38");

int Func_80797fc(int id, unsigned int base, int *out)
{
    int i;
    int *o;
    unsigned char *p;

    if (id > 7) {
        unsigned char *row;
        base = *((unsigned char *)GetEnemyInfo(id) + 0x34);
        if (base > 0x2b)
            base = 0;
        row = L88e38 + base * 24;
        i = 0;
        o = out;
        p = row + 4;
        for (; i < 4; i++) {
            int v = *p * 10;
            p++;
            *o++ = v;
        }
    } else {
        o = out;
        p = (unsigned char *)base + 0x24;
        for (i = 0; i < 4; i++) {
            int v = *p * 10;
            p++;
            *o++ = v;
        }
        if (id <= 7) {
            for (i = 0; i < 4; i++) {
                unsigned char *q = (unsigned char *)GetPCBaseStats(id) + 2;
                int off = i + 0x90;
                *out += q[off];
                out++;
            }
        }
    }
    return 0;
}
