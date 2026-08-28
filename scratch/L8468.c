extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(char *actor, int flags);
extern void __Func_8092950(int a, int b);
extern void __Func_800fe9c(void);
extern void OvlFunc_944_20084b0(void);

void OvlFunc_944_2008468(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0, 0xa4 << 16, 0x1410000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_944_20084b0();
    __CutsceneEnd();
}
