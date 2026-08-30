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
extern void __MapActor_SetSpeed(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void OvlFunc_common1_1254(int a);
extern void __SetCameraTarget(int a, int b);
extern void OvlFunc_common1_588(int a, int b);

void OvlFunc_955_20092f0(int a)
{
    unsigned char *gp;
    unsigned char *e;
    int r;
    int k;
    int sx;
    int sy;
    int m;

    sx = 0xc0 << 9;
    sy = 0xc0 << 8;
    m = 0x95 << 3;
    gp = gState;
    if (*(short *)(gp + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 1);
    if (r == 0) {
        __MessageID(0x209e);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0x99 << 19, -1, 0xb8 << 16, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0x9f << 3, 0xa8);
        k = 0xa1 << 3;
        __MapActor_SetSpeed(0, sx, sy);
        OvlFunc_common1_15b8(0, k, 0xb8);
        OvlFunc_common1_15b8(0, k, 0xd8);
        k -= 0x40;
        OvlFunc_common1_15b8(0, k, 0xd8);
        __ActorMessage(a, 0);
        OvlFunc_common1_15b8(0, k, 0xf8);
        OvlFunc_common1_15b8(0, m, 0xf8);
        __CutsceneWait(3);
        e = __MapActor_GetActor(0);
        *(int *)(e + 0x28) = 0x80 << 11;
        __MapActor_SetAnim(0, 0x1c);
        __MapActor_Surprise(0, 0x81 << 1);
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 1);
    } else if (r == 1) {
        __MessageID(0x209d);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 1);
    __CutsceneEnd();
}
