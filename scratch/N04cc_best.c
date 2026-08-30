extern unsigned char gState[];
extern int _AREA_8f;
extern int _AREA_90;

extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __Func_802106c(int a);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

int OvlFunc_common1_4cc(int a, int b)
{
    unsigned char *g;
    int r;

    __Func_808e118();
    __Func_8019908(b, 5);
    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_8f)
        __MessageID(0x2076);
    else if (*(short *)(g + (0xe0 << 1)) == (int)&_AREA_90)
        __MessageID(0x2078);
    else
        __MessageID(0x207a);
    __ActorMessage(a, 0);
    if (__GetFlag(b + (0x80 << 2)))
        return 2;
    b += 0x82 << 2;
    if (__GetFlag(b) == 0) {
        __SetFlag(b);
        __MessageID(0x207c);
        __Func_8092c40(a, 0);
        return __Func_8091c7c(0, 0);
    }
    r = __Func_802106c(0);
    if (r == 1)
        return 2;
    if (r == 2 || r == -1)
        return 3;
    return r;
}
