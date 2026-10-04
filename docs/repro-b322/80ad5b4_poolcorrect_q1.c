extern char *iwram_3001f2c;

int Func_80ad5b4(int i, int a, int b, int flag)
{
    char *base;
    int off;
    int v;

    base = iwram_3001f2c;
    off = 0x224;
    if (*(int *)(base + (i * 4 + off)) != 0) {
        i *= 2;
        off += 0x10;
        *(short *)(base + (i + off)) = a;
        i += 0x23c;
        v = b;
        if (flag != 0)
            v = (short)(0x8000 | b);
        *(short *)(base + i) = v;
    }
}
