/* OvlFunc_common1_1814  --  split out of
 * asm/overlays/common/common1_c_a_c_c_a_c.s; 1928, 1b08 and 1ecc (parked) stay in
 * _c.s. Matched from scratch.
 *
 * - Its table is .L16, reached as _TBL_L16 (aliased this batch in the three
 *   overlay.ld files that link common, beside _TBL_L10..L13): gcc emits its own
 *   `.L16:` in this function, so an `__asm__(".L16")` extern would be captured.
 * - A TERNARY CAN FREE A RELOAD REGISTER: `xx = x < f8 ? x + C : x - C` loads f8
 *   before the add, so r3 is taken when reload runs and every reload moves to the
 *   ROM's r0 (21 -> 3). REG_ALLOC_ORDER is {3,2,1,0,...}: a ROM with all its
 *   reloads in r0 means r1-r3 held live values. OvlFunc_common1_850 must be `int`.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
    unsigned char pad38[0x64 - 0x38];
    short f64;
};

extern unsigned char TBL_L16[] __asm__("_TBL_L16");
extern unsigned char *iwram_3001f3c;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Actor_WaitScript(struct Actor *a);
extern int OvlFunc_common1_850(int a, int b);
extern void __Func_8019908(int a, int b);
extern void __Func_801776c(int a, int b);

int OvlFunc_common1_1814(int a, int b)
{
    unsigned char *s;
    int flag;
    struct Actor *actor;
    int x;
    int xx;
    int z;
    int v;
    short *p;
    unsigned char *g;

    s = iwram_3001f3c;
    flag = __GetFlag(0x211);
    g = gState;
    actor = __MapActor_GetActor(*(int *)(g + 0x1f4));
    x = *(int *)(s + 0xe8);
    xx = x < actor->f8 ? x + 0xc0000 : x - 0xc0000;
    if (flag) {
        z = *(int *)(s + 0xec) + 0x100000;
        v = *(unsigned short *)(s + 0xe4);
    } else {
        z = *(int *)(s + 0xec) - 0x100000;
        v = *(unsigned short *)(s + 0xe2);
    }
    p = &actor->f64;
    *p = v;
    actor->f34 = 0x4000;
    actor->f30 = 0x10000;
    __Actor_TravelTo(actor, xx, 0, z);
    __SetFlag(0x211);
    __Actor_SetScript(actor, TBL_L16);
    while (*p)
        __WaitFrames(1);
    if (!flag) {
        OvlFunc_common1_850(0, a);
        __Func_8019908(a, 2);
    } else {
        OvlFunc_common1_850(0, b);
        __Func_8019908(b, 2);
    }
    g = gState;
    __Func_8019908(*(int *)(g + 0x1f4), 1);
    __Func_801776c(0x96a, 3);
    __Actor_WaitScript(actor);
    return flag;
}
