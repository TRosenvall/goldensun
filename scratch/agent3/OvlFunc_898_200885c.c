struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x5c];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int a);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_200885c(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    short saved;

    a = __MapActor_GetActor(0xf);
    saved = a->f6;
    p = &a->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x122d);
    __MapActor_SetAnim(0xf, 0);
    OvlFunc_898_200973c(0xf, 0, 2);
    OvlFunc_898_2009724(0xf, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p &= 1;
}
