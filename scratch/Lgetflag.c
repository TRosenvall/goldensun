extern unsigned char gFlags[];

int GetFlag(int idx)
{
    unsigned int i;
    unsigned int t;
    int m;
    int v;
    int r;

    m = 1 << (idx & 7);
    t = idx << 20;
    i = t >> 23;
    v = gFlags[i] & m;
    r = -v;
    r |= v;
    return (unsigned int)r >> 31;
}
