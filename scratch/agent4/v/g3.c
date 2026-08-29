extern int L37250[] __asm__(".L37250");
extern void Func_80008d8(int *dst, int n, int v);

typedef void (*Fn)(int *dst, int n, int v);

int Func_8021848(void)
{
    Fn fn;
    unsigned char *tbl;
    int *dst;
    int *q;
    int a, b, c, k, t, kk;

    a = 0;
    tbl = (unsigned char *)L37250;
    fn = Func_80008d8;
    for (; a <= 1; a++) {
        t = a * 3;
        t <<= 7;
        dst = (int *)(t + 0x6006280);
        for (b = 0; b <= 5; b++) {
            fn(dst, 0x40, 0x44444444);
            c = 1;
            q = dst + 1;
            for (; c <= 7; c++) {
                k = b;
                if (a != 1 || c > 1) {
                    if (a == 0) {
                        t = c - 2;
                        if (b > t) {
                            k = t;
                            if (k < 0)
                                k = 0;
                        }
                    }
                    kk = k;
                    kk <<= 3;
                    q[0] ^= *(int *)(tbl + kk);
                    kk += 4;
                    q[8] ^= *(int *)(tbl + kk);
                }
                q++;
            }
            dst += 0x10;
        }
    }
}
