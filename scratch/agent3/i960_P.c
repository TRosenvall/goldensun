typedef struct { unsigned char _bytes[704]; } GlobalState;

struct Actor {
    unsigned char pad0[0x28];
    int f28;
    unsigned char pad2c[0x29];
    unsigned char f55;
    unsigned char pad56[0xb];
    unsigned char f61;
};

extern GlobalState gState;
extern int _AREA_a5;
extern int _CONST_2;
extern int __GetFlagByte(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(int slot, int flags);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __SetDestMap(int a, int b);

void OvlFunc_960_2008c00(void)
{
    unsigned int base;
    unsigned int off;
    struct Actor *a1;
    struct Actor *a2;
    int b;
    int i;

    b = __GetFlagByte(0x218);
    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a1 = __MapActor_GetActor(*(int *)base);
    a2 = __MapActor_GetActor(b);
    __CutsceneStart();
    __Func_80933f8(-1, -2, -3, 0);
    __PlaySound(0xdb);
    __Actor_SetSpriteFlags(*(int *)base, 0);
    a2->f55 = 0;
    a1->f55 = 0;
    a1->f28 = 0;
    a1->f61 = 1;
    a2->f61 = 1;
    i = 0x3b;
    do {
        a1->f28 += 0x3333;
        a2->f28 += 0x3333;
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
    __SetFlag(0x122);
    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_a5)
        && __GetFlagByte(0x218) == 0xb)
        __SetDestMap((int)(&_CONST_2), 0x4d);
    else
        __SetDestMap((int)(&_CONST_2), 0x1b);
}
