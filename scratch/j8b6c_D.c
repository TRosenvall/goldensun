struct Entry {
    void *script;
    unsigned short a;
    unsigned short b;
};

extern char *iwram_3001ebc;
extern struct Entry L250c[] __asm__(".L250c");
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern int __Func_8010560(void *s, int a, int b);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern int __MapActor_SetAnim(int slot, int n);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_939_2008b6c(void)
{
    char *p;
    char *a;
    short *e;
    unsigned int i;
    int z;
    int n;
    int vx;
    int vz;
    int d;
    struct Entry *t;
    int ea;
    int eb;

    p = iwram_3001ebc;
    vx = 0x80 << 8;
    vz = 0x80 << 7;
    d = -8;
    t = L250c;
    __CutsceneStart();
    i = 8;
    z = 0;
    for (; i <= 0x41; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0)
            a[0x55] = z;
    }
    __PlaySound(0x9e);
    e = (short *)(p + (0xb6 << 1));
    n = *e - 4;
    ea = t[n].a;
    eb = t[n].b;
    __Func_8010560(t[n].script, ea, eb);
    __MapActor_SetSpeed(0, vx, vz);
    *(__MapActor_GetActor(0) + 0x55) = 0;
    __MapActor_SetAnim(0, 2);
    if (n != 6) {
        __Func_8092208(0, 2, d);
        __CutsceneWait(0xa);
    }
    __Func_8091e9c(*e);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
