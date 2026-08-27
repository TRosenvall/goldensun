extern unsigned char *__MapActor_GetActor(int slot);
extern int  __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092950(int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809202c(void);
extern void OvlFunc_923_2008cc0(void);

void OvlFunc_923_2009208(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *s;
    int f;
    int n1;
    int n2;
    int n3;
    int s1;
    int s2;
    int m;

    n1 = -1;
    n2 = -1;
    n3 = -1;
    s1 = 0x6666;
    s2 = 0x3333;
    a = __MapActor_GetActor(0);
    f = __GetFlag(0x109);
    if (f != 0)
        return;
    __CutsceneStart();
    __Func_80933f8(n1, n2, n3, 0);
    p = a + 0x55;
    *p = f;
    __MapActor_SetPos(0, *(short *)(a + 0xa) << 16,
                      (*(short *)(a + 0x12) << 16) + 0xfff00000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xe4);
    *(void **)(a + 0x6c) = (void *)OvlFunc_923_2008cc0;
    __MapActor_SetSpeed(0, s1, s2);
    __Func_8092304(0, 0, 8);
    __Func_8092950(0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    s = *(unsigned char **)(a + 0x50);
    m = -13;
    s[9] = (m & s[9]) | 4;
    __Func_8092304(0, 0, 0xa);
    *p = 3;
    *(int *)(a + 0x6c) = f;
    __Func_809202c();
    __CutsceneEnd();
}
