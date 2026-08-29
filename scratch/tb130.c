extern char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __Func_8091220(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_8092950(int a, int b);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_80933d4(int a, int b);
extern int __StartTask(void *f, int n);
extern void OvlFunc_881_200b1fc(void);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __SetFlag(int id);

void OvlFunc_881_200b130(void)
{
    char *p;

    __CutsceneStart();
    __PlaySound(0x8d);
    __Func_8091220(0, 0);
    __Func_8091200(0, 0);
    __Func_8091254(1);
    __WaitFrames(2);
    p = iwram_3001ebc;
    *(int *)(p + (0xe4 << 1)) = 1;
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __Func_80933d4(0x80 << 11, 0x80 << 8);
    __StartTask(OvlFunc_881_200b1fc, 0xc8 << 4);
    __Func_8091220(0, 0);
    __Func_8091200(0x10004, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(0x28);
    __CutsceneWait(0xf0);
    __Func_8091200(0, 0);
    __Func_8091254(0x50);
    __WaitFrames(0x5a);
    __Func_8091e9c(0x6d);
    __SetFlag(0x8d << 1);
    __CutsceneEnd();
}
