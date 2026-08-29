struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void OvlFunc_901_20084b4(int slot);

void OvlFunc_901_2008864(void)
{
    unsigned short *p;

    p = &__MapActor_GetActor(0xf)->f64;
    *p |= 2;
    __CutsceneStart();
    __MessageID(0x1cc1);
    OvlFunc_901_20084b4(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p = 0;
}
