extern char *iwram_3001e8c;
extern void Func_8018850(int a, int *b, int *c, int d);

int Func_8020b14(unsigned char *s)
{
    char *p;
    unsigned short *q;
    int n;
    int va, vb;

    p = iwram_3001e8c;
    n = 0;
    if (*s != 0) {
        q = (unsigned short *)(p + (0xeb << 4));
        do {
            *q = *s;
            s++;
            q++;
            n++;
        } while (*s != 0);
    }
    *(unsigned short *)(p + (n * 2 + (0xeb << 4))) = 0;
    Func_8018850(0, &vb, &va, 0);
    return vb;
}
