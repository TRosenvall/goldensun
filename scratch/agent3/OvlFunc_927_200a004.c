extern void __CutsceneStart(void);
extern void OvlFunc_927_2008ea8(int, int);
extern void __Func_80933f8(int, int, int, int);
extern void OvlFunc_927_2008d90(int, int, int, int);
extern void OvlFunc_927_2008e18(int);
extern void __Func_8092950(int, int);
extern void *__MapActor_GetActor(int);
extern void __Actor_SetSpriteFlags(void *, int);
extern void __CutsceneWait(int);
extern void __SetFlag(int);
extern void __MapActor_SetPos(int, int, int);
extern void __CutsceneEnd(void);

void OvlFunc_927_200a004(void)
{
    __CutsceneStart();
    OvlFunc_927_2008ea8(0x12, 1);
    __Func_80933f8(0xba << 18, -1, 0xfc << 17, 1);
    OvlFunc_927_2008d90(0x12, 0xba << 2, 0xfc << 1, 0x90 << 12);
    OvlFunc_927_2008e18(0x12);
    __Func_8092950(0x12, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x30a);
    __MapActor_SetPos(0x16, 0xba << 18, 0xfc << 17);
    __CutsceneEnd();
}
