extern int Func_8000888(int, int);
extern void _UpdateSprite(int *a, int *b, int *c, int d);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

void Func_809b86c(int e)
{
    int a[2];
    int b[4];
    int off;
    int x, y;
    int *base;

    base = *(int **)e;
    off = 0;
    if (*(unsigned char *)(e + 0x47) & 4)
        off = 0x1fc0000 - *(int *)(e + 8);
    a[0] = call_via(Func_8000888, *(int *)(e + 0x28), base[6]);
    a[1] = call_via(Func_8000888, *(int *)(e + 0x2c), base[6]);
    x = *(int *)(e + 4);
    b[0] = x;
    b[1] = off;
    y = *(int *)(e + 8);
    b[2] = y + off;
    b[3] = 0;
    if (x > -0x200000 && x < 0x1100000 && y > -0x200000 && y < 0xe00000)
        _UpdateSprite(base, b, a, 0);
}
