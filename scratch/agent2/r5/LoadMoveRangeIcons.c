extern void Func_80008d8(void *dst, int n, int v);
extern int Laf23c[] __asm__(".Laf23c");

typedef void (*FillFn)(void *dst, int n, int v);

void LoadMoveRangeIcons(void)
{
    FillFn fill;
    int *tbl;
    int *base;
    int *q;
    int shape, row, k, n, d;

    tbl = Laf23c;
    fill = Func_80008d8;
    for (shape = 0; shape <= 1; shape++) {
        base = (int *)(0x6005000 + shape * 3 * 128);
        for (row = 0; row <= 5; row++) {
            fill(base, 0x40, 0x44444444);
            q = base + 1;
            for (k = 1; k <= 6; k++) {
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
                    q[0] ^= tbl[n * 2];
                    q[8] ^= tbl[n * 2 + 1];
                }
                q++;
            }
            base += 0x10;
        }
    }
}
