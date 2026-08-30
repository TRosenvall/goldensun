extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23a0(unsigned int i)
{
    unsigned char *p;

    if (i > 0xab)
        return *(unsigned short *)Lc7420;
    p = Lc7420 + i * 8;
    return (unsigned int)(p[3] << 27) >> 28;
}
