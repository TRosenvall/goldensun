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
    fill = Func_80008d8;
    shape = 0;
    while (1) {
        t = shape * 3;
        t <<= 7;
        base = (int *)(t + 0x6005000);
        row = 0;
        while (1) {
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
                    off = n * 8;
                    q[0] ^= *(int *)(tbl + off);
                    off += 4;
                    q[8] ^= *(int *)(tbl + off);
                }
                q++;
            }
            base += 0x10;
            if (row == 5)
                break;
            row++;
        }
        if (shape == 1)
            break;
        shape++;
    }
}
