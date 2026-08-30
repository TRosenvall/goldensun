extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23a0(unsigned int i)
{
    unsigned char *t;
    unsigned char *p;

    t = Lc7420;
    if (i > 0xab)
        return *(unsigned short *)t;
    p = t + i * 8;
    return (unsigned int)(p[3] << 27) >> 28;
}
