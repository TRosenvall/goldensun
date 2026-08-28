extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80921c4(int a, int b, int c);

void OvlFunc_948_2008ad0(void)
{
    unsigned char *a;
    int sp1;
    int v;
    int p1, p2;
    int q1;

    sp1 = 0x81 << 1;
    v = 0x80 << 12;
    p1 = 0x92 << 2;
    p2 = 0xaa << 2;
    q1 = 0x80 << 7;
    if (__GetFlag(0x9c8) == 0) {
        __SetFlag(0x9c8);
        __CutsceneStart();
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __Func_8093500(0xf, 1);
        __Func_8093530();
        __Func_8092adc(0xf, 0, 0x14);
        __MapActor_Surprise(0xf, sp1);
        __Func_80925cc(0xf, 2);
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(0xf, 0x80 << 9, 0x80 << 8);
        __PlaySound(0x98);
        a = __MapActor_GetActor(0xf);
        *(int *)(a + 0x28) = v;
        __Func_80921c4(0xf, p1, p2);
        __Func_8092adc(0xf, q1, 0x14);
        __CutsceneEnd();
    }
}
