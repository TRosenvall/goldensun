/* Anim_DeathPlunge -- NON-MATCHING, 171 encodings of 375.  INSTRUCTION COUNT EXACT
 * (375 = 375), SIZE EXACT (840 bytes both), AND THE 38 RELOCATION SYMBOLS ARE IDENTICAL
 * IN THE SAME ORDER -- only offsets differ.  363 instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/cb1a4_DeathPlunge.c \
 *     asm/rom_c9000/rom_cb1a4.s --func Anim_DeathPlunge
 * FOUR functions in the reference; a text split is required.  No data work -- it
 * references only iwram_3001eec, iwram_3001e80, _FILE_7d and Task_BlitAnim.  (The
 * file's eight .rodata blobs belong to its other two functions; see
 * src/non_matching/rom_c9000/cb1a4_EPowerUp.c.)
 *
 * FRAME: ours `sub sp,#0x50` against the ROM's 0x54, AND THE FIRST DIVERGENCE *IS* THE
 * `sub sp` AT INDEX 13 -- so the entire prologue up to it is byte-exact.  18 windows /
 * 73 window-lines, 17 structural, most of them one or two register renames.
 *
 * THE 4-BYTE RESIDUE IS ONE UNREFERENCED SPILL WORD AT [sp,#0x34].  The ROM allocates
 * 13 scalar slots and NEVER READS OR WRITES 0x34 -- verified, no `sp,#0x34` appears
 * anywhere in the function.  That is the signature of a pseudo with a MEM home whose
 * every set and use was satisfied by reload inheritance.  THE SOURCE CONSTRUCT THAT
 * PRODUCES IT WAS NOT FOUND.
 *
 * Note this is a DIFFERENT shape from its file-mate's: EPowerUp's missing slot is
 * written twice and never read, this one is never touched at all, and Spore's
 * (src/non_matching/rom_c9000/ca1e4_Spore.c) is written once and never read -- where an
 * ARRAY fixed it.  Three variants of one family; the array trick is worth trying here.
 *
 * LEVERS THAT GOT IT HERE: base->r11, frame->r10, k=0x5a ->r8 (the ROM keeps that
 * multiplier in a high register); `actB` DELIBERATELY UNPINNED (the do-not-pin-a-
 * dereferenced-pointer rule below -- this is the function that found it); the two
 * frame-derived range values written as EXPLICIT LOCALS initialised to -0x38 / -0x2e
 * before the loop and incremented at its bottom, which reproduced the ROM's four-store
 * preheader [sp,#0x14]/[sp,#0x18]/[sp,#0x10]/[sp,#0xc] exactly; and `rad * sin(ang)`
 * with the LOOP-INVARIANT AS THE FIRST mul OPERAND.
 *
 * Declaration order: ctx(0x38) d0(0x30) tx(0x2c) d1(0x28) d2(0x24) bp(0x20) ang(0x1c)
 * pp(0x18) vp(0x14) t1(0x10) t2(0x0c) i(0x08), then va, pos.
 *
 * MEASURED NEGATIVES: a walking `pp0 = (int **)&iwram_3001eec` as the second local
 * (371/375, size 832); `register int rad __asm__("r6")` (38 windows, 117 lines); letting
 * `frame - 0x2e` stay inline so loop.c reduces it (frame drops to 0x44).
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

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern void *iwram_3001e80;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern int _Func_80b8530(int id);
extern void _Actor_Stop(void *a);
extern void _Actor_TravelTo(void *a, int x, int y, int z);
extern void _Actor_SetAnim(void *a, int n);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _Func_80bd7dc(int a);
extern void _PlaySound(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void UpdateScreenShake(int x, int y);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern int sin(int a);
extern int cos(int a);

void Anim_DeathPlunge(void *context)
{
    register unsigned char *base __asm__("r11");
    void *ctx;
    DrawFn d0;
    int tx;
    int d1;
    int d2;
    unsigned char *bp;
    int ang;
    vec3_t *pp;
    vec3_t *vp;
    int t1;
    int t2;
    State **slot;
    int *actA;
    int *actB;
    int tz;
    int arg;
    char *view;
    char *look;
    register int frame __asm__("r10");
    vec3_t va;
    vec3_t pos;

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_7d, base, 1, 1);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    d0 = (DrawFn)((char **)&iwram_3001eec)[7];
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    {
        int *r1 = (int *)_GetBattleActor((*slot)->f8);
        int *r2 = (int *)_GetBattleActor((*slot)->ids[0]);
        register int k __asm__("r8");
        actA = (int *)*r1;
        k = 0x5a;
        actB = (int *)*r2;
        tx = actA[2] + k * (actB[2] - actA[2]) / 100;
        tz = actA[4] + k * (actB[4] - actA[4]) / 100;
    }
    d1 = _Func_80b8530((*slot)->f8);
    d2 = _Func_80b8530((*slot)->ids[0]);
    _Actor_Stop(actA);
    _Actor_TravelTo(actA, tx, 0, tz);
    _Actor_SetAnim(actA, 2);
    *((unsigned char *)actA + 0x58) = 1;
    bp = (unsigned char *)actA + 0x5a;
    *bp = 1;
    *(int *)((char *)actA + 0x34) = 0x80 << 10;
    *(int *)((char *)actA + 0x30) = 0x80 << 12;
    WaitFrames(0x14);
    vp = &va;
    pp = &pos;
    t1 = -0x38;
    t2 = -0x2e;
    frame = 0;
    do {
        view = (char *)iwram_3001e80;
        look = view + 0xc;
        InitMatrixStack();
        MatrixSetLook(view, look);
        if (frame == 0) {
            actB[0xa] = 0xf0 << 12;
            actB[0x12] = 0x91eb;
            actA[0xa] = 0xf0 << 12;
            actA[0x12] = 0x91eb;
        }
        if (frame == 0xb) {
            actB[7] = -actB[7];
            actA[7] = -actA[7];
            actA[3] = actA[3] + d1;
            actB[3] = actB[3] + d2;
        }
        if (frame == 0x36) {
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 0xa);
            actB[0xa] = 0x80 << 12;
            actB[0x12] = 0xab85;
            actA[0xa] = 0xa0 << 11;
            actA[0x12] = 0x7851;
            actA[0xd] = 0x80 << 9;
            actA[0xc] = 0x80 << 10;
            *bp = 0;
            _Actor_Stop(actA);
            _Actor_TravelTo(actA, 0, 0, actA[4]);
        }
        InitMatrixStack();
        MatrixSetLook(view, look);
        vp->x = actA[2];
        vp->y = actA[3];
        vp->z = actA[4];
        Func_80e3944(vp, pp);
        pp->x = pp->x >> 1;
        if (frame >= 0x36 && frame <= 0x37) {
            d0(ctx, base, pp->x - 0x10, pp->y - 0x10, 0x20, 0x40);
        }
        if (t1 >= 0 && t1 <= 0xb) {
            register int off __asm__("r8");
            int rad;
            int i;
            off = (t1 / 2) << 11;
            rad = t2;
            i = 0;
            do {
                int x;
                int y;
                ang = i << 12;
                x = pos.x + ((sin(ang) * rad) >> 16);
                y = ((cos(ang) * rad) >> 16) - frame + 0x64;
                x -= 0x10;
                d0(ctx, base + off, x, y, 0x20, 0x40);
                i++;
            } while (i != 0x10);
        }
        if (frame == 0x40) {
            actB[7] = -actB[7];
            actA[7] = -actA[7];
            actA[3] = actA[3] - d1;
            actB[3] = actB[3] - d2;
            _Actor_SetAnim(actA, 0);
        }
        if (frame == 0x36) {
            _Func_80bd7dc(0x86);
        }
        if (frame == 0) {
            _PlaySound(0x88);
            *(int *)(base + 0x77a8) = 6;
        }
        if (frame == 0x35) {
            *(int *)(base + 0x77a8) = 6;
        }
        UpdateScreenShake(0x10, 0x10);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
        t1++;
        t2++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
