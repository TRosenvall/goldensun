extern unsigned int iwram_3001e40;

struct Ent {
    unsigned char pad0[5];
    unsigned char f5;
};

struct Obj {
    unsigned char pad0[0x1d];
    unsigned char flags;
    unsigned char pad1e[0x25 - 0x1e];
    unsigned char f25;
    unsigned char pad26;
    unsigned char f27;
    struct Ent *f28[1];
};

void Func_8096b88(unsigned char *e)
{
    struct Obj *o;
    struct Ent *q;
    struct Ent **list;
    unsigned int i;
    unsigned int n;

    if (*(unsigned char *)(e + 0x54) != 1)
        return;
    o = *(struct Obj **)(e + 0x50);
    if (o == 0)
        return;
    if (o->flags & 1)
        return;
    if (0 < o->f27) {
        n = o->f27;
        list = o->f28;
        do {
            q = *list++;
            q->f5 = iwram_3001e40 % 6;
            n--;
        } while (n != 0);
    }
    o->f25 = 1;
}
