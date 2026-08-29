extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __PlaySound(int id);
extern void __StartTask(void *fn, int a);
extern void OvlFunc_960_2008400(void);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __SetFlag(int id);
extern void __SetFlagByte(int id, int v);
extern void __CutsceneEnd(void);

void OvlFunc_960_2008464(int slot)
{
    unsigned char *gs;
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    int n, s;

    s = 0x101;
    gs = gState;
    n = *(int *)(gs + 0x1f4);
    a = __MapActor_GetActor(n);
    __MapActor_GetActor(slot);
    if (__GetFlag(0x20f) != 0)
        return;
    __CutsceneStart();
    __MapActor_Surprise(n, s);
    __MapActor_SetAnim(n, 9);
    p = __MapActor_GetActor(slot);
    if (p != 0)
        __MapActor_TravelTo(n, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(n);
    __PlaySound(0xf4);
    __StartTask(OvlFunc_960_2008400, 0xc8 << 4);
    q = a + 0x55;
    *q = 0;
    __Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc) + (0x80 << 14),
                     *(int *)(a + 0x10));
    __MapActor_WaitMovement(n);
    *(int *)(a + 0x28) = 0;
    *q = 4;
    gs[0x1f2] = 2;
    __SetFlag(0x20f);
    __SetFlagByte(0x86 << 2, slot);
    __SetFlagByte(0x84 << 2, 0xb4);
    __CutsceneEnd();
    *(unsigned short *)(iwram_3001ebc + 0x17c) = 0;
}
