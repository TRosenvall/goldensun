extern unsigned char gState[];

extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_939_2008eb0(void)
{
    unsigned char *g;
    int s;

    g = gState;
    if (*(short *)(g + (0x93 << 2)) == 0) {
        __CutsceneStart();
        __MapActor_Emote(8, 0x80 << 1, 2);
        __MapActor_Emote(9, 0x80 << 1, 0xf);
        __CutsceneWait(0x1e);
        __Func_809218c(8, 0x98, 0xa8);
        __Func_809218c(9, 0xa8, 0xa8);
        __MapActor_WaitMovement(8);
        __MapActor_WaitMovement(9);
        __MapActor_SetIdle(8);
        __MapActor_SetAnim(8, 0);
        __Func_8092adc(8, 0xc0 << 6, 0);
        __MapActor_SetIdle(9);
        __MapActor_SetAnim(9, 0);
        __Func_8092adc(9, 0xa0 << 7, 0);
        __MessageID(0x24da);
        __ActorMessage(8, 0);
        __SetFlag(0x90 << 2);
        s = 0xb;
        __Func_8010704(6, 0xb, 1, 1, 7, s);
        __Func_8010704(6, 0xb, 1, 1, 8, s);
        __Func_8010704(6, 0xb, 1, 1, 9, s);
        __CutsceneEnd();
    }
}
