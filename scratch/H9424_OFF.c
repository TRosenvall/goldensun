extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8093c00(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_15b8(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_955_20088ec(void);
extern void OvlFunc_955_2008950(void);
extern void OvlFunc_955_2008970(void);

void OvlFunc_955_2009424(int a)
{
    int r;
    int k;
    int off;

    off = 0xe1 << 1;
    if (*(short *)(gState + off) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 2);
    if (r == 0) {
        __MessageID(0x20a2);
        OvlFunc_955_20088ec();
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0xf6 << 18, -1, 0xe8 << 16, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        k = 0x87;
        OvlFunc_955_2008950();
        __ActorMessage(a, 0);
        k = k << 3;
        OvlFunc_common1_1078(0, k, 0x84 << 1);
        __CutsceneWait(0xf);
        __MapActor_SetSpeed(0, 0xc0 << 9, 0xc0 << 8);
        OvlFunc_common1_15b8(0, k, 0xd8);
        OvlFunc_common1_15b8(0, 0x85 << 3, 0xd8);
        OvlFunc_955_2008970();
        __Func_8093c00();
        __Func_80933f8(-1, -1, -1, 0);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 2);
    } else if (r == 1) {
        __MessageID(0x20a1);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 2);
    __CutsceneEnd();
}
