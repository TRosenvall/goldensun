extern int __GetFlag(int flag);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int OvlFunc_945_20092dc(void);
extern void OvlFunc_945_2009190(int a);
extern void OvlFunc_945_200c86c(int a);
extern void OvlFunc_945_2009804(int a, int b, int c);

void OvlFunc_945_20088ec(void)
{
    unsigned char *p;
    int slot;

    if (__GetFlag(0xc0 << 2) != 0) {
        slot = OvlFunc_945_20092dc();
        __CutsceneStart();
        OvlFunc_945_2009190(slot);
        __MessageID(0x1e9e);
        OvlFunc_945_200c86c(8);
        __MapActor_SetAnim(slot, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(slot, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(slot);
        __MapActor_SetPos(slot, 0, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x92b) != 0) {
        OvlFunc_945_2009804(8, 0x1e78, 0x99 << 4);
    } else if (__GetFlag(0x92a) != 0) {
        OvlFunc_945_2009804(8, 0x1e78, 0x917);
    } else if (__GetFlag(0x929) != 0) {
        OvlFunc_945_2009804(8, 0x1e78, 0x935);
    } else {
        OvlFunc_945_2009804(8, 0x1e78, 0x92c);
    }
}
