

struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void OvlFunc_898_2008938(int slot);

void OvlFunc_898_2008d78(void)
{
    unsigned short *p;

    p = &__MapActor_GetActor(0xf)->f64;
    *p |= 2;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0)
        __MessageID(0x123d);
    else
        __MessageID(0x134b);
    OvlFunc_898_2008938(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p &= 1;
}


/* --- the twin shape, same blocker, 2 of 24 --- */
