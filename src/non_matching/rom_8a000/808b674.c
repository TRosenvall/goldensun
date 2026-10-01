/* InitMapActors -- 0x0808b674.  PARKED at 14 of 195, BUT THE PARK'S FILE COULD
 * NEVER HAVE LANDED AND THIS ONE CAN.
 * ref: asm/rom_8a000/rom_8b674_a_a.s  (ONE function, no data section; whole-file
 *      conversion, no split -- confirmed again, tools/datacheck.py prints nothing)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_8a000/rom_8b674_a_a.c \
 *     asm/rom_8a000/rom_8b674_a_a.s --whole
 *
 * *** THE CORRECTION: THE PARKED FILE DIFFERED IN ITS RELOCATIONS. ***
 * The park recorded "14 encodings of 195 differ" and reported the pool entries as
 * one of four clusters.  objcmp --whole on the parked body says
 *     XX RELOCATIONS differ
 * on top of the 14.  A relocation difference is a hard fail -- `make compare`
 * cannot pass it -- so the park's 14 was not 14 away from a landing at all.  The
 * park's own cluster (d) explains why: it chose the `b` before `a` source order
 * because that "fixes the registers", and that puts ewram_200fe00 in the literal
 * pool ahead of gBuffer, which is the reverse of the ROM.
 *
 * THIS FILE IS 14 WITH THE RELOCATIONS IDENTICAL.  Two changes, and the second is
 * only worth anything once the first is in place:
 *   1. `a = &gBuffer[i];` BEFORE `b = &ewram_200fe00[i];`  -- pool order follows
 *      first reference, so this is what the ROM's pool wants.  ALONE it is 16 of
 *      195 (relocations clean): it costs the two `ldrb` at [101]/[104], because
 *      a and b come out in the opposite registers to the ROM.
 *   2. the condition's two flag tests SWAPPED, `b->flags` before `a->flags`.
 *      ALONE on the parked body it is meaningless; on top of (1) it takes 16
 *      back to 14 by restoring the ROM's PAIRING of pointer to test, which is
 *      exactly what the two `ldrb` encodings at [101]/[104] measure.  Both
 *      tests are side-effect-free loads, so the swap is semantics-neutral.
 *
 * Measured: parked body 14 + RELOCDIFF; (1) alone 16 clean; (1)+(2) 14 clean.
 * This is the brief's "edits each a clear regression, jointly a gain" shape in
 * miniature -- (1) alone LOOKS like a two-encoding regression and is the only
 * version that can ever link.
 *
 * ------------------------------------------------ WHAT THE 14 STILL ARE ------
 * Same four clusters, re-measured, with the park's own diagnoses tested:
 *
 *  a) [2] indices 30/32.  `movs r2, #0` and `mov ip, r8` in the opposite order.
 *     A sched2 tie decided on the last rung, INSN_LUID: both are independent of
 *     last_scheduled_insn (CLASS 3) and both have zero dependents inside the
 *     preheader, so the pre-sched source order decides, and the loop body's zero
 *     is hoisted to the END of the preheader and therefore always later than the
 *     `lim` copy.  NOT reachable by naming the zero: `int z = 0;` before `lim`,
 *     between `lim` and `w`, and after `w` all measure 185-186 (and +4 bytes) --
 *     `z` becomes a call-crossing allocno and the whole frame changes.  Dropping
 *     `lim` and comparing against `(int)g` directly is 166 and -4 bytes.
 *     `w` before `lim` is 15 -- the park's lever 6 confirmed, worth 1.
 *
 *  b) [5] indices 67-71.  The ROM copies g out of r8 and uses the REGISTER-OFFSET
 *     load (`mov r0, r8 / ldr r5, [r0, r3]`); we fold (`add r3, r8 / ldr r5,
 *     [r3]`).  INERT: `*(unsigned char **)(n + g)`, `*(unsigned char **)((int)g
 *     + n)`, `*(unsigned char **)&g[n]`, `(unsigned char *)*(int *)(g + n)`.
 *     WORSE: `((unsigned char **)g)[id + 5]` 16; and -- NEW -- folding `n` into
 *     the subscript at all (`*(unsigned char **)(g + (id * 4 + 0x14))`, or
 *     splitting it as `n = id * 4` plus a `+ 0x14` in the address) is 134 of 195
 *     at FOUR BYTES SHORT: the named `n` is load-bearing, it is what keeps the
 *     fifth call-crossing allocno and therefore the ROM's push list.
 *
 *  c) [2] indices 79/80.  `ldr r2, =0xfffff` against our `ldr r1`.  The park
 *     called it "reload scratch rotation (allocate_reload_reg / last_spill_reg)".
 *     IT IS NOT A RELOAD AT ALL -- it is an ordinary insn in the signed-division
 *     expansion, and the register is local-alloc's.  The ROM reuses r2, which the
 *     `asrs r2, r3, #20` two insns later also wants; we use r1 and leave r2 for
 *     the shift.  INERT: splitting the two divisions into named locals (either
 *     order, 14 and 23), `<< 7` instead of `* 128`, `128 * (...)` instead of
 *     `(...) * 128`, parenthesising each division.
 *
 *  d) [5] indices 90-96.  a and b exchanged plus one sched2 tie.  .17.lreg gives
 *     the number instead of the story: the two pointers are both refs=2, and the
 *     one with the SHORTER live length is allocated first and takes r2 --
 *     live=5 against live=9 here.  Whichever is written second is the shorter,
 *     and whichever is referenced first owns the earlier pool entry, so the two
 *     requirements really are in conflict through one variable.  The park said a
 *     shape getting both "was not found"; that is still true, and now it has a
 *     mechanism rather than an observation.  INERT: declaration order of a and b,
 *     `(struct Tile *)((char *)gBuffer + i * 4)`, plain pointer arithmetic.
 *     WORSE: inlining either subscript into the condition (21-26), reordering the
 *     0x1e0 test (22).
 *
 * SHIM: still ONE, `register unsigned char *g __asm__("r8")`, and still
 * load-bearing (the park measured removal at 50 of 195).  Needs a fakematch.txt
 * row if this lands.  tools/shimcount.py counts it.
 *
 * -- re-measured in scratch_elev/b316c/v_p2, v_p2b, v_p2c
 */

