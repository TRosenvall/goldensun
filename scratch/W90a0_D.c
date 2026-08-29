extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_808e118(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_943_20092f0(void);
extern void OvlFunc_943_200b9ec(int slot);

void OvlFunc_943_20090a0(void)
{
    unsigned char *p;
    int m;
    int b;
    int s1, s2, t1, u1, v1, w1, w2, e1;

    s1 = 0x33333;
    s2 = 0x19999;
    t1 = 0x30a;
    u1 = 0xc9 << 2;
    v1 = 0xaf << 2;
    w1 = 0xf6 << 16;
    w2 = 0x80 << 18;
    e1 = 0x101;
    if (__GetFlag(0x911)) {
        if (__GetFlag(0x922) == 0) {
            __CutsceneStart();
            __Func_808e118();
            OvlFunc_943_20092f0();
            __MapActor_SetSpeed(0x14, 0x6666, 0x3333);
            p = __MapActor_GetActor(0x14) + 0x5a;
            m = 0xfe;
            *p = m & *p;
            __Func_80921c4(0x14, 0xe8, 0xcc << 2);
            __CutsceneWait(1);
            p = __MapActor_GetActor(0x14) + 0x5a;
            b = 1;
            *p = *p | b;
            __CutsceneWait(0x14);
            __Func_809259c(0x14, 2);
            OvlFunc_943_200b9ec(0x14);
            __MapActor_SetSpeed(0x14, 0x13333, 0x9999);
            p = __MapActor_GetActor(0x14) + 0x5a;
            *p = m & *p;
            __Func_80921c4(0x14, 0xf4, u1);
            __CutsceneWait(1);
            p = __MapActor_GetActor(0x14) + 0x5a;
            *p = *p | b;
            __CutsceneWait(0x14);
            __MapActor_SetSpeed(0x14, s1, s2);
            __Func_80921c4(0x14, 0xf8, t1);
            __Func_80921c4(0x14, 0xf8, v1);
            __MapActor_SetPos(0x14, w1, w2);
            __Func_8092adc(0x14, 0, 0);
            __MapActor_Emote(0, e1, 0x3c);
            __CutsceneEnd();
        }
    }
}
