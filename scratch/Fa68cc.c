extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_926_200a68c(int a, int b)
{
    int vx, vy;

    __CutsceneStart();
    vx = 0xa0 << 10;
    vy = 0xa0 << 9;
    __MapActor_SetSpeed(0, vx, vy);
    __Func_809228c(0, a, b);
    __MapActor_Jump(0, 4, 0);
    __MapActor_SetAnim(0, 7);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 6);
    __CutsceneEnd();
}
