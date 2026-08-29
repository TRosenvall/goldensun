struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void OvlFunc_898_2008938(int slot);

void OvlFunc_898_2008d78(void)
{
    struct A *a;

    a = __MapActor_GetActor(0xf);
    a->f64 |= 2;
    __CutsceneStart();
    if (__GetFlag(0x855))
        __MessageID(0x134b);
    else
        __MessageID(0x123d);
    OvlFunc_898_2008938(0xf);
    __CutsceneEnd();
    a = __MapActor_GetActor(0xf);
    a->f64 &= 1;
}
