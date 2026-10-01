/* BaseAnim_Tackle -- NON-MATCHING, 8 ENCODINGS OF 402.  SIZE EXACT (916 bytes),
 * INSTRUCTION COUNT EXACT (402), FRAME EXACT (`sub sp, #0x48`), RELOCATIONS
 * EXACT (objcmp prints no RELOCATIONS line).  ONE function, no .rodata --
 * CONVERTS WHOLE when it lands, no split, no data work.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/dfa18_Tackle.c \
 *     asm/rom_c9000/rom_dfa18_c_c_c_c_a.s --func BaseAnim_Tackle
 *   XX ENCODINGS differ in 8 place(s) (ref 402, ours 402)
 *      first at index 60: ref 33bc  ours 6812
 *
 * BATCH 315 RE-OPENED THIS PARK'S "closed" MARK.  OUTCOME: ONE WINDOW IS CLOSED
 * WITH A STRUCTURAL ARGUMENT, THE OTHER IS OPEN WITH A NAMED MECHANISM AND A
 * CONCRETE NEXT STEP.  THE PARK'S DESCRIPTION OF BOTH WAS WRONG IN THE SAME WAY:
 * IT NAMED THE COMPETING INSNS WITHOUT READING THE DEPENDENCE TABLE.
 *
 * NOT POOL-INFLATED.  Every differing index was listed in batch 315, not just
 * the first.  All eight are real instructions -- no `.word` among them -- so the
 * figure is 8 of code:
 *   idx 60  ref 33bc adds | ours 6812 ldr      idx 61  ref 6812 ldr | ours 9206 str
 *   idx 62  ref 681b ldr  | ours 33bc adds     idx 63  ref 9206 str | ours 681b ldr
 *   idx 64  ref 9307 str  | ours 9904 ldr      idx 65  ref 9904 ldr | ours 9307 str
 *   idx 223 ref 9c06 ldr  | ours 9808 ldr      idx 225 ref 9808 ldr | ours 9c06 ldr
 *
 * ========== WINDOW @58 (SIX ENCODINGS) -- CLOSED, AND SHARPER THAN BEFORE ==========
 * ref   adds r2,#184 / adds r3,#188 / ldr r2,[r2] / ldr r3,[r3] /
 *       str r2,[sp,#24] / str r3,[sp,#28] / ldr r1,[sp,#16]
 * ours  adds r2,#184 / ldr r2,[r2] / str r2,[sp,#24] / adds r3,#188 /
 *       ldr r3,[r3] / ldr r1,[sp,#16] / str r3,[sp,#28]
 *
 * From .23.sched2 (-fsched-verbose=6), the block's dependence table:
 *      insn  code  dep  prio  cost   INSN_DEPEND
 *       114   173   0    16    2      188 138 120 1008
 *      1008   173   1    14    1      188 120 117
 *       117     5   1    13    1      188 123           (adds r2,#184)
 *       120     5   2     9    1      188 126           (adds r3,#188)
 *       123   173   1    12    2      188 138 135 1014 1011   (ldr r2,[r2])
 *      1011   173   1    10    2      188 174 150 138 135 126 (str r2,[sp,#24])
 *       126   173   2     8    2      188 138 137 1014        (ldr r3,[r3])
 *      1014   173   2     6    2      188 174 150 138 137     (str r3,[sp,#28])
 *
 * WHICH KEY ACTUALLY DECIDES: PRIORITY, NOT A TIE.  After insn 117 is scheduled
 * the choice is insn 123 (prio 12) against insn 120 (prio 9) and 123 wins at
 * rung 1 of rank_for_schedule.  BOTH COMPETITORS ARE SOURCE-LEVEL INSNS -- a
 * `ldr` of a struct field and an address add -- NOT the reload-generated spill
 * stores this park named.  Strike "the competing insns are RELOAD-GENERATED
 * SPILL STORES, which no source statement can precede" as a description of WHICH
 * insns compete.
 *
 * AND THE ROM'S ORDER IS NOT SCHEDULABLE AT ALL, WHICH IS THE REAL CLOSURE.
 * Read insn 1011's dependents: THEY INCLUDE INSN 126.  insn 1011 stores to
 * `(mem (plus (reg sp) 24))` with ALIAS SET 0 -- a reload spill slot, and set 0
 * CONFLICTS WITH EVERYTHING -- while insn 126 loads `(mem (reg r3) 19)`.
 * true_dependence therefore holds, so sched-deps emits a TRUE dependence
 * 1011 -> 126.  The ROM's order puts 126 BEFORE 1011.  SCHED2 CANNOT MOVE AN
 * INSN ABOVE ITS OWN PRODUCER, so no ready-list ranking, no priority change and
 * no pin can produce the ROM's grouping from this insn chain.  The ROM's shape
 * requires BOTH LOADS TO PRECEDE EITHER STORE IN THE CHAIN, and the chain order
 * is reload's: an output reload is emitted immediately after the insn that
 * defines the spilled value, so `ldr r2,[r2]` is always followed at once by
 * `str r2,[sp,#24]`.  THE BLOCKER IS RELOAD'S SPILL-STORE PLACEMENT, ONE PASS
 * EARLIER THAN sched2, and sched2 is merely downstream of it.  The brief's
 * alias-set lever is admissible here (there ARE MEMs in the window) but it
 * cannot help: the conflicting store is already in alias set 0, the widest
 * possible, and nothing in C narrows a reload spill slot.
 *   MEASURED INERT at 8, do not re-run: pinned r2/r3 on the two LOADED VALUES;
 *   pinned r2/r3 on the two POINTERS q0p/q1p; dropping the q0p/q1p locals.
 *   MEASURED WORSE: swapping the d0/d1 assignment order (11); swapping the
 *   q0p/q1p assignment order (11).
 *   The only route left is a source shape in which d0 and d1 are NOT both
 *   spilled at their defs -- and the ROM does spill both, so that route is
 *   almost certainly closed too.  Do not spend another brief on this window
 *   without first showing a chain in which 126 precedes 1011.
 *
 * ========== WINDOW @224 (TWO ENCODINGS) -- OPEN.  IT IS A LUID TIE. ==========
 * ref   ldr r4,[sp,#24] / mov r1,r9 / ldr r0,[sp,#32]
 * ours  ldr r0,[sp,#32] / mov r1,r9 / ldr r4,[sp,#24]
 * -- two spill reloads swapped around a matching `mov r1,r9`, and the other
 * three `_call_via_r4` sites match.  From .23.sched2:
 *      insn  code  dep  prio  cost   INSN_DEPEND
 *      1083   173   2    36    1      586 545 541
 *       541   173   4    35    2      586 550 545
 *       543   173   3    35    2      586 1089 550    (ldr r0,[sp,#32])
 *      1086   173   3    35    2      586 1095 550    (ldr r4,[sp,#24])
 * PRIORITY TIES AT 35 AND THE DEPENDENT COUNT TIES AT 3, so rank_for_schedule
 * falls through rung 4 (both relate identically to the last-scheduled insn) to
 * rung 6, INSN_LUID -- and LUID(543) < LUID(1086) because reload emitted the r0
 * input reload first.  THIS PARK CALLED IT "same reload-generated problem" as
 * @58.  IT IS NOT: @58 is blocked by a dependence that forbids the ROM's order,
 * @224 is a 3-3 TIE WITH NOTHING FORBIDDING IT.
 * AND BOTH COMPETITORS ARE MEM LOADS, so the batch-315 alias-set dependent-count
 * lever IS in scope in the ADDING direction: give insn 1086 a FOURTH dependent,
 * or take one off insn 543, and 1086 wins at rung 5 before LUID is reached.
 * 1086's dependents are {586, 1095, 550} -- 1095 is a later WRITE of r4, 550 the
 * indirect call; 543's are {586, 1089, 550}, 1089 being a later write of r0.
 * NEXT STEP: find a source shape that adds one later consumer or writer of the
 * r4 carrier inside this block (or removes 543's).  The coordinator's rule
 * applies -- an aliasing store must be LATER IN THE CHAIN than the load it is
 * meant to constrain.
 *   MEASURED INERT at 8: a pinned r4 `f0 = d0;` statement before the call.
 *   MEASURED WORSE: pinned r4 + pinned r0 together (14); a pinned r1
 *   `b1 = base;` statement (16).
 *
 * ============ BATCH 315: EVERY `extern void` RE-SWEPT FROM THIS BASELINE ============
 * All TWENTY void callees swept to `extern int` (tools/sweep_variants.py).
 * INERT at 8: AnimStart, AnimEnd, Func_8001af8, Func_80d6888, Func_80df90c,
 * Func_80e38b8, GetBattleActorPos3, InitMatrixStack, MatrixSetLook, StartTask,
 * StopTask, Task_BlitAnim, WaitFrames, _Func_80bd7dc,
 * _SetBattleActorKnockback, gfree.  WORSE: Func_80cd52c 10,
 * UpdateScreenShake 10, Func_80df9d0 11, LoadVFXFile 15.
 * So the callee-return-type lever is EXHAUSTED on this function.
 *
 * ============ THE LEVER THAT TOOK THIS PARK 12 -> 8, unchanged ============
 *     WHERE A sched2 WINDOW IS A PERMUTATION AGAINST A COMPILER-GENERATED
 *     OPERAND, GIVE THAT OPERAND A SOURCE STATEMENT BY PINNING IT TO THE HARD
 *     REGISTER THE ROM USES.
 *   @75  CLOSED (12 -> 10).  `{ register unsigned char *b0 __asm__("r0");
 *        b0 = base; Func_80df9d0(b0, gBuffer, 0x28, arg2); }`
 *   @305 CLOSED (10 -> 8).  `register vec3_t *pp __asm__("r7")` declared in the
 *        j-loop block, assigned `pp = &pos;` BETWEEN `j = 0;` and
 *        `p = (Part *)(base + (0xe1 << 7));`, then passed.  The PIN is what
 *        works: an unpinned `vec3_t *pp = &pos` local measures 148 and 4 bytes
 *        larger, because unpinned it takes a spill slot.
 * The same r0 edit landed Anim_Vine, so this is bank-wide.
 *
 * ============ THE DECLARATION-ORDER AND PIN LEVERS, all unchanged ============
 * THE ROM'S STACK LAYOUT TELLS YOU THE SOURCE'S DECLARATION ORDER DIRECTLY.  The
 * frame grows downward, so declared ARRAYS get the high offsets in REVERSE
 * declaration order and SPILLED SCALARS then fill downward in ASCENDING PSEUDO
 * NUMBER, i.e. declaration order.  Reading the ROM's slots high to low gives
 * `ctx(0x20), d1(0x1c), d0(0x18), view(0x14), gfx(0x10), hitp(0x0c), slot(0x08)`
 * -- so declare `ctx, d1, d0, view, gfx, hitp, slot`, with `d1` BEFORE `d0` even
 * though `d0` is used first.  One reorder took 204 -> 147.  Unspilled locals
 * consume a pseudo but no slot, so they can sit anywhere.
 * A BLOCK-SCOPED DECLARATION GETS A LATER PSEUDO NUMBER THAN A COMPILER TEMP and
 * therefore a lower spill slot: moving `Desc **slot` into a block opened AFTER
 * the `&hit` statement made `slot` the later pseudo, 37 -> 32.
 * "ASSIGN THE `base + K` POINTER LAST" is a repeatable statement-order lever and
 * was worth 26 -> 12 on its own: `j = 0` before `p = base + (0xe1<<7)`
 * (26 -> 23); `frame = 0` before `slot = base + 0x7828` (20 -> 15); `i = 0`
 * before `p = ...` (15 -> 14); a named `msk = 0xff` between them (14 -> 12).
 * `&x` PASSED DIRECTLY AS A CALL ARGUMENT, WITH THE POINTER LOCAL ASSIGNED
 * AFTERWARDS, IS A DIFFERENT SHAPE from passing the local: `f(..., &hit);
 * hitp = &hit;` gives compute-into-reg / copy-to-arg / store-to-slot, worth
 * 46 -> 37.
 * `base` IS r9 HERE and the pin is load-bearing (removing it costs 47 -> 224);
 * also pin the frame counter to r11 and the inner particle counter to r8.
 * A PINNED CALL-CLOBBERED REGISTER BREAKS cse1's CONSTANT SHARING, because
 * cse1's `invalidate_for_call` kills the equivalence at the intervening call --
 * `{ register int k3 __asm__("r3"); k3 = 0x7828; ... }` measured 47 -> 46 and
 * produced the ROM's two separate pool loads from one word; the same form works
 * for a SYMBOL in a register-offset load (`register char *tb __asm__("r4")` for
 * Data_ede48, 23 -> 20).
 * WRITE DESTRUCTIVE SHIFTS AS SEPARATE STATEMENTS: `sz >>= 4; sz += 2;` gives
 * the ROM's `asr r5,#4 / add r5,#2`.
 * NAMING ONE STRUCT FIELD INTO A LOCAL BEFORE A 6-ARGUMENT INDIRECT CALL was
 * worth 57 -> 47 (`int hx = hitp->x;`); naming a SECOND field in the same call
 * was 57 -> 114.  APPLY ONE FIELD AT A TIME.
 * A CONSTANT INDEX INTO A DATA SYMBOL GETS FOLDED INTO THE POOL WORD AND objcmp
 * CANNOT SEE IT: `(char *)Data_ede48 + (h - 2)` emitted a WRONG ADDEND on an
 * R_ARM_ABS32 while every instruction read correctly.  Hoist the index to a
 * local (`ix = h - 2;`).  A REAL objcmp BLIND SPOT.
 * Tackle dispatches on `variant` with a `switch` + BARE `default:` -- a bare
 * `default:` is what suppresses the jump table, the OPPOSITE of
 * Anim_UnleashIntro's recorded lever.  Read the branch polarity per function.
 * -fno-schedule-insns2 is 266, so sched2 is required.
 *
 * MEASURED NEGATIVES, do not re-run: unpinned `base` 224; `hitp` hoisted to the
 * top 367; a region-C `Desc *` named local 229; `slot` reused across regions C
 * and D 234; `slot` assigned before region C 229; `slot` assigned inside the loop
 * body 238; no `slot` local at all 110 at frame 0x44 (the address is SUNK INTO
 * THE LOOP BODY, not hoisted -- so it was never a "two pool loads" route); a
 * named `int co = 0x7828` inert; `ix + (char *)Data_ede48` inert; dropping the
 * q0p/q1p address locals inert; an r4 pin on `slot`'s constant inert; an r3 pin
 * on `frame = 0`'s zero inert; a `char *tbl` table local inert; an explicit
 * `vec3_t *pp = &pos` local 148 with size 4 larger; `bp = base` before
 * Func_80df9d0 310.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
 * Progression: 355 -> 311 -> 204 -> 147 -> 57 -> 47 -> 12 -> 8.
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
extern unsigned char gPtrs[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void Func_80df9d0(void *a, void *b, int c, int d);
extern void Func_80df90c(int a, int b, int c);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern int Random(void);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int v);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(void *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void BaseAnim_Tackle(void *context, int variant)
{
    register unsigned char *base __asm__("r9");
    void *ctx;
    DrawFn d1;
    DrawFn d0;
    char *view;
    unsigned char *gfx;
    vec3_t *hitp;
    Desc **slotA;
    unsigned char *pt;
    DrawFn *q0p;
    DrawFn *q1p;
    unsigned char *data;
    CopyFn copy;
    int fid;
    int arg;
    int arg2;
    register int frame __asm__("r11");
    vec3_t hit;
    vec3_t apos;
    vec3_t pos;

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    gfx = (unsigned char *)((char **)&iwram_3001eec)[2];
    slotA = (Desc **)(base + 0x7828);
    *slotA = (Desc *)context;
    AnimStart(0);
    if ((*slotA)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 0xb, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, 2);
    }
    pt = gPtrs;
    q0p = (DrawFn *)(pt + 0xb8);
    q1p = (DrawFn *)(pt + 0xbc);
    d0 = *q0p;
    d1 = *q1p;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_99, base, 1, 0);
    arg2 = 0x90;
    arg2 <<= 1;
    {
        register unsigned char *b0 __asm__("r0");
        b0 = base;
        Func_80df9d0(b0, gBuffer, 0x28, arg2);
    }
    LoadVFXFile(FILE_bd, base, 1, 1);
    switch (variant) {
    case 0:
        fid = FILE_c2;
        break;
    case 1:
        fid = FILE_b9;
        break;
    case 2:
        fid = FILE_bb;
        break;
    default:
        fid = FILE_c0;
        break;
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
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x4b;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    {
        Desc **s2 = (Desc **)(base + 0x7828);
        Func_80df90c((*s2)->f8, (*s2)->ids[0], 0xa);
        {
            int *ab = (int *)_GetBattleActor((*s2)->ids[0]);
            int *src = (int *)*ab;
            Part *p;
            int msk;
            register int i __asm__("r8");
            i = 0;
            msk = 0xff;
            p = (Part *)(base + (0xe1 << 7));
            do {
            p->x = src[2];
            p->y = src[3] + (0xa0 << 12);
            p->z = src[4];
            p->dx = (Random() & 0x1ff) << 11;
            p->dy = ((Random() & msk) - 0x40) << 11;
            p->dz = ((Random() & msk) - 0x80) << 11;
            if (p->x > 0) {
                p->dx = -p->dx;
            }
            p->life = i / 2 + 0x10;
                i++;
                p++;
            } while (i != 0x40);
        }
    }
    {
        register int k3 __asm__("r3");
        k3 = 0x7828;
        GetBattleActorPos3((*(Desc **)(base + k3))->ids[0], &hit);
    }
    hitp = &hit;
    {
    Desc **slot;
    frame = 0;
    slot = (Desc **)(base + 0x7828);
    do {
        if (frame <= 0xe) {
            GetBattleActorPos3((*slot)->f8, &apos);
            d0(ctx, base, apos.x / 2 - 0x10, apos.y - 0x30, 0x28, 0x20);
            d1(ctx, base, apos.x / 2 - 0x10, apos.y - 0x10, 0x28, 0x20);
        }
        if (frame == 0xa) {
            Func_80d6888((*slot)->ids[0], 7, 5, 0, 8);
            _SetBattleActorKnockback((*slot)->ids[0], 4);
            _Func_80bd7dc(0x86);
            *(int *)(base + 0x77a8) = 8;
        }
        if (frame >= 8 && frame <= 0x13) {
            int k = (frame - 8) / 2;
            int hx = hitp->x;
            d0(ctx, gBuffer + k * 0x3c0, hx / 2 - 0x10, apos.y - 0x28, 0x14, 0x30);
        }
        if (frame >= 8 && frame <= 0x3f) {
            Part *p;
            int j;
            register vec3_t *pp __asm__("r7");
            InitMatrixStack();
            MatrixSetLook(view, view + 0xc);
            j = 0;
            pp = &pos;
            p = (Part *)(base + (0xe1 << 7));
            do {
                int sz = p->life;
                if (sz > 0) {
                    int h;
                    int ix;
                    Func_80e3944(p, pp);
                    sz >>= 4;
                    sz += 2;
                    h = sz * 2;
                    ix = h - 2;
                    pos.x = pos.x >> 1;
                    {
                    register char *tb __asm__("r4");
                    tb = (char *)Data_ede48;
                    d0(ctx, gfx + *(unsigned short *)(tb + ix),
                       pos.x - sz / 2, pos.y - sz, sz, h);
                    }
                    Func_80e38b8(p, 0x3c, -0x200);
                    p->life = p->life - 1;
                }
                j++;
                p++;
            } while (j != 0x40);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x3c);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
