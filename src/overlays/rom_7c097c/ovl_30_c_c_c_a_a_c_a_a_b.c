/* OvlFunc_936_2008464  --  0x02008464
 *
 * Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a.s.
 *
 * ONE OF THE MAP-EXIT FAMILY. A search of every .s for the opening
 * `push {r5,r6,r7,lr} / ldr r3, =iwram_3001ebc / ldr r7, [r3] /
 * bl __CutsceneStart / mov r5, #8 / mov r6, #0` returns six functions; this is
 * one of them and src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b_b.c
 * (OvlFunc_939_2008b6c) carries the full derivation.
 *
 * All six clear the "busy" byte on actors 8..0x41 in a loop, look an entrance
 * up in an eight-byte-per-entry table, run its script, and walk the player out.
 * This one has no `if (n != 6)` guard on the trailing nudge, which is why gcc reuses r5 for the table index instead of keeping the entrance number in r4.
 *
 * The two levers the family needs: the loop induction variable hoisted OUT of
 * the `for` init (otherwise gcc emits the zero before it, and the ROM has them
 * the other way round), and the entry's two HALFWORD fields named as locals
 * immediately before the call while the script pointer stays inline.
 */
struct Entry {
    void *script;
    unsigned short a;
    unsigned short b;
};

extern char *iwram_3001ebc;
extern struct Entry L50e0[] __asm__(".L50e0");
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern int __Func_8010560(void *s, int a, int b);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern int __MapActor_SetAnim(int slot, int n);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_936_2008464(void)
{
    char *p;
    char *a;
    short *e;
    unsigned int i;
    int z;
    int n;
    int vx;
    int vz;
    int d;
    int ea;
    int eb;

    p = iwram_3001ebc;
    vx = 0x80 << 8;
    vz = 0x80 << 7;
    d = -8;
    __CutsceneStart();
    i = 8;
    z = 0;
    for (; i <= 0x41; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0)
            a[0x55] = z;
    }
    e = (short *)(p + (0xb6 << 1));
    n = *e - 1;
    __PlaySound(0x9e);
    ea = L50e0[n].a;
    eb = L50e0[n].b;
    __Func_8010560(L50e0[n].script, ea, eb);
    __MapActor_SetSpeed(0, vx, vz);
    *(__MapActor_GetActor(0) + 0x55) = 0;
    __MapActor_SetAnim(0, 2);
    __Func_8092208(0, 2, d);
    __CutsceneWait(0xa);
    __Func_8091e9c(*e);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
