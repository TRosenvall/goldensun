extern unsigned char *g(int);
void t(int s)
{
    unsigned char *p;
    unsigned char *q;
    p = g(s);
    p[0x55] = 0;
    q = *(unsigned char **)(p + 0x50);
    q[0x26] = 0;
}
