struct A {
    unsigned char pad00[6];
    short f6;
};

extern unsigned char gScript_926__0200c638[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_809280c(int a, int b, int c);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);

void OvlFunc_926_2008afc(void)
{
    int z;

    __CutsceneStart();
    __MapActor_GetActor(0xc)[0x5b] = 0;
    while (*(int *)(__MapActor_GetActor(0xc) + 0xc) > 0)
        __WaitFrames(1);
    z = 0;
    *(int *)(__MapActor_GetActor(0xc) + 0xc) = z;
    *(int *)(__MapActor_GetActor(0xc) + 0x3c) = 0x80 << 24;
    *(int *)(__MapActor_GetActor(0xc) + 0x28) = z;
    __MapActor_GetActor(0xc)[0x5b] = 1;
    __Func_809280c(0xc, 0, 0);
    if (__GetFlag(0x895))
        __MessageID(0x1a5b);
    else if (__GetFlag(0x89b))
        __MessageID(0x189e);
    else
        __MessageID(0x182a);
    __ActorMessage(0xc, 0);
    ((struct A *)__MapActor_GetActor(0xc))->f6 = 0x80 << 7;
    __MapActor_GetActor(0xc)[0x5b] = z;
    __MapActor_SetBehavior(0xc, gScript_926__0200c638);
    __CutsceneEnd();
}
