extern unsigned char gState[];
extern int __GetPartySize(void);
extern int __GetFlag(int id);

int OvlFunc_971_2008f30(int id)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    int n;
    int limit;
    int i;

    n = __GetPartySize();
    limit = 3;
    if (__GetFlag(0xb9 << 1) == 0)
        limit = 4;
    if (n > limit)
        n = limit;
    i = 0;
    if (i < n) {
        g = gState;
        p = g + (0xfc << 1);
        q = p;
        do {
            if (*q++ == 0xff)
                return 0;
            if (*p == id)
                return 1;
            p++;
            i++;
        } while (i < n);
    }
    return 0;
}
