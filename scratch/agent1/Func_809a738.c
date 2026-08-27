extern int Random(void);
extern int cos(int);
extern int sin(int);
extern int Func_8000888(int a, int b);
extern void _Actor_SetScript(unsigned char *e, void *script);
extern unsigned char Data_9f0b0[];

void Func_809a738(unsigned char *e)
{
    int (*f)(int, int);
    int r;
    int h;
    int dx;
    int dy;
    int v;
    unsigned short *c;
    int t;

    r = Random();
    h = *(unsigned short *)(e + 6);
    r += 0x80 << 10;
    v = cos(h);
    f = Func_8000888;
    dx = f(r, v);
    dy = f(r, sin(h));
    *(int *)(e + 8) += dx;
    *(int *)(e + 0x10) += dy;
    *(unsigned short *)(e + 6) += 0xfff0;
    c = (unsigned short *)(e + 0x66);
    if (*(short *)c != 0) {
        v = *c;
        *c = v - 1;
        *(unsigned short *)(e + 6) += 0x80 << 4;
    } else if (((unsigned int)Random() << 5) >> 16 == 0) {
        *c = (((unsigned int)Random() << 4) >> 16) + 8;
    }
    t = *(unsigned short *)(e + 0x64);
    t++;
    *(unsigned short *)(e + 0x64) = t;
    t <<= 16;
    if (t == (0xca << 15))
        _Actor_SetScript(e, Data_9f0b0);
}
