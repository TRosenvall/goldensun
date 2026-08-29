struct S {
    unsigned char pad00[9];
    unsigned char f9_lo : 2;
    unsigned char f9_mid : 2;
    unsigned char f9_hi : 4;
};

struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c[6];
    short f12;
    unsigned char pad14[0x3c];
    struct S *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x16];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092950(int a, int b);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern int __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809202c(void);
extern void OvlFunc_923_2008cc0(void);

void OvlFunc_923_2009208(void)
{
    struct A *a;
    struct S *s;
    int f;

    a = __MapActor_GetActor(0);
    f = __GetFlag(0x109);
    if (f == 0) {
        __CutsceneStart();
        __Func_80933f8(-1, -1, -1, 0);
        a->f55 = f;
        __MapActor_SetPos(0, a->fa << 16, (a->f12 << 16) + 0xfff00000);
        __Func_8092950(0, 0xf);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
        __MapTransitionIn();
        __WaitMapTransition();
        __PlaySound(0xe4);
        a->f6c = OvlFunc_923_2008cc0;
        __MapActor_SetSpeed(0, 0x6666, 0x3333);
        __Func_8092304(0, 0, 8);
        __Func_8092950(0, 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
        s = a->f50;
        s->f9_mid = 1;
        __Func_8092304(0, 0, 0xa);
        a->f55 = 3;
        a->f6c = (void *)f;
        __Func_809202c();
        __CutsceneEnd();
    }
}
