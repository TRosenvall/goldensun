/* Anim_Break -- 345 instructions, 800 bytes, 359 encodings and 37 relocations
 * identical.  FRAME `sub sp, #0x38`, the ROM's exactly.  Split out of
 * asm/rom_c9000/rom_d82b0.s; Anim_Drain stays in asm as rom_d82b0_c.s, parked at 143 of
 * 400.  THE FIRST rom_c9000 ANIMATION ENTRY POINT TO LAND.
 *
 * ITS .rodata IS EMITTED FROM C, NOT REHOMED.  `.Lee9f8` is `.incrom 0xee9f8, 0xeea08`
 * -- 16 bytes, read independently out of baserom.gba as four 32-bit words
 * {0x4000, 0x2000, 0x4000, 0x8000} -- and referenced ONLY by this function (zero
 * occurrences in Anim_Drain).  That is squarely docs/elevation.md ~21174's case, so the
 * array below carries `__asm__(".Lee9f8")` and gcc writes `.global .Lee9f8 /
 * .section .rodata / .align 2, 0` plus four `.word`s.  Both stage1.ld `.rodata` lines
 * stay, and the blob's address is unchanged because the two slots are adjacent and only
 * one object supplies data.
 *
 * ================================================================
 * objcmp's SIZE LINE IS A FALSE POSITIVE, BY EXACTLY THE BLOB'S LENGTH, WHEN THE
 * CANDIDATE *DEFINES* A .rodata ARRAY FROM C
 * ================================================================
 *
 * objcmp sums the object's sections, so this file screens as "ref 800 bytes, ours 816"
 * -- 16 bytes, the array -- while ENCODINGS and RELOCATIONS stay silent.  Confirmed both
 * ways: the identical file with `.Lee9f8` declared `extern` instead of defined reports a
 * clean `OK Anim_Break -- 800 bytes, 359 encodings and 37 relocations identical`.
 *
 * SO SCREEN WITH `extern`, THEN RESTORE THE DEFINITION FOR THE LANDING.  This is the
 * objcmp analogue of the documented tryc false pool warning, and it is the fourth
 * measurement-tool artefact recorded across batches 280-283.
 *
 * ================================================================
 * FOUR LEVERS, AND TWO OF THEM CONTRADICT RULES THIS BANK HAD ALREADY RECORDED
 * ================================================================
 *
 * A POOL LOAD HOISTED ABOVE A CALL IS A REGISTER-ROTATION LEVER, and it closed this
 * function.  Writing `gp = (char *)gBuffer;` BEFORE the surrounding call and
 * `g = (Part *)(gp + boff);` after it was the last 4 encodings here, and worth 185 -> 154
 * on Anim_Drain.  Mechanism: it LENGTHENS the pool pseudo's live range, and local_alloc
 * priority is floor_log2(refs)*refs/(death-birth), so the short-lived zero constant
 * beside it wins r3 first and the pool falls to r0 -- the ROM's assignment.  One
 * expression (`(char *)gBuffer + boff`) keeps the pool pseudo short and gcc gives it r3.
 *
 * `while (c)` AND `if (c) { do ... while (c); }` ARE DIFFERENT CODE, AND THE ROM SAYS
 * WHICH.  `duplicate_loop_exit_test` runs AFTER gcse, so a `while` guard cannot be
 * gcse'd, while an explicit `if` guard exists at gcse time and lets gcse hoist a
 * loop-invariant ADDRESS into the preheader.  This function's frame loop needs the `if`
 * form (the ROM has `slot = base+0x7828` in the preheader); ANIM_DRAIN'S NEEDS `while`
 * -- there the `if` form hoisted it into a callee-saved register and COST `base` ITS r9,
 * which was that function's entire dominant residue.  168 -> 135, and it put `base` back
 * in r9 WITHOUT A PIN.
 *
 * A COUNTER REUSED ACROSS SEVERAL LOOPS IS ONE VARIABLE.  This ROM keeps one counter in
 * r8 across THREE unrelated loops and one in r11 across two; BaseAnim_HauntAttack's keeps
 * one in r7 across four.  Splitting them per region cost a spill slot each time and 314
 * of 359.  THIS IS THE COUNTERPART OF "SPLIT REUSED LOCALS PER REGION", NOT A
 * CONTRADICTION OF IT -- that rule is about VALUES, this one about COUNTERS.
 *
 * PER-TARGET INDUCTION VARIABLES BELONG AS STRENGTH-REDUCED givs, WHICH CONTRADICTS THE
 * RULE RECORDED ON cf2a0_Revive.c AND ceb30_HauntAttack.c ("the four per-target induction
 * variables must be EXPLICIT LOCALS", claimed as a bank rule on two functions).  Writing
 * `boff = i * (0xe0 << 4); io = 0x24 + i * 2;` and letting strength_reduce build them
 * puts their initialising stores in the LOOP PREHEADER, AFTER THE GUARD BRANCH, where
 * this ROM has them -- explicit locals put them BEFORE the guard.  30 -> 10 disagreeing
 * in one edit.
 *
 * AND IT EXPLAINS THE SLOT MAP: SR pseudos are created after expand, so they take the
 * LOWEST slots.  So READ A SPILL SLOT BELOW A COMPILER TEMP AS EVIDENCE THAT VALUE IS A
 * giv RATHER THAN A DECLARED LOCAL -- which is the same corollary the declaration-order
 * rule reached from the other direction.
 *
 * Also: the "fill r0 last" lever applies to CALLS THROUGH A FUNCTION POINTER too
 * (`typedef int (*DrawFn)(...)`), worth 2 encodings on the argument setup here.
 *
 * MEASURED NEGATIVE, and it is a caution about this bank's headline advice: PINNING
 * `base` TO ITS ROM REGISTER IS NOT ALWAYS RIGHT.  On Anim_Drain it stopped gcc keeping
 * 0x7828 in a register (three extra pool loads per iteration) and grew the frame
 * 0x40 -> 0x44; PRESSURE, NOT A PIN, was the fix.  Pinning the counter to r8 shrank
 * Drain's frame to 0x34 (200 encodings).
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

const int Lee9f8[4] __asm__(".Lee9f8") = { 0x4000, 0x2000, 0x4000, 0x8000 };

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
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *g, int a, int b);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Break(void *context)
{
    vec3_t vin;
    vec3_t vout;
    unsigned char *base;
    void *ctx;
    DrawFn blit;
    unsigned char *gfx;
    int io;
    int boff;
    int **tbl;
    int **pp;
    State **slot;
    void *view;
    unsigned char *data;
    CopyFn copy;
    int i;
    int j;
    int frame;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    gfx = (unsigned char *)tbl[2];
    view = *(void **)((char *)tbl - 0x6c);
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    data = GetFile(FILE_b9);
    {
        register int q0 __asm__("r0");
        register int q2 __asm__("r2");
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, data, q2);
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    blit = (DrawFn)tbl[7];
    {
        int *q = &ewram_2010018;
        j = 0;
        do {
            j++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (j != (0x80 << 3));
    }
    InitMatrixStack();
    MatrixSetLook(view, (char *)view + 0xc);
    i = 0;
    while (i != (*(State **)(base + 0x7828))->f14) {
        {
            int *actor;
            int ab;
            int h;
            Part *g;
            char *gp;

            boff = i * (0xe0 << 4);
            io = 0x24 + i * 2;

            actor = (int *)_GetBattleActor(*(short *)((char *)*(State **)(base + 0x7828) + io));
            ab = *actor;
            h = _Func_80b8530(*(short *)((char *)*(State **)(base + 0x7828) + io)) / 2;
            vin.x = *(int *)(ab + 8);
            vin.y = h;
            vin.z = *(int *)(ab + 0x10);
            gp = (char *)gBuffer;
            Func_80e3944(&vin, &vout);
            vout.x >>= 1;
            j = 0;
            g = (Part *)(gp + boff);
            do {
                int mag = Random() & 0xff;
                int ang = Random() & 0xffff;
                g->x = ((sin(ang) * mag) >> 7) + (vout.x << 16);
                g->y = ((cos(ang) * mag) >> 3) + (vout.y << 16);
                g->vx = (0x80 - (Random() & 0xff)) << 9;
                g->vy = (-(Random() & 0xff) - 0x80) << 10;
                g->t = 0;
                j++;
                g++;
            } while (j != 0x80);
            i++;
        }
    }
    *(int *)(base + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x32;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    frame = 0;
    if ((*(State **)(base + 0x7828))->f14 * 0x14 + 0x38 != 0) {
      do {
        int th;
        int boff2;

        slot = (State **)(base + 0x7828);
        if (frame == 0x20) {
            _Func_80bd7dc(0);
        }
        i = 0;
        if ((*slot)->f14 != 0) {
            th = 0;
            boff2 = 0;
            do {
                if (frame == th) {
                    _PlaySound(0x8f);
                    Func_80d6888((*slot)->ids[i], 7, -1, i, 0x14);
                }
                if (frame > th) {
                    Part *g = gBuffer;
                    j = 0;
                    g = (Part *)((char *)g + boff2);
                    do {
                        if (g->t >= 0) {
                            int n = j % 3 + 1;
                            int w2 = n * 2;
                            unsigned char *src = gfx + Data_ede48[n - 1];
                            int x = *(short *)((char *)g + 2) - n / 2;
                            int y = *(short *)((char *)g + 6) - n;
                            blit(ctx, src, x, y, n, w2);
                            Func_80e3908(g, 0x3e, Lee9f8[j & 3]);
                            g->t += 1;
                            if (g->vy > 0 && *(short *)((char *)g + 6) > 0x70) {
                                g->t = -1;
                            }
                        }
                        j++;
                        g++;
                    } while (j != 0x80);
                }
                th += 0x14;
                boff2 += 0xe0 << 4;
                i++;
            } while (i != (*slot)->f14);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != (*slot)->f14 * 0x14 + 0x38);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
