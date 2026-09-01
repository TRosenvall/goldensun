extern char *iwram_3001f2c;
extern void Func_80a9bd8(void *node, int i, int x, int y, int cols);

void Func_80a9b94(int x, int y, int cols)
{
    void **q;
    void *n;
    int i;

    i = 0;
    q = (void **)(iwram_3001f2c + 0x48);
    do {
        n = *q++;
        if (n != 0)
            Func_80a9bd8(n, i, x, y, cols);
        i++;
    } while (i <= 0x1f);
}
