struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __Func_8093054(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned char *__Func_8093554(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);

void OvlFunc_931_20081d0(void)
{
    struct Actor *act;
    struct Actor *b;
    int f;
    int v;

    v = 0x80 << 7;
    __CutsceneStart();
    if (__GetFlag(0x909) != 0) {
        __MessageID(0x191f);
        __Func_8093054(0xe, 0);
    } else {
        __MapActor_SetAnim(0xe, 4);
        __MessageID(0x18c7);
        __Func_8093040(0xe, 0, 0xa);
        f = __GetFlag(0x8ff);
        if (f == 0) {
            act = __MapActor_GetActor(0x12);
            *(__Func_8093554() + 0x55) = f;
            __Func_80933d4(0x80 << 9, 0x80 << 6);
            __Func_80933f8(act->f8, act->fc, act->f10, 1);
            __Func_809280c(0, v, 0);
            __Func_8092adc(0xe, 0xc0 << 6, 0);
            __Func_8093530();
            __CutsceneWait(0x78);
            b = __MapActor_GetActor(0);
            __Func_80933f8(b->f8, b->fc, b->f10, 1);
            __Func_8093530();
        }
        __MapActor_DoAnim(0xe, 4);
    }
    __CutsceneEnd();
}
