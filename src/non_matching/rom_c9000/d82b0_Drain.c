/* Anim_Drain -- NON-MATCHING, 143 encodings of 400, size 884 against the ROM's 888 (-4),
 * 398 instructions against 400.  383 instructions.  FRAME `sub sp, #0x40` -- the ROM's.
 * tryc --align: 46 instructions in disagreeing regions of 401.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/d82b0_Drain.c \
 *     asm/rom_c9000/rom_d82b0_c.s --func Anim_Drain
 * Its file-mate Anim_Break LANDED in batch 283 (src/rom_c9000/rom_d82b0_b.c) and the file
 * was split, so this function now lives alone in the _c half and needs NO FURTHER SPLIT.
 * `base` is in r9 as the ROM has it, WITHOUT A PIN -- see below.
 *
 * BLOCKER CLASS: A SPILL-SLOT PERMUTATION OF THREE STRENGTH-REDUCED INDUCTION VARIABLES.
 * The ROM has `io` at sp+0x18, `boff2` at 0x14 and `boff` at 0x10; ours has boff2 0x18,
 * boff 0x14, io 0x10 -- a clean 3-cycle.
 *
 * SINCE giv PSEUDOS ARE CREATED AFTER expand, THEIR SLOT ORDER IS THE ORDER
 * strength_reduce BUILDS THEM IN, and that could not be steered: NINE spellings of the
 * two offsets -- byte offset, `&gBuffer[i*0x80]`, `(i<<4)*0xe0`, use-order swaps, explicit
 * increments -- were ALL EXACTLY INERT AT 143.  That is a different problem from the
 * declaration-order rule, which only places pseudos that exist at expand time.
 *
 * Plus about 12 scratch-register renames and ONE REAL INSTRUCTION: the ROM re-copies `ab`
 * out of r10 (`mov r3, r10`) before reading +0x10, because reading +0xc destroyed its
 * first copy; ours keeps one copy.  That is the whole 398-against-400.
 *
 * TWO LEVERS HERE RUN OPPOSITE TO ITS FILE-MATE'S, and both are measured:
 *
 * THIS FUNCTION NEEDS `while (c)` WHERE Anim_Break NEEDS `if (c) { do ... while (c); }`.
 * duplicate_loop_exit_test runs AFTER gcse, so a `while` guard cannot be gcse'd while an
 * explicit `if` guard can.  Here the `if` form hoisted a loop-invariant address into a
 * callee-saved register and COST `base` ITS r9 -- the entire dominant residue.  The
 * `while` form was 168 -> 135 and it put base back in r9 WITHOUT A PIN.
 *
 * AND PINNING `base` IS WRONG HERE.  It stopped gcc keeping 0x7828 in a register (three
 * extra pool loads per iteration) and grew the frame 0x40 -> 0x44.  PRESSURE, NOT A PIN,
 * WAS THE FIX.  Pinning the counter to r8 shrank the frame to 0x34 (200 encodings).  This
 * is the second caution against the bank's headline pin advice, after the
 * do-not-pin-a-dereferenced-pointer rule.
 *
 * A POOL LOAD HOISTED ABOVE A CALL IS A REGISTER-ROTATION LEVER, worth 185 -> 154 here:
 * `gp = (char *)gBuffer;` BEFORE the surrounding call and `g = (Part *)(gp + boff);` after
 * it lengthens the pool pseudo's live range, so a short-lived constant beside it wins r3
 * first and the pool falls to r0 -- the ROM's assignment.
 *
 * AND A RANGE-FOLD NEGATIVE WORTH THE ROW:
 * `if (dx >= -0xfff && dx <= 0xfff && dz >= ... )` FOLDS ONLY THE FIRST PAIR into the
 * unsigned range test -- the second stays two signed compares.  BOTH must be written
 * `(unsigned)(dx + 0xfff) <= 0x1ffe` explicitly.  Worth 135 -> 102.
 *
 * No per-file Makefile flag override applies to this stem.
 */
#include "gba/types.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
    int t;
} Part;

