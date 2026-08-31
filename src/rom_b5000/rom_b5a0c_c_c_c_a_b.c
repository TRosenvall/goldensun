extern int Func_80c23c0(void);
extern int iwram_3001e74;

int Func_80b6cdc(void)
{
    char *s;
    int flag;
    int i;
    int off;
    int a;

    flag = Func_80c23c0();
    s = (char *)iwram_3001e74;
    for (i = 0; i <= 5; i++) {
        off = i * 2;
        a = off + 4;
        if (*(short *)(s + a) != 0)
            continue;
        if (flag != 0)
            break;
        if (i > 4)
            continue;
        a = off + 6;
        if (*(short *)(s + a) == 0)
            break;
    }
    return i != 6;
}
