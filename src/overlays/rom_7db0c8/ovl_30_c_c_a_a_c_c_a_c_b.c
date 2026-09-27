/* OvlFunc_954_200833c  --  0x0200833c, split out of
 * asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c.s; OvlFunc_954_200842c stays
 * in _c.s. Matched from scratch, the OvlFunc_946_2009774 family: `g = gState`
 * local, x and z computed into locals before the f30/f34 stores.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
};

extern unsigned char gState[];
extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int anim);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __PlaySound(int id);

void OvlFunc_954_200833c(int slot, int dx, int dz)
{
    unsigned char *g;
    struct Actor *p;
    struct Actor *q;
    int x, z;

    g = gState;
    p = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    q = __MapActor_GetActor(slot);
    __CutsceneStart();
    x = ((p->f8 + (dx << 16)) & 0xfff00000) + 0x80000;
    z = ((p->f10 + (dz << 16)) & 0xfff00000) + 0x80000;
    p->f30 = 0x80 << 9;
    p->f34 = 0x80 << 8;
    __Actor_TravelTo(p, x, p->fc, z);
    __Actor_SetAnim(p, 0x1b);
    x = ((q->f8 + (dx << 16)) & 0xfff00000) + 0x80000;
    z = ((q->f10 + (dz << 16)) & 0xfff00000) + 0x80000;
    q->f30 = 0x80 << 9;
    q->f34 = 0x80 << 8;
    __Actor_TravelTo(q, x, q->fc, z);
    if (dx < 0 || dz < 0)
        __Actor_SetAnim(q, 4);
    else
        __Actor_SetAnim(q, 3);
    __PlaySound(0xe2);
    __Actor_WaitMovement(p);
    __Actor_SetAnim(q, 2);
    __PlaySound(0x90 << 1);
    __CutsceneEnd();
}
