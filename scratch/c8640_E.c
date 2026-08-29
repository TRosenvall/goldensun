struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_901_2008640(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    unsigned short zero;
    short saved;

    a = __MapActor_GetActor(0xf);
    p = &a->f64;
    saved = a->f6;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1cb4);
    __MapActor_SetAnim(0xf, 0);
    __Func_8092848(0xf, 0, 2);
    __Func_8093040(0xf, 0, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    zero = 0;
    *p = zero;
}
