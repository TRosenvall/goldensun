struct Actor {
    unsigned char pad0[0xc];
    int fc;
    unsigned char pad10[4];
    int f14;
    unsigned char pad18[0xa];
    unsigned char f22;
    unsigned char pad23[0x25];
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

extern unsigned char L2dd0[] __asm__(".L2dd0");
extern unsigned char ewram_2001000[];
extern unsigned char gState[];
extern int iwram_3001e70[];
extern int Func_8000888(int a, int b);

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8092950(int a, int b);
extern void __StartTask(void *f, int p);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int a, int b);
extern void __MapTransitionIn(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_Surprise(int slot, int v);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __PlaySound(int id);
extern void __SetDestMap(int map, int spot);
extern void __CutsceneWait(int n);
extern void __Func_8092b08(int slot, int n);
extern void __WaitFrames(int n);
extern void OvlFunc_918_2009224(void);
extern void OvlFunc_918_2009424(int a);
extern void OvlFunc_918_20097ec(void);
extern void OvlFunc_918_20098b8(void);
extern void OvlFunc_918_2008f58(int a);
extern void OvlFunc_918_2009244(void);

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

int OvlFunc_918_2009004(void)
{
    struct Actor *a8;
    struct Actor *a9;
    struct Actor *ac;
    unsigned char *b;
    unsigned char *q;
    short *st;
    unsigned char *gs;
    int k;
    int slot;

    a8 = __MapActor_GetActor(8);
    *(unsigned char **)L2dd0 = ewram_2001000;
    OvlFunc_918_2009224();
    a8->f55 = 0;
    a8->fc = 0xfff60000;
    a9 = __MapActor_GetActor(9);
    a9->f55 = 0;
    a9->fc = 0xfff60000;
    __Func_8092950(9, 0xf);
    OvlFunc_918_2009424(0);
    gs = gState;
    st = (short *)(gs + 0x1c2);
    if (*st != 0x13)
        __StartTask(OvlFunc_918_2009244, 0xc8 << 4);
    if (__GetFlag(0x844) != 0) {
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(8, 0, 0);
    }
    if (__GetFlag(0x109) != 0)
        OvlFunc_918_20097ec();
    b = (unsigned char *)iwram_3001e70[0];
    q = b + 0x104;
    *(int *)(q + 8) += call_via(Func_8000888, *(int *)(b + 0xec) + (0xa0 << 16), 0x1999);
    *(int *)(q + 0xc) += call_via(Func_8000888, *(int *)(b + 0xf0) + (0x88 << 16), 0x1999);
    *(int *)(q + 0x10) = 0xe666;
    *(int *)(q + 0x14) = 0xe666;
    __SetFlag(0x201);
    __SetFlag(0x20d);
    __SetFlag(0x20f);
    __SetFlag(0x213);
    __WaitFrames(1);
    OvlFunc_918_2008f58(0);
    *(int *)(iwram_3001e70[0x13] + 0x1c0) = 0x202;
    k = *st;
    slot = *(int *)(gs + 0x1f4);
    ac = __MapActor_GetActor(slot);
    if (k == 0x32 || k == 0x28 || k == 0x1e || k == 0x14) {
        __MapTransitionIn();
        __MapActor_SetAnim(slot, 0x1b);
        __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
        __MapActor_Surprise(slot, 0x101);
        __Func_80933f8(-1, -1, -1, 0);
        ac->f55 = 2;
        ac->fc = 0xc8 << 15;
        ac->f14 = 0xff600000;
        ac->f48 = 0x80 << 8;
        __PlaySound(0xcc);
        __SetDestMap(0x2d, k - 0xa);
        __CutsceneWait(0x14);
        ac->f22 = 2;
        __Func_8092b08(slot, 3);
        __CutsceneWait(2);
        __MapActor_Surprise(slot, 0x80 << 1);
        __CutsceneWait(8);
    } else if (k == 0xa) {
        if (__GetFlag(0x109) == 0)
            OvlFunc_918_20098b8();
    } else if (k == 0x13) {
        OvlFunc_918_2008f58(1);
    }
    return 0;
}
