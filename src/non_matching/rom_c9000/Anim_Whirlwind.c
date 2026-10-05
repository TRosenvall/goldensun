/* Anim_Whirlwind -- 0x080d3854, 441 instructions.  PARKED.
 * NON-MATCHING, 26 of 472 encodings differ.  SIZE AND COUNT BOTH EXACT
 * (1068 bytes / 472 encodings both sides), so the 26 IS a true distance and
 * every number quoted below is comparable.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Whirlwind.c \
 *     asm/rom_c9000/rom_d2d98_c.s --func Anim_Whirlwind
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT NEEDED.  tools/datacheck.py on
 * asm/rom_c9000/rom_d2d98_c.s reports a .rodata section and six functions
 * (Anim_Nereid, Anim_Froth, Anim_Whirlwind, Anim_Prism, ColorCycleVFXPalette,
 * Anim_Plasma).  Anim_Whirlwind reads exactly ONE data label, so the split
 *   *** MUST EXPORT: .global .Lee1ca
 * and nothing else.  No suffix is taken yet for this base -- rom_d2d98 has no
 * rom_d2d98_*.s in asm/ or rom_d2d98_*.c in src/, so the pieces are free to
 * take _a/_b/_c...  NO SHIMS, NO PINS: no volatile beyond the io.h registers,
 * no "+r" barrier, no do{}while(0), no .equ, no per-file flag override
 * (tools/shimcount.py is clean on this file).
 *
 * ================================================================
 * WHAT CLOSED 431-of-472 DOWN TO 26, in the order the levers paid
 * ================================================================
 *
 * 1. TWO COUNTER VARIABLES SHARED ACROSS FIVE DISJOINT LOOPS, worth
 *    442 -> 464 encodings (the candidate was 30 encodings SHORT before this).
 *    The ROM puts the particle-init counter, the inner 4-particle counter and
 *    the actor counter ALL in r8, and the 7-step fade counter, the m counter
 *    and the actor-inner counter ALL in r9.  gcc-2.96 gives ONE hard register
 *    per variable and never splits a live range, so three separate variables
 *    would not all land in r8 by accident: r8 is ONE source variable used in
 *    three regions and r9 is another.  This is the exact CONVERSE of the
 *    recorded one-variable-per-region rule, and the ROM's register file is
 *    what says which way round it goes -- a shared name is what pushes the
 *    counters out of the low callee-saved registers and into r8/r9, and the
 *    two `mov` instructions per iteration that costs are 7 of the 30
 *    instructions that were missing.
 *
 * 2. THE ACTOR LOOP'S BOUND MUST BE RE-DERIVED, NOT READ THROUGH `st`, worth
 *    466 -> 472 (and 408 -> 46 differing).  `while (i != (*(State **)(base +
 *    0x7828))->f14)` makes `base + 0x7828` a loop invariant that LICM hoists
 *    into its own register for the whole actor loop; `while (i != (*st)->f14)`
 *    reloads the cached pointer at the loop bottom instead and the function
 *    comes out 6 instructions short.  This is the recorded "do not cache a
 *    struct slot the ROM re-derives" lever, and `st` (= base + 0x7828, kept in
 *    a frame slot) is genuinely cached for the OTHER five reads -- the ROM has
 *    both spellings in one function and the register file tells them apart.
 *
 * 3. DECLARATION ORDER IS THE SPILL-SLOT MAP, DESCENDING, worth 46 -> 38.
 *    The five spilled scalars land at sp+0x28 base, 0x24 ctx, 0x20 yb, 0x1c st,
 *    0x18 fp -- i.e. FIRST-DECLARED GETS THE HIGHEST ADDRESS.  `yb` therefore
 *    has to be declared at FUNCTION scope between `ctx` and `st`, and the
 *    `DrawFn *fp` AFTER `st`, even though yb is only ever live inside the frame
 *    loop.  Six of the eight differing encodings were sp offsets.
 *
 * 4. `i = 0;` BEFORE `p = (Part *)(base + (0xe1 << 7));` in the init loop,
 *    worth 38 -> 32, and `frame = 0;` BEFORE `st = (State **)(base + 0x7828);`,
 *    worth 32 -> 26.  Both are pure scratch-register-numbering effects: the
 *    zero is materialised where the source puts it and the reload registers
 *    for the neighbouring pool load and stack reload fall out of that order.
 *
 * 5. NO int CARRIER FOR THE REGISTER WRITES.  `REG_BLDALPHA = 0x1010;` is
 *    correct as written.  This was got WRONG first: gcc emits `ldrh r3, .LC`
 *    for the HImode pool constant where the ROM disassembly shows
 *    `ldr r3, .Ld38bc  @ 0x1010`, which LOOKS like a width mismatch -- but
 *    Thumb-1 has no PC-relative LDRH, so gas assembles `ldrh rX, .LC` to the
 *    SAME `ldr rX,[pc,#N]` encoding.  Adding `int alpha = 0x1010;` to "fix" it
 *    swapped the address and value registers and turned the 0x1000 write into
 *    `mov #0x80 / lsl #5`, costing 8 encodings.  The same reasoning explains
 *    the pooled `0x4f` in `(0x4f - frame) | 0x1000`: it is an HImode constant,
 *    pooled, and `mov r2,#0x4f` is never what gcc emits there.
 *
 * 6. `angle` AND `row` ARE loop.c GIVS, NOT SOURCE LOCALS.  The first draft
 *    declared them and assigned them before the `while`, which is a semantic
 *    BUG (they were reset every iteration) and also puts their initialisation
 *    in the wrong basic block.  The ROM initialises both in the m-loop
 *    PREHEADER -- after the duplicate_loop_exit_test guard -- which only
 *    happens for a strength-reduced giv.  So the source writes
 *    `sin((frame << 11) + (k << 14))` and
 *    `q = (Part *)(base + k * 0x70 + (0xe1 << 7))` inline and lets loop.c
 *    build the two walkers.  Worth 440 -> 457.
 *
 * ALSO LOAD-BEARING, found on the first candidate and never moved:
 *   - `pp = tbl; base = *pp++; ctx = *pp;` for the ROM's `ldmia r3!, {r1}`.
 *   - `DrawFn fns[2]` with fns[0] written through the ARRAY, `f1 = g[8]` loaded
 *     into its own local FIRST, then `fp = fns; fp[1] = f1;` and the inner call
 *     made through `fp[i & 1]` -- the cd508_Confuse device, transferred whole.
 *   - one shared `int two = 2;` for both BuildDraw2DFuncEx stack arguments, but
 *     a PLAIN literal 2 for `*(int *)(base + (0xef << 7)) = 2` (the ROM emits
 *     `mov r3,#2` there, it does not reuse the r5 that holds `two`).
 *   - `copy = Func_8001af8;` as a local function pointer for `bl _call_via_r3`,
 *     and `clear = Func_80008d8;` assigned OUTSIDE the 7-step loop so the pool
 *     word is hoisted into a callee-saved register as the ROM has it.
 *   - `q->t = q->t + 1;` as a RE-READ, and the `if (q->t == 6)` test reading it
 *     again, so cse supplies the stored value and the increment/store pair
 *     cross-jumps between the taken and skipped arms.
 *   - `int idx = q->t / 2 + i / 2 * 3;` -- `q->t / 2` FIRST, because
 *     *thumb_addsi3 makes the ROM's `add r4, r3` name r4 (= q->t/2) as the
 *     destination-tied operand.
 *   - `r5 = rem * 5;` AS A NAMED LOCAL, then `(r5 << 9)` and `(r5 << 8)`: 5 is
 *     shift-cheap, so without the local expand_mult synthesises each product
 *     separately (the recorded named-scale-multiplicand rule).
 *   - `if (frame == k * 8 + i * 3 + 0x10)` with `k * 8` FIRST in the actor
 *     loop: that ordering is what makes loop.c strength-reduce BOTH `i * 3`
 *     (into the ROM's r10 walker) and `0x24 + i * 2` (into r4).  With
 *     `i * 3 + k * 8 + 0x10` only one of the two is reduced and `i * 3` is
 *     recomputed from scratch every actor iteration.
 *
 * ================================================================
 * THE 26 THAT ARE LEFT -- TWO RESIDUES, BOTH CHARACTERISED
 * ================================================================
 *
 * RESIDUE A, 12 encodings: THE TWO m-LOOP GIVS HAVE SWAPPED FRAME SLOTS.
 * The ROM puts the angle walker at sp+0x10 and the row walker at sp+0xc; we
 * put the angle at 0xc and the row at 0x10.  Everything else about the pair is
 * identical -- same preheader, same two `str`s, same increments (0x4000 and
 * 0x70), same load/store order -- so the 12 differing encodings are 8 sp
 * offsets plus the 4 scratch registers that follow from them.
 *
 * Spill slots descend with PSEUDO NUMBER (lever 3 proves that for decls), and
 * loop.c's record_giv PREPENDS to `bl->giv` while strength_reduce walks the
 * list from the head, so the LAST giv in insn order gets the lowest new pseudo
 * and therefore the HIGHEST slot.  Our angle expression is at the top of the
 * m-loop body and our row expression at the bottom, which yields row-high /
 * angle-low -- consistent.  For the ROM's angle-high / row-low the row
 * expression would have to be recorded FIRST, i.e. appear before the `sin`.
 * MEASURED, and it does not work:
 *   - `q = (Part *)(base + k*0x70 + (0xe1<<7));` moved to the top of the body:
 *     476 encodings and 1076 bytes -- `q` is then live across the three
 *     fns[0] calls, r6 is already holding `base + rem*0xa00` across them, and
 *     the extra pressure SPILLS `frame` and adds a sixth stack slot.  Wrong
 *     program, not a closer one.
 *   - the same expression DUPLICATED (once before the sin, once in place):
 *     31 of 472.  cse unifies the two, the giv order does not change, and the
 *     duplicate costs 5 encodings in the q preheader.  So the order is decided
 *     by where the SURVIVING set lands, and it cannot be moved without moving
 *     the value's live range.
 *   - `rem = frame / 2 % 3;` hoisted ahead of the sin, to test whether the
 *     angle is an expand TEMP whose pseudo sits between the `frame >> 31`
 *     invariant (sp+0x14) and the row giv rather than a new loop.c pseudo:
 *     241 of 472 with `rem` and `r5` both moved, 241 with only `rem` moved.
 *     Emphatically no.
 *   - INERT, all three still 26 of 472: `k * (4 * 0x1c)` for the stride,
 *     `(0xe1 << 7) + base + k * 0x70`, `(Part *)(base + (0xe1 << 7)) + k * 4`,
 *     `frame * 0x800 + k * 0x4000` for the angle, and swapping `q = ...` with
 *     `i = 0;`.
 * The next thing to try is a gcc-2.96 `-da` dump of the loop pass to see the
 * giv list order directly; that was not done here.
 *
 * RESIDUE B, 14 encodings: THE ACTOR-LOOP GUARD READS `st` WHILE THE LOOP
 * READS THE RE-DERIVED ADDRESS.  The ROM's guard is
 *   ldr r0,[sp,#0x1c] / ldr r3,[r0] / ldr r3,[r3,#0x14] / cmp r3,#0 / beq
 * and the loop's own preheader THEN computes `base + 0x7828` again into r7
 *   ldr r1,[sp,#0x28] / ldr r2,=0x7828 / adds r7,r1,r2
 * -- eight instructions across the two blocks.  We emit the same eight, but
 * because the guard is duplicate_loop_exit_test's COPY of the loop condition
 * it uses whichever spelling the condition uses, and then r7 is just a copy of
 * the guard's result (`adds r7,r2,#0`).  Two spellings in one loop is what the
 * ROM has and a `while` cannot produce it.  MEASURED:
 *   - `while (i != (*st)->f14)`, guard and bottom both via st: 466 encodings,
 *     6 SHORT (this is the same measurement as lever 2, from the other side).
 *   - `i = 0; if ((*st)->f14 != 0) { do { ... } while (i != (*(State **)
 *     (base+0x7828))->f14); }` -- the explicit-guard form, which is the only
 *     source shape that can carry two spellings: 470 encodings / 1064 bytes,
 *     2 SHORT, because cse then folds the guard's `*st` load into the one the
 *     m-loop's own bound test already made (the ROM keeps them separate; in
 *     the ROM that block has two predecessors and cse only works on extended
 *     basic blocks).  With `i = 0;` moved inside the `if`: also 470/1064.
 * So the guard shape is reachable and the guard's LOAD is not; the open
 * question is what keeps the two `*st` loads apart, and the honest answer is
 * that it is a CFG property we have not reproduced.
 *
 * ================================================================
 * BATCH 305 BRIEF C -- NINE MORE MEASUREMENTS, ALL NEGATIVE.  Baseline
 * re-confirmed as installed: 26 of 472, 1068 bytes both sides, differing at
 * objcmp indices 205,206,208,212,226,285,349,350,351,352,353,354 (residue A)
 * and 368-384 (residue B), exactly as described above.
 * ================================================================
 *
 * RESIDUE A IS NOT REACHABLE BY MOVING THE SOURCE EXPRESSION AT ALL.  The
 * header above proposes getting the row giv recorded first by making the row
 * expression appear before the `sin`.  Three ways of doing that WITHOUT moving
 * a pointer's live range across the three fns[0] calls -- which is what spilled
 * `frame` last time -- are EXACTLY INERT: an `int row = k * 0x70;` written
 * immediately before the `sin` with `q = (Part *)(base + row + (0xe1 << 7));`
 * kept at the bottom reads 26 of 472 with THE SAME TWELVE INDICES, as does the
 * same thing with `row` at function scope, and as does `k * (4 * 0x1c)` for the
 * stride with the addend order swapped.  Byte-identical residues across four
 * source shapes say loop.c derives the two givs from `k`'s uses in a canonical
 * order of its own and does not follow the order the source writes them in, so
 * the "record the row expression first" route is closed at the C level.  What
 * the residue actually is: sp+0xc carries the ROW walker in the ROM (`adds
 * r1,#112 / str r1,[sp,#12]`) and the ANGLE walker in ours (`adds r0,r0,r1`
 * with r1 = 0x4000), with the r2/r3 swap at 205-208 and the sp+0xc/sp+0x10
 * reads at 212/226/285 following from that one choice.
 *
 * RESIDUE B: EVERY WAY OF SPLITTING THE `st` SPELLING LOSES INSTRUCTIONS.  The
 * ROM needs the guard to read `st` (one `ldr r0,[sp,#0x1c]`) and the loop's own
 * preheader to re-derive `base + 0x7828` into r7 (three insns).  We emit the
 * three-insn derivation in the GUARD and a one-insn copy in the preheader --
 * same eight instructions, wrong distribution, because cse makes the preheader
 * reuse the value the dominating guard block already computed.  Four further
 * splits, none of which holds the count:
 *   - actor condition via `st`, the body's two `->ids[i]` reads via a fresh
 *     `State **sp4` assigned at the top of the actor body: 466 enc / 1056 bytes
 *     (6 SHORT), and the pool order moves too.
 *   - the same with `sp4` assigned in the actor loop's preheader: 466 / 1056.
 *   - EVERYTHING in the actor loop via `st` (condition and both body reads):
 *     461 / 1044, 11 SHORT -- the deepest loss of the four.
 *   - condition left re-deriving (as installed) and only the BODY via `st`:
 *     470 / 1064, 2 SHORT.
 * Also measured: the reverse of the recorded explicit-guard form -- guard
 * re-derives, `do {} while (i != (*st)->f14)` at the bottom -- 468 / 1060,
 * 4 SHORT; and the explicit guard via `st` with the m-loop's own bound switched
 * to the re-derived spelling to stop cse merging the two `*st` loads, which
 * goes the other way: 473 / 1072, one LONG.  So the guard's load cannot be
 * bought without paying somewhere else, and the recorded conclusion stands.
 *
 * THE COMMA-IN-CONDITION DEVICE DOES NOT HELP HERE, though it is what took
 * Anim_Froth from 10 to 4 in this same batch (see that file's header for the
 * mechanism: a variable assigned inside a while condition is one pseudo across
 * duplicate_loop_exit_test's two copies of it, so both copies get one hard
 * register, while the count is preserved because each copy keeps its own
 * materialisation).  Applied to the actor condition as
 * `while (sp3 = (State **)(base + 0x7828), i != (*sp3)->f14)` it is EXACTLY
 * INERT -- 26 of 472, same twelve indices -- because LICM hoists the invariant
 * assignment straight back out into the preheader and the guard copy keeps its
 * own three-insn derivation regardless.  Worth knowing before it is tried
 * again: the device only bites where the assigned value must be REMATERIALISED
 * at each copy, which is true of a pool constant and false of a loop invariant.
  *
 * ================================================================
 * BATCH 327B -- RESIDUE A: I RAN THE LOOP DUMP THIS HEADER ASKED FOR, AND THE
 * RECORDED CLOSURE REASON IS WRONG
 * ================================================================
 * Baseline re-derived as installed: 26 of 472, 1068 bytes and 472 encodings
 * both sides, so the 26 is still a true distance.
 *
 * The header above says "The next thing to try is a gcc-2.96 `-da` dump of the
 * loop pass to see the giv list order directly; that was not done here."  DONE.
 * loop.c writes its verbose log straight into `*.08.loop` (loop_dump_stream is
 * rtl_dump_file), so plain `-da` is enough and no extra flag is needed.
 *
 * The m-loop is "Loop from 386 to 769: 148 real insns", and its giv candidates
 * are
 *     Insn 413: giv reg 121 mult 16384 add 0              <- bare k << 14
 *     Insn 415: giv reg 122 mult 16384 add (reg:SI 310)   <- ANGLE, 310=frame<<11
 *     Insn 598: giv reg 188 mult 8   add 0
 *     Insn 600: giv reg 189 mult 7   add 0
 *     Insn 602: giv reg 190 mult 112 add 0                <- bare k * 0x70
 *     Insn 604: giv reg 191 mult 112 add (reg/v:SI 35)    <- ROW, the q ADDRESS
 *     giv of insn 602 not worth while, 136 vs 148.
 *     giv of insn 600 not worth while, 102 vs 148.
 *     giv of insn 598 not worth while, 0 vs 148.
 *     giv of insn 413 not worth while, 0 vs 148.
 *     giv at 604 reduced to (reg:SI 320)
 *     giv at 415 reduced to (reg:SI 321)
 *
 * CONFIRMED: record_giv prepends and strength_reduce walks from the head, so
 * the walk takes 604 before 415 -- the LATER insn gets the LOWER new pseudo and
 * therefore the HIGHER slot.  320 = row -> sp+0x10, 321 = angle -> sp+0xc,
 * which is our build; the ROM wants the reverse.
 *
 * REFUTED: "loop.c derives the two givs from `k`'s uses in a canonical order of
 * its own and does not follow the order the source writes them in."  Source
 * order IS followed.  All four of the inert rows recorded above moved a BARE
 * `k * 0x70` -- an `int row = k * 0x70;` before the sin, the same at function
 * scope, `k * (4 * 0x1c)`, the swapped addend order -- and a bare `k * 0x70` is
 * **insn 602's candidate, which is REJECTED (`not worth while, 136 vs 148`)**.
 * The surviving row giv is the q ADDRESS.  Four byte-identical inert rows were
 * measuring a giv that never existed in the output.
 *
 * AND THE LEVER WORKS.  Moving `i = 0; q = (Part *)(base + k*0x70 + (0xe1<<7));`
 * ABOVE the X/sin statement flips it, measured in that body's own `.08.loop`:
 *     giv at 433 reduced to (reg:SI 320)   <- ANGLE now the LOWER pseudo
 *     giv at 422 reduced to (reg:SI 322)   <- ROW
 * i.e. angle-high / row-low, the ROM's assignment.
 *
 * ITS PRICE, EXACTLY: 476 encodings / 1076 bytes / 474 instructions against the
 * ROM's 472 / 1068 / 470.  aligncmp reads 311 aligned-equal (65.9%), 210
 * differing in 79 hunks, and essentially all of it is ONE cause replicated --
 * the frame grows 52 -> 56 bytes, every sp offset shifts +4, and the frame-loop
 * counter leaves `fp`:
 *     ref  movs r2,#1 | add fp,r2 | mov r3,fp | cmp r3,#0x50
 *     ours ldr r2,[sp,#24] | adds r2,#1 | str r2,[sp,#24] | cmp r2,#0x50
 * It is NOT a lost allocno: `.18.greg` says `29 regs to allocate` in BOTH
 * bodies and the list is the same set in near-identical order (only the giv
 * pseudos renumber 321/320 -> 320/322 and 169/168/124 -> 175/174/130).  The
 * sixth slot is a reload-level outcome, and `fp` changes tenant -- the base body
 * uses it read-only (`mov rX, fp` x6), the moved body as an arithmetic
 * accumulator (`add fp,fp,rX` x3).
 *
 * THE PRESSURE-RELIEF CROSS IS DEAD.  Four bodies, all BYTE-IDENTICAL to each
 * other at 476 / 1076:
 *   q + `i = 0;` to the top, `r5 = rem * 5;` named (the recorded 476)   476
 *   the same with r5 UNNAMED (`rem * 5` inline in all three call args)  476
 *   the same with `rem` and `r5` both inlined (`frame / 2 % 3 * 5`)     476
 *   q alone to the top, `i = 0;` left after the three calls             476
 * So r5's naming and `i`'s placement are inert once q is above the calls, and
 * the register held across the three fns[0] calls is not what decides it.
 *
 * SO RESIDUE A IS BLOCKED ON REGISTER PRESSURE ACROSS THE THREE fns[0] CALLS,
 * not on loop.c's ordering.  The flip is available on demand and costs the frame
 * a sixth slot.  Anything that frees one register across those three calls
 * without changing the program is the whole remaining question.
*/
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*ClearFn)(void *dst, int size, int value);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];

