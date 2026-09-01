extern void _Func_801e9d4(int a, int n, int b, int c, int d);

void Func_80a14f0(int a0, int a1, int a2, int a3)
{
    int v;
    int n;

    v = a0;
    n = 1;
    while (n <= 0xf) {
        v = v / 10;
        if (v <= 9)
            break;
        n++;
    }
    n++;
    a2 -= n * 8;
    _Func_801e9d4(a0, n, a1, a2, a3);
}
