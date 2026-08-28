struct S {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_927_2008474(struct S *s);
extern void OvlFunc_927_2008608(struct S s);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_8092b08(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2008f94(void)
{
    struct S s;
    unsigned char *a;
    int v1, v2, m;
    int e1, f1;

    v1 = 0x80 << 7;
    v2 = 0x80 << 8;
    m = -0x10;
    __CutsceneStart();
    if (OvlFunc_927_2008474(&s) != 0) {
        OvlFunc_927_2008608(s);
        if (s.f04 == 9 && (s.f10 >> 20) == 0x1a) {
            __SetFlag(0xc4 << 2);
            __MapActor_SetAnim(9, 3);
            __MapActor_SetSpeed(9, v1, v2);
            __Func_809228c(9, 0, m);
            __CutsceneWait(0x2d);
            __MapActor_SetAnim(9, 8);
            __PlaySound(0xf0);
            __Func_8092b08(9, 1);
            a = __MapActor_GetActor(9);
            a += 0x23;
            *a = 2;
            e1 = 0x1f;
            f1 = 0x19;
            __Func_8010704(0x26, 0x1b, 4, 2, e1, f1);
        }
    }
    __CutsceneEnd();
}