struct Tile {
    unsigned char f0;
    unsigned char f1;
    unsigned char flags;
    unsigned char f3;
};

struct Rec {
    int w[6];
};

union hw { short h; int i; };

extern unsigned char *iwram_3001ebc;
extern unsigned char **iwram_3001e70;
extern unsigned char gState[];
extern struct Tile gBuffer[];
extern struct Tile ewram_200fe00[];
extern struct Rec L9f810[] __asm__(".L9f810");
extern void Func_808b9f8(void);
extern void LoadMapActors(unsigned char *a, int b);
extern int _Func_8011f54(int a, int x, int z);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern unsigned char *_CreateActor(int id, int x, int y, int z);
extern void _Camera_SetTarget(unsigned char *cam, unsigned char *a);
extern unsigned char *_Sprite_AddLayer(void *s, int n);

void InitMapActors(unsigned char *arg)
{
    register unsigned char *g __asm__("r8");
    unsigned char *s;
    unsigned char *s0;
    unsigned char *s2;
    unsigned char *r;
    unsigned char *act;
    unsigned char *cam;
    unsigned char *lay;
    struct Tile *a;
    struct Tile *b;
    int id;
    int i;
    int n;
    int w;
    int lim;
    int h;
    int y;
    int m;

    g = iwram_3001ebc;
    s0 = gState;
    id = *(int *)(s0 + 0x1f4);
    r = g + 0x200;
    *(struct Rec *)r = L9f810[0];
    *(struct Rec *)(g + 0x218) = L9f810[1];
    lim = (int)g;
    w = (int)g + 0xc;
    do {
        *(int *)w = 0;
        w -= 4;
    } while (w >= lim);
    Func_808b9f8();
    s = gState;
    m = 0xffff;
    ((union hw *)(r + 2))->h = m;
    ((union hw *)r)->h = id;
    *(int *)(r + 8) = *(int *)(s + 0x1dc);
    *(int *)(r + 0xc) = 0;
    *(int *)(r + 0x10) = *(int *)(s + 0x1e4);
    *(short *)(r + 0x14) = *(int *)(s + 0x1e8);
    LoadMapActors(r, id);
    LoadMapActors(arg, 8);
    n = id * 4 + 0x14;
    act = *(unsigned char **)(g + n);
    *(char *)(act + 0x22) = *(unsigned short *)(s + 0x1ec);
    i = *(int *)(act + 8) / 0x100000
        + *(int *)(act + 0x10) / 0x100000 * 128;
    a = &gBuffer[i];
    b = &ewram_200fe00[i];
    if (*(int *)(s + 0x1e0) != 0 && b->flags == 0xfd && a->flags == 0xfd) {
        *(char *)(s + 0x1f2) = 1;
        h = _Func_8011f54(0, *(int *)(act + 8),
                          *(int *)(act + 0x10) - 0x100000) - 0x200000;
        y = *(int *)(act + 0xc);
        y += h;
        *(int *)(act + 0xc) = y;
        *(int *)(act + 0x14) = y;
        *(char *)(act + 0x55) = 0;
        _Actor_SetSpriteFlags(act, 0);
        _Actor_SetAnim(act, 0xc);
    } else {
        s2 = gState;
        *(char *)(s2 + 0x1f2) = 0;
    }
    cam = _CreateActor(0x80 << 8, *(int *)(act + 8), *(int *)(act + 0xc),
                       *(int *)(act + 0x10));
    *(int *)(cam + 0x14) = *(int *)(act + 0x14);
    _Camera_SetTarget(cam, act);
    if (*(short *)(g + 0x19e) == 3) {
        lay = _Sprite_AddLayer(*(void **)(act + 0x50), 0x17);
        *(char *)(lay + 5) = 0xf;
        *(char *)(lay + 6) = 9;
    }
    *iwram_3001e70 = cam + 8;
    *(unsigned char **)(g + 0x1e0) = cam;
}
