struct A { unsigned char pad00[0x64]; unsigned short f64; };
extern int _CONST_2;
extern int _MSG_1cc0;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_901_20084b4(int slot);
extern void OvlFunc_901_200858c(void);

void OvlFunc_901_2008804(void)
{
    unsigned short *p;
    unsigned short two;
    unsigned short one;

    p = &__MapActor_GetActor(0xe)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    if (__GetFlag(0x307)) {
        __MessageID((int)&_MSG_1cc0);
        OvlFunc_901_20084b4(0xe);
    } else {
        OvlFunc_901_200858c();
        __SetFlag(0x307);
    }
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xe)->f64;
    one = 1;
    *p = one;
}
