extern char *iwram_3001e8c;
extern void Func_8097868(void);
extern void StartTask(void (*f)(void), int n);
extern void StopTask(void (*f)(void));
extern void _SetUIColor(int a, int b);
extern unsigned char gState[];

void Func_8097a7c(void)
{
    char *p;
    char *d;
    unsigned short *q;
    int off;
    int v;
    int n;

    p = iwram_3001e8c;
    off = 0xea4;
    d = p + off;
    off = 1;
    *d = off;
    v = 0x739c;
    q = (unsigned short *)0x50001e2;
    *q = v;
    q = (unsigned short *)((char *)q + 4);
    *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    q++; *q = v;
    n = 0x90 << 3;
    StartTask(Func_8097868, n);
}

void Func_8097adc(void)
{
    char *p;
    unsigned short *q;
    int v, z;

    p = iwram_3001e8c;
    StopTask(Func_8097868);
    q = (unsigned short *)0x50001e2;
    v = 0x7fff;
    z = 0;
    *q = v;
    *(unsigned short *)0x50001e6 = z;
    v = 0x294a;
    q = (unsigned short *)((char *)q + 0x14);
    *q = v;
    v = 0x5294;
    q++;
    *q = v;
    _SetUIColor(gState[0x205], gState[0x206]);
    p += 0xea4;
    *p = z;
}
