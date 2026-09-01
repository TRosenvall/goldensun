extern int Func_80b6c08(int kind, unsigned short *buf);

int Func_80be070(unsigned int a)
{
    unsigned short buf[8];
    int kind;
    int n;
    int i;

    kind = 1;
    if (a > 7)
        kind = 2;
    n = Func_80b6c08(kind, buf);
    for (i = 0; i < n; i++) {
        if (buf[i] == a)
            break;
    }
    return i != n;
}
