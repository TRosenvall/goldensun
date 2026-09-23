/* Anim_HelmSplitter -- NON-MATCHING, 300 encodings of 412.  INSTRUCTION COUNT EXACT
 * (412 = 412), SIZE EXACT (944 bytes both), FRAME EXACT (`sub sp,#0x24`, ON THE FIRST
 * CANDIDATE), and THE FIRST 84 INSTRUCTIONS ARE BYTE-EXACT.  386 instructions.
 * 52 windows / 211 lines.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/e6638_HelmSplitter.c \
 *     asm/rom_c9000/rom_e6638_a_c.s --func Anim_HelmSplitter
 * FOUR functions in the reference; a text split is required.
 *
 * NO DATA WORK, AND THE FIVE DOT-LABELS ARE REFERENCED RATHER THAN DEFINED HERE --
 * .Leedf4, .Leedfb, .Leee02, .Leee10 and .Leee17 are all defined AND `.global`-ed in the
 * sibling asm/rom_c9000/rom_e6638_c.s, so they are ordinary externs:
 * `extern unsigned char Leedf4[] __asm__(".Leedf4");` and so on.  Widths matter --
 * .Leee02 is `unsigned short`, .Leee10 and .Leee17 are `signed char` (the ROM uses
 * `ldrsb`).  This is the third batch-283 case where a dot-label turned out to live in a
 * sibling; GREP THE WHOLE OF asm/, NOT THE REFERENCE.
 *
 * THE SIZE-EXACT STEP WAS THE PINNED-CALL-CLOBBERED-REGISTER CSE BREAK, and this
 * function is its second confirmation.  Three `base + 0x7828` sites were CSE'd into one
 * shared constant, forcing `mov r0,r9 / add r5,r0,r7` where the ROM has the destructive
 * `add r5,r9`.  Wrapping the 2nd and 3rd sites in
 * `{ register int k __asm__("r3"); k = 0x7828; ... base + k ... }` took it from 416/412
 * instructions and size 952/944 to 412/412 WITH SIZE EXACT.
 *
 * REMAINING NAMED BLOCKER: the `slot`(r10) and `ap`(r11) roles are SWAPPED relative to
 * the ROM, which cascades through the frame loop.  Both pinning them (69 windows, 414
 * instructions) and reordering their declarations (INERT -- byte-identical output)
 * failed.  Declaration order is not the handle for a role swap between two locals that
 * both get registers; it only places SPILLED scalars.
 *
 * The two relocation differences are two sites spelling `_call_via_r6` where the ROM has
 * `_call_via_r4` -- the fns[0] pointer landed in a callee-saved register rather than the
 * call-clobbered r4.  Inlining the call target fixes that (it did on
 * src/non_matching/rom_c9000/dbbdc_ElementOrbs.c) BUT COSTS 2 INSTRUCTIONS HERE
 * (414/412), so it is left as residue.  Per-function, not per-bank.
 *
 * No per-file Makefile flag override applies to this stem.

 * ================================================================
 * BANK-WIDE FINDINGS FROM BATCH 283 -- these apply to every rom_c9000 entry point
 * ================================================================
 *
 * THE DECLARATION-ORDER RULE HELD ON ALL FIVE OF THIS AGENT'S FUNCTIONS WITH NO
 * NEGATIVE, and it is now the fastest thing in the toolbox.  Reading the ROM's
 * spilled-scalar slots high-to-low gave a declaration order that reproduced the map on
 * the FIRST compile every time: 7 of 8 slots in the ROM's relative order on
 * Anim_EPowerUp, 10 of 13 roles on Anim_DeathPlunge, and an EXACT frame on candidate 1
 * for both Anim_HelmSplitter and Anim_Unused_ElementOrbs.
 *
 * AND A COROLLARY THAT MAKES THE SLOT MAP READABLE: COMPILER-CREATED PSEUDOS
 * (strength-reduced IVs, loop-invariant hoists) LAND AT THE *LOWEST* SLOTS, below every
 * source local.  That is how you tell a giv from a source variable when reading the map
 * -- anything under the last declared scalar is gcc's, not the original author's.
 *
 * DO NOT PIN A POINTER YOU DEREFERENCE AT AN OFFSET TO A HIGH REGISTER.  This CORRECTS
 * the standing "pin base to its high register" advice, which is right for values
 * consumed WHOLE and wrong for a struct pointer.  `register int *p __asm__("r9")` puts a
 * hard hi-reg inside the MEM address, and reload then reloads THE WHOLE ADDRESS:
 * `mov r3,r9 / add r3,#0x10 / ldr r3,[r3]`.  An UNPINNED pseudo that global_alloc happens
 * to put in r9 gets the ROM's form instead: `mov r4,r9 / ldr r3,[r4,#0x10]`.  Removing
 * one such pin on Anim_DeathPlunge went 62 windows / 249 lines -> 18 windows / 73 lines
 * with the size becoming exact -- the largest single step of that session.
 *   PIN: base, frame counters, loop counters -- things consumed whole (`add rX, base`).
 *   DO NOT PIN: struct pointers you dereference at an offset.
 *
 * THE *SHAPE* OF AN INDUCTION EXPRESSION DECIDES WHETHER loop.c STRENGTH-REDUCES IT,
 * INDEPENDENT OF NAMING -- and it is the SHIFT, not the algebra.  On
 * Anim_Unused_ElementOrbs `frame * (i * 8 + 0x100)` was NOT reduced; the same value
 * written `frame * ((i + 0x20) << 3)` WAS, taking the function to size-exact and
 * 236 -> 166 window-lines.  `((i + 0x20) * 8)` -- multiply instead of shift -- measured
 * IDENTICAL TO THE UNREDUCED FORM.
 *
 * AND HAND-REDUCING A giv YOURSELF IS WORSE THAN FINDING THE SHAPE.  Writing the ROM's
 * accumulators literally (`a9 = frame<<8; ... a9 += frame<<3;` with r8/r9 pins) fixed
 * the structure but GREW THE FRAME 0x34 -> 0x3c.  Measured negative.
 *
 * LOAD AN INDIRECT-CALL TARGET AT THE CALL SITE, NOT INTO A HOISTED LOCAL.
 * `f = (DrawFn)fns[0]; f(...)` gives `_call_via_r5`/`_call_via_r6`;
 * `(*(DrawFn *)&fns[0])(...)` gives the ROM's `_call_via_r4` -- r4 is call-clobbered
 * under -fcall-used-r4, so the ROM reloads it from `[sp,#N]` immediately before each
 * `bl`.  On ElementOrbs this made the relocation symbol list IDENTICAL IN ORDER and
 * dropped 42 -> 36 windows.  It was HARMFUL on HelmSplitter (412 -> 414), so measure
 * per function.
 *
 * THE PINNED-CALL-CLOBBERED-REGISTER CSE BREAK TRANSFERS, confirmed on a second
 * function.  On HelmSplitter three `base + 0x7828` sites were CSE'd into one shared
 * constant, forcing `mov r0,r9 / add r5,r0,r7` where the ROM has the destructive
 * `add r5,r9`.  Wrapping the 2nd and 3rd sites in
 * `{ register int k __asm__("r3"); k = 0x7828; ... base + k ... }` took it from
 * 416/412 instructions and size 952/944 to 412/412 WITH SIZE EXACT.  (First found on
 * BaseAnim_Tackle -- see src/non_matching/rom_c9000/dfa18_Tackle.c.)
 *
 * A tryc BLIND SPOT TO ADD BESIDE THE POOL AND JUMP-TABLE ONES: `ldrh rX, label` AND
 * `ldr rX, label` ASSEMBLE TO THE SAME `ldr rX,[pc,#N]`, because GAS rewrites the
 * halfword form.  So a TEXT diff between a generated `.s` and a reference can show a
 * mnemonic difference where the BYTES are identical.  (The generated-`.s` grep for
 * `ldrh rN, .L` is still a valid diagnostic that gcc made a HImode fix -- what is
 * invalid is diffing that mnemonic against the reference.)  One candidate was burned
 * "fixing" exactly this.  objcmp sees through it; tryc cannot.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
} State;

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
} Part;

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef int (*SqrtFn)(int v);

extern int *iwram_3001eec[];
extern Part gBuffer[];
extern Part ewram_2010018[];

extern unsigned char Leedf4[] __asm__(".Leedf4");
extern unsigned char Leedfb[] __asm__(".Leedfb");
extern unsigned short Leee02[] __asm__(".Leee02");
extern signed char Leee10[] __asm__(".Leee10");
extern signed char Leee17[] __asm__(".Leee17");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern int Func_8000948(int v);
extern void _Actor_Stop(void *a);
extern void _Actor_TravelTo(void *a, int x, int y, int z);
extern void _Actor_SetAnim(void *a, int n);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int v);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void Func_80e3908(void *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_HelmSplitter(void *context)
{
    register unsigned char *base __asm__("r9");
    void *ctx;
    int frame;
    State **s0;
    State **slot;
    unsigned char *gfx;
    int *actA;
    int *actB;
    int tx;
    int tz;
    int dx;
    int dz;
    int arg;
    int sp0;
    SqrtFn sq;
    vec3_t *ap;
    vec3_t apos;
    void *fns[2];

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    s0 = (State **)(base + 0x7828);
    *s0 = (State *)context;
    gfx = (unsigned char *)((char **)&iwram_3001eec)[2];
    AnimStart(0);
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_61, base, 1, 1);
    LoadVFXFile(FILE_6d, base + (0xfa << 6), 1, 0);
    BuildDraw2DFuncs((*s0)->f4, fns);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    {
        Part *g = ewram_2010018;
        register int i __asm__("r8");
        i = 0;
        do {
            i++;
            g->f0 = 0;
            g++;
        } while (i != (0x80 << 3));
    }
    {
        register int k1 __asm__("r3");
        State **s1;
        int *a1;
        int *a2;
        k1 = 0x7828;
        s1 = (State **)(base + k1);
        a1 = (int *)_GetBattleActor((*s1)->f8);
        actA = (int *)*a1;
        a2 = (int *)_GetBattleActor((*s1)->ids[0]);
        actB = (int *)*a2;
        tx = actA[2];
        dx = (actB[2] - actA[2]) * 80 / 100;
        tz = actA[4];
        dz = (actB[4] - actA[4]) * 80 / 100;
        tx += dx;
        tz += dz;
        dx >>= 8;
        dz >>= 8;
    }
    sq = Func_8000948;
    sp0 = sq(dz * dz + dx * dx);
    sp0 <<= 8;
    sp0 = sp0 / 0x14;
    actA[0xd] = sp0;
    actA[0xc] = sp0;
    *((unsigned char *)actA + 0x58) = 1;
    actA[0xa] = 0xe0 << 11;
    actA[0x12] = 0xdeb8;
    actA[0x11] = 0;
    *((unsigned char *)actA + 0x5a) = 1;
    _Actor_Stop(actA);
    _Actor_TravelTo(actA, tx, 0, tz);
    _Actor_SetAnim(actA, 2);
    {
        register int k2 __asm__("r3");
        k2 = 0x7828;
        slot = (State **)(base + k2);
    }
    frame = 0;
    ap = &apos;
    do {
        GetBattleActorPos3((*slot)->f8, ap);
        REG_BG2X = (0x50 - ap->x) << 8;
        if ((unsigned)(frame - 8) <= 0xf) {
            int k = (frame - 8) / 2;
            DrawFn f;
            if (k > 6) {
                k = 6;
            }
            f = (DrawFn)fns[0];
            if ((*slot)->f4 == 0) {
                f(ctx, base + Leee02[k], Leee10[k] + 0x1e,
                  ap->y + Leee17[k] - 0x3c, Leedf4[k], Leedfb[k]);
            } else {
                f(ctx, base + Leee02[k], -Leee10[k] - Leedf4[k] + 0x6c,
                  ap->y + Leee17[k] - 0x3c, Leedf4[k], Leedfb[k]);
            }
        }
        if (frame == 0x12) {
            Part *g = gBuffer;
            register int i __asm__("r8");
            _Func_80bd7dc(0x86);
            Func_80d6888((*slot)->ids[0], 7, 5, 0, 8);
            _SetBattleActorKnockback((*slot)->ids[0], 6);
            *(int *)(base + 0x77a8) = 4;
            i = 0;
            do {
                int r = (Random() & 0x3f) + (0x80 << 1);
                int a = Random() & 0xffff;
                g->f0 = 0x80 << 15;
                g->f4 = 0xa0 << 15;
                g->fc = (r * sin(a)) >> 7;
                g->f10 = -(r * cos(a)) >> 6;
                g->f18 = (Random() & 0xf) + 0x10;
                i++;
                g++;
            } while (i != 0x10);
        }
        {
            Part *g = gBuffer;
            register int i __asm__("r8");
            i = 0;
            do {
                int t = g->f18;
                if (t > 0) {
                    g->f18 = t - 1;
                    Func_80e3908(g, 0x3c, 0);
                    if (g->f4 > (0xd0 << 15)) {
                        g->f10 = -g->f10 / 2;
                    } else if ((unsigned)g->f0 < 0x7f0000 && g->f4 >= 0) {
                        int n = (frame + i) / 4 % 6;
                        int px = g->f0 >> 16;
                        int py = g->f4 >> 16;
                        DrawFn f = (DrawFn)fns[0];
                        f(ctx, base + (0xfa << 6) + (n << 8), px - 8, py - 8, 0x10, 0x10);
                    }
                }
                i++;
                g++;
            } while (i != 0x80);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x46);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
