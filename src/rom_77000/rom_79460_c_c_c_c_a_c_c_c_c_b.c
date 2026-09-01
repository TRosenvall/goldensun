extern unsigned char *Func_8077330(int a);

int Func_807a550(unsigned char *out)
{
    unsigned char *t;
    unsigned char *p;
    int *q;
    int count;
    int i;
    unsigned char *z;

    count = 0;
    t = Func_8077330(0);
    p = t + 8;
    if (out != 0) {
        z = out + 3;
        *z = 0;
        z = out + 2;
        *z = 0;
        z = out + 1;
        *z = 0;
        *out = 0;
    }
    q = (int *)(t + (0x84 << 1));
    i = 0;
    if (*q != 0) {
        do {
            if (*(signed char *)(p + 3) == -1) {
                if (out != 0)
                    out[p[0]]++;
                count++;
            }
            i++;
            p += 4;
        } while (i != *q);
    }
    return count;
}
