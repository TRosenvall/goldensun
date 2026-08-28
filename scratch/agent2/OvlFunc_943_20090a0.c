typedef unsigned char u8;

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __Func_808e118(void);
extern void OvlFunc_943_20092f0(void);
extern int __MapActor_SetSpeed(int a, int b, int c);
extern u8 *__MapActor_GetActor(int a);
extern int __Func_80921c4(int a, int b, int c);
extern void __CutsceneWait(int a);
extern int __Func_809259c(int a, int b);
extern void OvlFunc_943_200b9ec(int a);
extern int __MapActor_SetPos(int a, int b, int c);
extern int __Func_8092adc(int a, int b, int c);
extern int __MapActor_Emote(int a, int b, int c);
extern void __CutsceneEnd(void);

void OvlFunc_943_20090a0(void)
{
    u8 *p;

    if (__GetFlag(0x911) != 0 && __GetFlag(0x922) == 0) {
        __CutsceneStart();
        __Func_808e118();
        OvlFunc_943_20092f0();
        __MapActor_SetSpeed(0x14, 0x6666, 0x3333);
        p = __MapActor_GetActor(0x14) + 0x5a;
        *p &= 0xfe;
        __Func_80921c4(0x14, 0xe8, 0x330);
        __CutsceneWait(1);
        p = __MapActor_GetActor(0x14) + 0x5a;
        *p |= 1;
        __CutsceneWait(0x14);
        __Func_809259c(0x14, 2);
        OvlFunc_943_200b9ec(0x14);
        __MapActor_SetSpeed(0x14, 0x13333, 0x9999);
        p = __MapActor_GetActor(0x14) + 0x5a;
        *p &= 0xfe;
        __Func_80921c4(0x14, 0xf4, 0x324);
        __CutsceneWait(1);
        p = __MapActor_GetActor(0x14) + 0x5a;
        *p |= 1;
        __CutsceneWait(0x14);
        __MapActor_SetSpeed(0x14, 0x33333, 0x19999);
        __Func_80921c4(0x14, 0xf8, 0x30a);
        __Func_80921c4(0x14, 0xf8, 0x2bc);
        __MapActor_SetPos(0x14, 0xf60000, 0x2000000);
        __Func_8092adc(0x14, 0, 0);
        __MapActor_Emote(0, 0x101, 0x3c);
        __CutsceneEnd();
    }
}
