extern unsigned char *iwram_3001ebc;
extern int iwram_3001e40;
extern int gState[];

extern unsigned char *__MapActor_GetActor(int slot);
extern int __TestCollision(unsigned char *a, int *v);
extern int __Random(void);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void __WaitFrames(int n);
extern void OvlFunc_933_2009054(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, void *h);

void OvlFunc_933_20092fc(void)
{
    int v[3];
    unsigned char buf[0x28];
    unsigned char *e;
    unsigned char *o;
    int slot;
    int hit;
    int fl;
    int x;
    unsigned char *p;
    unsigned char *m;

    OvlFunc_933_2009054();
    m = iwram_3001ebc;
    slot = gState[0x7d];
    o = *(unsigned char **)(m + 0x1e0);
    e = __MapActor_GetActor(slot);
    v[0] = *(int *)(e + 8);
    v[1] = *(int *)(e + 0xc);
    v[2] = *(int *)(e + 0x10) + (0xc0 << 9);
    hit = __TestCollision(e, v);
    fl = iwram_3001e40 & 4;
    if (fl == 0) {
        *(unsigned short *)((p = buf) + 0x22) = ((unsigned int)(__Random() << 12) >> 16) + (0xf8 << 8);
        x = *(int *)(e + 8) + (((unsigned int)(__Random() * 12) >> 16) << 16) - (6 << 16);
        OvlFunc_common0_10c(x, *(int *)(e + 0xc), *(int *)(e + 0x10), 0, fl,
                            ((unsigned int)(__Random() * 5) >> 16) * 0x1999 + 0x7ffd,
                            0x80 << 16, p);
    }
    if (hit < 0) {
        { register int q0 __asm__("r0"); register int q1 __asm__("r1");
          q0 = slot; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
        __Actor_TravelTo(e, *(int *)(e + 8), *(int *)(e + 0xc),
                         *(int *)(e + 0x10) + (0x80 << 12));
        __Actor_SetAnim(e, 7);
        __Actor_WaitMovement(e);
        do {
            __WaitFrames(1);
        } while (*(int *)(e + 0xc) != *(int *)(e + 0x14));
        __Actor_SetAnim(e, 6);
        __WaitFrames(3);
    } else {
        v[0] = *(int *)(e + 8);
        v[1] = *(int *)(e + 0xc);
        v[2] = *(int *)(e + 0x10) + (0x80 << 12);
        hit = __TestCollision(e, v);
        if (hit <= 0) {
            v[0] = *(int *)(e + 8) + 0x5b333;
            v[1] = *(int *)(e + 0xc);
            v[2] = *(int *)(e + 0x10) + 0x5b333;
            hit = __TestCollision(e, v);
            if (hit <= 0) {
                v[0] = *(int *)(e + 8) - 0x5b333;
                v[1] = *(int *)(e + 0xc);
                v[2] = *(int *)(e + 0x10) + 0x5b333;
                hit = __TestCollision(e, v);
                if (hit <= 0) {
                    *(int *)(o + 0x10) += 0xc0 << 9;
                    *(int *)(e + 0x10) += 0xc0 << 9;
                }
            }
        }
    }
}
