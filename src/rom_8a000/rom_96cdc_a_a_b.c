extern void _Actor_SetScript(void *a, void *s);
extern int sin(int x);
extern unsigned char Data_9f0b0[];

void Func_8096d2c(int *a)
{
    unsigned short *c;
    int *b;
    int t;
    int s;
    int v;
    int w;

    c = (unsigned short *)((char *)a + 0x64);
    t = *c + 1;
    b = *(int **)((char *)a + 0x68);
    *c = t;
    s = (short)t;
    if (s > 0x1f) {
        _Actor_SetScript(a, Data_9f0b0);
    } else {
        v = sin(s << 10);
        a[6] = v;
        a[7] = v;
        a[2] = b[2];
        w = 0x80 << 9;
        a[3] += w;
        w -= v;
        a[4] = b[4] + w * 5 + (0x90 << 12);
    }
}
