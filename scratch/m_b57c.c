extern char *iwram_3001ebc;
extern unsigned char gScript_881__0200d218[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void *__MapActor_GetActor(int slot);
extern void __Func_80936a0(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *s);
extern int __StartTask(void *f, int n);
extern void __Func_8091200(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);
extern void OvlFunc_881_200b4a0(void);

void OvlFunc_881_200b57c(void)
{
    char *e;
    int off;

    e = __MapActor_GetActor(8);
    __CutsceneWait(0x3c);
    __CutsceneStart();
    __Func_80936a0(0x9999, 1);
    *(int *)(e + 0x18) = 0x13333;
    *(int *)(e + 0x1c) = 0x13333;
    __SetCameraTarget(8, 1);
    __WaitFrames(1);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __MapActor_SetSpeed(8, 0x6666, 0x3333);
    *(short *)(e + 0x64) = 0;
    __MapActor_SetBehavior(8, gScript_881__0200d218);
    __StartTask(OvlFunc_881_200b4a0, 0xc8 << 4);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __Func_8091200(0x10003, 1);
    off = 0xe4 << 1;
    *(int *)(iwram_3001ebc + off) = 0x20;
    __MapTransitionIn();
    __CutsceneWait(0x78);
    __Func_80936a0(0x16666, 0x96 << 1);
    __CutsceneWait(0x87 << 1);
    *(int *)(iwram_3001ebc + off) = 0x10;
    *(short *)(0xa0 << 19) = 0x7fff;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x6f);
}
