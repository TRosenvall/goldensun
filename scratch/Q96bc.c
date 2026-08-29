struct Sprite {
    unsigned char pad0[5];
    unsigned char f05;
    unsigned char pad1[3];
    unsigned char f09;
    unsigned char pad2[0x12];
    unsigned char f1c;
    unsigned char pad3[0xa];
    unsigned char f27;
};

struct Actor {
    unsigned char pad0[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad1[0x1c];
    int f30;
    unsigned char pad2[4];
    int f38;
    int f3c;
    int f40;
    unsigned char pad3[0xc];
    struct Sprite *sprite;
    unsigned char pad4[1];
    unsigned char f55;
    unsigned char pad5[6];
    unsigned char f5c;
    unsigned char pad6[4];
    unsigned char f61;
};

extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092950(int a, int b);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int n);
extern void __UploadSpriteGFX(int a, int b, unsigned char *p);
extern void __StartTask(void (*fn)(void), int n);
extern void OvlFunc_936_200b90c(void);

void OvlFunc_936_20096bc(void)
{
    struct Actor *a;
    struct Sprite *s;
    unsigned char *p;
    int f;
    int off;
    int v;

    if (__GetFlag(0x941)) {
        __SetFlag(0x321);
        __SetFlag(0x913);
        __SetFlag(0x912);
        __SetFlag(0x915);
    }
    if (__GetFlag(0x94 << 4))
        __SetFlag(0x321);
    off = 0xe1 << 1;
    if (*(short *)(gState + off) == 0xe)
        __MapActor_SetPos(0x19, 0xd4 << 17, 0xb0 << 15);
    __Func_8092950(0x15, 2);
    f = __GetFlag(0x916);
    if (f) {
        __MapActor_SetPos(0x1a, 0, 0);
    } else {
        a = __MapActor_GetActor(0x1a);
        s = a->sprite;
        v = s->f09;
        v = v & -13;
        v = v | 4;
        s->f05 = s->f05 & -0x21;
        v = v & 0xf;
        s->f09 = v;
        s->f27 = f;
        a->f5c = 1;
        a->f55 = f;
        a->f0c = 0xa0 << 12;
        a->f61 = 1;
        p = __galloc_iwram(0x11, 0xc1 << 3);
        __LoadItemIcon(0xb5);
        __UploadSpriteGFX(s->f1c, 0x80, p + (0x80 << 3));
        __gfree(0x11);
        a->f30 = f;
        a->f38 = a->f08;
        a->f3c = a->f0c;
        a->f40 = a->f10;
        __StartTask(OvlFunc_936_200b90c, 0xc8 << 4);
    }
}
