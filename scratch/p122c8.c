extern unsigned char ewram_2020000[];
extern unsigned char L1353c[] __asm__(".L1353c");
extern int Func_8012204(int *v);

int Func_80122c8(int *v, int *out)
{
    unsigned char *m;
    int base;
    int t;
    int adj;

    base = Func_8012204(v);
    adj = 0;
    m = ewram_2020000 + (((v[0] / 0x200000) & 0x1f)
                         + (((v[2] / 0x200000) & 0x1f) << 5)) * 4;
    if (m[3] & 0x80)
        adj = 0x10;
    t = (unsigned int)(*(int *)m << 1) >> 25;
    *out = t;
    if (t == 0x15)
        adj = 0x20;
    return L1353c[adj + base];
}
