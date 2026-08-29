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
    unsigned int base2;
    unsigned int off;
    unsigned char *f;

    p = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(p + off) = 0x204;
    base = (unsigned int)&gState;
    if (*(short *)((char *)base + off) == (int)(&_AREA_1c)) {
        off = 0xe1;
        off <<= 1;
        base2 = base + off;
        off = 0;
        if (*(short *)((char *)base2 + off) == 5) {
            __ClearFlag(0x12f);
        } else {
            unsigned char m;
            f = __MapActor_GetActor(8) + 0x59;
            m = 0x10;
            *f = m | *f;
            if (__GetFlag(0x864)) {
                int e5, e6;
                __MapActor_SetPos(8, 0x15a0000, 0x1240000);
                __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
                f = __MapActor_GetActor(8) + 0x23;
                *f |= 2;
                __MapActor_SetAnim(8, 2);
                e5 = 0x13;
                e6 = 0x11;
                __Func_8010704(0x13, 0x4a, 9, 3, e5, e6);
            }
        }
    }
    return 0;
}
