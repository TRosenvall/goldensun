extern void *Func_8004970(int size);
extern void Func_8017aa4(void *buf, int b, int c, int d);
extern void free(void *p);

void Func_801e858(unsigned char *s, int b, int c, int d)
{
    unsigned short *buf;
    unsigned short *w;

    buf = Func_8004970(0x80 << 2);
    w = buf;
    if (*s != 0) {
        do {
            *w = *s;
            s++;
            w++;
        } while (*s != 0);
    }
    *w = 0;
    Func_8017aa4(buf, b, c, d);
    free(buf);
}
