struct T {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_915_2008474(struct T *t);
extern void OvlFunc_915_2008608(struct T t);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_915_2008244(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);

void OvlFunc_915_20089f8(void)
{
    struct T t;
    unsigned char *e;
    int p1, q1, p2, q2;

    __CutsceneStart();
    if (OvlFunc_915_2008474(&t)) {
        OvlFunc_915_2008608(t);
        if (t.f4 == 0xa && (t.f8 >> 20) == 0xc) {
            __MapActor_SetAnim(0xa, 3);
            __Func_809228c(0xa, -0x12, 6);
            __CutsceneWait(0x1e);
            __PlaySound(0xf0);
            __MapActor_SetAnim(0xa, 8);
            e = __MapActor_GetActor(0xa);
            e[0x23] = 2;
            q2 = 0;
            p1 = 0xb;
            q1 = 0x10;
            __Func_8010704(0x20, 0x14, 2, 4, p1, q1);
            p2 = 4;
            OvlFunc_915_2008244(2, 0xc, 0x10, 1, p2, q2);
            __SetFlag(0x201);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        }
    }
    __CutsceneEnd();
}
