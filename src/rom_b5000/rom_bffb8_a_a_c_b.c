extern int iwram_3001ef8;
extern short iwram_3001ad0[];
extern void Func_80c0cec(int a, int b, int c, int d);

void Func_80c01bc(void)
{
    int *pa;
    char *pb;
    int v;
    int c;
    int n;
    int t;

    pa = *(int **)&iwram_3001ef8;
    v = *pa;
    pb = *(char **)((char *)&iwram_3001ef8 - 0x78);
    c = 0x34 - v;
    if (c > 0x20)
        c = 0x20;
    if (c < 0)
        c = 0;
    iwram_3001ad0[1] = c;
    if ((unsigned int)v <= 0x50) {
        t = v * 360 + 0xaf80;
        *(unsigned short *)(pb + 0x36) = t;
    }
    n = *pa + 1;
    *pa = n;
    if ((unsigned int)n <= 0x50)
        Func_80c0cec(0, 0, 0, 0xb4 - n);
    else
        Func_80c0cec(0, 0, 0, 0x64);
}
