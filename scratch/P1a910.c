struct Rec {
    unsigned char pad00[0xa];
    unsigned short flag;
    unsigned char pad0c[0x34 - 0xc];
};

struct Pool {
    unsigned char pad000[0x68];
    struct Rec small[7];
    struct Rec large[5];
};

extern char *iwram_3001e98;

struct Rec *Func_801a910(int alloc)
{
    struct Pool *p;
    struct Rec *r;
    int i;

    p = (struct Pool *)iwram_3001e98;
    if (alloc != 0) {
        r = p->large;
        i = 0;
        do {
            if (r->flag == 0)
                return r;
            i++;
            r++;
        } while (i != 5);
        return 0;
    }
    r = p->small;
    i = 0;
    do {
        if (r->flag == 0)
            return r;
        i++;
        r++;
    } while (i != 7);
    return 0;
}
