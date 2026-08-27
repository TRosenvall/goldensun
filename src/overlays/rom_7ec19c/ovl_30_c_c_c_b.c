struct Spr {
    unsigned char pad0[9];
    unsigned char f9_0 : 2;
    unsigned char f9_2 : 2;
    unsigned char f9_4 : 4;
    unsigned char pad_a[0x14];
    unsigned short f1e;
    unsigned char pad20[6];
    unsigned char f26;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern void __SetFlag(int flag);
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_962_2008a78(void)
{
    unsigned char *g;
    unsigned char *base;
    unsigned char *p;
    struct Spr *s1;
    struct Spr *s2;
    struct Spr *s3;
    int z;
    int h;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 0x5a)
        __SetFlag(0x96f);
    base = iwram_3001ebc;
    *(int *)(base + (0xe0 << 1)) = 0x209;
    *(int *)(base + 0x1c8) = 0x18;
    p = __MapActor_GetActor(0xc);
    p += 0x59;
    *p |= 4;
    z = 0;
    p = __MapActor_GetActor(0xd);
    p += 0x59;
    *p |= 4;
    p = __MapActor_GetActor(0x14);
    s1 = *(struct Spr **)(p + 0x50);
    s1->f26 = z;
    h = 0x80 << 7;
    s2 = *(struct Spr **)(p + 0x50);
    s2->f1e = h;
    s3 = *(struct Spr **)(p + 0x50);
    s3->f9_2 = 1;
    p = __MapActor_GetActor(0x15);
    s1 = *(struct Spr **)(p + 0x50);
    s1->f26 = 0;
    s2 = *(struct Spr **)(p + 0x50);
    s2->f1e = h;
    *(p + 0x55) = 2;
    *(int *)(p + 0xc) = z;
    return 0;
}
