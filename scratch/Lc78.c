extern unsigned int gState;
extern void __Func_8079664(int n);
extern void __AddPartyMember(int n);
extern void __SetCameraTarget(int slot, int n);
extern unsigned char *__GetUnit(int slot);
extern void __Func_8091858(void);

void OvlFunc_common1_78(int slot)
{
    unsigned char *u;
    unsigned int g;
    unsigned int off;
    unsigned int o2;
    int q;
    int w;

    __Func_8079664(0);
    __Func_8079664(1);
    __Func_8079664(2);
    __Func_8079664(3);
    __Func_8079664(5);
    __AddPartyMember(slot);
    g = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    g += off;
    *(int *)g = slot;
    __SetCameraTarget(slot, 0);
    u = __GetUnit(slot);
    *(short *)(u + 0x38) = *(unsigned short *)(u + 0x34);
    o2 = 0x131;
    *(short *)(u + 0x3a) = *(unsigned short *)(u + 0x36);
    *(unsigned char *)(u + o2) = 0;

    q = (*(short *)(u + 0x38) << 14) / *(short *)(u + 0x34);
    if (q > (0x80 << 7))
        w = 0x80 << 7;
    else if (q < 0)
        w = 0;
    else
        w = q;
    *(short *)(u + 0x14) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x38) != 0)
        *(short *)(u + 0x14) = 1;

    q = (*(short *)(u + 0x3a) << 14) / *(short *)(u + 0x36);
    if (q > (0x80 << 7))
        w = 0x80 << 7;
    else if (q < 0)
        w = 0;
    else
        w = q;
    *(short *)(u + 0x16) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x3a) != 0)
        *(short *)(u + 0x16) = 1;
    __Func_8091858();
}
