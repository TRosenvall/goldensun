extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_80118c0(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void OvlFunc_938_2009494(void);
extern void OvlFunc_938_2008360(void);

void OvlFunc_938_2008264(void)
{
    unsigned char *g;
    char *p;
    int v;
    int y;
    int e;
    int s1, s2;

    __Func_80118c0(1);
    __Func_80118c0(2);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 9:
        if (__GetFlag(0x941)) {
            p = __MapActor_GetActor(8);
            v = 0x80 << 5;
            *(unsigned short *)(p + 6) = v;
            if (__GetFlag(0x914) == 0)
                OvlFunc_938_2009494();
        } else {
            __MapActor_SetPos(9, 0, 0);
            y = 0xd3 << 17;
            e = 0x38a0000;
            if (__GetFlag(0x321)) {
                __MapActor_SetPos(8, e, y);
                p = __MapActor_GetActor(8);
                v = 0xd0 << 8;
                *(unsigned short *)(p + 6) = v;
            }
        }
        break;
    case 0xa:
    case 0xb:
        if (__GetFlag(0x915)) {
            s1 = 4;
            s2 = 3;
            __CopyMapTiles(0x3a, 0x46, 0x36, 0x46, s1, s2);
            s1 = 0x37;
            s2 = 8;
            __Func_8010704(0x37, 9, 2, 1, s1, s2);
            __Func_800fe9c();
            __WaitFrames(1);
        }
        break;
    case 0x14:
        __MapActor_SetPos(9, 0, 0);
        if (__GetFlag(0x109) == 0)
            OvlFunc_938_2008360();
        break;
    }
}
