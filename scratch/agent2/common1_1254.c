extern unsigned char iwram_3001e68;
extern unsigned char gState[];

extern int L49 __asm__(".L49");
extern int L20 __asm__(".L20");
extern int L31 __asm__(".L31");

struct Ent {
    unsigned char pad0[6];
    unsigned char f6;
    unsigned char f7;
};

struct Actor {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x22 - 0x18];
    unsigned char f22;
    unsigned char pad23;
    int f24;
    unsigned char pad28[4];
    int f2c;
    unsigned char pad30[8];
    int f38;
    unsigned char pad3c[4];
    int f40;
    unsigned char pad44[0x55 - 0x44];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092adc(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __WaitFrames(int n);

void OvlFunc_common1_1254(int slot)
{
    struct Ent *e;
    struct Actor *act;
    unsigned char *g;
    unsigned char *p;
    int m;
    int zero;

    m = 0x80 << 7;
    e = *(struct Ent **)&iwram_3001e68;
    act = __MapActor_GetActor(slot);
    g = gState;
    p = g + 0x1f2;
    if (*p == 1) {
        *p = 0;
        __MapActor_SetAnim(slot, 1);
    } else {
        __Func_8092adc(slot, m, 0x1e);
        __MapActor_SetAnim(slot, 3);
        __CutsceneWait(0x1e);
    }
    zero = 0;
    e->f7 = zero;
    e->f6 = 0xf;
    act->f8 = L49;
    act->f10 = L20;
    act->f6 = L31;
    act->f38 = 0x80 << 24;
    act->f40 = 0x80 << 24;
    act->f24 = zero;
    act->f2c = zero;
    act->f55 = 3;
    act->f22 = 0;
    act->fc = zero;
    act->f14 = zero;
    __Actor_SetSpriteFlags(act, 1);
    __Actor_SetAnim(act, 0);
    __Actor_SetAnim(act, 1);
    __WaitFrames(1);
}
