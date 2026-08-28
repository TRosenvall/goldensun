extern char *iwram_3001e98;

char *Func_801a910(int alloc)
{
    char *b;
    char *q;
    int i;
    int off;

    b = iwram_3001e98;
    if (alloc != 0) {
        i = 0;
        q = b + (0xef << 1);
        off = 0;
        do {
            if (*(unsigned short *)q != 0)
                goto nextA;
            return b + off + (0xea << 1);
        nextA:
            i++;
            q += 0x34;
            off += 0x34;
        } while (i != 5);
        return 0;
    }
    i = 0;
    q = b + 0x72;
    off = 0;
    do {
        if (*(unsigned short *)q != 0)
            goto nextB;
        return b + 0x68 + off;
    nextB:
        i++;
        q += 0x34;
        off += 0x34;
    } while (i != 7);
    return 0;
}
