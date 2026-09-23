/* Anim_Unused_SkullCloud -- NON-MATCHING, 199 encodings of 303, size 680 against the
 * ROM's 684 (-4), 301 instructions against 303.  292 instructions in the reference.
 * ALL 35 RELOCATION SYMBOLS IDENTICAL IN THE SAME ORDER -- offsets only.
 * 41 windows / 181 lines.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/dbbdc_SkullCloud.c \
 *     asm/rom_c9000/rom_dbbdc_c_c_c_c_c_c_c.s --func Anim_Unused_SkullCloud
 * FIVE functions in the reference; a text split is required.  No data work -- the file's
 * .rodata belongs to Anim_Atalanta / BaseAnim_Breath, not to this function.
 *
 * FRAME: ours `sub sp,#0x3c` against the ROM's 0x40 -- ONE MISSING SPILLED SCALAR
 * again, the fourth instance of that family in this bank in one batch (see
 * cb1a4_EPowerUp.c, cb1a4_DeathPlunge.c and ca1e4_Spore.c, which are written-twice-
 * never-read, never-touched, and written-once-never-read respectively; the last was
 * fixed with an ARRAY).
 *
 * base->r10, the 0->r8 in the seeding loop.  Declaration order ctx(0x24) frame(0x20)
 * d1(0x1c) d0(0x18) t(0x14) view(0x10) look(0x0c) [t*6 giv](0x08), then pos, tv -- and
 * note the giv sitting at the LOWEST slot, which is the corollary below in action.
 *
 * NAMED BLOCKER: the ROM reaches the state through a REGISTER-OFFSET LOAD
 * (`ldr r2,=0x7828 / mov r4,r10 / ldr r3,[r4,r2]`, the constant REMATERIALISED at each
 * of two sites per frame) where ours forms a pointer pseudo
 * (`ldr r3,=0x7828 / add r3,r10 / ldr r3,[r3]`) and spills it.  SAME INSTRUCTION COUNT,
 * DIFFERENT FORM.  This is the CSE-constant class, and THE
 * `register int __asm__("r3")` FIX THAT WORKED ON HelmSplitter DID NOT REACH IT HERE
 * (43 windows) -- so that lever is site-dependent even within one bank.
 *
 * The `t*6` giv as an explicit `j6` local moved 181 -> 172 lines but OVERSHOT to
 * 305/303 instructions.
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
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
} State;

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern void *iwram_3001e80;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern int Func_80e3944(void *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Unused_SkullCloud(void *context)
{
    register unsigned char *base __asm__("r10");
    void *ctx;
    int frame;
    DrawFn d1;
    DrawFn d0;
    int t;
    char *view;
    char *look;
    unsigned char *data;
    CopyFn copy;
    int arg;
    int koff;
    vec3_t *pp;
    vec3_t *tp;
    vec3_t pos;
    vec3_t tv;

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_9e, base, 1, 1);
    LoadVFXFile(FILE_6c, base + (0xd8 << 5), 0, 0);
    data = (unsigned char *)GetFile(FILE_bb);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = (int)data;
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    d0 = (DrawFn)((char **)&iwram_3001eec)[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    d1 = (DrawFn)((char **)&iwram_3001eec)[8];
    {
        Part *g = (Part *)(base + (0xe1 << 7));
        register int z __asm__("r8");
        int i;
        i = 0;
        z = 0;
        do {
            int a = Random() & 0xffff;
            int r = Random() & 0xff;
            g->f0 = z;
            g->f4 = z;
            g->f8 = z;
            if (i % 6 == 5) {
                g->fc = z;
                g->f10 = z;
            } else {
                g->fc = (r * sin(a)) >> 7;
                g->f10 = (r * cos(a)) >> 9;
            }
            i++;
            g->f14 = z;
            g->f18 = z;
            g++;
        } while (i != 0x40);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    frame = 0;
    do {
        view = (char *)iwram_3001e80;
        t = 0;
        koff = 0x7828;
        if ((*(State **)(base + koff))->f14 != 0) {
            look = view + 0xc;
            tp = &tv;
            do {
                int *actor;
                int lo;
                int hi;
                actor = (int *)*(int **)_GetBattleActor(
                    (*(State **)(base + koff))->ids[t]);
                InitMatrixStack();
                MatrixSetLook(view, look);
                tp->x = actor[2];
                tp->y = 0xa0 << 13;
                tp->z = actor[4];
                MatrixTranslatev(tp);
                lo = t << 3;
                hi = lo + 0x28;
                if (frame >= lo && frame < hi) {
                    Part *g = (Part *)(base + (0xe1 << 7)) + t * 6;
                    int k = 0;
                    pp = &pos;
                    do {
                        int s = g->f18 / 6;
                        DrawFn f;
                        if (s > 5) {
                            s = 5;
                        }
                        Func_80e3944(g, pp);
                        pp->x = pp->x >> 1;
                        if (k == 5) {
                            Func_80e38b8(g, 0x3e, 0x80 << 4);
                            f = d0;
                            f(ctx, base + s * 1152 + (0xd8 << 5), pos.x - 0xc,
                              pos.y - 0x24, 0x18, 0x30);
                        } else {
                            Func_80e38b8(g, 0x3c, 0x80 << 2);
                            f = d1;
                            f(ctx, base + s * 1152, pos.x - 0xc, pos.y - 0x24,
                              0x18, 0x30);
                        }
                        g->f18 = g->f18 + 1;
                        k++;
                        g++;
                    } while (k != 6);
                }
                t++;
            } while (t != (*(State **)(base + koff))->f14);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
