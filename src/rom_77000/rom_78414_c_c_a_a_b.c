extern unsigned int gState;
extern int FindEmptyInventorySlot(int id);
extern int Func_80796c4(short *buf);

int Func_8078500(void)
{
    short buf[10];
    short *p;
    unsigned int r2;
    unsigned int r3;
    int n;
    int i;

    r3 = (unsigned int)&gState;
    r2 = 0xfa;
    r2 <<= 1;
    r3 += r2;
    if (FindEmptyInventorySlot(*(int *)r3) != 0xf)
        return 1;
    n = Func_80796c4(buf);
    p = buf;
    for (i = 0; i < n; i++) {
        if (FindEmptyInventorySlot(*p++) != 0xf)
            return 1;
    }
    return 0;
}
