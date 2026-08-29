extern unsigned char L84a8c[] __asm__(".L84a8c");
extern int *Func_8077330(int n);

int Func_807977c(unsigned char *dest)
{
    unsigned char *p;
    int n;
    int b;

    n = 0;
    for (p = L84a8c; p <= L84a8c + 0xf; p++) {
        b = *p;
        if (*Func_8077330(0) & (1 << b)) {
            *dest = b;
            n++;
            dest++;
        }
    }
    *dest = 0x20;
    return n;
}
