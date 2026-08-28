struct Sub {
    unsigned char pad0[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
    unsigned char pad_a[0x14];
    short f1e;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[4];
    int f24;
    int f28;
    int f2c;
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0xc];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern int Func_8000888(int a, int b);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __PlaySound(int id);
extern int __cos(int a);
extern int __sin(int a);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __CutsceneWait(int n);
extern void __MapActor_WaitMovement(int slot);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void OvlFunc_897_2008f64(void)
{
    struct Actor *a;
    struct Actor *b;
    unsigned int i;
    int s, x, y;

    for (i = 0x10; i <= 0x1f; i++) {
        a = __MapActor_GetActor(i);
        __MapActor_SetIdle(i);
        __Actor_SetSpriteFlags(a, 0);
        __Actor_SetAnim(a, 2);
        a->f50->sel = 0;
        a->f55 = 0;
        a->f30 = 0x80 << 12;
        a->f34 = 0xc0 << 8;
        a->f18 = 0x1cccc;
        a->f1c = 0x1cccc;
        a->f8 = 0xe8 << 16;
        a->fc = 0xa0 << 13;
        a->f10 = 0x84 << 16;
    }
    __PlaySound(0x91);
    {
        register int (*g)(int, int) __asm__("r10") = Func_8000888;

        for (i = 0; i <= 0xf; i++) {
            b = __MapActor_GetActor(i + 0x10);
            s = i << 12;
            b->f50->f1e = s + 0xffffc000;
            x = call_via(g, __cos(s), 0x80 << 17);
            y = call_via(g, __sin(s), 0x80 << 17);
            __Actor_TravelTo(b, b->f8 + x, b->fc, b->f10 + y);
        }
    }
    __CutsceneWait(0x14);
    __MapActor_WaitMovement(0x10);
    for (i = 0x10; i <= 0x1f; i++) {
        a = __MapActor_GetActor(i);
        __MapActor_SetIdle(i);
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
        a->f8 = 0;
        a->fc = 0;
        a->f10 = 0;
        a->f24 = 0;
        a->f28 = 0;
        a->f2c = 0;
        a->f38 = 0x80 << 24;
        a->f3c = 0x80 << 24;
        a->f40 = 0x80 << 24;
    }
}
