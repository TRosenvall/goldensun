/* Anim_EPowerUp -- NON-MATCHING, 343 encodings of 452, size 1040 against the ROM's
 * 1044 (-4), 450 instructions against 452.  420 instructions in the reference.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/cb1a4_EPowerUp.c \
 *     asm/rom_c9000/rom_cb1a4.s --func Anim_EPowerUp
 * FOUR functions in the reference; a text split is required.
 *
 * THE WHOLE -4 AND THE WHOLE -2 INSTRUCTIONS ARE ONE MISSING SPILLED SCALAR, AND IT IS
 * IDENTIFIED PRECISELY: the ROM's slot [sp,#0x20] holds the *(DrawFn *)iwram_3001f0c
 * blit pointer, SET TWICE AND LOADED ZERO TIMES (both uses are reload inheritance).
 * The two `str r6,[sp,#0x20]` are the two missing instructions.  Frame ours 0x48
 * against the ROM's 0x4c.
 *
 * That is the SAME SHAPE as BaseAnim_Spore's finding (a dead store the ROM keeps, which
 * only an array reproduces -- see src/non_matching/rom_c9000/ca1e4_Spore.c) and as
 * Anim_DeathPlunge's unreferenced word.  THREE INSTANCES IN ONE BANK IN ONE BATCH: a
 * frame exactly four bytes short with everything above one slot displaced means look for
 * a written-never-read or never-touched slot before re-reading the declaration list.
 *
 * EVERYTHING ELSE LANDS.  Declaration order ctx(0x24) blit(0x20) d0(0x1c) gfx(0x18)
 * view(0x14) slot(0x10) look(0x0c) ap(0x08), then apos, pos, pv.  base->r9, frame->r10,
 * h->r8, q = h/4 ->r11 -- and THE r11 PIN IS WHAT FORCED `ap = &apos` TO SPILL; without
 * it gcc gave &apos r11 and the frame was 0x44.  THE ENTIRE 32-PARTICLE DRAW LOOP IS
 * BYTE-EXACT.
 *
 * Blocker class: global_alloc's register-vs-memory home for one pseudo, plus a
 * whole-function r5/r6 role swap visible at index 7 (`mov r6,r0` against `mov r5,r0`) --
 * the "ROM prefers high where gcc prefers low" residue that
 * src/non_matching/rom_c9000/cf2a0_Revive.c names.
 *
 * MEASURED NEGATIVES: an explicit `m` live across the BuildDraw2DFuncEx call (448/452);
 * a destructive `p1 += (int)base` (446/452); inlining the base+0x7828 store instead of
 * an `s0` local (454/452).
 *
 * ================================================================
 * A SYMBOL TELL THAT THE REFERENCE .s GETS WRONG -- DO NOT "FIX" THE C
 * ================================================================
 *
 * Reference line 838 reads `ldr r6, =0x57`.  A POOL LOAD FOR A VALUE THAT FITS
 * `mov r6,#0x57` CAN ONLY BE A RELOCATION, so this is `_FILE_57` and `FILE_57` is the
 * correct C.  make compare is unaffected (_FILE_57 is an ABS symbol = 0x57); only
 * objcmp's relocation list flags the extra entry.  So the one relocation difference here
 * is the REFERENCE being under-annotated, not the candidate being wrong.
 *
 * ================================================================
 * THE .rodata DECISION FOR rom_cb1a4.s -- NO DATA WORK FOR THIS FUNCTION
 * ================================================================
 *
 * The file defines EIGHT .incrom blobs totalling 217 bytes, and NONE OF THE EIGHT IS
 * REFERENCED BY Anim_EPowerUp OR Anim_DeathPlunge.  The three small ones are used only
 * by Anim_Unused_SabreRain and the five larger ones only by Anim_ScreenShatter.  Since
 * only 2 of the file's 4 functions convert, the file splits regardless and THE .rodata
 * STAYS WITH THE RESIDUAL .s beside the two functions that read it.
 *
 * All eight were read out of baserom.gba and are recorded in the batch report.  At 217
 * bytes across eight blobs this is well past "a handful of readable words", so if a
 * future batch converts the other two, REHOMING (a _b.s holding only the data section)
 * is right rather than emitting from C.
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

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

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

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern void *iwram_3001e80;
extern void *iwram_3001f0c;
extern Part gBuffer[];
extern unsigned char gPtrs[];
extern unsigned short Data_ede5c[];

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int Random(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern int sin(int a);
extern int cos(int a);

void Anim_EPowerUp(void *context)
{
    register unsigned char *base __asm__("r9");
    void *ctx;
    DrawFn blit;
    DrawFn d0;
    unsigned char *gfx;
    char *view;
    State **slot;
    char *look;
    vec3_t *ap;
    State **s0;
    State **s1;
    State *st;
    unsigned char *pt;
    unsigned char *data;
    unsigned char *sp2;
    CopyFn copy;
    int fid;
    int fid0;
    int arg;
    register int frame __asm__("r10");
    vec3_t apos;
    vec3_t pos;
    vec3_t pv;

    base = (unsigned char *)galloc_iwram(0x27, 0x782c);
    ctx = galloc_iwram(0x28, 0x80 << 7);
    gfx = (unsigned char *)galloc_iwram(0x29, 0x60e);
    view = (char *)iwram_3001e80;
    s0 = (State **)(base + 0x7828);
    *s0 = (State *)context;
    AnimStart(0);
    *(int *)(base + 0x77b4) = 0x18;
    *(int *)(base + 0x77b8) = 0;
    REG_BLDALPHA = 0x100c;
    fid0 = FILE_57;
    REG_BG2PA = 0x100;
    LoadVFXFile(fid0, base, 1, 0);
    LoadVFXFile(FILE_76, gfx, 0, 0);
    switch ((*s0)->f0) {
    case 0:
        fid = FILE_48;
        break;
    case 1:
        fid = fid0;
        break;
    case 2:
        fid = FILE_47;
        break;
    default:
        fid = FILE_46;
        break;
    }
    data = (unsigned char *)GetFile(fid);
    {
        PIN3;
        q1 = (int)data;
        q0 = 0xa0;
        copy = Func_8001af8;
        q0 <<= 19;
        q2 = 0x80;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    {
        Part *g = gBuffer;
        register int i __asm__("r8");
        i = 0;
        do {
            g->f4 = 0;
            g->f0 = Random() & 0xffff;
            g->f8 = (Random() & 0x1ff) + i * 2;
            g->f18 = -i;
            i++;
            g++;
        } while (i != 0x80);
    }
    *(int *)(base + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x4b;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    pt = gPtrs;
    s1 = (State **)(base + 0x7828);
    d0 = *(DrawFn *)(pt + 0xb8);
    st = *s1;
    st->f18 = st->f18 + 1;
    if (st->f18 <= 0) {
        st->f18 = 1;
    }
    if ((*s1)->f18 > 4) {
        (*s1)->f18 = 4;
    }
    _PlaySound(0xd4);
    ap = &apos;
    look = view + 0xc;
    slot = (State **)(base + 0x7828);
    frame = 0;
    do {
        GetBattleActorPos3((*slot)->f8, ap);
        REG_BG2X = (0x40 - ap->x) << 8;
        if (frame > 0x31) {
            REG_BLDALPHA = (0x70 - frame * 2) | 0x1000;
        }
        if (frame == 0x10) {
            Func_80d6888((*slot)->ids[0], 7, -1, 0, 0x14);
        }
        if (frame <= 0x37) {
            register int h __asm__("r8");
            register int q __asm__("r11");
            register int n7 __asm__("r7");
            int n;
            h = frame / 2;
            q = h / 4;
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
            blit = (DrawFn)iwram_3001f0c;
            blit(ctx, base + (h - q * 4) * 0x440, 0x2f, ap->y - 0x40, 0x11, 0x40);
            n7 = (frame / 4) % 3;
            sp2 = base + n7 * 0x408 + 0x1100;
            blit(ctx, sp2, 0x28, ap->y - 0x24, 0x18, 0x2b);
            gfree(0x2f);
            BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
            blit = (DrawFn)iwram_3001f0c;
            blit(ctx, base + (h - q * 4) * 0x440, 0x40, ap->y - 0x40, 0x11, 0x40);
            blit(ctx, sp2, 0x40, ap->y - 0x24, 0x18, 0x2b);
            gfree(0x2f);
        }
        _GetBattleActor((*slot)->f8);
        InitMatrixStack();
        MatrixSetLook(view, look);
        {
            Part *g = gBuffer;
            register int i __asm__("r8");
            i = 0;
            do {
                if (g->f18 >= 0) {
                    int sz;
                    int w;
                    int ix;
                    pv.x = (g->f8 * sin(g->f0)) >> 4;
                    pv.z = -((g->f8 * cos(g->f0)) >> 4);
                    pv.y = g->f4;
                    g->f0 += 0x80 << 3;
                    g->f4 += 0xa0 << 11;
                    g->f8 += 0x40;
                    Func_80e3944(&pv, &pos);
                    pos.x = pos.x / 2;
                    sz = (*slot)->f18 + (i & 1);
                    w = sz * 2;
                    ix = w - 2;
                    d0(ctx, gfx + *(unsigned short *)((char *)Data_ede5c + ix),
                       pos.x - sz, pos.y - sz, w, w);
                }
                i++;
                g->f18 = g->f18 + 1;
                g++;
            } while (i != 0x20);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x38);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
    gfree(0x29);
    gfree(0x28);
    gfree(0x27);
}
