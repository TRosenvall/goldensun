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
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009454(void)
{
    struct T t;
    unsigned char *e;
    int p1, q1, p2, q2, p3, q3;

    __CutsceneStart();
    if (OvlFunc_927_2008474(&t)) {
        OvlFunc_927_2008608(t);
        if (t.f4 == 8 && (t.f10 >> 20) == 0x17) {
            p1 = 0x23;
            q1 = 0x44;
            __Func_8010704(0x23, 0x43, 4, 1, p1, q1);
        } else if (t.f4 == 0xa && (t.f8 >> 20) == 0x23) {
            __SetFlag(0x311);
            __MapActor_SetAnim(0xa, 3);
            __Func_809228c(0xa, -0x10, 6);
            __CutsceneWait(0x1e);
            __MapActor_SetAnim(0xa, 8);
            __PlaySound(0xf0);
            e = __MapActor_GetActor(0xa);
            e[0x23] = 2;
            q3 = 0;
            p2 = 0x22;
            q2 = 0x1e;
            __Func_8010704(0x2c, 0x1e, 2, 4, p2, q2);
            p3 = 4;
            OvlFunc_927_2008244(2, 0x23, 0x1e, 1, p3, q3);
        }
    }
    __CutsceneEnd();
}
