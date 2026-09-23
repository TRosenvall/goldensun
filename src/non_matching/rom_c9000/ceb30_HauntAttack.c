/* BaseAnim_HauntAttack -- NON-MATCHING, 286 encodings of 464 (WAS 384; advanced in batch
 * 283), size 1048 against the ROM's 1040 (+8), 468 instructions against 464.  442
 * instructions in the reference.  205 -> 191 disagreeing regions, frame still exactly
 * 0x54, and THE SPILL-WORD COUNT IS NOW 14 -- the ROM's.
 *
 * THE PARK'S NAMED NEXT STEP IS PARTLY DONE.  `aCopy` and `bCopy` were identified from the
 * ROM as COPIES OF `a` AND `base2` MADE IN THE INNERMOST DRAW BLOCK (`str [sp,#0x24]` from
 * `[sp,#0x14]`, `str [sp,#0x20]` from r11) and declared there; the explicit `v2` was
 * dropped so cse1 creates it; `hh` moved to function scope.
 *
 * WHAT REMAINS: the ROM keeps 0x7828 HOISTED INTO r2 ACROSS THE TARGET LOOP and uses
 * register-offset loads, where ours reloads the pool each time.  `int so = 0x7828;` was
 * RE-MEASURED with the new levers applied and is still WORSE (frame 0x54 -> 0x60,
 * 286 -> 373), so this park's original negative result on that spelling stands rather than
 * being an artefact of the older candidate.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/ceb30_HauntAttack.c \
 *     asm/rom_c9000/rom_ceb30_c_c_c_a.s
 * ONE function -- CONVERTS WHOLE when it lands, no split.  `.Lee090` is referenced
 * but DEFINED IN A DIFFERENT FILE (asm/rom_c9000/rom_ceb30_c_c_c_c.s), so it is an
 * ordinary extern and there is NO DATA WORK: declare
 * `extern unsigned char Lee090[] __asm__(".Lee090");`.
 *
 * tryc warns THE REFERENCE KEEPS ITS LITERAL POOL INSIDE THE FUNCTION, so objcmp is
 * the only authority here and every number is objcmp.
 *
 * `sub sp, #0x54` MATCHES EXACTLY, and the prologue, the LZ load, the if/else-if file
 * dispatch, the pinned DMA and BuildDraw2DFuncs are structurally exact on the second
 * candidate -- every difference through instruction 54 is a pool offset.  246
 * instructions in 80 disagreeing windows, all small.
 *
 * THIS IS THE PHASE ITS SIBLING BaseAnim_Tackle WAS IN BEFORE THE DECLARATION-ORDER
 * FIX, SO THE NEXT STEP IS KNOWN.  `base` is r10, `frame` is at sp+0x34, `variant` at
 * sp+0x3c, and the ROM's FOURTEEN spill words read HIGH TO LOW are:
 *
 *     variant, ctx, frame, kk, view, hh, aCopy, bCopy, v2, view+0xc, a, io, boff,
 *     0x100000
 *
 * Declaring in that order is the lever -- see
 * src/non_matching/rom_c9000/dfa18_Tackle.c for why (spilled scalars fill downward in
 * DECLARATION order while arrays take the high offsets in REVERSE declaration order,
 * so the ROM's slot map IS the source's declaration order).  Currently `a`/`io`/`boff`
 * occupy the ROM's `aCopy`/`bCopy`/`v2` slots; declaring the two copies explicitly
 * restores the offsets BUT COSTS A FIFTEENTH WORD (frame 0x58), so that permutation is
 * exactly where the next session should start.
 *
 * TWO MEASUREMENTS WORTH KEEPING:
 *
 * THE FOUR PER-TARGET INDUCTION VARIABLES (`a, io, boff, base2`) MUST BE EXPLICIT
 * LOCALS -- confirming src/non_matching/rom_c9000/cf2a0_Revive.c's negative result on
 * a second function, so that is now a bank rule rather than a one-off.
 *
 * `int so = 0x7828;` AS A NAMED OFFSET VARIABLE reproduces the ROM's register-offset
 * loads BUT COSTS A FRAME WORD (0x5c) -- net worse, 401.  This is the in-bank CONTROL
 * that proves its sibling's constant-rematerialisation residue is cse1 sharing rather
 * than noise: here the same constant has ~6 uses, gcc DOES keep it in a register, and
 * the ROM then uses register-offset loads instead of an add.
 *
 * `BaseAnim_*` TAKES `(context, variant)` -- published in the tree by landed
 * three-line wrappers, not guessed.  This one dispatches with an IF/ELSE-IF CHAIN
 * where its sibling Tackle uses a `switch` with a bare `default:`; read the branch
 * polarity per function.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
 *
 * NEXT: the fifteenth-word problem above.  It is a concrete, stated permutation, not
 * a search.
 */
