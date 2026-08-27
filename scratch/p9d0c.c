extern unsigned char L5160[] __asm__(".L5160");
extern char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __LoadFieldActors(unsigned char *p);
extern void __WaitFrames(int n);
extern void __Func_8092950(int a, int b);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_943_200ba00(int a, int b);
extern void __MapActor_Jump(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapTransitionOut(void);
extern void __Func_8091e9c(int n);

void OvlFunc_943_2009d0c(void)
{
    char *p;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80925cc(0x14, 1);
    __MessageID(0x1e41);
    __Func_8093040(0x14, 0, 0xa);
    OvlFunc_943_200ba00(0x16, 0xa0 << 7);
    __MapActor_Jump(0x16, 4, 0x14);
    __Func_809259c(0x16, 2);
    __Func_8093040(0x6016, 0, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xb);
}
