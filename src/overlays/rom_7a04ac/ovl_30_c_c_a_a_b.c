typedef struct {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
} S;

extern int OvlFunc_913_2008474(S *s);
extern void OvlFunc_913_2008608(S s);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int n);
extern void *__MapActor_GetActor(int a);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_809228c(int a, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_913_2008a68(void)
{
    S s;
    int m;
    int p1;
    int p2;
    int q1;
    int q2;

    m = -0x12;
    __CutsceneStart();
    if (OvlFunc_913_2008474(&s) != 0) {
        OvlFunc_913_2008608(s);
        if (s.b == 0xa) {
            if ((s.c >> 20) == 0x14) {
                __MapActor_SetAnim(0xa, 3);
                __Func_809228c(0xa, m, 6);
                __CutsceneWait(0x1e);
                __PlaySound(0xf0);
                __MapActor_SetAnim(0xa, 8);
                ((char *)__MapActor_GetActor(0xa))[0x23] = 2;
                q2 = 0;
                p1 = 0x13;
                p2 = 0x11;
                __Func_8010704(0, 0x11, 2, 4, p1, p2);
                q1 = 4;
                OvlFunc_913_2008244(2, 0x14, 0x11, 1, q1, q2);
                __SetFlag(0x80 << 2);
                __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
            }
        }
    }
    __CutsceneEnd();
}
