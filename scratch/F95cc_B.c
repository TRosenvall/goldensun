struct S { int a, b, c, d, e, f; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int OvlFunc_934_2008758(struct S *s);
extern void OvlFunc_934_20088ec(struct S s);
extern void OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);

void OvlFunc_934_20095cc(void)
{
    struct S s;
    unsigned char *p;
    int k;
    int q1, q2;

    q1 = 0x80 << 7;
    q2 = 0x80 << 8;
    __CutsceneStart();
    if (OvlFunc_934_2008758(&s)) {
        OvlFunc_934_20088ec(s);
        __MapActor_SetAnim(0xb, 3);
        __MapActor_SetSpeed(0xb, q1, q2);
        __Func_809228c(0xb, 0, -0x10);
        __CutsceneWait(0x2d);
        __PlaySound(0xf0);
        __MapActor_SetAnim(0xb, 8);
        p = __MapActor_GetActor(0xb);
        p[0x23] = 2;
        OvlFunc_934_2008528(0, 0xd, (s.e >> 20) - 1, 4, 2, 0);
        if ((s.e >> 20) == 0x14) {
            __SetFlag(0x205);
        } else {
            __SetFlag(0x81 << 2);
            k = 0xe;
            __Func_8010704(0xe, 0x11, 2, 1, k, 0x10);
            __Func_8010704(0xe, 0xd, 1, 1, k, 0xf);
        }
    }
    __CutsceneEnd();
}
