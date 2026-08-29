extern void *Func_8004970(int size);
extern void Func_8017c8c(void *buf, int b, unsigned int c, unsigned int d);
extern void free(void *p);

void UIDrawText(unsigned char *s, int b, unsigned int c, unsigned int d)
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
    c >>= 3;
    d >>= 3;
    Func_8017c8c(buf, b, c, d);
    free(buf);
}
