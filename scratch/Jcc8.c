extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern int OvlFunc_945_20092dc(void);
extern void OvlFunc_945_2009190(void);
extern void OvlFunc_945_200c86c(int n);

void OvlFunc_945_2008cc8(void)
{
    int s;
    unsigned char *p;
    char *w;

    __CutsceneStart();
    if (__GetFlag(0xc0 << 2)) {
        s = OvlFunc_945_20092dc();
        OvlFunc_945_2009190();
        __MessageID(0x1ea2);
        OvlFunc_945_200c86c(9);
        __MapActor_SetAnim(s, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(s, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(s);
        __MapActor_SetPos(s, 0, 0);
    } else {
        __MessageID(0x1e84);
        __Func_8093040(9, 0, 0x3c);
        __Func_80925cc(9, 1);
        __Func_8092c40(9, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            OvlFunc_945_200c86c(9);
            __MapActor_SetAnim(9, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(9, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(9);
            __MapActor_SetPos(9, 0, 0);
            __SetFlag(0xc0 << 2);
            if (__GetFlag(0x92b))
                __SetFlag(0x991);
            else if (__GetFlag(0x92a))
                __SetFlag(0x918);
            else if (__GetFlag(0x929))
                __SetFlag(0x936);
            else
                __SetFlag(0x92d);
        } else {
            w = iwram_3001ebc;
            *(unsigned short *)(w + (0xec << 1)) += 1;
            OvlFunc_945_200c86c(9);
        }
    }
    __CutsceneEnd();
}
