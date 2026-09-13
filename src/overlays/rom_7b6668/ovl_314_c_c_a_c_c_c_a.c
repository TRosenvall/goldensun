/* OvlFunc_928_2008d0c, the whole of goldensun/asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_a.s.
 *
 * Total .text for this TU = 220 bytes. The .s is replaced outright, so no
 * linker-script change is needed.
 *
 * Reads save bit 0x200; if it is set, builds a position one tile ahead of the
 * player actor -- the two horizontal coordinates snapped to the tile grid and
 * the angle rounded to a quadrant -- and, when nothing blocks it, plays the
 * jump cutscene there.
 *
 * THE WHOLE FILE IS ONE ALIAS DECISION. The angle read has to be given an
 * alias set that CONFLICTS with the three `int` stores into `vec`. Written
 * with a raw cast --
 *
 *     ang = (*(unsigned short *)(p + 6) + (0x80 << 6)) & (0xc0 << 8);
 *
 * -- the load sits in the `short` alias set, the stores sit in `int`, the two
 * do not conflict, and sched2 issues the load one cycle too early:
 *
 *     rom    str r3, [r6, #8] / mov r3, #0x80 / ldrh r1, [r5, #6] / lsl r3, #6
 *     ours   str r3, [r6, #8] / ldrh r1, [r5, #6] / mov r3, #0x80 / lsl r3, #6
 *
 * Reading the angle as `p->f6` off a struct that ALSO HAS `int` MEMBERS gives
 * the load the struct's alias set, which has `int` as a subset, so the load
 * now depends on the last `vec` store, becomes ready one cycle later, and the
 * constant takes the slot. Exact, on tree-default flags.
 *
 * WHY IT IS A TIE AND NOT A PREFERENCE -- read out of the `.23.sched2` dump
 * with `-fsched-verbose=5`:
 *
 *     insn 66  ldrh r1,[r5,#6]     prio 39   (39 = prio(add) 37 + load latency 2)
 *     insn 234 mov  r3,#0x80       prio 39   (39 = prio(lsl) 38 + 1)
 *
 * Equal priority, equal dependent counts, so `rank_for_schedule` falls through
 * to INSN_LUID and the lower LUID wins -- and the two constant-build insns are
 * RELOAD-GENERATED, spliced in immediately before their use, so they can never
 * have a LUID below the load's. No statement order, no named local, no pin and
 * no barrier can reach that, which is why eleven such spellings all measured
 * exactly 2. The only lever is the READY TIME, and the only thing that moves
 * the ready time is the memory dependence.
 *
 * The exemplar this was parked against, OvlFunc_946_2009a44 in
 * src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.c, is
 * NOT a counter-example: it emits the load FIRST and its ROM has the load
 * first too. The old note in the park had that backwards.
 *
 * THE AGGREGATE MUST CONTAIN THE STORE'S TYPE. Measured, everything else held:
 *
 *   raw `*(unsigned short *)(p + 6)`                     2 differing
 *   `struct { unsigned short h; }`                       2
 *   `union  { unsigned short h; }`                       2
 *   `union  { unsigned short h; char c[2]; }`            2
 *   `union  { int w; unsigned short h; }`            EXACT
 *   `struct { unsigned short h, pad; int w; }`       EXACT
 *   this struct                                      EXACT
 *
 * A `char` member is not enough; an `int` member is. `-fno-strict-aliasing`
 * also matches, and is the flag reading of the same fact -- but it is not
 * needed and must not be added, because the source form carries it.
 */

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x1d];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_8092158(int a, int b, int c);
extern void __vec3_translate(int dist, int ang, int *v);
extern int __TestCollision(struct Actor *a, int *v);

void OvlFunc_928_2008d0c(void)
{
    struct Actor *p;
    int vec[3];
    int *v;
    unsigned char *f;
    int saved;
    int ang;

    p = __MapActor_GetActor(0);
    f = &p->f55;
    saved = *f;
    if (__GetFlag(0x80 << 2) != 0) {
        v = vec;
        v[0] = (p->f8 & 0xfff00000) + (0x80 << 12);
        v[1] = p->fc;
        v[2] = (p->f10 & 0xfff00000) + (0x80 << 12);
        ang = (p->f6 + (0x80 << 6)) & (0xc0 << 8);
        __vec3_translate(0x80 << 14, ang, v);
        if (__TestCollision(p, v) == 0) {
            __CutsceneStart();
            __Actor_SetAnim(p, 6);
            __WaitFrames(6);
            __PlaySound(0x98);
            __Actor_SetAnim(p, 7);
            p->f30 = 0xc0 << 10;
            p->f34 = 0x80 << 10;
            p->f28 = 0x80 << 11;
            *f = *f & 0x7e;
            __Actor_SetSpriteFlags(p, 0);
            __Func_8092158(0, *(short *)((char *)v + 2), *(short *)((char *)v + 0xa));
            __Actor_SetAnim(p, 6);
            __Actor_SetSpriteFlags(p, 1);
            *f = saved;
            __CutsceneEnd();
        }
    }
}
