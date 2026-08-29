extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809280c(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_959_200a52c(void)
{
    unsigned char *a;
    int s1,t1,s2,t2,s3,t3;
    int p1, p2;

    p1 = 0xe4 << 1;
    p2 = 0xdc << 1;
    s1 = 0xb333; t1 = 0x5999;
    s2 = 0xb333; t2 = 0x5999;
    s3 = 0xb333; t3 = 0x5999;
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(3, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
    __Func_8092adc(0, 0, 0);
    __MapActor_SetSpeed(2, s1, t1);
    __Func_809218c(2, p1, 0xc0);
    __MapActor_SetSpeed(3, s2, t2);
    __Func_809218c(3, p2, 0xb8);
    __MapActor_SetSpeed(1, s3, t3);
    __Func_809218c(1, 0xe0 << 1, 0xf0);
    __MapActor_WaitMovement(2);
    __Func_809280c(2, 0xc, 0);
    __MapActor_WaitMovement(1);
    __MapActor_WaitMovement(3);
    __Func_809280c(1, 0xc, 0);
    __Func_809280c(3, 0xc, 0);
    __CutsceneWait(0xf);
}
