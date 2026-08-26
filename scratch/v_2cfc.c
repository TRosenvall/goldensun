struct A { unsigned char pad00[0x64]; unsigned short f64; };
extern int _CONST_2;
extern unsigned char iwram_3001ebc[];
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void OvlFunc_898_2008938(int slot);

void OvlFunc_898_2008cfc(void)
{
    unsigned short *p;
    unsigned short two;
    char *base;

    p = &__MapActor_GetActor(0xe)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0) {
        __MessageID(0x123c);
    } else {
        __MessageID(0x1349);
        if (__GetFlag(2)) {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
        }
    }
    OvlFunc_898_2008938(0xe);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xe)->f64;
    *p &= 1;
}
