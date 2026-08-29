extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_934_2008cf8(void);

void OvlFunc_934_2009258(void)
{
    unsigned char *a;
    int s1;
    int v1, v2;
    int x1, y1, x2, y2, x3, y3;
    int w1;

    s1 = 0x81 << 1;
    v1 = 0x80 << 10;
    v2 = 0x80 << 9;
    x1 = 0xbe << 2;  y1 = 0x8c << 1;
    x2 = 0xbe << 2;  y2 = 0x9c << 1;
    x3 = 0xc6 << 2;  y3 = 0x9c << 1;
    w1 = 0xc0 << 8;
    if (__GetFlag(0x80 << 2) != 0 && __GetFlag(0x201) == 0) {
        __SetFlag(0x201);
        __SetFlag(0x302);
        __CutsceneStart();
        __MapActor_Surprise(8, s1);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(8, v1, v2);
        __Func_80921c4(8, x1, y1);
        __Func_80921c4(8, x2, y2);
        __Func_80921c4(8, x3, y3);
        __CutsceneWait(0xa);
        __Func_8092adc(8, w1, 0x14);
        a = __MapActor_GetActor(8);
        *(void **)(a + 0x6c) = (void *)OvlFunc_934_2008cf8;
        __CutsceneEnd();
    }
}
