struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[9];
    unsigned char f26;
    unsigned char f27;
};

struct A {
    unsigned char pad00[0x50];
    struct S *spr;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[6];
    unsigned char f5c;
};

extern struct A *__CreateActor(int kind, int x, int y, int z);
extern void *__galloc_iwram(int tag, int size);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int slot, int n, void *src);
extern void __gfree(int tag);
extern struct A *gSlot __asm__(".L2488");

void OvlFunc_922_2009b1c(void)
{
    struct A *act;
    struct S *s;
    void *buf;
    int z;

    z = 0;
    act = __CreateActor(0x16, 0xf8 << 16, 0x80 << 12, 0x98 << 16);
    if (act == 0)
        return;
    s = act->spr;
    s->f26 = z;
    s->f27 = z;
    s->f5_b5 = 0;
    s->f9 &= 0xf;
    act->f55 = z;
    act->f5c = 1;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xe6);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
    gSlot = act;
}
