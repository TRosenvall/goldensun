extern char *iwram_3001e8c;
extern int GetPortrait(void);
extern void CloseUIBox(void *p, int n);

void Func_8019e48(void)
{
    char *b;
    char *p;
    unsigned char *q;
    int v;
    int k;
    int off;
    int t;
    int i;

    b = iwram_3001e8c;
    p = b + (0xa0 << 3);
    v = GetPortrait();
    if (v == -1)
        return;
    if (*(unsigned short *)(b + 0x12ee) == v)
        k = 1;
    else if (*(unsigned short *)(b + 0x12ec) == v)
        k = 0;
    else
        return;
    off = 0x12f0 + k * 2;
    t = *(unsigned short *)(b + off);
    i = 0;
    do {
        q = *(unsigned char **)p;
        if (q[4] == 2 && q[0xe] == t) {
            CloseUIBox(p, 2);
            return;
        }
        i++;
        p += 0x24;
    } while (i != 8);
}
