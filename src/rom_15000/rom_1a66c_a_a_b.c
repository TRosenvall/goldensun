extern int iwram_3001e98;

void Func_801a778(void)
{
    char *p;
    unsigned short *f;
    int zero;

    p = (char *)iwram_3001e98;
    zero = 0;
    *(int *)(p + 0xd2 * 4) = zero;
    *(unsigned short *)(p + 0x39a) = zero;
    f = (unsigned short *)(p + 0x39e);
    if (*f & 0x80) {
        *(unsigned short *)(p + 0xe7 * 4) = zero;
        *f = zero;
    }
    *(unsigned short *)(p + 0xe8 * 4) = zero;
    *(unsigned short *)(p + 0x394) = zero;
}
