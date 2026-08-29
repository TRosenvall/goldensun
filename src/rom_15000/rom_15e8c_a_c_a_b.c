extern unsigned char *iwram_3001e8c;
extern void Func_80170f8(int a, int b, int c, int d);
extern void Func_8017248(int a, int b, int c, int d, int e);
extern void Func_80008d8(void *dst, int size, int value);

void Func_8016230(unsigned char *win)
{
    unsigned char *base;
    int a, b, c, d, f;
    int z;
    void (*fp)(void *, int, int);

    base = iwram_3001e8c;
    d = *(unsigned short *)(win + 0xa);
    f = *(unsigned short *)(win + 0x16);
    z = 0;
    *(unsigned short *)(win + 0x1a) = z;
    a = *(unsigned short *)(win + 0xc);
    b = *(unsigned short *)(win + 0xe);
    c = *(unsigned short *)(win + 8);
    if (f & 8) {
        if (f & 0x20) {
            Func_80170f8(a, b, c, d);
            fp = Func_80008d8;
            fp((void *)0x6002500, 0xf0 << 4, 0x44444444);
        } else {
            fp = Func_80008d8;
            fp((void *)0x6002500, 0xf0 << 4, 0);
        }
        Func_8017248(a, b, c, d, 0);
    } else {
        Func_80170f8(a, b, c, d);
    }
    base[0xea3] = 1;
}
