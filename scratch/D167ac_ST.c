struct Src {
    unsigned char pad0[0x16];
    unsigned short f16;
    unsigned short f18;
    unsigned short f1a;
};

struct Dst {
    unsigned char pad0[0xea8];
    unsigned short fea8;
    unsigned short feaa;
    unsigned short feac;
    unsigned short feae;
};

extern struct Dst *iwram_3001e8c;

void Func_80167ac(struct Src *a)
{
    struct Dst *p;

    p = iwram_3001e8c;
    p->feae = a->f16;
    p->feac = a->f18;
    p->fea8 = a->f1a;
}
