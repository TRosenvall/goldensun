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
extern int OvlFunc_927_2008474(struct T *t);
extern void OvlFunc_927_2008608(struct T t);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_927_20099b8(void)
{
    struct T t;
    unsigned char *e;
    int f;

    f = 0;
    __CutsceneStart();
    if (OvlFunc_927_2008474(&t)) {
        OvlFunc_927_2008608(t);
        if (t.f4 == 9) {
            __Func_8010704(0x26, 0x44, 1, 4, t.f8 >> 20, 0x44);
            if ((t.f8 >> 20) == 0x2a) {
                __Func_8010704(0x1a, 0x14, 2, 4, t.f8 >> 20, 0x17);
                __Func_8092b08(9, 1);
                f = 1;
                __SetFlag(0x312);
            }
        } else if (t.f4 == 0xb) {
            if ((t.f8 >> 20) == 0x28) {
                __Func_8010704(0x1a, 0x14, 2, 4, t.f8 >> 20, 0x20);
                __Func_8092b08(0xb, 1);
                f = 1;
                __SetFlag(0x313);
            }
        } else if (t.f4 == 8) {
            __Func_8010704(0x2a, 0x31, 1, 4, t.f8 >> 20, 0x31);
        }
        if (f == 0) {
            __CutsceneEnd();
            return;
        }
        __MapActor_SetAnim(t.f4, 3);
        __Func_809228c(t.f4, 0x12, 6);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(t.f4, 8);
        __PlaySound(0xf0);
        e = __MapActor_GetActor(t.f4);
        e[0x23] = 2;
    }
    __CutsceneEnd();
}
