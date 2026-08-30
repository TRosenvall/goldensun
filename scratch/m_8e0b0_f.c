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
    unsigned int i;
    int v;

    if ((*(unsigned char *)(e + 0x54) & 0xf) != 1)
        return;
    o = *(struct Obj **)(e + 0x50);
    v = visible - 1;
    if (visible == 0)
        v = L9e6b8[(iwram_3001e40 >> 1) & 7];
    for (i = 0; i < o->f27; i++) {
        q = o->f28[i];
        if (q != 0 && q->f10 != 0 && q->f5 != 0xf)
            q->f5 = v;
    }
    o->f25 = 1;
}
