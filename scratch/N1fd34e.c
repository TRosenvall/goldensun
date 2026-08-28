extern int iwram_3001800;
extern int sin(int a);

void Func_801fd34(void)
{
    unsigned short *p;
    int i;
    int t, a, b, c;

    i = 0;
    p = (unsigned short *)0x50001d0;
    do {
        t = sin((iwram_3001800 + i * 8) * 3 << 8) / 0x4000;
        a = t * 2 + 0x16;
        b = t + 0x10;
        c = t + 0x14;
        *p = (c << 10) | (b << 5) | a;
        p++;
        i++;
    } while (i <= 3);
}
