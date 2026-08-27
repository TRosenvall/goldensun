extern unsigned char *galloc_ewram(int slot, int size);
extern void _Func_8011590(void);

void Func_808c44c(void)
{
    unsigned char *a;
    unsigned char *b;
    signed char *p;
    unsigned char *q;

    a = galloc_ewram(0x1b, 0xccc);
    if (*(short *)(a + (0xcf << 1)) == 3) {
        b = galloc_ewram(0x1f, 0xa8 << 3);
        if (b != 0) {
            p = (signed char *)(b + 0x53d);
            if (*p != 0) {
                b[0x53a] = 0;
                b[0x53b] = 0;
                b[0x53c] = 1;
                *p = 0;
            }
        }
        q = *(unsigned char **)(a + (0xf0 << 1));
        q[0x5b] = 1;
        _Func_8011590();
    }
}
