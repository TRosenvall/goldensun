/* Anim_Unused_ElementOrbs -- NON-MATCHING, 155 encodings of 273.  INSTRUCTION COUNT
 * EXACT (273 = 273), SIZE EXACT (616 bytes both), FRAME EXACT (`sub sp,#0x34`), AND ALL
 * 31 RELOCATION SYMBOLS IDENTICAL IN THE SAME ORDER -- offsets only.  262 instructions.
 * 36 windows / 146 window-lines, 23 structural.  THE BEST-POSITIONED OF ITS AGENT'S
 * FIVE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/dbbdc_ElementOrbs.c \
 *     asm/rom_c9000/rom_dbbdc_c_c_c_c_c_c_c.s --func Anim_Unused_ElementOrbs
 * FIVE functions in the reference; a text split is required.  No data work -- Data_ede48
 * is external (.incdata in asm/rom_c9000/rom_eda78.s), and the file's own .rodata
 * belongs to Anim_Atalanta / BaseAnim_Breath.
 *
 * base->r11, frame->r10, the 0xff mask->r8 in the seeding loop.  Declaration order
 * ctx(0x10) tp(0x0c) nf(0x08), then tv, pos, fns.
 *
 * TWO OF THIS BATCH'S NEW LEVERS WERE FOUND HERE:
 *
 * THE SHAPE OF AN INDUCTION EXPRESSION DECIDES WHETHER loop.c STRENGTH-REDUCES IT, AND
 * IT IS THE SHIFT RATHER THAN THE ALGEBRA.  `frame * (i * 8 + 0x100)` was NOT reduced;
 * the same value as `frame * ((i + 0x20) << 3)` WAS, taking this function from 275/273
 * instructions and 236 window-lines to 273/273 size-exact and 166 lines.
 * `((i + 0x20) * 8)` -- multiply instead of shift -- measured IDENTICAL TO THE
 * UNREDUCED FORM.
 *
 * LOADING THE INDIRECT-CALL TARGET AT THE CALL SITE rather than into a hoisted local
 * gives the ROM's `_call_via_r4`, and it is what made the relocation symbol list
 * identical in order (42 -> 36 windows).
 *
 * RESIDUE: ONE OF THE TWO ANGLE givs IS STILL NOT STRENGTH-REDUCED -- the positive
 * (yaw) one, USED ONCE, where the negative one is used three times, so loop.c's benefit
 * threshold rejects it.  The ROM reduces both.  `-nf * (...)` for the yaw multiplier and
 * `* 8` instead of `<< 3` both measured 275/273.
 *
 * That is the same threshold arithmetic as the giv findings in
 * src/non_matching/rom_b5000/80bfba4.c and the named-multiplier lever in
 * docs/elevation.md -- and note the direction here is the OPPOSITE of the usual one: we
 * need gcc to reduce MORE, not less, so the named-multiplier lever (which SUPPRESSES
 * reduction) is the wrong tool and `flag_reduce_all_givs` is off by default.
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
} Part;

typedef struct {
    int f0;
    int f4;
    int f8;
} State;

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixPush(void);
extern void MatrixPop(void);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void MatrixRoll(int a);
extern int Func_80e3944(void *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Unused_ElementOrbs(void *context)
{
    register unsigned char *base __asm__("r11");
    void *ctx;
    vec3_t *tp;
    int nf;
    State **s0;
    int arg;
    char *view;
    register int frame __asm__("r10");
    vec3_t tv;
    vec3_t pos;
    void *fns[2];

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    s0 = (State **)(base + 0x7828);
    *s0 = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_8c, base, 1, 1);
    BuildDraw2DFuncs((*s0)->f4 ^ 1, fns);
    {
        Part *g = gBuffer;
        register int mask __asm__("r8");
        int i;
        int z;
        mask = 0xff;
        i = 0;
        z = 0;
        do {
            g->f0 = ((Random() & mask) - 0x7f) << 16;
            g->f4 = ((Random() & mask) - 0x7f) << 16;
            g->f8 = ((Random() & mask) - 0x7f) << 16;
            i++;
            g->fc = z;
            g->f10 = z;
            g->f14 = z;
            g->f18 = z;
            g++;
        } while (i != (0x80 << 1));
    }
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    tp = &tv;
    tp->x = 0;
    tp->y = 0xa0 << 15;
    tp->z = 0;
    frame = 0;
    do {
        Part *g;
        int i;
        view = (char *)iwram_3001e80;
        InitMatrixStack();
        MatrixSetLook(view, view + 0xc);
        MatrixTranslatev(tp);
        nf = -frame;
        g = gBuffer;
        i = 0;
        do {
            if (frame > i / 4 && g->f18 == 0) {
                int m;
                int sz;
                int w;
                int ix;
                int off;
                MatrixPush();
                m = i & 3;
                switch (m) {
                case 0:
                    MatrixYaw(frame * ((i + 0x20) << 3));
                    break;
                case 1:
                    MatrixPitch(nf * ((i + 0x20) << 3));
                    break;
                case 2:
                    MatrixRoll(nf * ((i + 0x20) << 3));
                    break;
                case 3:
                    MatrixPitch(nf * ((i + 0x20) << 3));
                    MatrixRoll(nf * ((i + 0x20) << 3));
                    break;
                }
                Func_80e3944(g, &pos);
                pos.x = pos.x >> 1;
                MatrixPop();
                if (pos.z <= 0xf9) {
                    pos.z = 0xfa;
                }
                if (pos.z > 0x27a) {
                    pos.z = 0x27a;
                }
                sz = 9 - (pos.z - 0xfa) / 0x40;
                w = sz * 2;
                ix = w - 2;
                off = m * 770 + *(unsigned short *)((char *)Data_ede48 + ix);
                (*(DrawFn *)&fns[0])(ctx, base + off, pos.x - sz / 2, pos.y - sz, sz, w);
                Func_80e38b8(g, 0x3c, 0);
                if (frame > i / 4 + 0x1e) {
                    g->fc = g->fc + ((-g->f0) >> 8);
                    g->f10 = g->f10 + ((-g->f4) >> 8);
                    g->f14 = g->f14 + ((-g->f8) >> 8);
                }
            }
            i++;
            g++;
        } while (i != 0x40);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0xa0);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