extern unsigned char Lee1ca[] __asm__(".Lee1ca");
extern unsigned char Data_edeca[], Data_eded0[];
extern unsigned short Data_edebe[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void Func_80008d8(void *dst, int size, int value);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(int n);
extern void gfree(int tag);

void Anim_Whirlwind(void *context)
{
    void **g;
    void **pp;
    unsigned char *base;
    void *ctx;
    int yb;
    CopyFn copy;
    DrawFn fns[2];
    DrawFn f1;
    unsigned char *data;
    State **st;
    DrawFn *fp;
    int two;
    int frame;
    int i;
    int k;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BLDALPHA = 0x1010;
    data = (unsigned char *)GetFile(FILE_ce);
    {
        int d0;
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, data, 0x80);
    }
    data += 0x80;
    DecompressLZ(data, base);
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    fns[0] = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
    f1 = (DrawFn)g[8];
    fp = fns;
    fp[1] = f1;
    {
        Part *p;
        i = 0;
        p = (Part *)(base + (0xe1 << 7));
        do {
            p->x = Random() & 0x1f;
            p->y = (Random() & 0x3f) + 0x10;
            p->t = -(Random() & 0xf);
            i++;
            p++;
        } while (i != 0x10);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    StartTask(Task_BlitAnim, 0x90 << 3);
    REG_BLDALPHA = 0x1000;
    WaitFrames(1);
    _PlaySound(0x8d);
    frame = 0;
    st = (State **)(base + 0x7828);
    do {
        ClearFn clear;

        yb = sin(frame << 10) << 4;
        if (frame == 0x20) {
            _Func_80bd7dc(0x85);
        }
        clear = Func_80008d8;
        k = 0;
        do {
            if (frame == k * 8 + 0x10) {
                clear(ctx, 0x80 << 7, 0x8080808);
            }
            k++;
        } while (k != 7);
        if ((*st)->f4 == 1) {
            yb += 0x80 << 14;
        } else {
            yb += 0xffe00000;
        }
        if (frame <= 0x10) {
            REG_BLDALPHA = frame | 0x1000;
        }
        if (frame > 0x3f) {
            REG_BLDALPHA = (0x4f - frame) | 0x1000;
        }
        k = 0;
        while (k != Lee1ca[(*st)->f18 * 3]) {
            int X, Y, rem, r5;
            Part *q;

            X = ((Lee1ca[(*st)->f18 * 3 + 1] * sin((frame << 11) + (k << 14)) + yb) >> 16) + 0x28;
            Y = (cos((frame << 11) + (k << 14)) << 1) >> 16;
            rem = frame / 2 % 3;
            r5 = rem * 5;
            fns[0](ctx, base + (r5 << 9) + 0xc56, X, Y + 0x10, 0x28, 0x20);
            fns[0](ctx, base + (r5 << 8) + 0x2a56, X, Y + 0x30, 0x28, 0x20);
            fns[0](ctx, base + (r5 << 9) + 0x1156, X, Y + 0x50, 0x28, 0x20);
            i = 0;
            q = (Part *)(base + k * 0x70 + (0xe1 << 7));
            do {
                if (q->t >= 0) {
                    int idx = q->t / 2 + i / 2 * 3;
                    fp[i & 1](ctx, base + Data_edebe[idx], q->x + X, q->y + Y,
                              Data_edeca[idx], Data_eded0[idx]);
                }
                q->t = q->t + 1;
                if (q->t == 6) {
                    q->x = Random() & 0x1f;
                    q->y = (Random() & 0x3f) + 0x10;
                    q->t = 0;
                }
                i++;
                q++;
            } while (i != 4);
            k++;
        }
        i = 0;
        while (i != (*(State **)(base + 0x7828))->f14) {
            k = 0;
            do {
                if (frame == k * 8 + i * 3 + 0x10) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 4);
                    _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 6);
                }
                k++;
            } while (k != 7);
            i++;
        }
        *(int *)(base + 0x77a8) = 1;
        UpdateScreenShake(Lee1ca[(*st)->f18 * 3 + 2], Lee1ca[(*st)->f18 * 3 + 2] << 1);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x50);
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}
