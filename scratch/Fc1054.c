extern int Func_80b6c08(int kind, void *buf);
extern void Func_80c0f98(int id, int flag);

void Func_80c1054(void)
{
    short buf[0xe];
    short *base;
    int off;
    int n;

    n = Func_80b6c08(3, buf);
    if (n > 0) {
        base = buf;
        off = 0;
        do {
            Func_80c0f98(*(short *)((char *)base + off), 0);
            n--;
            off += 2;
        } while (n != 0);
    }
}
