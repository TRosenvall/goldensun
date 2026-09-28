/* Field_Catch -- PARK, first-candidate draft.
 * NON-MATCHING, 237 encodings of 259.  NOT a distance -- 261 encodings / 592 bytes against 259 / 588, two instructions long.
 * READ `--align`: 121 of 262.  No shims.  Two independent blockers: the ROM spills the
 * target-actor pointer to sp+0 (eight long-lived pseudos against our six), and gcc
 * strength-reduces `i * (0xc0 << 8)` into an accumulator where the ROM keeps the `mul` --
 * `-fno-strength-reduce` shortens the loop but does NOT restore the multiply, so a
 * STRENGTH_CFLAGS rule is not the answer on its own.
 * The reference .s comment is stale: it names this function RunFieldAbility with a
 * "~500-instruction body"; it is Field_Catch at 253.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809ad70.c \
 *     asm/rom_8a000/rom_9ad70_a_c.s --func Field_Catch
 * asm/rom_8a000/rom_9ad70_a_c.s (grep -ci func_start = 1, datacheck: no data;
 * whole-file conversion when it lands).
 *
 * NOT a true distance: objcmp --whole 237 of 259 differ, ours 261 encodings /
 * 592 bytes against 259 / 588 -- TWO INSTRUCTIONS AND FOUR BYTES LONG.
 * tryc --align 121 of 262.  Everything after the prologue is positionally
 * shifted, so both numbers over-report.
 *
 * The .s's leading comment is STALE: it names the function RunFieldAbility with
 * a "~500-instruction body". It is Field_Catch at 253.
 *
 * WHAT IS ALREADY RIGHT: the whole control skeleton -- the 0x28 frame, three
 * vec3s, the s->f20 signed-char test, `vec3_translate(0x80 << 14, s->f00, &b.x)`
 * (three args: dist, angle, vec -- and the dist constant is CSE'd with the
 * y-component addend, which is why the ROM leaves it in r0 across the call),
 * `CreateParticleActor(0xd7, c.x, c.y, c.z)` reaching r1/r2/r3 with the three
 * values just stored into C, the four loops, and the pooled `.word 0` with its
 * mid-function `.pool` -- which is a QImode `strb` of a literal 0 taking the
 * same force_const_mem path that a HImode store does, and it reproduces from a
 * plain `a->f55 = 0`.
 *
 * DECLARATION ORDER IS A FRAME-LAYOUT LEVER, and it is what the two numbers
 * disagree about.  `FRAME_GROWS_DOWNWARD` is 1 for ARM (arm.h:1297), so
 * assign_stack_local hands the FIRST-allocated local the HIGHEST sp offset and
 * the LAST-allocated -- which is always a reload spill -- offset 0.  Declaring
 * the vec3s `c, v, b` instead of `b, v, c` puts b at the lowest local offset,
 * matching the ROM's 4 / 0x10 / 0x1c.  tryc --align calls that a REGRESSION
 * (119 -> 121) and objcmp calls it an improvement (265 -> 261 encodings,
 * 600 -> 592 bytes).  objcmp is right; this is the recorded "a count that RISES
 * while the length becomes right is progress", and the aligned view is the one
 * that misleads.
 *
 * THE BLOCKER: the ROM SPILLS the target-actor pointer to sp+0 and the
 * candidate keeps it in r11.  The ROM has seven long-lived pseudos --
 * r5 (state), r6 (particle), r7 (entity then the loop counter), r9/r11 (the two
 * vec3 addresses) and r8/r10 (the per-loop `from`/`to` copies) -- so the eighth,
 * the target pointer, spills; and because a spill slot is allocated last it
 * lands at offset 0 and pushes all three vec3s up by four.  This candidate's
 * `p`/`q` are folded into the address pseudos, so only six are live and nothing
 * spills.  Writing `pa`/`pb` as separate locals and `p = pa; q = pb;` per loop
 * (t4_v2.c) does not separate them either: 251 of 259, 596 bytes.
 *
 * SECOND, INDEPENDENT BLOCKER: gcc strength-reduces `i * (0xc0 << 8)` into an
 * accumulator (`add r9, r2` per iteration) and rewrites the exit test as
 * `cmp r7,#0xa / ble`; the ROM recomputes `mul r0, r3` every iteration and
 * tests `cmp r7,#0xb / blt`.  `-fno-strength-reduce` shortens the loop by five
 * lines but does NOT restore the multiply, so STRENGTH_CFLAGS is not the answer
 * on its own.  loop.c's giv creation is the pass to read.
 *
 * THE MUL LEVER POINTED THE WRONG WAY AGAIN, third batch running.  The ROM has
 * `mov r0, r7 / mul r0, r3` -- the counter copied, the difference on the right --
 * so the procedure says write `(q[0] - p[0]) * i`.  Measured, that spelling is
 * 254 of 262 against 119: a catastrophic regression, because it also changes
 * which operand the strength reducer picks.  `i * (q[0] - p[0])` stays.
 *
 * SHIMS: none.
 */
