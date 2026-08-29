extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_80933d4(int a, int b);
extern void __SetCameraTarget(int slot, int n);
extern void __Func_8093530(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_934_20091a0(void)
{
    unsigned char *a;
    int w1, w2;
    int c1, c2;
    int s1;
    int v1, v2;
    int p1;
    int q, r1, r2;

    c1 = 0x80 << 9;
    c2 = 0x80 << 6;
    w1 = 0xc0 << 8;
    s1 = 0x81 << 1;
    v1 = 0x80 << 9;
    v2 = 0x80 << 8;
    p1 = 0xc6 << 2;
    q  = 0x80 << 12;
    r1 = 0xc6 << 2;
    r2 = 0x8c << 1;
    w2 = 0xc0 << 8;
    if (__GetFlag(0x80 << 2) == 0) {
        __SetFlag(0x80 << 2);
        __CutsceneStart();
        __Func_80933d4(c1, c2);
        __SetCameraTarget(8, 1);
        __Func_8093530();
        __CutsceneWait(0x3c);
        __Func_8092adc(8, w1, 0x14);
        __MapActor_Surprise(8, s1);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(8, v1, v2);
        __Func_80921c4(8, p1, 0xf8);
        __PlaySound(0x98);
        a = __MapActor_GetActor(8);
        *(int *)(a + 0x28) = q;
        __Func_80921c4(8, r1, r2);
        __CutsceneWait(0x14);
        __Func_8092adc(8, w2, 0x14);
        __CutsceneWait(0x1e);
        __CutsceneEnd();
    }
}
