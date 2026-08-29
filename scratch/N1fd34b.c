extern int iwram_3001800;
extern int sin(int a);

void Func_801fd34(void)
{
    unsigned short *p;
    int i;
    int t;

    i = 0;
    p = (unsigned short *)0x50001d0;
    do {
        t = sin((iwram_3001800 + i * 8) * 3 << 8) / 0x4000;
        *p = ((t + 0x14) << 10) | ((t + 0x10) << 5) | (t * 2 + 0x16);
        p++;
        i++;
    } while (i <= 3);
}
