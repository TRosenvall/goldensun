typedef unsigned char u8;
typedef unsigned short u16;

extern u8 gState[];
extern void __Func_8079664(int i);
extern void __AddPartyMember(int id);
extern void __SetCameraTarget(int id, int a);
extern u8 *__GetUnit(int id);
extern void __Func_8091858(void);

void OvlFunc_common1_78(int id)
{
    u8 *g;
    u8 *u;
    int v;
    int t;

    __Func_8079664(0);
    __Func_8079664(1);
    __Func_8079664(2);
    __Func_8079664(3);
    __Func_8079664(5);
    __AddPartyMember(id);
    g = gState;
    g += 0xfa * 2;
    *(int *)g = id;
    __SetCameraTarget(id, 0);
    u = __GetUnit(id);
    *(u8 *)(u + 0x131) = 0;
    *(u16 *)(u + 0x38) = *(u16 *)(u + 0x34);
    *(u16 *)(u + 0x3a) = *(u16 *)(u + 0x36);
    v = (*(short *)(u + 0x38) << 14) / *(short *)(u + 0x34);
    t = 0x4000;
    if (v <= t) {
        t = 0;
        if (v >= 0)
            t = v;
    }
    *(u16 *)(u + 0x14) = t;
    if (*(u16 *)(u + 0x14) == 0 && *(short *)(u + 0x38) != 0)
    {
        t = 1;
        *(u16 *)(u + 0x14) = t;
    }
    v = (*(short *)(u + 0x3a) << 14) / *(short *)(u + 0x36);
    t = 0x4000;
    if (v <= t) {
        t = 0;
        if (v >= 0)
            t = v;
    }
    *(u16 *)(u + 0x16) = t;
    if (*(u16 *)(u + 0x16) == 0 && *(short *)(u + 0x3a) != 0)
    {
        t = 1;
        *(u16 *)(u + 0x16) = t;
    }
    __Func_8091858();
}
