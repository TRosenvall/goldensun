extern unsigned char *Func_8077330(int which);

int Func_807a3a8(unsigned int kind, int a, int b)
{
    unsigned char *rec;
    unsigned char *tab;
    unsigned char *p;
    unsigned char *q;
    int *cnt;
    int i;
    int n;
    int found;
    int t;
    int off;

    t = 0;
    if (kind > 7)
        t = 1;
    rec = Func_8077330(t);
    tab = rec + 8;
    cnt = (int *)(rec + 0x108);
    found = 0;
    i = 0;
    n = *cnt;
    p = tab;
    q = rec + 9;
    off = 0;
    while (i < n) {
        off = i * 4;
        if (a == *p && b == *q) {
            *cnt = n - 1;
            found = 1;
            break;
        }
        n = *cnt;
        i++;
        p += 4;
        q += 4;
    }
    while (i < *(int *)(tab + 0x100)) {
        off = i * 4;
        *(int *)(tab + off) = *(int *)(tab + off + 4);
        i++;
    }
    return found;
}
