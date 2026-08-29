extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091e9c(int n);
extern void OvlFunc_945_200c670(int n);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200bf94(void)
{
    char *p;

    __CutsceneStart();
    __MapActor_SetAnim(9, 5);
    OvlFunc_945_200c8e8(0x18, 1, 0);
    __MapActor_SetPos(0, 0, 0);
    OvlFunc_945_200c8e8(0x11, 0, 0);
    OvlFunc_945_200c670(0);
    OvlFunc_945_200c8e8(8, 1, 0x14);
    __Func_80933d4(0x6666, 0xccc);
    __Func_80933f8(0xdc << 17, -1, 0xb0 << 16, 1);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(9, 7);
    __CutsceneWait(0x1e);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    OvlFunc_945_200c670(0x10);
    __CutsceneWait(0x50);
    OvlFunc_945_200c670(0);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(9, 7);
    __CutsceneWait(0x1e);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    OvlFunc_945_200c670(0x10);
    __CutsceneWait(0x50);
    OvlFunc_945_200c670(0);
    __CutsceneWait(0x5a);
    __PlaySound(0xbc);
    __CutsceneWait(0x1e);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x203;
    OvlFunc_945_200c8e8(9, 0, 0);
    if (__GetFlag(0x92b))
        __Func_8091e9c(0x14);
    else if (__GetFlag(0x92a))
        __Func_8091e9c(0x12);
    else if (__GetFlag(0x929))
        __Func_8091e9c(0x11);
    else if (__GetFlag(0x928))
        __Func_8091e9c(0x10);
    else
        __Func_8091e9c(0xd);
}
