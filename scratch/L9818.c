extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_927_2009818(void)
{
    unsigned char *a;

    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xd4 << 1, 0xf0 << 1, 0x79999);
    __CutsceneWait(2);
    OvlFunc_927_2008e18(0xe);
    __Func_8092950(0xe, 0xf);
    a = __MapActor_GetActor(0xe);
    __Actor_SetSpriteFlags(a, 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x305);
    __MapActor_SetPos(0x11, 0xd4 << 17, 0xf0 << 17);
    __CutsceneEnd();
}
