extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_954_2008134(void);
extern void OvlFunc_954_2008158(void);
extern void OvlFunc_954_2008178(void);

void OvlFunc_954_20095e0(int a)
{
    unsigned char *g;
    int r;
    int n1, n2, z1, z2, q1, q2, e1, e2, k3;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 3);
    n1 = 0xb8 << 2;
    n2 = 0xc8;
    z1 = 0;
    z2 = 0;
    q1 = 0x80 << 9;
    q2 = 0x80 << 8;
    e1 = 0x105;
    e2 = 0x3c;
    k3 = 3;
    if (r == 0) {
        __MessageID(0x2095);
        OvlFunc_954_2008134();
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0xd2 << 18, -1, 0xd8 << 16, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_954_2008158();
        __CutsceneWait(0x3c);
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, n1, n2);
        __Func_8092adc(0, z1, z2);
        OvlFunc_954_2008178();
        __MapActor_SetSpeed(0, q1, q2);
        __Func_80921c4(0, 0xcc << 2, 0xc8);
        __CutsceneWait(0x1e);
        __MapActor_Emote(0, e1, e2);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 3);
    } else if (r == 1) {
        __MessageID(0x2094);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, k3);
    __CutsceneEnd();
}
