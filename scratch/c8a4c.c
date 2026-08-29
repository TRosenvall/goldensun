struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern char *iwram_3001ebc;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_2008a4c(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short *q;
    unsigned short two;
    short saved;

    a = __MapActor_GetActor(0xe);
    p = &a->f64;
    saved = a->f6;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1339);
    if (__GetFlag(2)) {
        q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *q = *q + 1;
    }
    __MapActor_SetAnim(0xe, 0);
    OvlFunc_898_200973c(0xe, 0, 2);
    OvlFunc_898_2009724(0xe, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = *p & 1;
}
