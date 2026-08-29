extern char *iwram_3001f2c;
extern void Func_80a1c2c(void *node, int i, int x, int y, int cols);

void Func_80a1bdc(int x, int y, int cols)
{
    char *p;
    int i;

    p = iwram_3001f2c + 0x48;
    i = 0;
    do {
        if (*(void **)p != 0)
            Func_80a1c2c(p, i, x, y, cols);
        i++;
        p += 4;
    } while (i <= 0x1f);
}
