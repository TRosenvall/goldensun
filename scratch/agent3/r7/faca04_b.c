extern char *iwram_3001f2c;
extern void Func_80acab8(int win, int a, int b, int id, int s0, int s1, int s2,
                         int s3, int s4);

int Func_80aca04(int which, int flags)
{
    char *p;

    p = iwram_3001f2c;
    if (which == 0) {
        Func_80acab8(*(int *)(p + 0x34), 0, 0, *(unsigned char *)(p + 0x259),
                     1, 0, 2, flags, 1);
        Func_80acab8(*(int *)(p + 0x24), 0, 0, *(unsigned char *)(p + 0x258),
                     0, 1, 2, flags, 0);
    } else {
        {
            int id = *(unsigned char *)(p + 0x21b);
            Func_80acab8(*(int *)(p + 0x34), 0, 0, id, 1, 0, 2, flags, 1);
        }
        Func_80acab8(*(int *)(p + 0x24), 0, 0, *(unsigned char *)(p + 0x21a),
                     0, 0, 1, flags, 0);
    }
    return 1;
}
