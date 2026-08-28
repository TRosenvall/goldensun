typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_6a;
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_941_2008210(void);
extern void OvlFunc_941_2008384(void);
extern void OvlFunc_941_20080d4(void);

int OvlFunc_941_2009a0c(void)
{
    unsigned char *p;
    unsigned int base;
    unsigned int off;

    p = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(p + off) = 0x204;
    base = (unsigned int)&gState;
    if (*(short *)((char *)base + off) == (int)(&_AREA_6a)) {
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        *(int *)(__MapActor_GetActor(0xb) + 0x1c) = 0xf333;
        if (__GetFlag(0x201))
            OvlFunc_941_2008210();
        if (__GetFlag(0x202))
            OvlFunc_941_2008384();
        if (__GetFlag(0x200))
            OvlFunc_941_20080d4();
        if (__GetFlag(0x203))
            __MapActor_SetAnim(0xb, 5);
        if (__GetFlag(0x204))
            __MapActor_SetAnim(9, 5);
    }
    return 0;
}
