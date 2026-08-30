extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);
extern void OvlFunc_944_20084b0(void);

void OvlFunc_944_2008468(void)
{
    int slot;

    __CutsceneStart();
    slot = 0;
    __MapActor_SetPos(slot, 0xa4 << 16, 0x1410000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_944_20084b0();
    __CutsceneEnd();
}
