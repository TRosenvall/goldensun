struct Ent {
    unsigned char pad0[5];
    unsigned char f5;
    unsigned char pad6[0x10 - 6];
    int f10;
};

struct Obj {
    unsigned char pad0[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
    unsigned char pada[0x25 - 0xa];
    unsigned char f25;
    unsigned char pad26;
    unsigned char f27;
    struct Ent *f28[1];
};

struct Actor {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    unsigned char padc[4];
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Obj *f50;
    unsigned char pad54[0x6c - 0x54];
    void (*f6c)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_888_200a67c(void);

void OvlFunc_888_200a5c4(void)
{
    struct Actor *a;
    struct Actor *b;
    struct Obj *o;
    struct Ent *e;
    unsigned int i;

    a = __MapActor_GetActor(8);
    if (a != 0) {
        __MapActor_SetPos(0xe, a->f8, a->f10);
    }
    __MapActor_SetAnim(0xe, 0);
    b = __MapActor_GetActor(0xe);
    b->f6 = __MapActor_GetActor(8)->f6;
    __MapActor_GetActor(0xe)->f6c = OvlFunc_888_200a67c;
    o = __MapActor_GetActor(0xe)->f50;
    for (i = 0; i < o->f27; i++) {
        e = o->f28[i];
        if (e != 0 && e->f10 != 0) {
            e->f5 = 0xa;
        }
    }
    o->f25 = 1;
    __MapActor_GetActor(0xe)->f23 &= 0xfe;
    o->sel = 2;
}
