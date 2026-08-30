extern unsigned int iwram_3001e40;
extern unsigned char L9e6b8[] __asm__(".L9e6b8");

struct Ent {
    unsigned char pad0[5];
    unsigned char f5;
    unsigned char pad6[0x10 - 6];
    int f10;
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

void Func_808e0b0(unsigned char *e, int visible)
{
    struct Obj *o;
    struct Ent *q;
    struct Ent **list;
    unsigned int i;
    unsigned int n;
    int v;

    if ((*(unsigned char *)(e + 0x54) & 0xf) != 1)
        return;
    o = *(struct Obj **)(e + 0x50);
    v = visible - 1;
    if (visible == 0)
        v = L9e6b8[(iwram_3001e40 >> 1) & 7];
    i = 0;
    if (i < o->f27) {
        list = o->f28;
        n = o->f27;
        do {
            q = *list++;
            if (q != 0 && q->f10 != 0 && q->f5 != 0xf)
                q->f5 = v;
            n--;
        } while (n != 0);
    }
    o->f25 = 1;
}
