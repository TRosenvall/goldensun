extern void Func_8003dec(unsigned char *h, int n);

void Func_801c154(unsigned char *h, int a, int b)
{
    int n;
    int v;

    a &= 0x1ff;
    v = *(unsigned short *)(h + 6);
    n = 0xfffffe00;
    n &= v;
    n |= a;
    *(unsigned short *)(h + 6) = n;
    h[4] = b;
    Func_8003dec(h, 0xfc);
}
