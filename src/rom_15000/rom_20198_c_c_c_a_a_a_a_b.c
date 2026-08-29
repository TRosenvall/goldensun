extern char *iwram_3001e8c;
extern void Func_8018850(int a, int *b, int *c, int d);

int Func_8020b14(unsigned char *s)
{
    char *p;
    unsigned short *q;
    int n;
    int off;
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
    off = n * 2;
    off += 0xeb << 4;
    *(unsigned short *)(p + off) = 0;
    Func_8018850(0, &vb, &va, 0);
    return vb;
}
