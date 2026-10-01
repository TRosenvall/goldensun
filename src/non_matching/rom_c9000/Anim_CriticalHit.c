/* Anim_CriticalHit -- 0x080e40a4, 672 ROM instructions (707 encodings).
 *
 * NON-MATCHING, 15 of 707 encodings differ.   [batch 310c: was 19]
 *
 * MEASUREMENT -- THIS COUNT IS A TRUE DISTANCE.  SIZE IS EXACT (1612 bytes both
 * sides, objcmp prints no SIZE line) and the instruction COUNT IS EXACT
 * (707 / 707), so the 15 ranks directly.  First differing index 78.
 * aligncmp ranks within that, SEPARATELY:
 *
 *     aligned-equal 701 of 707 = 99.2%,  12 differing/ins/del in 11 hunks
 *     (was 697 of 707 = 98.6%, 16 differing in 12 hunks)
 *
 * RELOCATIONS: 84 rows both sides, AND THEY NOW MATCH EXACTLY -- objcmp prints
 * no RELOCATIONS line at all.  The two `_call_via_r5` / `_call_via_r6` rows at
 * 0x336 and 0x340 that were the whole recorded relocation discrepancy are
 * closed; see BATCH 310C below.
 *
 * Verify with (the delivered park body, runnable as written):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_CriticalHit.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_CriticalHit
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_CriticalHit.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s Anim_CriticalHit -v
 * Installed path is src/non_matching/rom_c9000/Anim_CriticalHit.c; substitute it
 * for the scratch path once this body replaces that file.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * register pin, no barrier, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * BATCH 310C -- THE SHARED BLOCKER IS CLOSED, AND IT CLOSED ON BOTH FUNCTIONS
 * FROM ONE CHANGE
 * ================================================================
 *
 * THE PAIR HYPOTHESIS HELD.  The recorded blocker here was explicitly "the same
 * blocker as Anim_Djinni's, in the same shape, on a DIFFERENT function".  One
 * three-token change, written once and applied verbatim to both, moved both:
 *
 *     int clen = 0x80 << 7;          <-- a new local, INITIALISED AT ITS
 *                                        DECLARATION
 *     ...
 *     cl(ctx, clen);
 *     cl((void *)0x6004000, clen);
 *
 *     Anim_Djinni       26 -> 16 of 738, relocations now EXACT
 *     Anim_CriticalHit  19 -> 15 of 707, relocations now EXACT
 *
 * > AN INT CARRIER INITIALISED AT ITS DECLARATION MAKES ITS PSEUDO LIVE FROM
 * > FUNCTION ENTRY, WHICH IS HOW YOU MAKE A QUANTITY LOSE A HARD REGISTER.
 * > allocno_compare's priority is log2(n_refs) * freq / LIVE_LENGTH, so a range
 * > starting at entry is the LONGEST range and the LOWEST priority.  The pseudo
 * > is allocated last, loses, and because its REG_EQUIV is a constant, reload
 * > REMATERIALISES it at each reference rather than spilling it.  That is the
 * > ROM's form.  The recorded reading -- "the reference's pseudo FAILS to get a
 * > hard register and reload rematerialises it" -- was RIGHT; what was missing
 * > was that the source handle is the INITIALISER POSITION, not the spelling of
 * > the constant.
 *
 * WHY THE FOUR RECORDED SPELLINGS READ INERT.  They were all short-range forms
 * -- `0x4000`, `0x80 << 7`, `n << 7` off a block-scoped local, two
 * separately-scoped locals.  A body assignment or a block-scoped initialiser
 * keeps the range SHORT, which RAISES the constant's priority: the exact
 * opposite of what is wanted.  Measured on Anim_Djinni in this batch: `int
 * clen;` declared with `clen = 0x80 << 7;` assigned in the epilogue is 26 with
 * the veneers still r6; the same assignment moved earlier (before either
 * `StopTask`) is also 26.  Only the declaration initialiser reaches it.
 * DECLARATION RANK IS FREE (before `cl`, after `cl`, first in the whole list --
 * all identical) and so is the SPELLING (`0x4000` identical).
 *
 * HUNK A IS NOW GONE.  Our clear-call block is byte-exact against the
 * reference, both calls, including the asymmetric argument-0 schedule: the ROM
 * puts a STACK RELOAD as argument 0 BEFORE the `lsl r1,#7` and a POOL LOAD as
 * argument 0 AFTER it, this function has one of each, and we match both.  That
 * asymmetry is the discriminator for Anim_Djinni's last two encodings, where
 * argument 0 is a pool load and we still hoist it.
 *
 * ================================================================
 * THE REMAINING 15 -- ELEVEN HUNKS, EVERY ONE A SINGLE-SLOT TRANSPOSITION
 * ================================================================
 *
 * Twelve differing positions in eleven hunks, and every hunk is ONE instruction
 * moved by ONE slot.  Same instructions, same registers, same roles, same
 * immediates:
 *
 *   ref[78:80]    `ldr r2,[r2] / str r2,[sp,#0x38]` two slots early in ours
 *                 -- the gPtrs block, the same class as Anim_Djinni's hunk 1,
 *                 and 4 encodings here against 12 there.
 *   ref[170]      `add r5, fp` one slot late in ours
 *   ref[246]      `ldr r0,[pc,#392]` one slot late in ours (the 4862/4863
 *                 encoding difference is the displacement moving with it, not
 *                 a different operand)
 *   ref[365]      `ldr r0,[sp,#0x34]` one slot late in ours
 *   ref[465]      `add r5, fp` one slot late in ours -- the SAME instruction
 *                 as ref[170] at a second site, which makes it a class of two
 *                 rather than two accidents, and the first thing to look at
 *                 next.
 *
 * `add r5, fp` is `base + <something>` with base in r11; both sites are loop
 * preheaders.  The pair appearing identically at two disjoint sites is the
 * recorded "a register repeating across disjoint loops suggests a partition" --
 * and the recorded caution applies: it is NOT proof, the direction is not
 * fixed, and a partition must be applied WHOLE before anything is concluded.
 *
 * ================================================================
 * SPLIT SHAPE -- AND A CORRECTION TO THE BRIEF THAT SENT ME HERE
 * ================================================================
 * This is NOT a whole-file conversion.  asm/rom_c9000/rom_e3958_c_c_c_c_a.s
 * holds THREE functions -- Anim_Attack, BaseAnim_Attack, Anim_CriticalHit -- so
 * a split IS required.  What is true, and it is the part that matters, is that
 * the split needs ZERO `.global` exports: `tools/datacheck.py` prints nothing
 * for this stem because the file HAS NO DATA SECTION AT ALL, and
 * `tools/split_s.py ... --dry-run` does not refuse.
 *
 * THE ORDER IS NOT SYMMETRIC AND THE DRY RUNS PROVE IT:
 *
 *   split for BaseAnim_Attack  -> _a.s Anim_Attack        (46 lines)
 *                                 _b.s BaseAnim_Attack   (711 lines)
 *                                 _c.s Anim_CriticalHit  (723 lines)
 *   split for Anim_CriticalHit -> _a.s Anim_Attack + BaseAnim_Attack (757)
 *                                 _b.s Anim_CriticalHit  (723 lines)
 *
 * So CUTTING FOR BaseAnim_Attack FIRST is the good order: it leaves this
 * function ALONE in _c.s, and elevating it afterwards is then a pure file
 * rename with no further split.  Cutting for this function first buries
 * BaseAnim_Attack in a two-function _a.s that would have to be split again.
 * Whoever lands either one should land both in the same sitting, off the
 * Attack-first cut.  `split_s.py` was run ONLY with `--dry-run`.
 *
 * ONE LINKER-SCRIPT HAZARD, FLAGGED BECAUSE IT IS EASY TO MISS: stage1.ld names
 * this object TWICE -- line 1919 `(.text)` and line 1988 `(.rodata)` -- even
 * though the .s HAS no `.rodata` section.  The `.rodata` entry is contributing
 * nothing today, but a split must still leave the script consistent; do not
 * assume the absence of a data section means the absence of a data line.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * barriers, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE ROM'S COUNTERS PARTITION THE LOOPS, AND THE
 * PARTITION IS READABLE OFF THE REGISTERS -- 71.7% -> 89.4% IN ONE EDIT
 * ================================================================
 *
 * This function has six loops: four that run 0x40 times (the crit seeding
 * loop, the inner draw loop of the first frame loop, the reseed loop, the inner
 * draw loop of the second frame loop) and two that do not (the two 0x20-frame
 * loops, plus a 7-iteration fade loop at the end).
 *
 * I gave them three counters on the obvious reading -- `i` for the standalone
 * 0x40 loops and the fade loop, `k` for the two inner loops, `frame` for the
 * two frame loops -- and was EIGHT INSTRUCTIONS SHORT.  The reference uses
 * TWO, and says so plainly: EVERY inner/0x40 counter is `sl` (r10) and EVERY
 * frame-ish counter is r9, the fade loop included.  Repartitioning to
 *
 *     i      -> all FOUR 0x40-iteration loops
 *     frame  -> both frame loops AND the 7-iteration fade loop
 *
 * moved aligncmp 71.7% -> 89.4%, the differing/ins/del count 242 -> 90, and the
 * length from 8 short to 2 long.  `k` disappeared entirely.
 *
 * THE MECHANISM, WHICH IS WHY THE HIGH REGISTERS ARE THE TELL.  gcc-2.96's
 * `allocno_compare` ranks by roughly log2(n_refs) * freq / live_length, so a
 * LONGER live range means LOWER priority.  Unifying a counter across four
 * disjoint loops gives one pseudo whose live_length is the SUM of four ranges
 * -- the recorded "one variable per region" complement read the other way --
 * which drops it below the walkers and masks and lands it in r9/r10.  Thumb-1
 * cannot use r8-r11 as an ALU or `cmp` operand, so the ROM then PAYS for that
 * placement: `movs r1,#1 / add sl,r1 / mov r2,sl / cmp r2,#0x40` where a low
 * register needs only `adds r7,#1 / cmp r7,#0x40`.
 *
 * > A COUNTER IN A HIGH REGISTER IS NOT AN ACCIDENT AND IT IS NOT FREE.  When
 * > the reference spends two extra instructions per loop to keep a counter in
 * > r9/r10, that is the allocator telling you the counter is shared across MORE
 * > loops than you have written.  Our stream being SHORT is the signature.
 * > Read which register each loop's counter uses FIRST and let the repeats
 * > define the partition; do not infer it from what the loops mean.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (1) THE ORACLE: src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c (Anim_Fireball,
 *     matching) is the same animation family and supplied, verbatim in shape:
 *     `pp = g; base = *pp++; ctx = *pp;`, `cam = *(void **)((char *)g - 0x6c)`,
 *     `extern void *gPtrs[]` with `gPtrs[0x2e]`/`gPtrs[0x2f]`, the `DrawFn *fp`
 *     indirection over a two-element local array, `(int *)*_GetBattleActor(...)`
 *     with `extern int *_GetBattleActor(int)`,
 *     `MatrixSetLook(cam, (char *)cam + 0xc)`, the explicit
 *     `(unsigned)(frame - K) <= N` range test, and `Data_edeXX[w - 1]` with
 *     `w * 2` last.  First candidate: 8 short, 71.7% aligned.
 *
 * (2) THE COUNTER REPARTITION above, and in the SAME edit `cam2` deleted as a
 *     declared local.  71.7% -> 89.4%.  The `cam + 0xc` spill slot (sp+0x08)
 *     sits BELOW the gcse-created `&va` pointer (sp+0x0c), and a DECLARED local
 *     cannot rank below a gcse pseudo -- expand_decl runs first, so declared
 *     locals always get the LOWER pseudo numbers and therefore the HIGHER
 *     slots.  Writing `(char *)cam + 0xc` inline inside the frame loop makes it
 *     a loop.c hoist, created after gcse, and the two slots swap into place.
 *
 *     > THE SPILL-SLOT ORDER DATES THE PASS THAT CREATED THE PSEUDO, not just
 *     > the declaration order: parms, then declared locals in declaration
 *     > order, then cse/gcse pseudos, then loop.c's.  A slot BELOW a pseudo you
 *     > know gcse made cannot belong to anything you declared.
 *
 * (3) THE Anim_Fireball `f1` INTERMEDIATE, WORTH THE LAST INSTRUCTION AND THE
 *     EXACT SIZE: 709 -> 707, 91.2% -> 92.8%.  `d[0] = gPtrs[0x2e]; fp = d;
 *     fp[1] = gPtrs[0x2f];` makes gcc spill `fp` and RELOAD it to perform the
 *     `[fp,#4]` store, one instruction the ROM does not spend.  Naming the
 *     second pointer first --
 *
 *         d[0] = (DrawFn)gPtrs[0x2e];
 *         f1   = (DrawFn)gPtrs[0x2f];
 *         fp   = d;
 *         fp[1] = f1;
 *
 *     -- finishes the gPtrs chain BEFORE the `&d` address is formed, so the
 *     address is still live in the register when the store happens.  Moving
 *     `fp = d;` earlier instead measured WORSE (90.7%): the reload came back.
 *
 * (4) THE FUNCTION POINTER MUST BE ASSIGNED AFTER THE CALL THAT FEEDS IT:
 *     43 of 707, 92.8% -> 95.5%, and it fixed the ENTIRE prologue register
 *     rotation -- the first differing index jumped from 21 to 78.  The palette
 *     copy is `bl _call_via_r3` in the reference: r3 is CALL-CLOBBERED, so the
 *     pointer cannot be live across `GetFile`.  Written as
 *
 *         cp = Func_8001af8;
 *         cp(dst, GetFile(FILE_8e), 0x80);
 *
 *     the pseudo spans the GetFile call, needs a call-saved register, takes r5,
 *     and pushes `slot` to r6 and `crit` to r7 for the whole prologue -- a
 *     one-position rotation across sixty instructions, from one statement.
 *     Hoisting the call out first:
 *
 *         void *f = GetFile(FILE_8e);
 *         cp = Func_8001af8;
 *         cp(dst, f, 0x80);
 *
 *     puts the definition after the call, r3 suffices, and the rotation goes.
 *
 *     > A `_call_via_rN` VENEER NAMES THE REGISTER CLASS OF THE POINTER.  A LOW
 *     > veneer register (r0-r3, call-clobbered here) proves the pointer does
 *     > NOT cross a call, which fixes where its assignment can stand.  Read the
 *     > veneer register before writing the statement, not after.
 *
 * (5) TWO WALKING POINTERS, PARTITIONED THE WAY THE REGISTERS SAY:
 *     43 -> 23, 95.5% -> 98.4%.  Same lever as Anim_Djinni in this batch and
 *     the same reasoning: the reference's crit seeding loop and reseed loop
 *     BOTH walk in r5 while the first frame loop's inner walker is r6, so the
 *     two standalone loops share one variable (disjoint ranges, one pseudo --
 *     register inherited) and the inner loop needs its own.  I had it the other
 *     way round.  Nothing about what the loops DO suggests this grouping; only
 *     the repeated register does.
 *
 * (6) `i = 0;` BEFORE THE `base + K` WALKER at all three remaining preheaders
 *     (Anim_Vine's recorded "assign the base + K pointer LAST"): 23 -> 19,
 *     98.4% -> 98.6%.  The reference materialises the counter, the mask and the
 *     zero and only then emits `add r5, fp`.
 *
 * (7) `w = w >> 3; w = w + 2;` as TWO STATEMENTS, not `w = (w >> 3) + 2;`, for
 *     the second frame loop's sprite size, plus `frame = 0;` before the fade
 *     loop's `base + K` pointer: 89.4% -> 91.2% together.  The reference emits
 *     the two destructive in-place forms `asrs r6, r6, #3` then `adds r6, #2`;
 *     the single expression computes the shift into a temp and adds out of
 *     place.  One C statement per emitted instruction is what reproduces an
 *     in-place chain on a variable that is its own input.
 *
 * ================================================================
 * MECHANISMS READ OFF THE REFERENCE
 * ================================================================
 *
 *   - `slot` IS ASSIGNED TWICE, and the second assignment is load-bearing
 *     (first candidate 699 -> 701 instructions).  The reference forms
 *     `base + 0x7828` into r5 at the top, spends r5 on the `&d` address in the
 *     `fp` block, and then RE-FORMS it -- `ldr r5,=0x7828 / add r5,fp` -- for
 *     the LoadVFXFile sequence, where it survives six calls in a callee-saved
 *     register.  One variable, two live ranges, written as two assignments;
 *     inside the four loops the address is written out at every use instead,
 *     because there the reference re-computes it every time.  This is the
 *     recorded "the unit is the REGION" rule with the region boundary visible.
 *
 *   - `crit` IS A VARIABLE, NOT A RE-TEST: `movs r6,#1 / cmp r3,#0xc7 / bgt /
 *     movs r6,#0` and a `cmp r6,#1` three hundred instructions later.  Written
 *     as `crit = 1; if (f0 <= 0xc7) crit = 0;` -- the `bgt` skipping the store
 *     is what that spells, not `crit = (f0 > 0xc7)`.
 *
 *   - `shake = 0x40 - va.x` COMPILES THE 0x40 OUT OF THE LOOP COUNTER.  The
 *     reference emits `mov r0, sl / subs r0, r0, r3` with sl still holding `i`
 *     at its exit value: cse's `record_jump_equiv` knows i == 0x40 after
 *     `while (i != 0x40)`.  Same mechanism as the literal `0` inside
 *     `if (t == 0)` on Anim_Djinni, and the same rule -- write the LITERAL, not
 *     the variable; the variable would be a different program.  The fade
 *     loop's `strh` of 0x20 is the same thing off `frame`.
 *
 *   - `&va` IS NOT A DECLARED POINTER.  The reference computes `sp+0x64` into
 *     sp+0x0c inside the crit branch AND in the empty else arm -- that is
 *     gcse PRE inserting the address on the edge where it is not available,
 *     because `&va` is passed to GetBattleActorPos2 on both sides of the join.
 *     Writing `&va` and `va.x` textually produces both copies.  Note the
 *     reference then reads `va.x` THROUGH the pointer (`ldr r3,[r2]`) but
 *     `va.y` DIRECTLY (`ldr r2,[sp,#0x68]`): cse rewrites the offset-0 access
 *     because that MEM is literally `(mem (reg))`, and leaves the offset-4 one
 *     alone.  Both forms fall out of the same plain source.
 *
 *   - THE FOUR-WAY SPRITE CHAIN IS AN if / else-if CHAIN, not a switch: a run
 *     of `cmp rN,#1 / bgt`, `cmp rN,#3 / bgt`, `cmp rN,#5 / bgt`, `cmp rN,#7 /
 *     bgt` with the first three tails cross-jumped to one shared call site.
 *
 *   - `_Actor_SetAnimSpeed`'s actor sits in r8, so EVERY field access costs a
 *     `mov rN, r8` first.  The five saves are written in the order
 *     0x24, 0x28, 0x2c, 0x48, 0x34 but their SPILL SLOTS descend
 *     0x24, 0x20, 0x1c, 0x18, 0x14 -- so the DECLARATION order is
 *     0x24, 0x28, 0x2c, 0x34, 0x48 and the ASSIGNMENT order is not the same
 *     list.  Separating the two is the Anim_Djinni rule again.
 *
 *   - `((short *)q)`-style halfword traffic: `shake` lives in a spill slot and
 *     is stored to `iwram_3001ad0[2]` as a short, which is why the reference
 *     forms `add r1, sp, #0x30` before the `ldrh` -- Thumb has no sp-relative
 *     `ldrh`, so reload must materialise the address.  Nothing is owed there.
 *
 *   - `0xa0 << 19`, `0xef << 7`, `0xe1 << 7`, `0xe1 << 6`, `0x90 << 3`,
 *     `0xc8 << 4`, `0x80 << 7` and `0xc9 << 3` written as shifts, for the
 *     thumb `movs`+`lsls` constant splitter.
 *
 *   - THE 0x1f80 / 0x1f81 WRITES TO REG_BG1CNT ARE ALREADY RIGHT AND MUST NOT
 *     BE "FIXED".  We emit `ldrh r3, .LC` where the reference prints
 *     `ldr r3, .LC @ 0x1f80`; gas assembles the two identically, which is the
 *     recorded invisible-carrier case.  Chasing it once cost eight encodings on
 *     another function.  No `int` carrier is used here.
 *
 * ================================================================
 * MEASURED AND INERT / WORSE
 * ================================================================
 *   - naming `mask` for BOTH seeding loops (0xffff then 0xff, one variable):
 *     MUCH WORSE -- 709 instructions, size +4, aligncmp 86.6%.  Naming it for
 *     the second loop ONLY: byte-identical to not naming it at all.  So unlike
 *     Anim_Fireball, this function wants its masks left as literals; the
 *     constants reach their callee-saved registers through cse without help,
 *     and the extra live range only costs.
 *   - `fp = d;` moved above `d[0] = ...`: 90.7%, worse than lever (3)'s 92.8%.
 *
 * ================================================================
 * RULED OUT, with what was measured:
 *   - NOT a mis-read program.  Size and count exact; all 84 relocations present
 *     in the reference's order with only two veneer registers differing; I read
 *     the immediates in every differing hunk and not one constant, shift
 *     amount, structure offset or branch condition differs.
 *   - NOT sched1: it does not run in this build.
 *   - NOT declaration order: every spill offset in the frame is exact, both the
 *     five address-taken aggregates (0x64, 0x58, 0x4c, 0x40, 0x38) and the
 *     twelve slots from 0x34 down to 0x08.
 *   - NOT a FILE-STRUCTURE refusal: the split needs no exports and both dry
 *     runs are clean.
 *   - NOT the pin class: shimcount is zero and no pin was tried.
 *
 * NEXT MOVE: hunk A is now a two-function pattern with a named mechanism, so it
 * is worth attacking as a class rather than per function -- find the source
 * shape that makes a constant pseudo LOSE a register while a nearby pointer
 * keeps one, and two parks close at once.  The four transpositions are worth
 * less than that and should wait.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*ClearFn)(void *dst, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern short iwram_3001ad0[];
extern char *iwram_3001e74;
extern Part gBuffer[];
extern unsigned char ewram_2013840[];
extern unsigned short Data_ede5c[];

extern void _Func_80c0df4(int a, int b, int c);
extern void WaitFrames(unsigned int n);
extern void InitRenderTilemapBG1(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);
extern int *_GetBattleActor(int id);
extern int Random(void);
extern void _Actor_SetAnimSpeed(int *actor, int speed);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _PlaySound(int id);
extern void InitMatrixStack(void);
extern void MatrixRoll(int a);
extern void MatrixPitch(int a);
extern void MatrixYaw(int a);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *p, int a, int b);
extern void Func_80008d4(void *dst, int len);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern int _Func_80b8530(int id);
extern void _Func_80c0700(unsigned short a, int b);
extern void Func_80cdd14(void);
extern void gfree(int tag);

void Anim_CriticalHit(void *context)
{
    vec3_t va;
    vec3_t vb;
    vec3_t vc;
    vec3_t ve;
    DrawFn d[2];
    void *ctx;
    int shake;
    void *base2;
    void *cam;
    int s24;
    int s28;
    int s2c;
    int s34;
    int s48;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    State **slot;
    int *actor;
    int *actorB;
    int crit;
    int frame;
    int i;
    int h;
    Part *p;
    Part *q;
    CopyFn cp;
    int clen = 0x80 << 7;
    ClearFn cl;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    crit = 1;
    if (((State *)context)->f0 <= 0xc7) {
        crit = 0;
    }
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    _Func_80c0df4(((State *)context)->f8, ((State *)context)->fc, 0x82);
    WaitFrames(1);
    InitRenderTilemapBG1();
    REG_BG1CNT = 0x1f80;
    if ((*slot)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    }
    d[0] = (DrawFn)gPtrs[0x2e];
    f1 = (DrawFn)gPtrs[0x2f];
    fp = d;
    fp[1] = f1;
    slot = (State **)(base + 0x7828);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_49, base, 1, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_4a, gBuffer, 1, 1);
    if ((*slot)->f8 > 7) {
        void *f = GetFile(FILE_8e);
        cp = Func_8001af8;
        cp((volatile u16 *)(0xa0 << 19), f, 0x80);
    }
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_76, base2, 0, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    if (crit == 1) {
        actor = (int *)*_GetBattleActor((*slot)->f8);
        i = 0;
        p = (Part *)(base + (0xe1 << 7));
        do {
            p->x = (Random() & 0x3f) + 0x10;
            p->y = 0;
            p->z = 0;
            p->vx = Random() & 0xffff;
            p->vy = Random() & 0xffff;
            p->vz = Random() & 0xffff;
            i++;
            p++;
        } while (i != 0x40);
        _Actor_SetAnimSpeed(actor, 0);
        s24 = actor[9];
        s28 = actor[10];
        s2c = actor[11];
        s48 = actor[18];
        s34 = actor[13];
        actor[9] = 0;
        actor[10] = 0;
        actor[11] = 0;
        actor[13] = 0;
        actor[18] = 0;
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        shake = 0x40 - va.x;
        iwram_3001ad0[2] = shake;
        iwram_3001ad0[3] = 0x50;
        *(int *)(base + 0x77b4) = 0x18;
        *(int *)(base + 0x77b8) = 0;
        StartTask(Func_80cd4b4, 0xc8 << 4);
        _PlaySound(0xd4);
        frame = 0;
        do {
            _Func_80c0df4((*(State **)(base + 0x7828))->f8,
                          (*(State **)(base + 0x7828))->fc, 0x82);
            i = 0;
            q = (Part *)(base + (0xe1 << 7));
            do {
                if (q->x >= 0 && frame >= i / 4) {
                    int w = (i & 1) + 5;
                    InitMatrixStack();
                    MatrixRoll(q->vz);
                    MatrixPitch(q->vx);
                    MatrixYaw(q->vy);
                    Func_80e3944((vec3_t *)q, &vc);
                    vc.x = vc.x + 0x40;
                    vc.y = vc.y + va.y + 0x18;
                    if (vc.z < -0x3c) {
                        vc.z = -0x3c;
                    }
                    if (vc.z > 0x3c) {
                        vc.z = 0x3c;
                    }
                    vc.z = vc.z + 0x3c;
                    d[0](ctx, (char *)base2 + Data_ede5c[w - 1], vc.x - w,
                         vc.y - w, w * 2, w * 2);
                    q->x = q->x - 4;
                }
                i++;
                q++;
            } while (i != 0x40);
            *(int *)(base + 0x7824) = 1;
            WaitFrames(1);
            frame++;
        } while (frame != 0x20);
        StopTask(Func_80cd4b4);
        _Actor_SetAnimSpeed(actor, 0x10);
        actor[9] = s24;
        actor[10] = s28;
        actor[11] = s2c;
        actor[13] = s34;
        actor[18] = s48;
    }
    cl = Func_80008d4;
    cl(ctx, clen);
    cl((void *)0x6004000, clen);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    REG_BG1CNT = 0x1f81;
    GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &vb);
    if ((*(State **)(base + 0x7828))->f4 == 0) {
        shake = 0x20 - vb.x;
    } else {
        shake = 0x60 - vb.x;
    }
    if (shake > 0) {
        shake = 0;
    }
    if (shake < -0x80) {
        shake = -0x80;
    }
    vb.x = vb.x + shake;
    iwram_3001ad0[3] = 0x50;
    iwram_3001ad0[2] = shake;
    actorB = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    h = _Func_80b8530((*(State **)(base + 0x7828))->ids[0]) / 2;
    i = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = actorB[2];
        p->y = actorB[3] + h;
        p->z = actorB[4];
        p->vx = (Random() & 0xff) << 10;
        p->vy = (Random() & 0xff) << 10;
        p->vz = ((Random() & 0xff) - 0x7f) << 10;
        if (p->x > 0) {
            p->vx = -p->vx;
        }
        p->t = i + 0x10;
        i++;
        p++;
    } while (i != 0x40);
    frame = 0;
    do {
        if (frame == 5) {
            _Func_80bd7dc(0x86);
        }
        if (frame == 4) {
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 0);
        }
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        va.y = va.y + 0x10;
        if (frame <= 1) {
            d[0](ctx, base, 0, 0, 0x78, 0x78);
        } else if (frame <= 3) {
            d[0](ctx, base + (0xe1 << 6), 0, 0, 0x78, 0x78);
        } else if (frame <= 5) {
            d[0](ctx, gBuffer, 0, 0, 0x78, 0x78);
        } else if (frame <= 7) {
            d[0](ctx, ewram_2013840, 0, 0, 0x78, 0x78);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        if ((unsigned)(frame - 4) <= 0x1b) {
            i = 0;
            do {
                int m = i / 2;
                Part *r = (Part *)(base + m * 28 + (0xe1 << 7));
                int w = r->t;
                if (w > 0) {
                    Func_80e3944((vec3_t *)r, &ve);
                    ve.x = ve.x + shake;
                    w = w >> 3;
                    w = w + 2;
                    ve.y = ve.y + 0x10;
                    fp[m & 1](ctx, (char *)base2 + Data_ede5c[w - 1],
                              ve.x - w, ve.y - w, w * 2, w * 2);
                    Func_80e38b8(r, 0x3c, -0x400);
                    r->t = r->t - 1;
                }
                i++;
            } while (i != 0x40);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x20);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    iwram_3001ad0[3] = 0x20;
    {
        unsigned short *t;
        frame = 0;
        t = (unsigned short *)(iwram_3001e74 + (0xc9 << 3));
        do {
            _Func_80c0700(*t, 6 - frame);
            WaitFrames(1);
            frame++;
        } while (frame != 7);
    }
    Func_80cdd14();
}
