void Func_80f2ebc(short *a, short *b, short *c, int n)
{
    int i;

    if (n > 0) {
        i = 0x5ff;
        do {
            *c = (*b - *a) / n;
            a++;
            b++;
            c++;
        } while (--i >= 0);
    }
}
