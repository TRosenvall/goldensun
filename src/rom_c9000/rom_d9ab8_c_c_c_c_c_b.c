/* Anim_Fireball -- MATCHING.  1192 bytes, 536 encodings and 49 relocations
 * identical to the ROM.  Split out of asm/rom_c9000/rom_d9ab8_c_c_c_c_c.s,
 * where it is the SECOND of FOUR functions (Anim_Quake, Anim_Fireball,
 * Anim_Frost, Anim_Ray) followed by a .rodata tail.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b301e/fireI1.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c.s --func Anim_Fireball
 *
 * SPLIT SHAPE: TEXT-ONLY, FIVE NEW EXPORTS.  tools/datacheck.py says the file
 * needs a TEXT/DATA SPLIT and that Anim_Fireball reads .Leea41, .Leea44,
 * .Leea4a, .Leea50 and .Leea56, none of which is `.global` today (the file's
 * existing EXPORTS are .Leea08/.Leea20/.Leea2c, a different set).  Add
 *
 *     .global .Leea41
 *     .global .Leea44
 *     .global .Leea4a
 *     .global .Leea50
 *     .global .Leea56
 *
 * IMMEDIATELY BEFORE THEIR LABELS in the data-only remainder -- do not emit the
 * blobs from C, they sit inside the long .rodata run that ends at 0xeeb40.
 * Every other symbol already exists: iwram_3001eec and gPtrs and gBuffer are in
 * wram.sym; Task_SpinCamera is defined at asm/rom_c9000/rom_d6504_a_a.s:11;
 * Data_ede48 is already an extern in the LANDED src/rom_c9000/rom_d82b0_b.c and
 * rom_e0564_a_b.c.  ONE TO CHECK AT LINK TIME: `_Func_80c0cec` is so far only
 * spelled by PARKS (src/non_matching/rom_c9000/80cd86c.c and 80cd594.c), never
 * by a landed file -- it is a `bl` target in this bank's hand-written asm so the
 * definition exists, but this is the first .c to reference it and `make compare`
 * is the place that proves it.
 * NO SHIMS, NO PINS (tools/shimcount.py is clean), no per-file flag override.
 *
 * ================================================================
 * THE ONE THAT MATTERS: `(int)fp + k` INSTEAD OF `(char *)fp + k`, BECAUSE
 * POINTER ARITHMETIC IS CANONICALISED AND INTEGER ARITHMETIC IS NOT
 * ================================================================
 *
 * The last two encodings, and NINE other spellings could not touch them.  The
 * indirect blit is reached as `fp[k]` with k already scaled to a BYTE offset
 * (4 or 0, constant-folded in both arms), and the ROM emits
 *
 *     ldr r0, [sp, #0xc]   @ fp, reloaded from its spill slot
 *     mov r5, ip           @ k, copied out of r12
 *     ldr r4, [r5, r0]
 *
 * while every pointer-typed spelling gives the SAME `ldr r4, [r5, r0]` with the
 * two VALUES swapped -- fp in r5 and k in r0.  The printed operand positions
 * match, so this is not a scheduling difference: the PLUS inside the MEM has
 * its operands the other way round, and reload therefore hands the first
 * address reload the other register.  `(char *)fp + k` and `k + (char *)fp`
 * are the same tree, because C's fold canonicalises pointer arithmetic to put
 * the POINTER first.  Casting the pointer to `int` removes that rule:
 *
 *     (*(DrawFn *)(k + (int)fp))(ctx, ...)      <-- this file, 0 of 536
 *     (*(DrawFn *)((char *)fp + k))(ctx, ...)       2 of 536
 *
 * MEASURED INERT against the 2: `k + (char *)fp`; a named `DrawFn *fk` local
 * for the resolved slot; a named `DrawFn fn` local for the loaded function
 * (that one costs 241); declaring `k` at function scope before `fp`, first
 * among the scalars, or with `fp` moved last in the declaration list (the
 * pseudo-number theory does NOT reach it); writing the arms as if/else instead
 * of assign-then-correct; and a `register DrawFn * __asm__("r0")` pin.
 * `int fp` held as an integer throughout (`fp = (int)fns`, `*(DrawFn *)(fp+4)`,
 * `k + fp`) is ALSO exact -- same lever, uglier.
 *
 * ================================================================
 * FOUR MORE LEVERS, in the order they paid
 * ================================================================
 *
 * (1) ONE `Part *` FOR BOTH DRAW LOOPS, worth 544 -> 532 instructions and the
 *     whole frame.  The ROM keeps r6 for the particle walker in BOTH the
 *     matrix/homing loop and the sprite loop, r7 for the init loop's walker,
 *     r9 for base, r10 for the shared counter, r11 for base+0x7828 and r8 for
 *     `i / 2`.  With a block-local `Part *` per draw loop we create three
 *     pointer pseudos, one of them spills, base+0x7828 spills too, `i / 2 +
 *     0x30` lands in a HIGH register (three instructions per materialisation
 *     instead of two), and `sub sp` comes out 0x40 against the ROM's 0x38.
 *     This is the "pointers split, counters unify" pair resolving the OTHER way
 *     from Anim_Flare: here BOTH are unified.
 *
 * (2) `q = gBuffer;` MUST BE INSIDE AN EXPLICIT GUARD, NOT A `while` PREHEADER,
 *     worth 3 encodings per draw loop.  The ROM's `ldr r6,=gBuffer` sits AFTER
 *     the loop's entry test, which is where loop.c puts a strength-reduced
 *     initialiser -- but the giv route is WRONG here: `gBuffer + i` with i == 0
 *     gives `movs r7,#0 / ldr r3,=gBuffer / adds r6,r7,r3`, because gcc-2.96
 *     does not fold `PLUS (symbol, 0)` into an address (Anim_Froth's identical
 *     construct is exact only because its base is `reg + const`, which needs
 *     the add anyway).  So write the guard by hand:
 *
 *         i = 0;
 *         if (i != Leea41[...]) { q = gBuffer; do { ... } while (i != Leea41[...]); }
 *
 * (3) AND THE SECOND GUARD MUST BE SPELLED `i != n`, NOT `n != 0`, worth 5.
 *     Written as `if (Leea41[...] != 0)` the two guards are identical
 *     source-level tests in straight-line dominance, cse unifies the second
 *     one's two loads with the first's, the block collapses to a bare
 *     `cmp r3,#0`, and thread_jumps then sends the first guard's false edge
 *     PAST BOTH LOOPS -- four instructions and one `adds r2,r6,#0` short.  The
 *     ROM re-reads `Leea41[(*(State **)(base+0x7828))->f18]` for the second
 *     guard.  `if (i != ...)` with i already 0 is the same test, is folded to
 *     the same `cmp r3,#0` later, and is NOT recognised as a duplicate in time.
 *     A `while` loop is the other way to get this (the guard is manufactured
 *     after cse), and it is what the second loop uses -- but then (2) is lost;
 *     the hand-written guard buys both at once.
 *
 * (4) `sin(a) * amp`, NOT `amp * sin(a)`.  The ROM seeds the Thumb 2-address
 *     multiply from amp (`adds r3,r5,#0 / muls r3,r0`), which is the operand
 *     the source names SECOND.  Same for cos.  This is the commutative-operand
 *     lever, and it costs nothing to test.
 *
 * ALSO LOAD-BEARING, found on the first candidate and not moved since:
 *   - `cam = *(void **)((char *)g - 0x6c);` -- the bank's recorded negative-
 *     offset spelling, which reuses the iwram_3001eec pool word instead of
 *     adding an =iwram_3001e80 one.
 *   - `gPtrs[0x2e]` / `gPtrs[0x2f]`, not `*(DrawFn *)(gPtrs + 0xb8)`.  0xb8 is
 *     past Thumb's `ldr rd,[rn,#imm]` range, so gcc emits the ROM's
 *     `mov r3,r5 / add r3,#0xb8 / ldr r3,[r3]` on its own.
 *   - `DrawFn fns[2]` written through the ARRAY for [0] and through `fp = fns`
 *     for [1], with the g[0x2f] value loaded into `f1` BEFORE `fp = fns` --
 *     Anim_Confuse's reload-inheritance lever, verbatim.
 *   - DECLARATION ORDER IS THE FRAME MAP: v(0x2c) and fns(0x24) are the two
 *     aggregates, then the spilled scalars descend in declaration order --
 *     ctx 0x20, frame 0x1c, cam 0x18, base2 0x14, yb 0x10, fp 0xc.
 *   - `int mask = 0x7f;` as a source local (the ROM holds it in r11).
 *   - `w = 0xa - (v.z - 0xa0) / 64` with `Data_ede48[w - 1]` and `w * 2` as the
 *     last argument -- gcc CSEs `w*2` and derives the byte index as `2w - 2`.
 *   - `Leea44[u]` as an EMBEDDED ASSIGNMENT inside argument 3 of the sprite
 *     blit, `unsigned char w`, so the pool records Leea56 before Leea44 before
 *     Leea50 before Leea4a.  Anim_Flare's lever, transplanted unchanged.
 *   - `(unsigned)q->t <= 0xb` (unsigned guard) around a SIGNED `q->t / 2`.
 *   - `q->t = q->t + 1;` as a RE-READ, and `i % s->f14` recomputed for the
 *     Func_80d6888 call rather than shared with the homing block's copy.
 *   - `Func_80d6888(s2->ids[k3], 0xa, 5, k3, 4)` -- argument 4 IS k3; the ROM's
 *     `mov r3,r0` after __modsi3 would be dead otherwise.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned char Leea41[] __asm__(".Leea41");
extern unsigned char Leea44[] __asm__(".Leea44");
extern unsigned char Leea4a[] __asm__(".Leea4a");
extern unsigned char Leea50[] __asm__(".Leea50");
extern unsigned short Leea56[] __asm__(".Leea56");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int *_GetBattleActor(int id);
extern int  _Func_80b8530(int id);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Task_SpinCamera(void);
extern void _Func_80c0cec(int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Fireball(void *context)
{
    vec3_t v;
    DrawFn fns[2];
    void *ctx;
    int frame;
    void *cam;
    void *base2;
    int yb;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    int *actor;
    Part *p;
    Part *q;
    int i;
    int mask;
    int arg;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    cam = *(void **)((char *)g - 0x6c);
    base2 = g[2];
    *(State **)(base + 0x7828) = (State *)context;
    if (((State *)context)->f4 == 1) {
        AnimStart(1);
    } else {
        AnimStart(0);
    }
    LoadVFXFile(FILE_b4, base, 1, 1);
    LoadVFXFile(FILE_73, base2, 0, 0);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    fns[0] = (DrawFn)gPtrs[0x2e];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    f1 = (DrawFn)gPtrs[0x2f];
    fp = fns;
    fp[1] = f1;
    REG_BLDALPHA = 0x1010;
    actor = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->f8);
    yb = actor[3] + _Func_80b8530((*(State **)(base + 0x7828))->f8);
    p = gBuffer;
    i = 0;
    mask = 0x7f;
    do {
        int a = Random();
        int amp = (Random() & mask) + 0x7f;
        p->vx = sin(a) * amp >> 6;
        p->vy = (((Random() & mask) - 0x10) << 16) >> 6;
        p->vz = cos(a) * amp >> 6;
        p->x = actor[2];
        p->y = yb;
        p->z = actor[4];
        p->t = -1;
        i++;
        p++;
    } while (i != 0x40);
    *(int *)(base + 0x77ac) = 0;
    *(int *)(base + 0x77b0) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_SpinCamera, arg);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, arg);
    frame = 0;
    while (frame != (Leea41[(*(State **)(base + 0x7828))->f18] >> 1) + 0x84) {
        if ((unsigned)(frame - 0x11) <= 0x3e) {
            *(int *)(base + 0x77ac) = 0x100;
        } else {
            *(int *)(base + 0x77ac) = 0;
        }
        if (frame == (Leea41[(*(State **)(base + 0x7828))->f18] >> 1) + 0x6c) {
            _Func_80bd7dc(0x85);
        }
        _Func_80c0cec(0, 0, 0, 0x64);
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        i = 0;
        if (Leea41[(*(State **)(base + 0x7828))->f18] != 0) {
            q = gBuffer;
            do {
            int h = i / 2;
            if (frame > h && q->t == -1) {
                int w;
                int k;
                Func_80e3944((vec3_t *)q, &v);
                v.x >>= 1;
                if (v.z <= 0x9f) {
                    v.z = 0xa0;
                }
                if (v.z > 0x31f) {
                    v.z = 0x31f;
                }
                w = 0xa - (v.z - 0xa0) / 64;
                k = 4;
                if (frame >= h + 0x30) {
                    k = 0;
                }
                (*(DrawFn *)(k + (int)fp))((void *)ctx, (char *)base2 + Data_ede48[w - 1], v.x - w / 2,
                      v.y - w, w, w * 2);
                q->x += q->vx;
                q->y += q->vy;
                q->z += q->vz;
            }
            if (frame > h + 0x30 && q->t == -1) {
                State *s = *(State **)(base + 0x7828);
                int *t = (int *)*_GetBattleActor(s->ids[i % s->f14]);
                int ax, ay, az;
                ax = q->vx + ((t[2] - q->x) >> 9);
                q->vx = ax;
                ay = q->vy + ((t[3] - q->y) >> 9);
                q->vy = ay;
                az = q->vz + ((t[4] - q->z) >> 9);
                q->vz = az;
                if (frame < h + 0x55) {
                    q->vx = ax * 60 / 64;
                    q->vy = ay * 60 / 64;
                    q->vz = az * 60 / 64;
                }
                if (q->y < 0) {
                    q->t = 0;
                    q->x = v.x;
                    q->y = v.y;
                    _PlaySound(0x88);
                    {
                        State *s2 = *(State **)(base + 0x7828);
                        int k3 = i % s2->f14;
                        Func_80d6888(s2->ids[k3], 0xa, 5, k3, 4);
                    }
                    *(int *)(base + 0x77a8) = 2;
                }
            }
            i++;
            q++;
            } while (i != Leea41[(*(State **)(base + 0x7828))->f18]);
        }
        i = 0;
        if (i != Leea41[(*(State **)(base + 0x7828))->f18]) {
            q = gBuffer;
            do {
            if ((unsigned)q->t <= 0xb) {
                int u = q->t / 2;
                unsigned char w;
                fp[1]((void *)ctx, base + Leea56[u],
                      q->x - ((w = Leea44[u]) >> 1),
                      q->y + Leea50[u] - 0x38, w, Leea4a[u]);
                q->t = q->t + 1;
            }
            i++;
            q++;
            } while (i != Leea41[(*(State **)(base + 0x7828))->f18]);
        }
        if (*(int *)(base + 0x77b0) == 0) {
            *(int *)(base + 0x77b0) = 1;
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    StopTask(Task_SpinCamera);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