typedef struct {
    int x;
    int y;
    int z;
} vec3;

struct Ent {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
};

struct Actor {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    unsigned char pad34[0x55 - 0x34];
    unsigned char f55;
};

struct FState {
    int f00;
    int f04;
    int f08;
    int f0c;
    struct Ent *f10;
    struct Actor *f14;
    unsigned char pad18[8];
    signed char f20;
};

extern struct FState *iwram_3001f30;

extern void vec3_translate(int dist, int angle, int *v);
extern struct Actor *CreateParticleActor(int id, int x, int y, int z);
extern void Func_8097384(void);
extern void Func_809748c(void);
extern void _PlaySound(int id);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetColorswap(struct Actor *a, int n);
extern void _Actor_SetPos(struct Actor *a, int x, int z);
extern void _DeleteActor(struct Actor *a);
extern void WaitFrames(int n);

void Field_Catch(void)
{
    struct FState *s;
    struct Ent *e;
    struct Actor *a;
    struct Actor *tgt;
    vec3 c;
    vec3 v;
    vec3 b;
    int *p;
    int *q;
    int i;
    int t;

    s = iwram_3001f30;
    tgt = s->f14;
    e = s->f10;
    v.x = e->f08;
    v.y = e->f0c + (0x80 << 13);
    v.z = e->f10;
    if (s->f20 != 0) {
        b.x = e->f08;
        b.y = e->f0c + (0x80 << 14);
        b.z = e->f10;
        vec3_translate(0x80 << 14, s->f00, &b.x);
    } else {
        b.x = s->f04;
        b.y = s->f08 + (0x80 << 14);
        b.z = s->f0c;
    }
    c.x = s->f04;
    c.y = s->f08 + (0x80 << 14);
    c.z = s->f0c;
    a = CreateParticleActor(0xd7, c.x, c.y, c.z);
    if (a == 0)
        return;
    Func_8097384();
    _PlaySound(0x8a);
    a->f06 = e->f06;
    a->f30 = 0x14ccc;
    a->f55 = 0;
    _Actor_SetAnim(a, 5);
    _Actor_SetColorswap(a, 1);
    i = 0;
    p = &v.x;
    q = &b.x;
    do {
        a->f08 = p[0] + i * (q[0] - p[0]) / 10;
        a->f0c = p[1] + i * (q[1] - p[1]) / 10;
        a->f10 = p[2] + i * (q[2] - p[2]) / 10;
        t = i * (0xc0 << 8) / 10 + (0x80 << 7);
        a->f18 = t;
        a->f1c = t;
        i++;
        WaitFrames(1);
    } while (i < 0xb);
    WaitFrames(0xa);
    _Actor_SetAnim(a, 6);
    WaitFrames(0xf);
    i = 9;
    do {
        a->f0c += 0xfffe0000;
        WaitFrames(1);
        i--;
    } while (i >= 0);
    _Actor_SetAnim(a, 5);
    _PlaySound(0x84);
    if (tgt != 0)
        _Actor_SetPos(tgt, 0xfff70000, tgt->f0c);
    WaitFrames(0x14);
    i = 0xc;
    do {
        a->f0c += 0xc0 << 9;
        WaitFrames(1);
        i--;
    } while (i >= 0);
    WaitFrames(0xa);
    _PlaySound(0x72);
    i = 0;
    p = &b.x;
    q = &v.x;
    do {
        a->f08 = p[0] + i * (q[0] - p[0]) / 10;
        a->f0c = p[1] + i * (q[1] - p[1]) / 10;
        a->f10 = p[2] + i * (q[2] - p[2]) / 10;
        t = i * 0xffff4000 / 10 + (0x80 << 9);
        a->f18 = t;
        a->f1c = t;
        i++;
        WaitFrames(1);
    } while (i < 0xb);
    _DeleteActor(a);
    Func_809748c();
}
