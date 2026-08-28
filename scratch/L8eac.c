extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(char *actor, int flags);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_8092950(int a, int b);
extern void __Func_8091e9c(int a);
extern void OvlFunc_965_2008cd0(char *actor);

void OvlFunc_965_2008eac(int a)
{
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(__MapActor_GetActor(0) + 0xc) = 0x82 << 16;
    *(int *)(__MapActor_GetActor(0) + 0x48) = 0x80 << 7;
    *(int *)(__MapActor_GetActor(0) + 0x44) = 0;
    *(char *)(__MapActor_GetActor(0) + 0x55) = 0;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xa);
    __PlaySound(0xcc);
    *(char *)(__MapActor_GetActor(0) + 0x55) = 3;
    *(int *)(__MapActor_GetActor(0) + 0x28) = 0xfffb0000;
    OvlFunc_965_2008cd0(__MapActor_GetActor(0));
    __Func_8092950(0, 0xf);
    __Func_8091e9c(a);
    __CutsceneEnd();
}
