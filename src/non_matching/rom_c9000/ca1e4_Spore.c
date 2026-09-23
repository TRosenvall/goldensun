/* BaseAnim_Spore -- NON-MATCHING, and the honest state is "TWO INSTRUCTIONS SHORT",
 * not the difference count.  objcmp: 287 encodings of 398, size 892 against the ROM's
 * 896, 396 instructions against 398.  379 instructions in the reference.
 *
 * **DO NOT QUOTE THE 287 AS A DISTANCE.**  Once the stream shifts by one instruction
 * every `ldr [pc,#N]` counts, and this reference keeps its pool INSIDE the function.
 * The real state: THE ENTIRE PROLOGUE THROUGH `StartTask` IS BYTE-EXACT, and the
 * particle-init loop, the frame loop, the z-clamp, the division-by-64 and the draw
 * call are all structurally exact.  287 is STABLE AT EXACTLY THAT VALUE ACROSS FIVE
 * DIFFERENT SOURCE VARIANTS -- i.e. the count is not responding to anything, which is
 * itself the signal that it is measuring the shift and not the defect.
 *
 * FRAME EXACT (`sub sp, #0x44`).  THE 41 RELOCATION SYMBOLS ARE IDENTICAL IN THE SAME
 * ORDER -- only their offsets shift, so the RELOCATIONS-differ line is a consequence
 * of the two-instruction deficit and not a symbol problem.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/ca1e4_Spore.c \
 *     asm/rom_c9000/rom_ca1e4_c_c.s
 * ONE function, no .rodata -- CONVERTS WHOLE when it lands, no split, no data work.
 *
 * Parameters are confirmed by the landed wrappers src/rom_c9000/rom_ca1e4_b.c
 * (Anim_SleepStar -> variant 1) and rom_ca1e4_c_b.c (Anim_SoothingStar -> variant 0),
 * so `(context, variant)` is published rather than guessed.
 *
 * ================================================================
 * THE FINDING WORTH CARRYING: A DEAD STORE THE ROM KEEPS, AND ONLY AN ARRAY
 * REPRODUCES IT
 * ================================================================
 *
 * `[sp,#0x1c]` is WRITTEN ONCE -- the gPtrs+0xbc blit pointer -- AND NEVER READ.
 * Written as two scalars, gcc-2.96's flow analysis DELETES the store, the frame comes
 * out 0x40, and every offset above `view` is four bytes low (360 differing).  Written
 * as an array:
 *
 *     DrawFn fns[2];
 *     fns[0] = *(DrawFn *)(pt + 0xb8);   // the one that is called
 *     fns[1] = *(DrawFn *)(pt + 0xbc);   // dead, but an array element cannot be deleted
 *
 * the store SURVIVES, `fns` lands at 0x18/0x1c, and THE WHOLE PROLOGUE BECOMES
 * BYTE-EXACT.  Two other spellings failed: a `volatile` load keeps the load but gives
 * the pseudo no slot (frame stays 0x40), and an empty `__asm__` input constraint
 * likewise.
 *
 * **CHECK EVERY ENTRY POINT IN THIS BANK FOR A WRITTEN-NEVER-READ SPILL SLOT BEFORE
 * CONCLUDING THE DECLARATION LIST IS WRONG.**  A frame that is exactly four bytes
 * short with everything above one slot displaced is this shape.
 *
 * ================================================================
 * BLOCKER CLASS: register allocation -- TWO CALLEE-SAVED COPIES THE ROM MAKES AND gcc
 * DOES NOT
 * ================================================================
 *
 * Located by walking the `bl` anchors: ours is +1 instruction before the first
 * `Random` and -1 in each of the two tail arms.
 *
 * THE TAIL ARMS, both.  ROM: `add r2,r3,r4 / ldr r3,[r2] / ldr r3,[r3,#0x14] / ... /
 * beq / adds r5,r2,#0` -- it computes `base+0x7828` into a CALL-CLOBBERED register for
 * the count guard and then COPIES IT TO A CALLEE-SAVED ONE for the loop.  Ours
 * computes it straight into r5 and has no copy.  Splitting the guard's pointer from
 * the loop's pointer in the source (`Desc **s0 = ...; if ((*s0)->f14) { Desc **slot =
 * s0; ... }`) was INERT -- gcc coalesces them.
 *
 * THE +1 WINDOW.  ROM `ldr r5,=0x3ff / ands r5,r0` (two) against ours
 * `ldr r3,=0x3ff / adds r5,r0,#0 / ands r5,r3` (three).  The ROM makes the MASK's
 * register the destination so no copy of r0 is needed.  `0x3ff & Random()` and
 * `m = Random(); m &= 0x3ff;` both INERT.
 *
 * MULTIPLY OPERAND ORDER, two sites, 4 encodings.  ROM `adds r3,r5,#0 / muls r3,r0`
 * (operand 1 = `m`) against ours `adds r3,r0,#0 / muls r3,r5`.  `sin(ang) * m` INERT;
 * a named `sv = sin(ang)` local INERT.  `commutative_operand_precedence`
 * canonicalises it away from the source, so this is NOT the recorded
 * write-the-variable-first lever -- that one works where canonicalisation does not
 * reach.
 *
 * All three are the same shape: THE ROM SPENDS A COPY TO GET A VALUE INTO A
 * CALLEE-SAVED REGISTER AND gcc DOES NOT.  Compare the REDUNDANT-COPY PRESSURE class
 * in src/non_matching/rom_b5000/80ba2c0.c, where nine spellings were all coalesced or
 * reverted -- same family, different bank.
 *
 * This is ONE REGISTER-ALLOCATION DECISION from exact, not a rewrite.
 *
 * The bank's levers all applied and are not repeated here -- see
 * src/non_matching/rom_c9000/dfa18_Tackle.c for the declaration-order rule (the ROM's
 * spill map IS the source's declaration order), the `base` high-register pin, the
 * pinned-call-clobbered-register CSE break, and the "assign the base+K pointer last"
 * statement-order lever.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
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
extern Part gBuffer[];
extern unsigned char gPtrs[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _Func_80bd7dc(int a);
extern void _PlaySound(int id);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void BaseAnim_Spore(void *context, int variant)
{
    unsigned char *base;
    void *ctx;
    DrawFn fns[2];
    char *view;
    unsigned char *gfx;
    int *src;
    char *look2;
    unsigned char *pt;
    unsigned char *data;
    CopyFn copy;
    int fid;
    int arg;
    int two;
    register int frame __asm__("r11");
    register vec3_t *vp __asm__("r9");
    vec3_t v;
    vec3_t pos;

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    gfx = (unsigned char *)((char **)&iwram_3001eec)[2];
    *(void **)(base + 0x7828) = context;
    if (variant == 0) {
        AnimStart(0);
    } else {
        AnimStart(1);
    }
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    pt = gPtrs;
    fns[0] = *(DrawFn *)(pt + 0xb8);
    BuildDraw2DFuncEx(0x2f, 7, 7, 0xb, two);
    fns[1] = *(DrawFn *)(pt + 0xbc);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    if (variant == 0) {
        fid = FILE_7c;
    } else {
        fid = FILE_7b;
    }
    data = GetFile(fid);
    {
        PIN3;
        q1 = (int)data;
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    src = *(int **)_GetBattleActor((*(Desc **)(base + 0x7828))->f8);
    {
        Part *p = gBuffer;
        register int i __asm__("r8");
        int m;
        int ang;
        i = 0;
        do {
            m = Random() & 0x3ff;
            ang = Random() & 0xffff;
            p->x = src[2];
            p->y = src[3] + (0xa0 << 11);
            p->z = src[4];
            m += 0x20;
            p->dx = (m * sin(ang)) >> 8;
            p->dy = ((Random() & 0xff) - 0x20) << 9;
            p->dz = -(m * cos(ang) * 2) >> 8;
            p->life = (Random() & 0x1f) + 0x30;
            if (variant == 0) {
                p->dx = p->dx / 2;
                p->dz = p->dz / 2;
            }
            i++;
            p++;
        } while (i != 0x100);
    }
    look2 = view + 0xc;
    frame = 0;
    vp = &v;
    do {
        InitMatrixStack();
        MatrixSetLook(view, look2);
        {
            Part *q = gBuffer;
            register int k __asm__("r8");
            k = 0;
            do {
                if (frame >= k / 0x20 * 8) {
                    int life = q->life;
                    if (life >= 0) {
                        int z;
                        int sz;
                        int h;
                        int ix;
                        vp->x = q->x + (sin((k * 4 + life) << 10) << 4);
                        vp->y = q->y;
                        vp->z = q->z;
                        Func_80e3944(vp, &pos);
                        pos.x = pos.x >> 1;
                        z = pos.z;
                        if (z <= 0x139) {
                            z = 0x13a;
                            pos.z = z;
                        }
                        if (z > 0x27a) {
                            pos.z = 0x27a;
                            z = 0x27a;
                        }
                        sz = 6 - (z - 0x13a) / 0x40;
                        h = sz * 2;
                        ix = h - 2;
                        fns[0](ctx, gfx + *(unsigned short *)((char *)Data_ede48 + ix),
                           pos.x - sz / 2, pos.y - sz, sz, h);
                        Func_80e38b8(q, 0x3e, 0x400);
                        if (variant == 1) {
                            if (src[2] < 0) {
                                q->dx = q->dx + 0x2000;
                            } else {
                                q->dx = q->dx - 0x2000;
                            }
                        }
                        q->life = q->life - 1;
                    }
                }
                k++;
                q++;
            } while (k != 0x80);
        }
        if (variant == 1) {
            Desc **s0 = (Desc **)(base + 0x7828);
            register int n __asm__("r8");
            n = 0;
            if ((*s0)->f14 != 0) {
                Desc **slot = s0;
                int io = 0x24;
                int th = 0x30;
                do {
                    if (frame == th) {
                        _Func_80bd7dc(-1);
                        Func_80d6888(*(short *)((char *)*slot + io), 7, 5, n, 8);
                    }
                    n++;
                    io += 2;
                    th += 8;
                } while (n != (*slot)->f14);
            }
        } else {
            Desc **s0 = (Desc **)(base + 0x7828);
            register int n __asm__("r8");
            n = 0;
            if ((*s0)->f14 != 0) {
                Desc **slot = s0;
                int io = 0x24;
                int th = 0x30;
                do {
                    if (frame == th) {
                        _PlaySound(0x7e);
                        _Func_80bd7dc(-1);
                        Func_80d6888(*(short *)((char *)*slot + io), 7, -1, n, 8);
                    }
                    n++;
                    io += 2;
                    th += 8;
                } while (n != (*slot)->f14);
            }
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x80);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
