extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void OvlFunc_888_200b270(void);
extern void OvlFunc_888_200888c(void);

int OvlFunc_888_20085cc(void)
{
    char *p;
    unsigned char *g;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(1);
    __CutsceneWait(1);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 0xa:
    case 0xb:
    case 0xc:
        if (__GetFlag(0x855))
            __MapActor_SetPos(0xa, 0xc8 << 16, 0xa0 << 15);
        __ClearFlag(0x12f);
        break;
    case 0x14:
        OvlFunc_888_200b270();
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
        __ClearFlag(0x12f);
        break;
    case 0x1d:
    case 0x20:
    case 0x23:
        __ClearFlag(0x12f);
        break;
    case 0x15:
        OvlFunc_888_200b270();
        __SetFlag(0x201);
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
        __ClearFlag(0x12f);
        break;
    }
    return 0;
}
