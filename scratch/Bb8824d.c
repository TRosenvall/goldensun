extern int Random(void);
extern int Func_80b6b40(int a, short *buf);
extern void Func_80b8064(int n);
extern void WaitFrames(int n);
extern void _Func_80175a0(int id);

int Func_80b8824(void)
{
    short buf[14];
    int n;
    int i;
    int off;
    char *base;
    char *walk;

    if ((((unsigned int)Random() << 4) >> 16) != 0) {
        base = (char *)buf;
        n = Func_80b6b40(1, (short *)base);
        i = 0;
        if (n != 0) {
            walk = base;
            off = 0;
            do {
                Func_80b8064(*(short *)(walk + off));
                i++;
                WaitFrames(8);
                off += 2;
            } while (i != n);
        }
        WaitFrames(0x16);
        return 1;
    }
    _Func_80175a0(0x844);
    return 0;
}
