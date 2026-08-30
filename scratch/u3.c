extern unsigned char gState[];
extern void OvlFunc_common1_2c4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_common1_4cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);
extern void OvlFunc_common1_1490(int a, int b, int c);
extern void OvlFunc_common1_14f4(int a, int b, int c);
extern void OvlFunc_common1_1550(void);
extern void __SetCameraTarget(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);

void OvlFunc_956_200a4d0(int a)
{
    unsigned char *gp;
    int r;

    gp = gState;
    if (*(short *)(gp + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 6);
    gp = 0;
    if (r == 0) {
        __MessageID(0x20c7);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0xa1 << 19, -1, 0x98 << 16, 1);
        __Func_8093530();
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_1490(0xb4, 0x58, 0);
        __CutsceneWait(0x3c);
        __ActorMessage(a, 0);
        OvlFunc_common1_14f4(0x20, 0x54, 0xa);
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_14f4(0x60, 0x54, 0x1e);
        __CutsceneWait(0x3c);
        __ActorMessage(a, 0);
        OvlFunc_common1_1550();
        __CutsceneWait(2);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 6);
    } else if (r == 1) {
        __MessageID(0x20c6);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 6);
    __CutsceneEnd();
}
