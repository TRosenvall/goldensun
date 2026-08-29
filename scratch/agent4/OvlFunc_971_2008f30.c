extern unsigned char gState[];
extern int __GetPartySize(void);
extern int __GetFlag(int id);

int OvlFunc_971_2008f30(int id)
{
    int n;
    int limit;
    int i;
    unsigned char *p;

    n = __GetPartySize();
    limit = 3;
    if (__GetFlag(0xb9 << 1) == 0)
        limit = 4;
    if (n > limit)
        n = limit;
    p = gState + (0xfc << 1);
    for (i = 0; i < n; i++) {
        if (p[i] == 0xff)
            return 0;
        if (p[i] == id)
            return 1;
    }
    return 0;
}
