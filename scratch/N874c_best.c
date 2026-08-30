extern unsigned char *iwram_3001ed0;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern int OvlFunc_888_200a7d4(void);

void OvlFunc_888_200874c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MapActor_SetAnim(0, 0);
    __MapActor_SetAnim(1, 0);
    __MapActor_SetAnim(0xb, 0);
    __MapActor_SetAnim(0xc, 0);
    __MapActor_SetAnim(8, 0);
    __MapActor_SetAnim(9, 0);
    __MapActor_SetAnim(0xa, 0);
    __Func_8091200(0x10002, 0);
    __Func_8091254(0x78);
    __CutsceneWait(0xb4);
    p = iwram_3001ed0;
    *(short *)(p + 0xe5a) = 0xf8 << 7;
    *(short *)(p + 0xe5c) = 0xf8 << 7;
    *(short *)(p + 0xe5e) = 0xf8 << 7;
    p[0xa8 << 6] = 0;
    p[0x2a01] = 1;
    p[0x2a02] = 1;
    p[0x2a03] = 1;
    __CutsceneWait(1);
    __Func_8019aa0(0x116d, 1, 0);
    __Func_8091200(0, 0);
    __Func_8091254(0x78);
    __CutsceneWait(0x78);
    __CutsceneWait(0x3c);
    if (OvlFunc_888_200a7d4() == 0) {
        __CutsceneEnd();
        __Func_8091e9c(0x14);
    } else {
        __CutsceneEnd();
        __Func_8091e9c(0x32);
    }
}
