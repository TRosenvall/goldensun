extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];

extern unsigned char *__MapActor_GetActor(int slot);
extern int  __GetFlag(int id);
extern void __SetFlag(int id);
extern void __SetFlagByte(int id, int v);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __PlaySound(int id);
extern int __StartTask(void (*fn)(void), int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void OvlFunc_881_200b678(void);

void OvlFunc_881_200b6dc(int arg)
{
    unsigned char *gs;
    unsigned char *actor;
    unsigned char *p;
    unsigned char *o;
    int slot;
    int z;
    int m;

    gs = gState;
    m = 0x101;
    slot = *(int *)(gs + (0xfa << 1));
    actor = __MapActor_GetActor(slot);
    z = __GetFlag(0xbc << 2);
    if (z != 0)
        return;
    __CutsceneStart();
    __MapActor_Surprise(slot, m);
    __MapActor_SetAnim(slot, 9);
    o = __MapActor_GetActor(arg);
    if (o != 0)
        __MapActor_TravelTo(slot, *(short *)(o + 0xa), *(short *)(o + 0x12));
    __MapActor_WaitMovement(slot);
    __PlaySound(0xf4);
    __StartTask(OvlFunc_881_200b678, 0xc8 << 4);
    p = actor + 0x55;
    *p = z;
    __Actor_TravelTo(actor, *(int *)(actor + 8), *(int *)(actor + 0xc) + (0x80 << 14), *(int *)(actor + 0x10));
    __MapActor_WaitMovement(slot);
    *(int *)(actor + 0x28) = z;
    *p = 4;
    gs[0xf9 << 1] = 2;
    __SetFlag(0xbc << 2);
    __SetFlagByte(0xbe << 2, 0xb4);
    __CutsceneEnd();
    *(short *)(*(unsigned char **)iwram_3001ebc + (0xbe << 1)) = z;
}
