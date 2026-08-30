extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23a0(unsigned int i)
{
    unsigned char *t;
    unsigned int idx;
    unsigned char *p;

    if (i > 0xab)
        return *(unsigned short *)Lc7420;
    t = Lc7420;
    idx = i << 3;
    p = (unsigned char *)(idx + (unsigned int)t);
    return (unsigned int)(p[3] << 27) >> 28;
}
