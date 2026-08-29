extern int divsi3_RAM(int a, int b);
static int (*const dv)(int, int) = divsi3_RAM;

void Func_80f2ebc(short *a, short *b, short *c, int n)
{
    int i;
    int u, v;

    if (n > 0) {
        i = 0x5ff;
        do {
            u = *a;
            v = *b;
            *c = dv(v - u, n);
            a++;
            b++;
            c++;
        } while (--i >= 0);
    }
}
