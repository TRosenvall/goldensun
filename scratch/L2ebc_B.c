void Func_80f2ebc(short *a, short *b, short *c, int n)
{
    int i;
    int u, v;

    if (n > 0) {
        i = 0x5ff;
        do {
            u = *a;
            v = *b;
            *c = (v - u) / n;
            a++;
            b++;
            c++;
        } while (--i >= 0);
    }
}
