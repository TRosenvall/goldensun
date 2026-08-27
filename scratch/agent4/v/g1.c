extern int L37250[] __asm__(".L37250");
extern void Func_80008d8(int *dst, int n, int v);

typedef void (*Fn)(int *dst, int n, int v);

void Func_8021848(void)
{
    Fn fn;
    int *tbl;
    int *dst;
    int *q;
    int a, b, c, k, t;

    tbl = L37250;
    fn = Func_80008d8;
    a = 0;
    while (1) {
        dst = (int *)(((a * 3) << 7) + 0x6006280);
        for (b = 0; b <= 5; b++) {
            fn(dst, 0x40, 0x44444444);
            q = dst + 1;
            for (c = 1; c <= 7; c++) {
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
                    q[0] ^= tbl[k * 2];
                    q[8] ^= tbl[k * 2 + 1];
                }
                q++;
            }
            dst += 0x10;
        }
        a++;
        if (a > 1)
            break;
    }
}
