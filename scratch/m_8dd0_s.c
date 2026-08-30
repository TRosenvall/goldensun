extern int _AREA_00;

struct Ent {
    unsigned char pad0[5];
    unsigned char f5;
    unsigned char pad6[0x16 - 6];
    unsigned char f16;
};

struct Obj {
    unsigned char pad0[0x26];
    unsigned char f26;
    unsigned char f27;
    struct Ent *f28[1];
};

struct Actor {
    unsigned char pad0[6];
    short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Obj *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(struct Actor *a, int n);

void OvlFunc_951_2008dd0(int slot, int *src, int h, int v, int sp)
{
    struct Actor *e;
    struct Obj *q;
    struct Ent *r;
    struct Ent **list;
    unsigned int i;
    unsigned int n;
    int m;
    int t;

    e = __MapActor_GetActor(slot);
    if (e != 0) {
        e->f8 = *src++;
        e->fc = *src++;
        e->f10 = *src;
        e->f6 = h;
        e->f55 = (int)&_AREA_00;
        e->f50->f26 = (int)&_AREA_00;
        __Actor_SetAnimSpeed(e, sp);
    }
    q = e->f50;
    i = 0;
    if (i < q->f27) {
        m = 0xff;
        list = q->f28;
        n = q->f27;
        do {
            r = *list++;
            if (r->f5 != v) {
                t = r->f16 | m;
                r->f5 = v;
                r->f16 = t;
            }
            n--;
        } while (n != 0);
    }
}
