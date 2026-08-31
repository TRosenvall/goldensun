extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_WaitMovement(int slot);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);

void OvlFunc_923_200996c(void)
{
    int m;
    int off;
    int p1;
    int p2;

    p1 = 0x88 << 16;
    p2 = 0x90 << 16;
    if (__GetFlag(0x94 << 2) == 0) {
        __SetFlag(0x94 << 2);
        __CutsceneStart();
        m = 0xffff0000;
        *(int *)(__MapActor_GetActor(0xc) + 0x18) = m;
        *(int *)(__MapActor_GetActor(0xd) + 0x18) = m;
        *(int *)(__MapActor_GetActor(0xe) + 0x18) = m;
        __MapActor_SetPos(3, p1, p2);
        __Func_8092adc(3, 0x80 << 7, 0xa);
        off = 0xe0 << 1;
        *(int *)(iwram_3001ebc + off) = 0x201;
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0x3c);
        __Func_809280c(3, 0, 0);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0x1e);
        __Func_809218c(3, 0x88, 0x48);
        __CutsceneWait(0x28);
        __Func_80925cc(0, 1);
        __MapActor_WaitMovement(3);
        __MapActor_SetPos(3, 0, 0);
        __SetFlag(0x872);
        *(int *)(iwram_3001ebc + off) = 0x81 << 2;
        __CutsceneEnd();
    }
}
