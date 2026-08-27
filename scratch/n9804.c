extern char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern char *__MapActor_GetActor(int slot);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __MapActor_SetAnim(int slot, int n);
extern int __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void OvlFunc_945_200c86c(int slot);

void OvlFunc_945_2009804(int slot, int msg, int flag)
{
    char *a;
    unsigned short *q;

    __CutsceneStart();
    __MessageID(msg);
    __Func_8092c40(slot, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        OvlFunc_945_200c86c(slot);
        __MapActor_SetAnim(slot, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(slot, *(short *)(a + 0xa), *(short *)(a + 0x12));
        __MapActor_WaitMovement(slot);
        __MapActor_SetPos(slot, 0, 0);
        __SetFlag(0xc0 << 2);
        __SetFlag(flag);
    } else {
        q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *q += 1;
        OvlFunc_945_200c86c(slot);
    }
    __CutsceneEnd();
}
