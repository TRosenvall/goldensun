struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9_lo : 2;
    unsigned char f9_mid : 2;
    unsigned char f9_hi : 4;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[10];
    unsigned char f27;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x13];
    unsigned char f23;
    unsigned char pad24[0xc];
    int f30;
    unsigned char pad34[4];
    int f38;
    int f3c;
    unsigned char pad40[0x10];
    struct S *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char f56;
    unsigned char pad57[5];
    unsigned char f5c;
    unsigned char pad5d[4];
    unsigned char f61;
    unsigned char pad62[10];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern int __GetFlag(int id);
extern void *__galloc_iwram(int tag, int n);
extern void __gfree(int tag);
extern void __LoadItemIcon(int id);
extern void OvlFunc_946_200ae70(void);

void OvlFunc_946_200aed8(int slot)
{
    struct A *a;
    struct S *s;
    void *buf;
    int z;

    a = __MapActor_GetActor(slot);
    s = a->f50;
    s->f9_mid = 1;
    s->f5_b5 = 0;
    s->f9_hi = 0;
    z = 0;
    s->f27 = z;
    __Actor_SetSpriteFlags(a, 0);
    a->f5c = z;
    a->f55 = z;
    if (__GetFlag(0x109) == 0)
        a->fc += 0x80 << 14;
    a->f23 &= 0xfe;
    a->f61 = 1;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xb5);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
    a->f38 = a->f8;
    a->f30 = z;
    a->f3c = a->fc;
    a->f5c = 1;
    a->f6c = OvlFunc_946_200ae70;
    a->f56 = z;
}
