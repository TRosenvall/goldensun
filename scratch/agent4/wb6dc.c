struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[0x55 - 0x2c];
    unsigned char f55;
};

struct Ent {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c[0x12 - 0xc];
    short f12;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern struct Ent *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __SetFlagByte(int id, int v);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __PlaySound(int id);
extern void __StartTask(void (*fn)(void), int a);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void OvlFunc_881_200b678(void);

void OvlFunc_881_200b6dc(int slot)
{
    unsigned char *gs;
    struct Actor *a;
    struct Ent *e;
    int leader;
    int arg;

    gs = gState;
    leader = *(int *)(gs + 0x1f4);
    a = (struct Actor *)__MapActor_GetActor(leader);
    if (__GetFlag(0xbc << 2) == 0) {
        __CutsceneStart();
        __MapActor_Surprise(leader, 0x101);
        __MapActor_SetAnim(leader, 9);
        e = __MapActor_GetActor(slot);
        if (e != 0) {
            __MapActor_TravelTo(leader, e->fa, e->f12);
        }
        __MapActor_WaitMovement(leader);
        __PlaySound(0xf4);
        arg = 0xc8;
        arg <<= 4;
        __StartTask(OvlFunc_881_200b678, arg);
        a->f55 = 0;
        __Actor_TravelTo(a, a->f8, a->fc + (0x80 << 14), a->f10);
        __MapActor_WaitMovement(leader);
        a->f28 = 0;
        a->f55 = 4;
        *(gs + 0x1f2) = 2;
        __SetFlag(0xbc << 2);
        __SetFlagByte(0xbe << 2, 0xb4);
        __CutsceneEnd();
        *(short *)(iwram_3001ebc + (0xbe << 1)) = 0;
    }
}
