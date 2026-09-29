/* Declarations inlined from the batch-250 scratch header h048.h when this
 * candidate was landed in batch 300 -- the tree keeps its .c files
 * self-contained rather than carrying per-function headers. */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
};

struct A {
    int f0;
    unsigned char pad04[8 - 4];
    int f8;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern unsigned int __Random(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern int __TestCollision(struct Actor *a, int *b);
extern void __MapActor_Surprise(int slot, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __WaitFrames(int n);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);


void OvlFunc_968_200c048(struct A *a)
{
    int col[3];
    struct P t;
    unsigned char *p;
    unsigned char *gs;
    struct Actor *actor;
    int slot;
    struct Actor *q;
    int u;
    int res;
    int w;

    p = iwram_3001ebc;
    gs = gState;
    slot = *(int *)(gs + (0xfa << 1));
    q = *(struct Actor **)(p + (0xf0 << 1));
    actor = __MapActor_GetActor(slot);
    col[0] = actor->f8 + (a->f0 * 3 << 15);
    col[1] = actor->fc;
    col[2] = actor->f10 + (a->f8 * 3 << 15);
    res = __TestCollision(actor, col);
    u = iwram_3001e40;
    u = u & 4;
    if (u == 0) {
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
        OvlFunc_968_2008118(actor->f8 + ((__Random() * 12 >> 16) << 16)
                                + 0xfffa0000,
                            actor->fc, actor->f10,
                            ((__Random() * 5 >> 16) * 0x1999 + 0x7ffd) * a->f0,
                            u,
                            a->f8 * ((__Random() * 5 >> 16) * 0x1999 + 0x7ffd),
                            0x80 << 16, &t);
    }
    if (res < 0) {
        __MapActor_Surprise(slot, 0x81 << 1);
        __Actor_TravelTo(actor, actor->f8, actor->fc,
                         actor->f10 + (0x80 << 12));
        __Actor_SetAnim(actor, 7);
        __Actor_WaitMovement(actor);
        do {
            __WaitFrames(1);
        } while (actor->fc != actor->f14);
        __Actor_SetAnim(actor, 6);
        __WaitFrames(3);
        return;
    }
    col[0] = actor->f8 + (a->f0 << 19);
    col[1] = actor->fc;
    col[2] = actor->f10 + (a->f8 << 19);
    res = __TestCollision(actor, col);
    if (res > 0)
        return;
    col[0] = actor->f8 + a->f0 * 0x5b333 - a->f8 * 0x5b333;
    col[1] = actor->fc;
    col[2] = actor->f10 + a->f8 * 0x5b333 - a->f0 * 0x5b333;
    res = __TestCollision(actor, col);
    if (res > 0) {
        actor->f8 += a->f8 * 3 << 15;
        actor->f10 += a->f0 * 3 << 15;
        return;
    }
    w = (a->f0 + a->f8) * 0x5b333;
    col[0] = w + actor->f8;
    col[1] = actor->fc;
    col[2] = w + actor->f10;
    res = __TestCollision(actor, col);
    if (res > 0) {
        actor->f8 -= a->f8 * 3 << 15;
        actor->f10 -= a->f0 * 3 << 15;
        return;
    }
    q->f8 += a->f0 * 3 << 15;
    actor->f8 += a->f0 * 3 << 15;
    q->f10 += a->f8 * 3 << 15;
    actor->f10 += a->f8 * 3 << 15;
}
