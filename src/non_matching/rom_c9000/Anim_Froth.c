/* Anim_Froth -- 0x080d33c0, 510 instructions.  PARKED, FOUR ENCODINGS OUT.
 *
 * NON-MATCHING, 4 of 529 encodings differ (was 10; batch 305 brief C).  It is a
 * TRUE distance and then some: SIZE matches (1172 = 1172), the instruction
 * COUNT matches (529 = 529), RELOCATIONS are IDENTICAL, and tools/aligncmp.py
 * reads 527 of 529 aligned-equal (99.6%).  The whole residue is ONE
 * TWO-INSTRUCTION SWAP occurring at TWO call sites.  This is the closest
 * non-matching function in the tree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Froth.c \
 *     asm/rom_c9000/rom_d2d98.s --func Anim_Froth
 *
 * SPLIT SHAPE: TEXT-ONLY, ONE NEW EXPORT.  asm/rom_c9000/rom_d2d98.s holds SIX
 * functions (Anim_Nereid, Anim_Froth, Anim_Whirlwind, Anim_Prism,
 * ColorCycleVFXPalette, Anim_Plasma) plus a .rodata tail, so converting one
 * needs a TEXT/DATA SPLIT.  tools/datacheck.py: Anim_Froth reads .Lee1c4 and
 * nothing else, so the remainder needs exactly
 *
 *     .global .Lee1c4
 *
 * immediately before its label (.Lee1c4 is 0xee1c4..0xee1ca, SIX bytes, indexed
 * as unsigned char at f18*2 and f18*2+1 -- a table of {count, frames} pairs).
 * Every other symbol already exists.  NO SHIMS, NO PINS (tools/shimcount.py is
 * clean), no per-file flag override.
 *
 * ================================================================
 * THE ONLY RESIDUE LEFT: THE ARGUMENT-SETUP ORDER OF BOTH StartTask CALLS
 * ================================================================
 *
 * Four encodings, at objcmp indices 160/161 and 172/173, and aligncmp scores
 * them as an INSERT/DELETE PAIR at each site -- i.e. a pure MOVE of one
 * instruction past its neighbour, not a wrong instruction anywhere.  The ROM
 * fills r1 before r0 at both sites --
 *
 *     adds r1, r5, #0          @ arg, from the shared 0x90 << 3
 *     ldr  r0, =Task_SpinCamera
 *     bl   StartTask
 *
 * -- and we emit the pool load first.  It is NOT the "fill r0 last" shim class:
 * the landed Anim_Confuse next door has the SAME `arg = 0x90; arg <<= 3;` idiom
 * at two StartTask calls and its ROM emits `ldr r0,.L36 / mov r1,r6`, i.e. OUR
 * order, byte-exact.  So the two ROM functions differ in a way the source does
 * not express.
 *
 * WHY IT IS AN EXPAND-ORDER RESIDUE, NOT A SCHEDULING ONE.  On ARM the constant
 * pool is built by machine_dependent_reorg, which runs AFTER sched2, so at
 * schedule time the first argument is still `(set (reg r0) (symbol_ref))` -- a
 * plain move with the same cost as the r1 copy, not a load.  The two insns
 * therefore TIE on INSN_PRIORITY and on dependent count, and gcc-2.96's
 * rank_for_schedule breaks that tie with `INSN_LUID (tmp) - INSN_LUID (tmp2)`,
 * i.e. it PRESERVES EXPAND ORDER.  expand_call's load_register_parameters walks
 * i = 0 .. num_actuals-1, so r0 is emitted first and no source spelling of the
 * two arguments can change that.  The ROM's order needs the r1 move to carry
 * the lower LUID, which this call shape cannot produce.
 *
 * MEASURED INERT -- ALL STILL EXACTLY 4 of 529, SAME FOUR INDICES.  Twenty-one
 * spellings and nine source positions:
 *   spelling: `arg = 0x90 << 3;` as one statement; `arg = 0x480;`;
 *     `arg *= 8;` and `arg = arg << 3;` for the shift; two separate `arg`/`arg2`
 *     locals; the shift inlined at both call sites with no local at all;
 *     `unsigned arg`; `extern void StartTask(void (*)(void), int)`;
 *     `extern void StartTask(int, int)` with `(int)Task_SpinCamera`;
 *     `extern void StartTask();` (no prototype at all); named `void *t1/t2`
 *     locals assigned immediately before each call; `&Task_SpinCamera`;
 *     `(void *)Task_SpinCamera`; the task symbols declared `extern char x[]`
 *     so the argument is an array decay; and
 *     `StartTask((arg <<= 3, Task_SpinCamera), arg)`, which forces arg's
 *     computation into argument 0's own evaluation.
 *   position: the `arg = 0x90; arg <<= 3;` pair moved before both
 *     base+0x77ac/0x77b0 stores, between them, and split so the shift alone
 *     sits immediately before the first call.
 *   flags: -fno-schedule-insns2 (204 differ), -fno-schedule-insns,
 *     -fno-cse-follow-jumps, -fno-peephole, -fno-delayed-branch (all 4);
 *     -fno-force-mem (37), -fno-rerun-cse-after-loop (324, +4 insns),
 *     -fno-expensive-optimizations (393, +6), -fno-caller-saves (389).
 *     NOTHING moves indices 160/161/172/173.
 * MEASURED WORSE: the task locals assigned at the top of the function (532
 *   instructions, +3); reusing the counter/index local `n` as the task argument
 *   (8 of 529 -- it breaks the table allocation described below).
 *
 * ================================================================
 * WHAT CLOSED 10 DOWN TO 4: A COMMA-ASSIGNED INDEX AND TABLE PAIR IN THE
 * OUTER LOOP CONDITION.  ONE STATEMENT, SIX ENCODINGS.
 * ================================================================
 *
 * The six encodings that used to differ were the register the `.Lee1c4` pool
 * load got at the two loop tails: the ROM uses r2 at BOTH, we used r4 at the
 * inner tail (swapping with the `mov rN, sl` copy of the state pointer, which
 * the ROM puts in r4) and r0 at the outer tail.  The function loads the table
 * at FOUR sites and the ROM's registers are r2 (outer entry guard), r4 (the
 * `frame == Lee1c4[...] - 0x10` test), r2 (inner tail), r2 (outer tail) -- so it
 * is NOT one pseudo, it is four independent rematerialisations whose reload
 * picks we had to rotate.  Ours were r2, r4, r4, r0.
 *
 * The fix is to write the OUTER while condition as
 *
 *     while (n = (*(State **)(base + 0x7828))->f18 * 2 + 1,
 *            tbl = Lee1c4,
 *            frame != tbl[n]) {
 *
 * with `unsigned char *tbl;` and `int n;` declared LAST.  That one statement
 * fixes all six: the inner tail's two pseudos swap into the ROM's `mov r4, sl`
 * / `ldr r2, =.Lee1c4`, and the outer tail's load becomes r2.
 *
 * WHY IT WORKS, IN TWO PARTS, BOTH OF WHICH ARE LOAD-BEARING.
 *
 * (a) `tbl` MAKES THE OUTER CONDITION'S TWO COPIES ONE PSEUDO.  The condition is
 *     duplicated by duplicate_loop_exit_test into the entry guard and the loop
 *     bottom, and a source variable assigned inside it is ONE declared C
 *     variable, so gcc-2.96 -- one pseudo per variable, no SSA renaming -- gives
 *     the whole thing ONE hard register, r2, at both copies.  The assignment
 *     must be INSIDE the condition: a plain `tbl = Lee1c4;` before the loop is
 *     held in a callee-saved register instead and the two pool loads collapse
 *     (527, two instructions SHORT, 335 differ).  Re-assigning in the condition
 *     keeps a pool load at each copy, so the COUNT is preserved at 529, and the
 *     rotation this costs reload is what lands the inner tail as a side effect.
 *
 * (b) `n` KEEPS THE `+ 1` IN THE INDEX REGISTER.  With `tbl[... * 2 + 1]` and no
 *     `n`, gcc reassociates to `(tbl + idx) + 1` and folds the 1 into the load's
 *     immediate: `adds r3,r3,r2 / ldrb r3,[r3,#1]`.  The ROM keeps the 1 in the
 *     index and uses the register-offset form: `adds r3,#1 / ldrb r3,[r2,r3]`.
 *     Thumb-1 `ldrb rd,[rn,rm]` has NO immediate field, so the two forms cost
 *     the same and gcc picks the immediate one whenever the base is a pointer
 *     VARIABLE (with the bare symbol it cannot, which is why the base candidate
 *     matched at indices 183/184).  Assigning the index to its own local puts
 *     the `+ 1` inside n's computation, where combine cannot reach it.
 *
 * THE COMMA ORDER IS LOAD-BEARING: `n` FIRST, `tbl` SECOND.
 *     n then tbl ......................  4 of 529   <- this file
 *     tbl then n ...................... 10 of 529
 *     tbl only, no n ..................  8 of 529  (183/184 and 491/493 are the
 *                                       folded ldrb; 451/452/454/457 all match)
 *     n reusing a dead donor instead of its own local: `arg` 9, `two` 9,
 *                                       `mask` 458 (RELOC), `yb` 474 (+2 insns)
 * And the device must be applied to the OUTER condition only:
 *     inner condition too ............. 34 of 529
 *     inner condition only ............ 11 of 529
 *     both, incl. the -0x10 test ..... 335 of 529, 527 insns (2 SHORT)
 * `tbl`/`n` DECLARATION POSITION IS INERT (first, last, and beside `flags` all
 * read 4 of 529) -- no spill offset differs anywhere in the function, so the
 * frame map recorded in lever (1) below is already exact.
 *
 * MEASURED AGAINST THE TABLE RESIDUE AND RULED OUT, BOTH DIRECTIONS OF THE
 * Anim_Fireball PLUS-CANONICALISATION LEVER -- it looks identical on the surface
 * and it costs an instruction whichever operand leads:
 *   `*(unsigned char *)(idx + (int)Lee1c4)` .. 528, 352 differ  (recorded)
 *   `*(unsigned char *)((int)Lee1c4 + idx)` .. 528, 352 differ  (new)
 *   `*(Lee1c4 + idx)` at all three sites ..... 528, 352 differ  (new)
 *   the same at the outer sites only ......... 528, 362 differ  (new)
 * There the two PLUS operands were both pseudos needing reloads and the order
 * decided the pick; here the table pseudo is already in the position the ROM has
 * it (both emit `ldrb r3,[rTable,rIndex]`), and the ARRAY form is what keeps it
 * there.  Also inert or worse: a named `unsigned char *tbl = Lee1c4;` held local
 * (527, 335/334 differ, either position); `extern const unsigned char Lee1c4[]`;
 * `unsigned char Lee1c4[][2]` with `[f18][0]`/`[f18][1]`; an explicit `int j`
 * stepped by 2 instead of `i * 2` (22 of 529); declaring `flags` first or last.
 *
 * ================================================================
 * THE ONE THAT MATTERS MOST: `signed char *flags = gBuffer;` IS A MISSING
 * QUANTITY, NOT A CONVENIENCE -- 62 SPAN TO 10
 * ================================================================
 *
 * With the four flag accesses written as `gBuffer[i]` the candidate is ONE
 * INSTRUCTION SHORT (527 against 529) and every scratch register in the
 * function is rotated: the ROM puts the gBuffer address in r4 or r0 and the
 * copy of the counter in r0 or r1, and we put the address in r2 or r3 -- once
 * as `ldrsb r3,[r3,r0]`, reusing the destination as the base, which the ROM
 * never does.  The missing instruction is a second `mov r0,r8`: the ROM copies
 * the counter into r1 for the `ldrsb`, that r1 is then clobbered by
 * `ldr r1,[r3,#0x14]`, and a fresh copy is needed for __modsi3, while we had
 * already put the counter in r0 and reload inherited it.
 *
 * Naming the array pointer -- `signed char *flags; ... flags = gBuffer;` in the
 * init loop's preheader, with `flags[i]` at all four sites -- creates the
 * pseudo the ROM's allocation was built around.  It is REMATERIALISED, not
 * held: all four accesses still load the symbol from the pool, exactly as the
 * ROM does, so this is the REG_EQUIV path and not a cached pointer.  Its
 * DECLARATION POSITION is inert; only its existence matters.
 *
 * ================================================================
 * FIVE MORE LEVERS THAT PAID, WITH THEIR SINGLE-DROP MEASUREMENTS
 * ================================================================
 *
 * (1) DECLARATION ORDER IS THE FRAME MAP, worth 6 encodings.  `vec3_t v` is the
 *     only aggregate and takes 0x18..0x23; the three spilled scalars then
 *     descend in DECLARATION order -- ctx 0x14, fn1 0x10, fn0 0xc -- and the
 *     loop-2 induction variable reload takes 0x8.  Declaring the two blit
 *     pointers as `DrawFn fns[2]` puts fns[0] at 0x10 and ctx at 0xc, which is
 *     the ROM's layout upside down.  Note fn1 is declared BEFORE fn0 even
 *     though fn0 is assigned first.
 *
 * (2) A WALKING POINTER DERIVED INSIDE THE LOOP BODY, worth the position of two
 *     preheader blocks.  `Part *tgt = (Part *)(base + (0xe8 << 7)) + i;` and
 *     `Part *q = (Part *)(base + (0xe1 << 7)) + i;` written INSIDE the body let
 *     loop.c strength-reduce them, and a giv's initialiser is inserted AFTER
 *     the duplicated exit test -- which is where the ROM has it.  A preheader
 *     assignment lands before the test.  (This works here and NOT in
 *     Anim_Fireball, whose base is a bare symbol; see that file's lever (2).)
 *
 * (3) `int mask = 0xff;` AS A SOURCE LOCAL, ordered `i = 0; mask = 0xff;
 *     q = ...;`, worth 17 -> 10.  Written as two literal `Random() & 0xff` the
 *     constant is hoisted by loop.c and lands LAST in the preheader; a source
 *     local is materialised where the source puts it, which is second-to-last.
 *     `mask` is NOT reusable as a second quantity: as the `k` modulus it reads
 *     452 differ (+2 insns), as the 0x1f randomiser mask 468, as the
 *     screen-shake temp 470 (+2).
 *
 * (4) TWO SEPARATE STORES FOR base+0x77ac, not a `?:`.  The ROM computes the
 *     address in BOTH arms and cross-jumps only `str r3,[r2]`; a single store
 *     with a conditional value computes the address once.  Anim_Flare's rule.
 *
 * (5) RE-DERIVE base+0x7828 EVERYWHERE; DO NOT NAME IT.  An explicit
 *     `State **sp3` for the inner-loop region costs an instruction (527, 322
 *     differ).  The ROM holds it in r10 inside the inner loop because LICM
 *     hoists the address there, and re-derives it in the outer body and after
 *     the loop -- which is what the plain `(*(State **)(base + 0x7828))`
 *     spelling produces by itself.
 *
 * ALSO LOAD-BEARING: `(unsigned)q->t <= 0xf` around a SIGNED `q->t / 2`;
 * `q->t = q->t + 1` as a re-read; `i % f14` recomputed for each of the three
 * uses (the intervening calls clobber f14, so cse cannot share them) but shared
 * as `k` WITHIN the Func_80d6888 call, whose argument 4 IS k; `ax * 60 / 64`
 * for the ROM's `lsl #4 / sub / lsl #2` chain; `v.x >>= 1` (a shift, not `/ 2`);
 * `Data`-free `if (v.z <= 0x9f)` and `if (v.z > 0x31f)` as two separate ifs so
 * cse feeds the second from the first's register; and Anim_Confuse's
 * `d0 = 0xa0; copy = Func_8001af8; p = data; data += 0x80; d0 <<= 19;` block
 * transplanted verbatim, pin-free.
 *
 * MEASURED INERT AND THEREFORE NOT WORTH RE-TRYING (all 10 of 529 on the old
 * body, i.e. they neither helped nor hurt): a shared `int zero = 0;` local for
 * the two base+0x77ac/0x77b0 stores (cse already makes that one `movs r2,#0`);
 * hoisting the inner loop's `k` to function scope; declaring `arg` first or
 * swapping `arg` and `frame`.  MEASURED WORSE: `two` reused for the
 * base+(0xef<<7) store (531, +2 -- the ROM emits a fresh `movs r3,#2` there,
 * exactly as Anim_Whirlwind records); a shared `int one = 1;` for the three
 * 1-stores (527, -2); reusing `yb`, `two` or `arg` as the screen-shake temp
 * (470/531, and 59 differ with the POOL ORDER changed).
 *
 * ================================================================
 * WHERE TO GO NEXT
 * ================================================================
 *
 * Only the StartTask argument order is left, and the analysis above says it is
 * decided by expand's forward walk over the argument registers, which no
 * spelling of THIS call reaches.  The one experiment still worth doing is a
 * gcc-2.96 `-da` dump comparison of the .sched2 and .greg dumps against the
 * landed Anim_Confuse in the same bank, which has the SAME StartTask idiom
 * resolving the other way: that pair is a controlled experiment on this residue
 * and nothing else in the corpus is.  If the dumps show the two insns tying on
 * priority in both, the difference is upstream of the RTL we can author and the
 * function should be shipped as a four-encoding park, not chased further.
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
extern void StartTask(void *fn, int arg);
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