/* BaseAnim_HauntAttack -- NON-MATCHING, 384 encodings of 464, size 1048 against the
 * ROM's 1040 (+8), 468 instructions against 464.  442 instructions in the reference.
 * EARLY, but the frame is already exact and the next step is known precisely.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/ceb30_HauntAttack.c \
 *     asm/rom_c9000/rom_ceb30_c_c_c_a.s
 * ONE function -- CONVERTS WHOLE when it lands, no split.  `.Lee090` is referenced
 * but DEFINED IN A DIFFERENT FILE (asm/rom_c9000/rom_ceb30_c_c_c_c.s), so it is an
 * ordinary extern and there is NO DATA WORK: declare
 * `extern unsigned char Lee090[] __asm__(".Lee090");`.
 *
 * tryc warns THE REFERENCE KEEPS ITS LITERAL POOL INSIDE THE FUNCTION, so objcmp is
 * the only authority here and every number is objcmp.
 *
 * `sub sp, #0x54` MATCHES EXACTLY, and the prologue, the LZ load, the if/else-if file
 * dispatch, the pinned DMA and BuildDraw2DFuncs are structurally exact on the second
 * candidate -- every difference through instruction 54 is a pool offset.  246
 * instructions in 80 disagreeing windows, all small.
 *
 * THIS IS THE PHASE ITS SIBLING BaseAnim_Tackle WAS IN BEFORE THE DECLARATION-ORDER
 * FIX, SO THE NEXT STEP IS KNOWN.  `base` is r10, `frame` is at sp+0x34, `variant` at
 * sp+0x3c, and the ROM's FOURTEEN spill words read HIGH TO LOW are:
 *
 *     variant, ctx, frame, kk, view, hh, aCopy, bCopy, v2, view+0xc, a, io, boff,
 *     0x100000
 *
 * Declaring in that order is the lever -- see
 * src/non_matching/rom_c9000/dfa18_Tackle.c for why (spilled scalars fill downward in
 * DECLARATION order while arrays take the high offsets in REVERSE declaration order,
 * so the ROM's slot map IS the source's declaration order).  Currently `a`/`io`/`boff`
 * occupy the ROM's `aCopy`/`bCopy`/`v2` slots; declaring the two copies explicitly
 * restores the offsets BUT COSTS A FIFTEENTH WORD (frame 0x58), so that permutation is
 * exactly where the next session should start.
 *
 * TWO MEASUREMENTS WORTH KEEPING:
 *
 * THE FOUR PER-TARGET INDUCTION VARIABLES (`a, io, boff, base2`) MUST BE EXPLICIT
 * LOCALS -- confirming src/non_matching/rom_c9000/cf2a0_Revive.c's negative result on
 * a second function, so that is now a bank rule rather than a one-off.
 *
 * `int so = 0x7828;` AS A NAMED OFFSET VARIABLE reproduces the ROM's register-offset
 * loads BUT COSTS A FRAME WORD (0x5c) -- net worse, 401.  This is the in-bank CONTROL
 * that proves its sibling's constant-rematerialisation residue is cse1 sharing rather
 * than noise: here the same constant has ~6 uses, gcc DOES keep it in a register, and
 * the ROM then uses register-offset loads instead of an add.
 *
 * `BaseAnim_*` TAKES `(context, variant)` -- published in the tree by landed
 * three-line wrappers, not guessed.  This one dispatches with an IF/ELSE-IF CHAIN
 * where its sibling Tackle uses a `switch` with a bare `default:`; read the branch
 * polarity per function.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
 *
 * NEXT: the fifteenth-word problem above.  It is a concrete, stated permutation, not
 * a search.
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
} Desc;

typedef struct {
    int x;
    int y;
    int z;
    int dx;
    int dy;
    int dz;
    int life;
} Part;

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern unsigned char gBuffer[];
extern int ewram_2010018;
extern unsigned char Lee090[] __asm__(".Lee090");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern int DecompressLZ(void *src, void *dst);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80dbb9c(void);
extern void *_GetBattleActor(int id);
extern int _Func_80b8530(int rec);
extern int Random(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int sin(int a);
extern int Func_80e3944(void *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void BaseAnim_HauntAttack(void *context, int variant)
{
    register unsigned char *base __asm__("r10");
    void *ctx;
    int frame;
    int kk;
    int j;
    char *view;
    unsigned char *data;
    CopyFn copy;
    int fid;
    int arg;
    vec3_t pos;
    void *fns[2];

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    *(Desc **)(base + 0x7828) = (Desc *)context;
    AnimStart(1);
    DecompressLZ(GetFile(FILE_69), base);
    if (variant == 0) {
        fid = FILE_bb;
    } else if (variant == 1) {
        fid = FILE_8d;
    } else {
        fid = FILE_91;
    }
    data = GetFile(fid);
    {
        PIN3;
        q1 = (int)data;
        q0 = 0xa0;
        q2 = 0x80;
        copy = Func_8001af8;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    BuildDraw2DFuncs((*(Desc **)(base + 0x7828))->f4, fns);
    {
        int *q = &ewram_2010018;
        int n = 0x80;
        int mone = 1;
        j = 0;
        mone = -mone;
        n <<= 3;
        do {
            j++;
            *q = mone;
            q = (int *)((char *)q + 0x1c);
        } while (j != n);
    }
    kk = 0;
    if ((*(Desc **)(base + 0x7828))->f14 != 0) {
        register int mask __asm__("r9");
        register int boff __asm__("r11");
        mask = 0xff;
        boff = 0;
        do {
            Desc **s = (Desc **)(base + 0x7828);
            int *src;
            Part *p;
            char *gp;
            register int yv __asm__("r8");
            gp = (char *)gBuffer;
            src = (int *)*(int *)_GetBattleActor((*s)->f8);
            yv = _Func_80b8530((*s)->f8);
            p = (Part *)(gp + boff);
            j = 0;
            do {
                p->y = yv;
                p->x = src[2];
                p->z = src[4];
                p->dx = ((Random() & mask) - 0x80) << 10;
                p->dy = ((Random() & mask) - 0x80) << 10;
                p->dz = ((Random() & mask) - 0x80) << 10;
                j++;
                p->life = 0;
                p++;
            } while (j != 0x80);
            kk++;
            boff += 0xe0 << 4;
        } while (kk != (*(Desc **)(base + 0x7828))->f14);
    }
    arg = 0x90;
    arg <<= 3;
    StartTask(Func_80dbb9c, arg);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, arg);
    _PlaySound(0x92);
    frame = 0;
    while (frame != Lee090[variant * 2 + 1] + (*(Desc **)(base + 0x7828))->f14 * 20) {
        if (frame == 0x50) {
            if (variant == 0) {
                _Func_80bd7dc(0x86);
            } else {
                _Func_80bd7dc(0x85);
            }
        }
        InitMatrixStack();
        MatrixSetLook(view, view + 0xc);
        {
            int *q = (int *)(base + (0xd3 << 7));
            int ang = frame << 10;
            int c = 0x80;
            j = 0;
            c <<= 13;
            do {
                j++;
                *q++ = (c - (sin(ang) << 4)) >> 10;
                ang += 0x400;
            } while (j != 0xa0);
        }
        kk = 0;
        if ((*(Desc **)(base + 0x7828))->f14 != 0) {
            int a;
            int io;
            int boff;
            register int base2 __asm__("r11");
            base2 = 0;
            do {
                int hh;
                Desc **s = (Desc **)(base + 0x7828);
                register int *ab __asm__("r9");
                a = kk * 5;
                io = 0x24 + kk * 2;
                boff = kk * (0xe0 << 4);
                ab = (int *)*(int *)_GetBattleActor(*(short *)((char *)*s + io));
                hh = _Func_80b8530(*(short *)((char *)*s + io)) / 2;
                if (frame == base2 + 0x47) {
                    if (variant == 0) {
                        _PlaySound(0x86);
                    } else {
                        _PlaySound(0x85);
                    }
                }
                if (frame == base2 + 0x46) {
                    Func_80d6888(*(short *)((char *)*(Desc **)(base + 0x7828) + io),
                                 7, 5, kk, 0x1a);
                }
                if (frame > base2 && Lee090[variant * 2] != 0) {
                    int a2 = a;
                    int b2 = base2;
                    register vec3_t *pp __asm__("r8");
                    Part *p;
                    j = 0;
                    pp = &pos;
                    p = (Part *)(gBuffer + boff);
                    do {
                        if (frame > (a2 * 2 + j) * 2 && p->life >= 0) {
                            int px;
                            int m;
                            Func_80e3944(p, pp);
                            px = pp->x >> 1;
                            pp->x = px;
                            m = j % 3;

                            ((DrawFn)fns[0])(ctx, base + (m * 5 << 7), px - 0xa,
                                             pp->y - 0x10, 0x14, 0x20);
                            Func_80e38b8(p, 0x3e, 0);
                            if (frame > b2 + j + 0x1e) {
                                int dx = ab[2] - p->x;
                                int dy = ab[3] + hh - p->y;
                                int dz = ab[4] - p->z;
                                p->dx += dx >> 9;
                                p->dy += dy >> 9;
                                p->dz += dz >> 9;
                                if ((unsigned)((dx >> 9) + 0xfff) <= 0x1ffe
                                    && (unsigned)((dz >> 9) + 0xfff) <= 0x1ffe) {
                                    p->life = -1;
                                }
                            }
                        }
                        j++;
                        p++;
                    } while (j != Lee090[variant * 2]);
                }
                kk++;
                base2 += 0x14;
            } while (kk != (*(Desc **)(base + 0x7828))->f14);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Func_80dbb9c);
    AnimEnd();
}
