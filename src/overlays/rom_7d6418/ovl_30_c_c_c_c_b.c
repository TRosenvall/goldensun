/* OvlFunc_951_200973c  --  0x0200973c
 *
 * The tail of goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_c.s. HAND-SPLIT,
 * not with split_s.py: the file's `.data` and `.bss` sections follow this
 * function, and split_s.py keeps trailing data with the function it follows,
 * which would have carried the whole overlay's data into this .c and dropped
 * it. The two functions ahead of it and ALL the data stay in _a.s, and the five
 * .bss objects both sides reach needed `.global` added, since `.lcomm` is local.
 * The split was verified byte-neutral with make compare green before any C
 * landed -- which is the only thing that separates a layout mistake from a bad
 * decompilation.
 *
 * A camera pan with a scripted hand-off at frame 0x10.
 *
 * THE [cse] MARKER DID NOT HOLD, AND THE FLAG IS ACTIVELY HARMFUL HERE: it
 * costs 3 aligned on the otherwise-exact source, and made every intermediate
 * spelling worse. The repeats are in mutually exclusive arms, which rerun-CSE
 * does not common. Fourth counter-example in this batch. NO CSE_CFLAGS RULE.
 *
 * THE UN-ROTATED LOOP NEEDS NO goto. `for (init; ; inc)` with a trailing break
 * produces the ROM's `b body / inc: / body: ... bne inc` shape by itself --
 * expand_end_loop carries a Cygnus-local transform that does exactly this
 * rewrite. The hand-written backward `goto` spelling is WORSE, 27 against 11,
 * because it is not a natural loop to loop.c and therefore gets no
 * loop-invariant motion at all. Reach for `goto` only to DENY invariant motion,
 * never to obtain this shape.
 *
 * A POINTER LOCAL IS HOW YOU FORCE AN ADDRESS INTO A REGISTER LICM REFUSES TO
 * HOIST, and the refusal is a printed cost model rather than a guess.
 * move_movables hoists only when threshold * savings * lifetime >= insn_count.
 * A global read ONCE inside the loop has lifetime 1, because its address load
 * and its use are adjacent, so in an 89-insn loop it can never clear the bar and
 * gcc rematerialises the pool load at the use. The `-da` .08.loop dump prints
 * the verdict per insn, "move-insn savings N" against "not desirable". Naming
 * the pointer before the loop is what puts it in a callee-saved register.
 *
 * THE SAME LEVER RUNS BOTH WAYS AND THE `mov` DECIDES. For the camera struct
 * LICM *did* hoist, but hoisting inserts a COPY -- `ldr rA, =sym` then
 * `mov rB, rA` -- because the pre-loop and in-loop uses are separate pseudos; a
 * pointer local makes them one and the copy disappears. For the frame counter
 * the ROM's own output HAS that copy, so it must stay a bare global and a
 * pointer local there would be wrong.
 *
 *      ldr rA, =sym reloaded at each use   -> LICM declined -> pointer local
 *      ldr rA, =sym then mov rB, rA        -> LICM hoisted  -> bare global
 *
 * Three globals in one function, two spellings, and the ROM prints which is
 * which. 11 aligned to 5 to exact.
 *
 * Not a trap, though it looks like one: the `ldr r3, =0xffff / strh` came out
 * right from the plainest spelling on a `short` field. That is blocker 1b's
 * pooled side, which is what the ROM wants here -- check which side you are on
 * before reaching for an int local.
 */
struct Vec { int x, y, z; };

struct Ovl951Cam {
    /* 0x00 */ short unk0;
    /* 0x02 */ short unk2;
    /* 0x04 */ struct Vec v[4];
    /* 0x34 */ int unk34[3];
    /* 0x40 */ int unk40;
    /* 0x44 */ int unk44;
    /* 0x48 */ int unk48;
    /* 0x4c */ int unk4c;
};

extern struct Ovl951Cam L2070 __asm__(".L2070");
extern int L20c0 __asm__(".L20c0");
extern int L2130 __asm__(".L2130");
extern int L2134 __asm__(".L2134");
extern int L2138 __asm__(".L2138");

extern int ewram_2000434;

extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int actor, int anim);
extern int __MapActor_GetActor(int slot);
extern void __Actor_SetAnim(int actor, int anim);
extern void __WaitFrames(int frames);
extern void OvlFunc_951_2008e44(int a, int b);

int OvlFunc_951_200973c(int a)
{
    struct Ovl951Cam *c = &L2070;
    int *p = &ewram_2000434;

    c->v[0].y = 0;
    c->v[1].y = 0;
    c->v[2].y = 0;
    c->v[3].y = 0;
    L20c0 = a;
    L2134 = 0;
    c->unk2 = 0xffff;

    for (L2130 = 0; ; L2130++) {
        if (L2130 == 0x32)
            __PlaySound(300);
        if (L2130 == 0x10) {
            __MapActor_SetAnim(*p, 0x1d);
            c->unk2 = 0;
            c->unk40 = 0x14ccc;
            c->unk44 = 0x40000;
            c->unk48 = -0x20000;
            c->v[0].x = 0x780000;
            c->v[0].y = 0x100000;
            c->v[0].z = 0x980000;
            c->unk4c = 300;
            if (L20c0 == 1) {
                __Actor_SetAnim(__MapActor_GetActor(0x10), 3);
                __Actor_SetAnim(__MapActor_GetActor(0x11), 0);
                OvlFunc_951_2008e44(0xf, 1);
                OvlFunc_951_2008e44(0xe, 1);
                OvlFunc_951_2008e44(0xd, 1);
            } else {
                __Actor_SetAnim(__MapActor_GetActor(0xb), 3);
                __Actor_SetAnim(__MapActor_GetActor(0xc), 0);
                OvlFunc_951_2008e44(0xa, 1);
                OvlFunc_951_2008e44(9, 1);
                OvlFunc_951_2008e44(8, 1);
            }
        }
        __WaitFrames(1);
        if (L2134 == 1)
            break;
    }
    return L2138;
}
