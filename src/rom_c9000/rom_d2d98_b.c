/* Anim_Froth -- 0x080d33c0, 510 instructions.  MATCHING.  LANDED IN BATCH 314,
 * BRIEF B.  Was parked at "4 of 529, the argument-setup order of both StartTask
 * calls, an EXPAND-ORDER residue which no source spelling of THIS call reaches".
 * THE RESIDUE WAS NOT EXPAND ORDER; see THE CORRECTION below.
 *
 * tools/objcmp.py, PRODUCTION FLAGS (the tree default for this path; no
 * per-file Makefile override applies to this stem):
 *   OK Anim_Froth -- 1172 bytes, 529 encodings and 47 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c9000/rom_d2d98_b.c asm/rom_c9000/rom_d2d98.s --func Anim_Froth
 *
 * SPLIT SHAPE: TEXT-ONLY, AND THE PARK'S EXPORT LIST WAS INCOMPLETE.
 * asm/rom_c9000/rom_d2d98.s holds SIX functions (Anim_Nereid, Anim_Froth,
 * Anim_Whirlwind, Anim_Prism, ColorCycleVFXPalette, Anim_Plasma) plus a .rodata
 * tail, so converting one needs a TEXT/DATA SPLIT.  The park recorded only
 *     .global .Lee1c4
 * on the grounds that Anim_Froth reads that label and nothing else.  That is
 * true of Anim_Froth, but `tools/split_s.py --dry-run asm/rom_c9000/rom_d2d98.s
 * Anim_Froth` splits THREE ways -- _a (Anim_Nereid), _b (Anim_Froth), _c (the
 * rest plus .rodata) -- and REFUSES with THREE crossing labels:
 *     .global .Lee1ac      (read by _a / Anim_Nereid)
 *     .global .Lee1b4      (read by _a / Anim_Nereid)
 *     .global .Lee1c4      (read by _b / Anim_Froth)
 * All three exports are required before the split, and a `.global` emits no
 * bytes.  Export and verify `make compare` BEFORE splitting, so the two changes
 * stay separable.  .Lee1c4 is 0xee1c4..0xee1ca, SIX bytes, indexed as unsigned
 * char at f18*2 and f18*2+1 -- a table of {count, frames} pairs.
 * tools/shimcount.py: CLEAN.  No pins, no shims, no fakematch.txt row.
 *
 * ================= WHAT LANDED IT: ONE WORD ==============================
 *
 *     extern int StartTask(void *fn, int arg);
 *
 * -- `int`, not `void`.  Nothing else in the file changed.
 *
 * ================= THE CORRECTION ========================================
 * THE CALLEE IS NOT VOID AND THIS TREE SAYS SO.  include/task.h declares
 *     s32 StartTask(taskfunc_t *task, u32 priority);
 * The park declared `extern void StartTask(void *fn, int arg);` locally and then
 * proved, from load_register_parameters' forward walk over args[], that r0 must
 * carry the lower INSN_LUID and that no spelling of the two arguments could
 * change it.  That proof is correct and it is beside the point: WITH A VOID
 * CALLEE THE TIE NEVER REACHES INSN_LUID.  A void call is `*call_insn` and
 * never SETS r0, so reg_last_sets[r0] still names the call's own r0 argument
 * fill, and the SECOND StartTask's `ldr r0,=Task_BlitAnim` takes a
 * REG_DEP_OUTPUT on the FIRST call's `ldr r0,=Task_SpinCamera`.  The r0 fill
 * therefore carries one more INSN_DEPEND entry than the r1 copy, and
 * rank_for_schedule's "more dependents wins" puts r0 first -- our order --
 * before LUID is ever consulted.  Declared `int`, the call is
 * `*call_value_insn`, it SETS r0, that output dependence attaches to the CALL,
 * the r0 fill drops to ONE dependent while the r1 copy keeps TWO (the call, and
 * the second site's own `mov r1,r5` output dep, which the call does NOT
 * intercept because a call_value sets r0 and not r1), and the r1 copy wins the
 * count.  Both sites flip together, which is exactly what was observed: 4 -> 0.
 *
 * The park's 21-spelling list contains three declarations of StartTask and
 * EVERY ONE OF THEM RETURNS void -- `void (*)(void)` arguments, `(int, int)`
 * with a cast, and `extern void StartTask();` with no parameter list.  The
 * return type was the one axis never varied.  THE CHEAP SWEEP THAT CATCHES
 * THIS: grep every park's `extern void F(...)` against include/ and against
 * `int F(` definitions under src/.  The sibling park
 * src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c_a.c (LANDED; was src/non_matching/ovl_77dd1c/2009b18.c) carried the identical error at
 * `__StartTask` and landed in the same batch on the same one word.
 *
 * AND IT EXPLAINS THE CONTROLLED EXPERIMENT THE PARK COULD NOT.  The park
 * noted that the landed Anim_Confuse next door has the SAME
 * `arg = 0x90; arg <<= 3;` idiom at two StartTask calls and that ITS reference
 * emits `ldr r0 / mov r1,r6` -- our old order -- byte-exact, and concluded that
 * "the two ROM functions differ in a way the source does not express".  They do
 * not: the dependent count depends on whether a SECOND call in the same block
 * re-writes the same argument register, and the two functions differ in that
 * and nothing else.  No dump comparison against Anim_Confuse is needed any
 * more; that open item is closed.
 *
 * ================= WHAT CLOSED 10 DOWN TO 4, AND IS STILL LOAD-BEARING ====
 * A COMMA-ASSIGNED INDEX AND TABLE PAIR IN THE OUTER LOOP CONDITION.  ONE
 * STATEMENT, SIX ENCODINGS.  The function loads .Lee1c4 at FOUR sites and the
 * reference's registers are r2 (outer entry guard), r4 (the
 * `frame == Lee1c4[...] - 0x10` test), r2 (inner tail), r2 (outer tail) -- four
 * independent rematerialisations whose reload picks had to be rotated.  Write
 * the OUTER while condition as
 *
 *     while (n = (*(State **)(base + 0x7828))->f18 * 2 + 1,
 *            tbl = Lee1c4,
 *            frame != tbl[n]) {
 *
 * with `unsigned char *tbl;` and `int n;`.
 *
 * (a) `tbl` MAKES THE OUTER CONDITION'S TWO COPIES ONE PSEUDO.  The condition is
 *     duplicated by duplicate_loop_exit_test into the entry guard and the loop
 *     bottom, and a source variable assigned inside it is ONE declared C
 *     variable, so gcc-2.96 -- one pseudo per variable, no SSA renaming -- gives
 *     the whole thing ONE hard register, r2, at both copies.  The assignment
 *     must be INSIDE the condition: a plain `tbl = Lee1c4;` before the loop is
 *     held in a callee-saved register instead and the two pool loads collapse
 *     (527, two instructions SHORT, 335 differ).  Re-assigning in the condition
 *     keeps a pool load at each copy, so the COUNT is preserved at 529.
 * (b) `n` KEEPS THE `+ 1` IN THE INDEX REGISTER.  With `tbl[... * 2 + 1]` and no
 *     `n`, gcc reassociates to `(tbl + idx) + 1` and folds the 1 into the load's
 *     immediate: `adds r3,r3,r2 / ldrb r3,[r3,#1]`.  The reference keeps the 1
 *     in the index and uses the register-offset form: `adds r3,#1 /
 *     ldrb r3,[r2,r3]`.  Thumb-1 `ldrb rd,[rn,rm]` has NO immediate field, so
 *     the two forms cost the same and gcc picks the immediate one whenever the
 *     base is a pointer VARIABLE.  Assigning the index to its own local puts the
 *     `+ 1` inside n's computation, where combine cannot reach it.
 *
 * THE COMMA ORDER IS LOAD-BEARING: `n` FIRST, `tbl` SECOND.
 *     n then tbl ......................  4 of 529   <- this file's old figure
 *     tbl then n ...................... 10 of 529
 *     tbl only, no n ..................  8 of 529
 *     n reusing a dead donor instead of its own local: `arg` 9, `two` 9,
 *                                       `mask` 458 (RELOC), `yb` 474 (+2 insns)
 * And the device must be applied to the OUTER condition only:
 *     inner condition too ............. 34 of 529
 *     inner condition only ............ 11 of 529
 *     both, incl. the -0x10 test ..... 335 of 529, 527 insns (2 SHORT)
 * `tbl`/`n` DECLARATION POSITION IS INERT -- no spill offset differs anywhere in
 * the function, so the frame map in lever (1) below is already exact.
 *
 * BOTH DIRECTIONS OF THE Anim_Fireball PLUS-CANONICALISATION LEVER ARE RULED
 * OUT here -- it looks identical on the surface and costs an instruction
 * whichever operand leads: `*(unsigned char *)(idx + (int)Lee1c4)` 528/352;
 * `*(unsigned char *)((int)Lee1c4 + idx)` 528/352; `*(Lee1c4 + idx)` at all
 * three sites 528/352; the same at the outer sites only 528/362.  There the two
 * PLUS operands were both pseudos needing reloads and the order decided the
 * pick; here the table pseudo is already where the reference has it (both emit
 * `ldrb r3,[rTable,rIndex]`) and the ARRAY form is what keeps it there.  Also
 * inert or worse: a named `unsigned char *tbl = Lee1c4;` held local (527,
 * 335/334 differ, either position); `extern const unsigned char Lee1c4[]`;
 * `unsigned char Lee1c4[][2]` with `[f18][0]`/`[f18][1]`; an explicit `int j`
 * stepped by 2 instead of `i * 2` (22 of 529).
 *
 * ================= `signed char *flags = gBuffer;` IS A MISSING QUANTITY ===
 * 62 SPAN TO 10.  With the four flag accesses written as `gBuffer[i]` the
 * candidate is ONE INSTRUCTION SHORT (527 against 529) and every scratch
 * register in the function is rotated: the reference puts the gBuffer address
 * in r4 or r0 and the copy of the counter in r0 or r1, and we put the address in
 * r2 or r3 -- once as `ldrsb r3,[r3,r0]`, reusing the destination as the base,
 * which the reference never does.  The missing instruction is a second
 * `mov r0,r8`: the reference copies the counter into r1 for the `ldrsb`, that r1
 * is then clobbered by `ldr r1,[r3,#0x14]`, and a fresh copy is needed for
 * __modsi3, while we had already put the counter in r0 and reload inherited it.
 * Naming the array pointer -- `signed char *flags; ... flags = gBuffer;` in the
 * init loop's preheader, with `flags[i]` at all four sites -- creates the pseudo
 * the reference's allocation was built around.  It is REMATERIALISED, not held:
 * all four accesses still load the symbol from the pool, so this is the
 * REG_EQUIV path and not a cached pointer.  Its DECLARATION POSITION is inert;
 * only its existence matters.
 *
 * ================= FIVE MORE LEVERS THAT PAID ============================
 * (1) DECLARATION ORDER IS THE FRAME MAP, worth 6 encodings.  `vec3_t v` is the
 *     only aggregate and takes 0x18..0x23; the three spilled scalars then
 *     descend in DECLARATION order -- ctx 0x14, fn1 0x10, fn0 0xc -- and the
 *     loop-2 induction variable reload takes 0x8.  Declaring the two blit
 *     pointers as `DrawFn fns[2]` puts fns[0] at 0x10 and ctx at 0xc, which is
 *     the reference's layout upside down.  Note fn1 is declared BEFORE fn0 even
 *     though fn0 is assigned first.
 * (2) A WALKING POINTER DERIVED INSIDE THE LOOP BODY, worth the position of two
 *     preheader blocks.  `Part *tgt = (Part *)(base + (0xe8 << 7)) + i;` and
 *     `Part *q = (Part *)(base + (0xe1 << 7)) + i;` written INSIDE the body let
 *     loop.c strength-reduce them, and a giv's initialiser is inserted AFTER the
 *     duplicated exit test -- which is where the reference has it.  A preheader
 *     assignment lands before the test.  (This works here and NOT in
 *     Anim_Fireball, whose base is a bare symbol; see that file's lever (2).)
 * (3) `int mask = 0xff;` AS A SOURCE LOCAL, ordered `i = 0; mask = 0xff;
 *     q = ...;`, worth 17 -> 10.  Written as two literal `Random() & 0xff` the
 *     constant is hoisted by loop.c and lands LAST in the preheader; a source
 *     local is materialised where the source puts it, which is second-to-last.
 *     `mask` is NOT reusable as a second quantity: as the `k` modulus it reads
 *     452 differ (+2 insns), as the 0x1f randomiser mask 468, as the
 *     screen-shake temp 470 (+2).
 * (4) TWO SEPARATE STORES FOR base+0x77ac, not a `?:`.  The reference computes
 *     the address in BOTH arms and cross-jumps only `str r3,[r2]`; a single
 *     store with a conditional value computes the address once.  Anim_Flare's
 *     rule.
 * (5) RE-DERIVE base+0x7828 EVERYWHERE; DO NOT NAME IT.  An explicit
 *     `State **sp3` for the inner-loop region costs an instruction (527, 322
 *     differ).  The reference holds it in r10 inside the inner loop because LICM
 *     hoists the address there, and re-derives it in the outer body and after
 *     the loop -- which is what the plain `(*(State **)(base + 0x7828))`
 *     spelling produces by itself.
 *
 * ALSO LOAD-BEARING: `(unsigned)q->t <= 0xf` around a SIGNED `q->t / 2`;
 * `q->t = q->t + 1` as a re-read; `i % f14` recomputed for each of the three
 * uses (the intervening calls clobber f14, so cse cannot share them) but shared
 * as `k` WITHIN the Func_80d6888 call, whose argument 4 IS k; `ax * 60 / 64`
 * for the reference's `lsl #4 / sub / lsl #2` chain; `v.x >>= 1` (a shift, not
 * `/ 2`); `Data`-free `if (v.z <= 0x9f)` and `if (v.z > 0x31f)` as two separate
 * ifs so cse feeds the second from the first's register; and Anim_Confuse's
 * `d0 = 0xa0; copy = Func_8001af8; p = data; data += 0x80; d0 <<= 19;` block
 * transplanted verbatim, pin-free.
 *
 * ================= MEASURED AGAINST THE StartTask RESIDUE AND STILL INERT ==
 * Kept so the spellings are not re-spent.  All of these read exactly 4 of 529
 * with a VOID declaration and are superseded by the int return:
 *   `arg = 0x90 << 3;` as one statement; `arg = 0x480;`; `arg *= 8;` and
 *   `arg = arg << 3;`; two separate `arg`/`arg2` locals; the shift inlined at
 *   both call sites with no local at all; `unsigned arg`; named `void *t1/t2`
 *   locals assigned immediately before each call; `&Task_SpinCamera`;
 *   `(void *)Task_SpinCamera`; the task symbols declared `extern char x[]` so
 *   the argument is an array decay; `StartTask((arg <<= 3, Task_SpinCamera),
 *   arg)`; and nine source positions of the `arg = 0x90; arg <<= 3;` pair.
 *   Flags: -fno-schedule-insns2 (204 differ), -fno-schedule-insns,
 *   -fno-cse-follow-jumps, -fno-peephole, -fno-delayed-branch (all 4);
 *   -fno-force-mem (37), -fno-rerun-cse-after-loop (324, +4 insns),
 *   -fno-expensive-optimizations (393, +6), -fno-caller-saves (389).
 *   -fno-schedule-insns is inert because sched1 DOES NOT RUN in this build.
 * MEASURED WORSE: the task locals assigned at the top of the function (532
 *   instructions, +3); reusing the counter/index local `n` as the task argument
 *   (8 of 529 -- it breaks the table allocation above); `two` reused for the
 *   base+(0xef<<7) store (531, +2 -- the reference emits a fresh `movs r3,#2`
 *   there, exactly as Anim_Whirlwind records); a shared `int one = 1;` for the
 *   three 1-stores (527, -2); reusing `yb`, `two` or `arg` as the screen-shake
 *   temp (470/531, and 59 differ with the POOL ORDER changed).
 * MEASURED INERT: a shared `int zero = 0;` local for the two base+0x77ac/0x77b0
 *   stores (cse already makes that one `movs r2,#0`); hoisting the inner loop's
 *   `k` to function scope; declaring `arg` first or swapping `arg` and `frame`.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern signed char gBuffer[];
extern unsigned char Lee1c4[] __asm__(".Lee1c4");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int *_GetBattleActor(int id);
extern int  _Func_80b8530(int id);
extern int  Random(void);
extern int StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Task_SpinCamera(void);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Froth(void *context)
{
    vec3_t v;
    void *ctx;
    DrawFn fn1;
    DrawFn fn0;
    void **g;
    void **pp;
    unsigned char *base;
    unsigned char *data;
    CopyFn copy;
    int *actor;
    Part *q;
    int yb;
    int mask;
    signed char *flags;
    int i;
    int two;
    int arg;
    int frame;
    unsigned char *tbl;
    int n;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    data = GetFile(FILE_cd);
    {
        unsigned char *p;
        int d0;
        d0 = 0xa0;
        copy = Func_8001af8;
        p = data;
        data += 0x80;
        d0 <<= 19;
        copy((volatile u16 *)d0, p, 0x80);
    }
    DecompressLZ(data, base);
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    fn0 = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, two);
    REG_BLDALPHA = 0xf0f;
    {
        int id = (*(State **)(base + 0x7828))->f8;
        fn1 = (DrawFn)g[8];
        actor = (int *)*_GetBattleActor(id);
    }
    yb = actor[3] + _Func_80b8530((*(State **)(base + 0x7828))->f8);
    i = 0;
    mask = 0xff;
    flags = gBuffer;
    q = (Part *)(base + (0xe1 << 7));
    do {
        q->x = actor[2];
        q->y = yb;
        q->z = actor[4];
        q->vx = (((Random() & mask) - 0x7f) << 16) >> 5;
        q->vy = (((Random() & 0x7f) - 0x10) << 16) >> 6;
        q->vz = (((Random() & mask) - 0x7f) << 16) >> 5;
        q->t = -1;
        flags[i] = 0;
        i++;
        q++;
    } while (i != 0x1e);
    i = 0;
    while (i != (*(State **)(base + 0x7828))->f14) {
        Part *tgt = (Part *)(base + (0xe8 << 7)) + i;
        int *t = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[i]);
        tgt->x = t[2];
        tgt->y = 0;
        tgt->z = t[4];
        i++;
    }
    *(int *)(base + 0x77ac) = 0;
    *(int *)(base + 0x77b0) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_SpinCamera, arg);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, arg);
    _PlaySound(0xa4);
    frame = 0;
    while (n = (*(State **)(base + 0x7828))->f18 * 2 + 1, tbl = Lee1c4, frame != tbl[n]) {
        void *cam;
        cam = iwram_3001e80;
        if ((unsigned)(frame - 0x11) <= 0x2e) {
            *(int *)(base + 0x77ac) = 0x180;
        } else {
            *(int *)(base + 0x77ac) = 0;
        }
        if (frame == Lee1c4[(*(State **)(base + 0x7828))->f18 * 2 + 1] - 0x10) {
            _Func_80bd7dc(0x84);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        i = 0;
        while (i != Lee1c4[(*(State **)(base + 0x7828))->f18 * 2]) {
            Part *q = (Part *)(base + (0xe1 << 7)) + i;
            if (frame > i * 2 && flags[i] == 0) {
                Func_80e3944((vec3_t *)q, &v);
                v.x >>= 1;
                if (v.z <= 0x9f) {
                    v.z = 0xa0;
                }
                if (v.z > 0x31f) {
                    v.z = 0x31f;
                }
                fn0(ctx, base + 0xc00, v.x - 6, v.y - 0xc, 0xc, 0x18);
                q->x += q->vx;
                q->y += q->vy;
                q->z += q->vz;
            }
            if (frame > i * 2 + 0x30 && flags[i] == 0) {
                Part *tgt = (Part *)(base + i % (*(State **)(base + 0x7828))->f14 * 0x1c
                                     + (0xe8 << 7));
                int ax, ay, az;
                ax = q->vx + ((tgt->x - q->x) >> 9);
                q->vx = ax;
                ay = q->vy + ((tgt->y - q->y) >> 9);
                q->vy = ay;
                az = q->vz + ((tgt->z - q->z) >> 9);
                q->vz = az;
                if (frame < i * 2 + 0x55) {
                    q->vx = ax * 60 / 64;
                    q->vy = ay * 60 / 64;
                    q->vz = az * 60 / 64;
                }
                if (q->y < 0) {
                    flags[i] = 1;
                    q->t = 0;
                    q->x = v.x;
                    q->y = v.y + (Random() & 0x1f) - 0x10;
                    {
                        int k = i % (*(State **)(base + 0x7828))->f14;
                        Func_80d6888((*(State **)(base + 0x7828))->ids[k], 7, 5, k, 4);
                    }
                    _SetBattleActorKnockback(
                        (*(State **)(base + 0x7828))
                            ->ids[i % (*(State **)(base + 0x7828))->f14], 0);
                    *(int *)(base + 0x77a8) = 4;
                    _PlaySound(0x84);
                }
            }
            if ((unsigned)q->t <= 0xf) {
                fn0(ctx, base + ((q->t / 2 % 3) << 10), q->x - 0x10, q->y - 0x38,
                       0x10, 0x40);
                fn1(ctx, base + ((q->t / 2 % 3) << 10), q->x, q->y - 0x38,
                       0x10, 0x40);
                q->t = q->t + 1;
            }
            i++;
        }
        {
            int sh = (*(State **)(base + 0x7828))->f18 * 2 + 2;
            UpdateScreenShake(sh, sh);
        }
        if (*(int *)(base + 0x77b0) == 0) {
            *(int *)(base + 0x77b0) = 1;
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_SpinCamera);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
