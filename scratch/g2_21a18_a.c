extern volatile unsigned short _TBL_372c0[] __asm__(".L372c0");

void Func_8021a18(unsigned char *dst0)
{
    unsigned short *dp;
    unsigned short *src;
    unsigned int v;
    unsigned int out;
    int a;
    int b;
    int c;
    int row;
    int col;
    int k;
    int off;

    a = 0;
    b = 0;
    c = 0;
L_outer:
    row = 0;
    off = (c + a) << 6;
L_row:
    dp = (unsigned short *)(dst0 + off);
    src = (unsigned short *)(0x6000600 + (row << 5));
    col = 0;
L_col:
    v = *src;
    out = 0;
    src++;
    k = 0;
L_nib:
    out |= _TBL_372c0[(v & 0xf) + b] << (k << 2);
    k++;
    v >>= 4;
    if (k <= 3)
        goto L_nib;
    col++;
    *dp = out;
    dp++;
    if (col <= 0xf)
        goto L_col;
    row++;
    off += 0x20;
    if (row <= 9)
        goto L_row;
    b += 0x10;
    a++;
    c += 4;
    if (a <= 1)
        goto L_outer;
}
