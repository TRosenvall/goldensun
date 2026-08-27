extern void Func_80008d8(void *dst, int n, int v);
extern int Laf23c[] __asm__(".Laf23c");

typedef void (*FillFn)(void *dst, int n, int v);

void LoadMoveRangeIcons(void)
{
    FillFn fill;
    char *tbl;
    int *base;
    int *q;
    int shape, row, k, n, d, t, off;

    tbl = (char *)Laf23c;
    shape = 0;
    fill = Func_80008d8;
    for (; shape <= 1; shape++) {
        t = shape * 3;
        t <<= 7;
        row = 0;
        base = (int *)(t + 0x6005000);
        for (; row <= 5; row++) {
            fill(base, 0x40, 0x44444444);
            k = 1;
            q = base + 1;
            for (; k <= 7; k++) {
                n = row;
                if (shape != 1 || k > 1) {
                    if (shape == 0) {
                        d = k - 2;
                        if (row > d) {
                            n = d;
                            if (n < 0)
                                n = 0;
                        }
                    }
                    off = n * 8;
                    q[0] ^= *(int *)(tbl + off);
                    off += 4;
                    q[8] ^= *(int *)(tbl + off);
                }
                q++;
            }
            base += 0x10;
        }
    }
}
