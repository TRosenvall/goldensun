extern int _Func_801eb64(int a, int b, int c, int d, int e);

void Func_80a33d4(unsigned char *a, int b)
{
    int i;
    int *p;
    int k;

    k = 0xa8;
    p = (int *)(a + 0x48);
    for (i = 0; i <= 7; i++)
        *p++ = _Func_801eb64(2, i, b, 0xf8, k);
    p = (int *)(a + 0x68);
    for (i = 8; i <= 0xf; i++)
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, k);
    p = (int *)(a + 0x88);
    for (i = 0x10; i <= 0x1f; i++)
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, k);
}