extern int *iwram_3001eec[];
extern int ewram_2010018;
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern void *gPtrs[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void *_GetBattleActor(int id);
extern int _Func_80b8530(int id);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e38b8(Part *g, int a, int b);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Drain(void *context)
{
    vec3_t vtmp;
    void *ctx;
    DrawFn blit;
    unsigned char *gfx;
    void *view;
    int h2;
    unsigned char *base;
    int **tbl;
    int **pp;
    unsigned char *data;
    CopyFn copy;
    int kind;
    int fid;
    int i;
    int j;
    int frame;
    int arg;
    int th;
    int boff2;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    gfx = (unsigned char *)tbl[2];
    view = *(void **)((char *)tbl - 0x6c);
    kind = (unsigned int)(-((State *)context)->f18 | ((State *)context)->f18) >> 31;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    if (kind == 0) {
        fid = FILE_b9;
    } else {
        fid = FILE_c0;
    }
    data = GetFile(fid);
    {
        register int q0 __asm__("r0");
        register int q2 __asm__("r2");
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, data, q2);
    }
    {
        int *q = &ewram_2010018;
        j = 0;
        do {
            j++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (j != (0x80 << 3));
    }
    i = 0;
    while (i != (*(State **)(base + 0x7828))->f14) {
        int *actor;
        int ab;
        register int h __asm__("r10");
        Part *g;
        char *gp;

        gp = (char *)gBuffer;
        actor = (int *)_GetBattleActor((*(State **)(base + 0x7828))->ids[i]);
        ab = *actor;
        h = _Func_80b8530((*(State **)(base + 0x7828))->ids[i]) / 2;
        g = (Part *)(gp + i * (0xe0 << 4));
        j = 0;
        do {
            g->x = *(int *)(ab + 8);
            g->y = *(int *)(ab + 0xc) + h;
            g->z = *(int *)(ab + 0x10);
            g->vx = ((Random() & 0xff) - 0x80) << 10;
            g->vy = ((Random() & 0xff) - 0x80) << 10;
            g->vz = ((Random() & 0xff) - 0x80) << 10;
            g->t = 0;
            j++;
            g++;
        } while (j != 0x80);
        i++;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    blit = (DrawFn)gPtrs[0xb8 / 4];
    *(int *)(base + (0xef << 7)) = 3;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x4040404;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    _PlaySound(0x8e);
    frame = 0;
    while (frame != (*(State **)(base + 0x7828))->f14 * 0x14 + 0x48) {
        int *actor;
        int ab;

        actor = (int *)_GetBattleActor((*(State **)(base + 0x7828))->f8);
        ab = *actor;
        h2 = _Func_80b8530((*(State **)(base + 0x7828))->f8) / 2;
        if (frame == 0x40) {
            _Func_80bd7dc(0x85);
        }
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if (frame == 0x28) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 7, -1, -1, 0);
        }
        if (frame == (*(State **)(base + 0x7828))->f14 * 0x14 + 0x34) {
            Func_80d6888((*(State **)(base + 0x7828))->f8, 0, -1, -1, 0);
        }
        i = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            do {
                boff2 = i * (0xe0 << 4);
                th = i * 0x14;
                if (frame == th) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 0x2a);
                }
                if (frame > th) {
                    Part *g = (Part *)((char *)gBuffer + boff2);
                    j = 0;
                    do {
                        if (g->t >= 0) {
                            Func_80e3944((vec3_t *)g, &vtmp);
                            vtmp.x >>= 1;
                            blit(ctx, gfx + Data_ede48[5], vtmp.x - 3, vtmp.y - 6, 6, 0xc);
                            Func_80e38b8(g, 0x3e, 0);
                            if (frame > th + j + 0xa) {
                                int dx = *(int *)(ab + 8) - g->x;
                                int dy = *(int *)(ab + 0xc) + h2 - g->y;
                                int dz = *(int *)(ab + 0x10) - g->z;
                                dx >>= 8;
                                g->vx += dx;
                                dy >>= 8;
                                g->vy += dy;
                                dz >>= 8;
                                g->vz += dz;
                                if ((unsigned int)(dx + 0xfff) <= 0x1ffe && (unsigned int)(dz + 0xfff) <= 0x1ffe) {
                                    g->t = -1;
                                }
                            }
                        }
                        j++;
                        g++;
                    } while (j != 0x20);
                }
                i++;
            } while (i != (*(State **)(base + 0x7828))->f14);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
