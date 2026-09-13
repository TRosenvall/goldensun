/* OvlFunc_957_2008de8, the whole of goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c_c.s.
 *
 * Total .text for this TU = 196 bytes. The .s is replaced outright and the
 * overlay.ld line keeps its asm/ prefix, so no linker-script change is needed.
 *
 * Takes the actor whose slot id sits at gState+0x1F4, builds a position one
 * step ahead of it along its facing quadrant, and when nothing blocks that
 * position plays the jump cutscene there.
 *
 * UNPARKED BY THE ALIAS DEVICE, and the park it closes was wrong about its own
 * blocker. It was recorded as "instruction placement (pre-reload scheduling),
 * TWO lines", parked as a scheduling wall with "NEXT: nothing source-level".
 * The score was right -- 2 of 84, verified -- but the class was not: this tree
 * never runs sched1 (no .21.sched dump is produced, the pass order goes
 * 20.ce2 -> 23.sched2), so there is no pre-reload scheduling to blame, and the
 * residue is reachable from source.
 *
 * THE RESIDUE. The ROM interleaves the angle mask's constant build into the
 * tail of the three vector stores; written with raw casts we hoist it one slot
 * earlier, into the load-use stall after the third vector load:
 *
 *     rom    ldr r3, [r5, #0x10] / str r3, [r6, #8] / mov r1, #0xf0 / ldrh r3, [r5, #6] / lsl r1, #8
 *     ours   ldr r3, [r5, #0x10] / mov r1, #0xf0    / str r3, [r6, #8] / ldrh r3, [r5, #6] / lsl r1, #8
 *
 * Reading the actor through a struct that has `int` members gives the angle
 * load the struct's alias set, which has `int` as a subset, so it now conflicts
 * with the three `int` stores into `vec`. Exact, on tree-default flags.
 *
 * THE MECHANISM IS NOT THE ONE RECORDED FOR OvlFunc_928_2008d0c. That function
 * is the same shape and the same device closes it, but there the new dependence
 * works by DELAYING THE LOAD'S READY TIME so it is absent from the ready list
 * at the contested slot. Here the load's ready time is irrelevant -- it is not
 * in the contested ready list either way. What moves is a PRIORITY, and it
 * moves because the new edge REPLACES a zero-cost one. Read out of the
 * `.23.sched2` dump with `-fsched-verbose=5`:
 *
 *     insn 43   str r3,[r6,#8]      prio 37   -> 39
 *     insn 46   ldrh r3,[r5,#6]     prio 37      37   (dep count 4 -> 6)
 *     insn 210  mov r1,#0xf0        prio 37      37   (reload-generated)
 *
 * Raw-cast, insns 43, 46 and 210 all sit at priority 37. 43 and 46 were already
 * joined by an edge -- an ANTI-dependence, because 43 reads r3 and 46 rewrites
 * it -- but an anti-dependence has LINK COST ZERO, so it contributes nothing to
 * 43's longest-path priority. Under the struct spelling the same pair is joined
 * by a TRUE memory dependence of cost 2, and 43's priority becomes 37 + 2 = 39.
 * The three-way tie at 37 is gone: at t=57 the ready list is `62 212 210 43`
 * and the store wins outright on priority, no tie-break consulted. The constant
 * then takes t=59 and the ROM's order falls out.
 *
 * > Generalising: the device does not only push a READY TIME later. Where the
 * > two insns are ALREADY joined by an anti- or output dependence, promoting
 * > that edge to a true dependence raises the PRODUCER'S priority by the link
 * > cost, because priority is a longest path and anti-dependence links cost 0.
 * > That is why the same device reaches residues pointing in OPPOSITE
 * > directions -- which retires this park's "the pairing is the finding"
 * > reading of 2008d0c and 2008de8 wanting opposite things.
 *
 * WHICH AGGREGATE. Measured here, all against the ROM's 84 lines:
 *
 *   raw casts throughout (the park)                       2 differing
 *   struct only on the three vec LOADS, raw angle read    2
 *   struct only on the angle read, raw elsewhere      EXACT
 *   `union { int w; unsigned short h; }` on the angle  EXACT
 *   this struct, used throughout                       EXACT
 *
 * The angle read is the whole lever; the vec loads' spelling is inert.
 *
 * `-fno-strict-aliasing` also reaches EXACT and is the flag reading of the same
 * fact. It is not needed and must not be added -- the source form carries it.
 *
 * SECOND COUNTEREXAMPLE TO THE -fno-schedule-insns2 SIGN RULE. The flag
 * REGRESSES 2 -> 17 here, which by the old rule says sched2 is already right
 * and alias is the wrong axis. Alias is the only axis that works. As on
 * 2008d0c, sched2 gets 82 of 84 encodings right and one tie wrong, so turning
 * it off measures the 82 it was getting right. The rule is diagnostic for a
 * REGION, not for a single adjacent pair. `-fno-schedule-insns` is inert, as it
 * is everywhere in this tree.
 *
 * THE NAMED gState OFFSET IS STILL REQUIRED, and it is not a scheduling matter.
 * Written inline as `(unsigned char *)&gState + (0xfa << 1)` the sum folds to a
 * single pooled symbol: 82 instructions against 84, 192 bytes against 196, 71
 * differing. Naming it (`off = 0xfa << 1;`) keeps the `mov #0xfa / lsl #1 / add`
 * the ROM has. That was the one thing the park had right and it is confirmed
 * here.
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

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_8092158(int a, int b, int c);
extern void __vec3_translate(int dist, int ang, int *v);
extern int __TestCollision(struct Actor *a, int *v);

void OvlFunc_957_2008de8(void)
{
    struct Actor *p;
    int vec[3];
    int *v;
    unsigned char *f;
    int saved;
    int ang;
    int off;

    off = 0xfa << 1;
    p = __MapActor_GetActor(*(int *)((unsigned char *)&gState + off));
    f = &p->f55;
    saved = *f;
    v = vec;
    v[0] = p->f8;
    v[1] = p->fc;
    v[2] = p->f10;
    ang = p->f6 & (0xf0 << 8);
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
