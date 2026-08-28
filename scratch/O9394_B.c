extern unsigned char gState[];
extern int _AREA_98;
extern int _AREA_9e;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_958_2009394(void)
{
    unsigned char *g;
    int s1, s2;
    int x1, y1, x2, y2;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_98)) {
        __SetFlag(0xa2 << 1);
        if (__GetFlag(0x9a << 4))
            __MapActor_SetPos(0xb, 0, 0);
    }
    x1 = 0xdc << 17;
    y1 = 0x9a << 17;
    x2 = 0xae << 18;
    y2 = 0x90 << 17;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_9e)) {
        if (*(short *)(g + (0xe1 << 1)) == 1) {
            s1 = 0x6b;
            s2 = 0x11;
            __Func_8010704(0x6c, 0x11, 1, 1, s1, s2);
        }
        if (__GetFlag(0x9a2)) {
            __MapActor_SetPos(8, x1, y1);
            __MapActor_SetAnim(8, 2);
            s1 = 0x1b;
            s2 = 0x13;
            __Func_8010704(0x1d, 0x13, 1, 1, s1, s2);
        }
        if (__GetFlag(0x9a5)) {
            __MapActor_SetPos(9, 0, 0);
            __MapActor_SetPos(0xa, x2, y2);
            __MapActor_SetAnim(0xa, 2);
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
    }
    return 0;
}
