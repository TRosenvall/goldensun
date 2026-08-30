extern unsigned short _TBL_372c0[] __asm__(".L372c0");

void Func_8021a18(unsigned char *dst0)
{
    unsigned short *dp;
    unsigned short *src;
    unsigned int v;
    unsigned int out;
    int a;
    int b;
    int row;
    int col;
    int k;

    for (a = 0, b = 0; a <= 1; a++) {
        for (row = 0; row <= 9; row++) {
            dp = (unsigned short *)(dst0 + ((a * 10 + row) << 5));
            src = (unsigned short *)(0x6000600 + (row << 5));
            for (col = 0; col <= 0xf; col++) {
                v = *src;
                out = 0;
                src++;
                for (k = 0; k <= 3; k++) {
                    out |= _TBL_372c0[(v & 0xf) + b] << (k * 4);
                    v >>= 4;
                }
                *dp = out;
                dp++;
            }
        }
        b += 0x10;
    }
}
