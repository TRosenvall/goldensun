extern unsigned char gState[];
extern unsigned char L40c0[] __asm__(".L40c0");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Actor_SetAnim(void *actor, int anim);
extern void __Actor_TravelTo(void *actor, int x, int y, int z);
extern void __Actor_WaitMovement(void *actor);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __PlaySound(int id);
extern unsigned char *__galloc_ewram(int a, int b);
extern void __Camera_SetTarget(int cam, void *actor);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_20084c0(int slot, int x, int z)
{
    unsigned char *gs;
    unsigned char *a;
    unsigned char *b;
    unsigned char *t;
    int lead;
    int dir;
    int off;
    int dx;
    int dz;

    gs = gState;
    lead = *(int *)(gs + 0x1f4);
    a = __MapActor_GetActor(lead);
    b = __MapActor_GetActor(slot);
    dir = (*(unsigned short *)(a + 6) + 0x1000) & 0xe000;
    off = (*(int *)(b + 8) >> 20) != x / 2;
    x <<= 19;
    z <<= 19;
    if (off) {
        dx = (x - *(int *)(b + 8)) / 2;
        dz = 0;
    } else {
        dx = 0;
        dz = (z - *(int *)(b + 0x10)) / 2;
    }
    __CutsceneStart();
    __MapActor_SetAnim(lead, 8);
    __CutsceneWait(6);
    *(int *)(b + 0x30) = 0x8000;
    *(int *)(b + 0x34) = 0x3333;
    __Actor_SetAnim(b, L40c0[dir / 0x4000]);
    __Actor_TravelTo(b, x, 0, z);
    __CutsceneWait(6);
    __MapActor_SetAnim(lead, 2);
    t = __galloc_ewram(0x1b, 0xccc);
    __Camera_SetTarget(*(int *)(t + 0x1e0), b);
    __MapActor_SetSpeed(lead, 0x8000, 0x3333);
    __Actor_SetAnim(a, 2);
    __Actor_TravelTo(a, *(int *)(a + 8) + dx, 0, *(int *)(a + 0x10) + dz);
    __PlaySound(0xef);
    __Actor_WaitMovement(a);
    __Actor_SetAnim(a, 1);
    __Actor_WaitMovement(b);
    __PlaySound(0x120);
    __PlaySound(0xd5);
    __Actor_SetAnim(b, 1);
    __CutsceneWait(0xf);
    __CutsceneEnd();
}

void OvlFunc_955_200862c(void)
{
    unsigned char *p1;
    unsigned char *p2;
    unsigned char *p3;
    int m;
    int v;

    m = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, 0xe, m);
    p1 = __MapActor_GetActor(0xf);
    v = *(int *)(p1 + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v, m);
    p2 = __MapActor_GetActor(0x10);
    v = *(int *)(p2 + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v, m);
    p3 = __MapActor_GetActor(0x11);
    v = *(int *)(p3 + 0x10) >> 20;
    __Func_8010704(0xd, 0x1c, 4, 1, 0x12, v);
}
