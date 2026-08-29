/* OvlFunc_939_2008b6c  --  0x02008b6c
 *
 * Cut out of goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b.s.
 * The entrance table it indexes, `.L250c`, stays in the sibling piece and is
 * reached with the asm-label extension.
 *
 * The map-exit cutscene: clear the "busy" byte on every actor in the map, play
 * the door cue, run the exit script for this entrance, walk the player out.
 *
 * THE ENTRANCE TABLE IS `{ void *script; u16 a; u16 b; }` -- eight bytes an
 * entry, read off `lsl r4, r5, #3` and the three loads at +0, +4 and +6.
 *
 * TWO ORDERING LEVERS, and the second is the interesting one.
 *
 * 1. `i = 8;` HAS TO BE HOISTED OUT OF THE `for`. Written as
 *    `z = 0; for (i = 8; ...)`, gcc emits `mov r6, #0` before `mov r5, #8`;
 *    the ROM has them the other way round. Written as `i = 8; z = 0; for (; ...)`
 *    they come out in source order. Two instructions.
 *
 * 2. THE TWO HALFWORD FIELDS MUST BE NAMED, the script pointer must not. The
 *    ROM loads the entry's `a` and `b` and only then the script pointer into
 *    r0:
 *
 *        rom    add r3, r4, #4 / ldrh r1, [r0, r3] / add r3, r0
 *                 / ldrh r2, [r3, #2] / ldr r0, [r0, r4]
 *        ours   ldr r0, [r2, r3] / add r3, #4 / ldrh r1, [r2, r3] / ...
 *
 *    Naming `a` and `b` as locals immediately before the call gets the ROM's
 *    order; the return-type lever does not (`__Func_8010560` is declared `int`
 *    here for a different reason and changing it moves nothing). Naming the
 *    TABLE instead -- a `struct Entry *t` assigned before the loop, which is
 *    the basic-block lever's usual shape -- is 71 lines against 67 and 70
 *    differing, much worse: it makes the pointer live across the loop.
 *
 * That second one is the carried-vs-rebuilt rule choosing carried. The two
 * halfwords are consumed immediately by one call, so they want adjacency; the
 * table base is not a value the ROM carries at all.
 *
 * The loop's zero IS carried -- one `int z` across sixty `__MapActor_GetActor`
 * calls, which is why the ROM holds it in r6.
 */
struct Entry {
    void *script;
    unsigned short a;
    unsigned short b;
};

extern char *iwram_3001ebc;
extern struct Entry L250c[] __asm__(".L250c");
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

void OvlFunc_939_2008b6c(void)
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
    __PlaySound(0x9e);
    e = (short *)(p + (0xb6 << 1));
    n = *e - 4;
    ea = L250c[n].a;
    eb = L250c[n].b;
    __Func_8010560(L250c[n].script, ea, eb);
    __MapActor_SetSpeed(0, vx, vz);
    *(__MapActor_GetActor(0) + 0x55) = 0;
    __MapActor_SetAnim(0, 2);
    if (n != 6) {
        __Func_8092208(0, 2, d);
        __CutsceneWait(0xa);
    }
    __Func_8091e9c(*e);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
