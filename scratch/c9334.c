struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int a);
extern void OvlFunc_926_2008e94(void);
extern void OvlFunc_926_2008bf4(void);
extern void OvlFunc_926_2008db4(void);
extern void OvlFunc_926_2008cd4(void);
extern void OvlFunc_926_200902c(int n);

void OvlFunc_926_2009334(void)
{
    struct E *e;
    unsigned short a;

    e = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(0x14);
    a = e->f6;
    if ((unsigned short)(a - 0x2000) <= 0x3fff)
        OvlFunc_926_2008e94();
    else if ((unsigned short)(a - 0x6000) <= 0x3fff)
        OvlFunc_926_2008bf4();
    else if ((unsigned short)(a + 0x6000) <= 0x3fff)
        OvlFunc_926_2008db4();
    else
        OvlFunc_926_2008cd4();
    __MapActor_SetAnim(0, 1);
    OvlFunc_926_200902c(1);
    __CutsceneEnd();
}
