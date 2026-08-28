typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_1c;
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_906_2008414(void)
{
    unsigned char *p;
    unsigned int base;
    unsigned int off;
    unsigned char *a;

    p = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(p + off) = 0x204;
    base = (unsigned int)&gState;
    if (*(short *)((char *)base + off) == (int)(&_AREA_1c)) {
        off = 0xe1;
        off <<= 1;
        base += off;
        off = 0;
        if (*(short *)((char *)base + off) == 5) {
            __ClearFlag(0x12f);
        } else {
            unsigned char m;
            a = __MapActor_GetActor(8);
            m = 0x10;
            a[0x59] = m | a[0x59];
            if (__GetFlag(0x864)) {
                __MapActor_SetPos(8, 0x15a0000, 0x1240000);
                __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
                a = __MapActor_GetActor(8);
                a[0x23] |= 2;
                __MapActor_SetAnim(8, 2);
                __Func_8010704(0x13, 0x4a, 9, 3, 0x13, 0x11);
            }
        }
    }
    return 0;
}
