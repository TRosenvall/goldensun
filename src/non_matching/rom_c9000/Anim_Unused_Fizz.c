/* Anim_Unused_Fizz -- PARKED.  0x080d5e54, 747 instructions in the ROM listing.
 * NON-MATCHING, 118 of 770 encodings differ.
 * SIZE  ref 1712 bytes, ours 1712  -- EXACT.
 * COUNT ref 770, ours 770          -- EXACT.
 * Both axes exact, so 118 IS a true distance, not a saturated figure.
 * tools/aligncmp.py: aligned-equal 703 of 770 (91.3%), 80 differing in 42 hunks.
 * RELOCATIONS: the symbol SEQUENCE matches; the only offset differences are the
 * three one-instruction pool-load transpositions listed under BLOCKERS.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent -- no register pins, no
 * `.equ`, no `asm volatile`, no per-file Makefile flag override.  Pin-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Unused_Fizz.c \
 *     asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s --func Anim_Unused_Fizz
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Unused_Fizz.c \
 *     asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s Anim_Unused_Fizz -v
 *
 * ================================================================
 * SPLIT SHAPE -- THREE-WAY, ONE NEW EXPORT, AND IT IS NOT THIS FUNCTION'S
 * ================================================================
 *
 * asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s holds FOUR functions --
 * BaseAnim_ParticleCloud (0x080d52c8), Anim_Sleep (0x080d59b0), Anim_Curse
 * (0x080d5c48) and Anim_Unused_Fizz (0x080d5e54, LAST) -- then a .rodata tail
 * whose only label is `.Lee2ae`.  `tools/split_s.py --dry-run <file>
 * Anim_Unused_Fizz` gives:
 *
 *   asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c_a.s   ParticleCloud + Sleep + Curse
 *   src/rom_c9000/rom_d5258_c_c_c_c_c_c_c_b.c   THIS FUNCTION
 *   asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c_c.s   the .rodata
 *
 * tools/datacheck.py says Anim_Unused_Fizz READS NO DATA LABEL, so this file
 * needs no `.global` of its own.  The ONE export the split needs is for a
 * SIBLING: `.Lee2ae` is read by BaseAnim_ParticleCloud, which the split moves
 * into the `_a` piece while the label stays in `_c`.  Add
 *
 *     .global .Lee2ae
 *
 * immediately before its label, prove `make compare` green with only that
 * change, then split and prove it green again, then write the .c.  Every symbol
 * this file names is already global: Data_ede48 / Data_ede84 / Data_ede96 are
 * `.incdata` in asm/rom_c9000/rom_eda78.s (and `.incdata` expands to
 * `.global \sym`, include/macros.inc:46), iwram_3001eec / iwram_3001e80 /
 * gBuffer are in wram.sym, and Func_8000948, Func_80e38b8, Func_80e3944,
 * Func_80d6888, BuildDraw2DFuncs, MatrixPush/Pop/Yaw/Pitch/Roll and
 * MatrixTranslatev are each already spelled by a landed or parked .c in this
 * bank.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE VIEW POINTER IS A SOURCE LOCAL, ONE PER LOOP --
 * 630 -> 139 OF 770, AND IT IS WHAT MADE SIZE AND COUNT EXACT
 * ================================================================
 *
 * The first four candidates were stuck EIGHT BYTES SHORT on the frame
 * (`sub sp,#0xa4` against the ROM's `#0xac`) with every single spill-slot
 * reference off by 8, which alone accounted for roughly a hundred hunks.  Two
 * spill slots were missing and the tell was in the INNER loop:
 *
 *     ROM    ldr r0, [sp, #0x28]   / ldr r1, [sp, #0x24]   / bl MatrixSetLook
 *     ours   ldr r3, .L+24 / ldr r0,[r3] / mov r1,r0 / add r1,#12 / bl ...
 *
 * The ROM REUSES the view pointer and `view + 0xc` across an intervening
 * _GetBattleActor and InitMatrixStack.  A CALL CLOBBERS MEMORY, so cse can
 * never carry a GLOBAL's load across one -- `MatrixSetLook(iwram_3001e80, ...)`
 * written twice MUST reload it, which is what we were doing.  The reuse is only
 * legal if the value is in a PSEUDO, i.e. if the source holds it in a local:
 *
 *     void *view = iwram_3001e80;
 *     InitMatrixStack();
 *     MatrixSetLook(view, (char *)view + 0xc);
 *
 * and then `view + 0xc` is itself a pseudo that cse can hoist and share with
 * the inner loop's call -- that is the SECOND of the two missing slots.  The ROM
 * even loads it BEFORE InitMatrixStack in the first loop and before the two
 * _PlaySound tests, which no inline expression could produce.
 *
 * AND IT MUST BE ONE VARIABLE PER LOOP, declared inside each loop body.  In the
 * first frame loop the ROM keeps view in r5 across calls; in the second it is
 * SPILLED at sp+0x28.  One pseudo gets one hard register or one slot, never
 * both, so a single function-level `view` cannot produce the ROM -- this is
 * batch 306's one-variable-per-region complement, and here the two regions want
 * opposite treatment because loop 2 has an inner loop and more pressure.
 *
 * ================================================================
 * THE OTHER LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (1) view as a per-loop local (above): 630 -> 139, SIZE and COUNT both
 *     EXACT from that one change, aligned 580 -> 680 (75.3% -> 88.3%).
 * (2) `BuildDraw2DFuncs((*slot)->f4 ^ 1, (void **)(fp = fns))`, the pointer
 *     assigned INSIDE the argument: 641 -> 630.  Written as a separate
 *     `fp = fns;` statement before the call, gcc materialises &fns BEFORE the
 *     `f4 ^ 1` and then reloads the temp for the argument; the ROM computes
 *     `f4 ^ 1` first and &fns second.  Anim_Bind's "BuildDraw2DFuncs takes the
 *     POINTER" with the assignment moved to the call site.
 * (3) NAME THE BLIT'S SOURCE POINTER: `src = (char *)base2 + Data_ede84[m];`
 *     ahead of the call: 138 -> 131.  This also fixes the POOL ORDER -- the ROM
 *     records Data_ede84 before Data_ede96, and reading the width first (the
 *     obvious spelling) reverses them.  Same defect and same fix as in this
 *     batch's Anim_Frost.  NOTE THE BOUND: naming the OFFSET instead
 *     (`k2 = Data_ede84[m]` with `base2 + k2` in the argument) measures 134 and
 *     is WORSE -- lever 4 wants the POINTER here, because the add belongs to
 *     the argument in Frost and to its own statement here.
 * (4) THE THREE NEGATIONS ARE NAMED LOCALS AND THE ADDS ARE `+=`:
 *         int dx = -p->f0;  int dy = -p->f4;  int dz = -p->f8;
 *         p->fc += dx >> 8;  p->f10 += dy >> 8;  p->f14 += dz >> 8;
 *     131 -> 127 for the named negations, 127 -> 118 for spelling the adds
 *     field-first.  The ROM's `adds r3,r3,r2` seeds the Thumb 2-address add
 *     from the LOADED FIELD, so `p->fc += X` and `p->fc = X + p->fc` are
 *     DIFFERENT INSTRUCTIONS: the second gives `adds r2,r2,r3` and defers all
 *     three stores to the end of the block, the first stores each result
 *     immediately and reuses r3 for all three.  Measured in both directions:
 *     `(dx >> 8) + p->fc` is 127, `p->fc += dx >> 8` is 118.
 * (5) `MathFn root = Func_8000948;` as a BLOCK-LOCAL, which is what makes the
 *     ROM's `ldr r3,=Func_8000948 / bl _call_via_r3`.  Anim_Ray's
 *     `ClearFn fill = ...` lever, transplanted unchanged.
 * (6) The folded `while` guards.  BOTH frame loops are `while`, and the ROM
 *     says so: the second one's entry test is `mov r2,#0x48 / neg r2,r2 /
 *     cmp r3,r2` against a bottom test of `frame != (f14 << 3) + 0x48` -- the
 *     entry copy with frame folded to 0, i.e. `duplicate_loop_exit_test`
 *     (jump.c:1137), which a `do`-`while` can never reach.
 * (7) `x * x` WRITTEN OUT, NOT VIA NAMED LOCALS.  Measured the other way:
 *     `int x = p->f0 >> 8; ... x * x + y * y + z * z` is 118 (this file) and
 *     the fully inlined `(p->f0 >> 8) * (p->f0 >> 8) + ...` measures 222.  The
 *     named locals are what let the ROM compute each square immediately after
 *     its own load.
 *
 * ALSO LOAD-BEARING, found on the first candidate and never moved:
 *   - the declaration list IS the frame map: the SEVEN vec3_t locals descend
 *     0xa0, 0x94, 0x88, 0x7c, 0x70, 0x64, 0x58 in declaration order with
 *     `DrawFn fns[2]` last at 0x50, then the spilled scalars descend from
 *     base at 0x4c.
 *   - `sin(ang) * mag`, not `mag * sin(ang)`: the ROM's `mov r3,r5 / mul r3,r0`
 *     seeds the destination from the operand the source names SECOND.
 *   - `w = 9 - (ve.z - 0xfa) / 64` with `Data_ede48[w - 1]` and `w * 2` as the
 *     last argument -- gcc CSEs `w*2` and derives the byte index as `2w - 2`
 *     (`sub r3,r4,#2`).  Anim_Fireball's idiom verbatim.
 *   - `fns[0](...)` read straight out of the frame slot at three sites and
 *     `fp[1](...)` through the pointer at two: the ROM distinguishes them
 *     (`ldr r4,[sp,#0x50]` against `ldr r0,[sp,#0x3c] / ldr r4,[r0,#4]`).
 *   - ONE counter `i` across all five loops and ONE `q` across the two gBuffer
 *     seed loops.  The walker split was measured THREE TIMES on three different
 *     bases (411, 411, 407 against 139, 131, 118) and is WRONG here -- the
 *     opposite of Anim_Ray, where the three gBuffer walkers ARE three
 *     variables.  Read the loop, not the family; this is the sixth
 *     converse-in-one-family pair in the corpus.
 *   - `MatrixYaw(frame * (i * 0x20 + 0x100))` and
 *     `MatrixPitch(-frame * (i * 0x20 + 0x100))` inside a 4-arm `switch (i & 3)`:
 *     loop.c strength-reduces both into givs stepping by `frame << 5` and
 *     `-frame << 5`, and `-frame` becomes the spilled temp at sp+0x2c.
 *
 * ================================================================
 * WHAT IS LEFT: 118 ENCODINGS IN THREE GROUPS, NONE OF THEM PROGRAM SHAPE
 * ================================================================
 *
 * (A) THE PHASE-2 SEED LOOP'S REGISTER PAIR, ~24 encodings and the largest
 *     group.  The ROM walks gBuffer in r5 and holds the `0xff` mask in r6; we
 *     walk in r7 and hold the mask in r5.  Our r7 is INHERITED: it is the same
 *     `q` the first init loop used, and the ROM's first init loop also uses r7,
 *     so the ROM's phase-2 walker is a quantity that got r5 on its own.  The
 *     obvious reading -- that it is therefore a second variable -- IS WRONG AND
 *     MEASURED WRONG THREE TIMES (411 / 411 / 407, against 118 here), including
 *     with the new declaration placed last so it shifted no other pseudo
 *     number.  Declaring the mask (`int mask = 0xff`) is BYTE-IDENTICAL.  So
 *     the ROM reaches r5 for a quantity that is the same variable as the r7 one,
 *     which a single pseudo cannot do -- unless the ROM's phase-2 walker REUSES
 *     some third variable whose earlier range already lives in r5.  `slot`
 *     (`State **`, r5 across the opening calls, dead well before phase 2) is
 *     the only candidate in the frame, and reusing it needs a cast the rest of
 *     the function does not want.  This is lever 1 with no type-clean donor:
 *     the next thing to try, and the reason it is not tried here.
 * (B) THE TWO INNER-LOOP GIVS ARE BORN IN THE OPPOSITE ORDER, ~14 encodings.
 *     The n-loop has two loop.c givs, `idx = 0x24 + n*2` (for `ids[n]`) and
 *     `off = n * 0x700` (for `&gBuffer[n * 0x40]`).  The ROM gives idx the
 *     HIGHER slot (sp+0xc against sp+0x8) and emits its initialiser FIRST; we
 *     do the reverse, and every increment and reload in the n-loop's tail
 *     swaps r0/r1/r2 with it.  A higher slot means a LOWER pseudo number, and
 *     `record_giv` PREPENDS to `bl->giv`, so the ROM's idx was found LAST -- its
 *     last textual use of `n` is an `ids[n]`, ours is the `&gBuffer[n * 0x40]`
 *     that builds the inner walker.  The program's statement order is fixed by
 *     the ROM's own control flow, so this is not reachable by moving a
 *     statement; spelling the walker as an explicit byte offset
 *     (`(Unit *)((char *)gBuffer + n * 0x700)`) is BYTE-IDENTICAL.  Ranked
 *     second because it is a consequence of (A)-style allocation, not of shape.
 * (C) THREE ONE-INSTRUCTION POOL-LOAD TRANSPOSITIONS, at ref[107], ref[232]
 *     and ref[367], each a `ldr rX,<pool>` we emit one insn earlier than the
 *     ROM, plus the `.word 0x00000000` at the very end moving with them.  Every
 *     register in all three is already correct, so these are issue-time only.
 *     sched1 DOES NOT RUN in this build, so they are sched2, whose tie-break is
 *     priority -> dependent count -> INSN_LUID; a pool load has no dependents,
 *     which leaves only LUID, and LUID preserves EXPAND order.  Two of the three
 *     are the first argument of a call, where precompute_register_parameters
 *     and load_register_parameters fix argument 0 first and no spelling
 *     reorders it.
 *
 * NOT A RESIDUE, recorded so it is not chased: the ROM writes gBuffer field
 * 0x8 in the first init loop and READS field 0xc in the same records later --
 * the field it initialises is not the field it integrates.  That asymmetry is
 * in the ROM, reproduced here as written, and is not a mis-read struct: every
 * offset is confirmed against both loops.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef int (*MathFn)(int v);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18;
} Unit;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern Unit gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned short Data_ede84[];
extern unsigned char  Data_ede96[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int *_GetBattleActor(int id);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixPush(void);
extern void MatrixPop(void);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void MatrixRoll(int a);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Unit *g, int a, int b);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern int  Func_8000948(int v);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Unused_Fizz(void *context)
{
    vec3_t va;
    vec3_t vb;
    vec3_t vc;
    vec3_t vd;
    vec3_t ve;
    vec3_t vf;
    vec3_t vg;
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int n;
    void *base2;
    DrawFn *fp;
    int *a2;
    void **gp;
    void **pp;
    State **slot;
    int *a1;
    Unit *q;
    Unit *p;
    int frame;
    int i;

    gp = iwram_3001eec;
    pp = gp;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = gp[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_92, base, 1, 1);
    LoadVFXFile(FILE_73, base2, 0, 0);
    BuildDraw2DFuncs((*slot)->f4 ^ 1, (void **)(fp = fns));
    a1 = (int *)*_GetBattleActor((*slot)->f8);
    a2 = (int *)*_GetBattleActor((*slot)->ids[0]);
    q = gBuffer;
    i = 0;
    do {
        int ang = Random() & 0xffff;
        int mag;
        mag = (Random() & 0xff) + 0x80;
        q->f0 = 0;
        q->f4 = ((Random() & 0x1f) + 0x14) << 16;
        q->f8 = 0;
        q->fc = sin(ang) * mag >> 5;
        q->f10 = 0;
        q->f14 = cos(ang) * mag >> 5;
        q->f18 = 0;
        i++;
        q++;
    } while (i != 0x40);
    *(int *)(base + 0x7780) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x480);
    va.x = a1[2];
    va.y = 0;
    va.z = a1[4];
    vb.x = a2[2];
    vb.y = 0x5a0000;
    vb.z = 0;
    vc.x = (vb.x - va.x) / 0x28;
    vc.y = (vb.y - va.y) / 0x28;
    vc.z = (vb.z - va.z) / 0x28;
    frame = 0;
    do {
        void *view = iwram_3001e80;
        if (frame == 8) {
            _PlaySound(0xd4);
        }
        if (frame == 0x50) {
            _PlaySound(0x8e);
        }
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if ((unsigned)(frame - 0x1e) <= 0x27) {
            va.x += vc.x;
            va.y += vc.y;
            va.z += vc.z;
        }
        MatrixTranslatev(&va);
        if (frame == 0) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 7, -1, -1, 0);
        }
        if (frame == 0x18) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 0, -1, -1, 0);
        }
        i = 0;
        p = gBuffer;
        do {
            if (frame > i && p->f18 == 0) {
                int w;
                MatrixPush();
                switch (i & 3) {
                case 0:
                    MatrixYaw(frame * (i * 0x20 + 0x100));
                    break;
                case 1:
                    MatrixPitch(-frame * (i * 0x20 + 0x100));
                    break;
                case 2:
                    MatrixRoll(-frame * (i * 0x20 + 0x100));
                    break;
                case 3:
                    MatrixPitch(-frame * (i * 0x20 + 0x100));
                    MatrixRoll(-frame * (i * 0x20 + 0x100));
                    break;
                }
                Func_80e3944((vec3_t *)p, &ve);
                ve.x >>= 1;
                MatrixPop();
                if (ve.z <= 0xf9) {
                    ve.z = 0xfa;
                }
                if (ve.z > 0x27a) {
                    ve.z = 0x27a;
                }
                w = 9 - (ve.z - 0xfa) / 64;
                fns[0]((void *)ctx, (char *)base2 + Data_ede48[w - 1],
                       ve.x - w / 2, ve.y - w, w, w * 2);
                Func_80e38b8(p, 0x3c, 0);
                if (frame > i + 0x1e) {
                    int dx = -p->f0;
                    int dy = -p->f4;
                    int dz = -p->f8;
                    p->fc += dx >> 8;
                    p->f10 += dy >> 8;
                    p->f14 += dz >> 8;
                }
            }
            i++;
            p++;
        } while (i != 0x20);
        if (frame > 0x52) {
            vd.x = 0;
            vd.y = sin(frame << 10) << 2;
            vd.z = 0;
            Func_80e3944(&vd, &ve);
            ve.x >>= 1;
            fp[1]((void *)ctx, base, ve.x - 0xa, ve.y - 0x11, 0x14, 0x22);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x62);
    i = 0;
    q = gBuffer;
    do {
        q->f0 = ((Random() & 0xff) - 0x7f) << 15;
        q->f4 = ((Random() & 0x7f) + 0x40) << 15;
        q->f8 = ((Random() & 0xff) - 0x7f) << 15;
        q->f18 = 0;
        i++;
        q++;
    } while (i != 0x200);
    LoadVFXFile(FILE_ba, base2, 0, 0);
    frame = 0;
    while (frame != (((*(State **)(base + 0x7828))->f14 << 3) + 0x48)) {
        void *view = iwram_3001e80;
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if (frame >= ((*(State **)(base + 0x7828))->f14 << 3) + 0x28) {
            vb.y += 0x40000;
        }
        vf.x = vb.x;
        vf.y = vb.y;
        vf.z = vb.z + sin(frame << 11) * 40;
        Func_80e3944(&vf, &vg);
        vg.x >>= 1;
        fns[0]((void *)ctx, base, vg.x - 0xa, vg.y - 0x11, 0x14, 0x22);
        n = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            do {
                int k;
                int *a;
                a = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[n]);
                k = n * 8;
                InitMatrixStack();
                MatrixSetLook(view, (char *)view + 0xc);
                va.x = a[2];
                va.y = 0x280000;
                va.z = a[4];
                MatrixTranslatev(&va);
                if (frame == k + 0x1e) {
                    _PlaySound(0x7e);
                }
                if (frame == k + 0x28) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[n], 7, -1, -1, 0);
                }
                if (frame == k + 0x40) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[n], 0, -1, -1, 0);
                }
                if (frame > k) {
                    MatrixYaw((frame - k) << 9);
                    i = 0;
                    p = &gBuffer[n * 0x40];
                    do {
                        if (frame > i / 2 + k) {
                            MathFn root = Func_8000948;
                            int x = p->f0 >> 8;
                            int y = p->f4 >> 8;
                            int z = p->f8 >> 8;
                            int d = root(x * x + y * y + z * z) >> 9;
                            if (d != 0) {
                                int m;
                                unsigned char w;
                                char *src;
                                Func_80e3944((vec3_t *)p, &vg);
                                vg.x >>= 1;
                                if (vg.z <= 0x139) {
                                    vg.z = 0x13a;
                                }
                                if (vg.z > 0x27a) {
                                    vg.z = 0x27a;
                                }
                                m = (i * 4 + frame) % 9;
                                src = (char *)base2 + Data_ede84[m];
                                w = Data_ede96[m];
                                fp[1]((void *)ctx, src,
                                      vg.x - (w >> 1), vg.y - (w >> 1), w, w);
                                p->f0 -= p->f0 / d;
                                p->f4 -= p->f4 / d;
                                p->f8 -= p->f8 / d;
                            }
                        }
                        i++;
                        p++;
                    } while (i != 0x40);
                }
                n++;
            } while (n != (*(State **)(base + 0x7828))->f14);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
