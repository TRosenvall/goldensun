struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __UI_Sanctum(int slot);
extern int __Func_8093054(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_921_2008abc(void)
{
    unsigned short d;
    int v;

    v = 0xc0 << 6;
    d = __MapActor_GetActor(0)->facing + 0x5fff;
    if (d <= 0x3ffe) {
        __CutsceneStart();
        if (__GetFlag(0x82d) == 0) {
            __MessageID(0x1553);
            __ActorMessage(0x13, 0);
            __SetFlag(0x82d);
        }
        __CutsceneEnd();
        __UI_Sanctum(0x13);
    } else {
        __CutsceneStart();
        if (__GetFlag(0x881)) {
            __MessageID(0x1671);
            __ActorMessage(0x13, 0);
        } else if (__GetFlag(3)) {
            __MessageID(0x1572);
            __ActorMessage(0x13, 0);
        } else {
            __MessageID(0x1554);
            __Func_8093054(0x13, 0);
            __Func_8092adc(0x13, v, 0xa);
        }
        __CutsceneEnd();
    }
}
