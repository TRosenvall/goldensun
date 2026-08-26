struct A { unsigned char pad00[0xa]; short fa; unsigned char pad0c[6]; short f12; };

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern int OvlFunc_945_20092dc(void);
extern void OvlFunc_945_2009190(int slot);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_2009804(int a, int b, int c);

void OvlFunc_945_2008e14(void)
{
    int s;
    struct A *a;

    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(s);
        __MessageID(0x1ea3);
        OvlFunc_945_200c86c(0xd);
        __MapActor_SetAnim(s, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(s, a->fa, a->f12);
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x995);
    } else if (__GetFlag(0x92a)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x91c);
    } else if (__GetFlag(0x929)) {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x93a);
    } else {
        OvlFunc_945_2009804(0xd, 0x1e88, 0x931);
    }
}
