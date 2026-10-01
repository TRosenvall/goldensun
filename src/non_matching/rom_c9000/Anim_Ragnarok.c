/* Anim_Ragnarok (asm/rom_c9000/rom_e7320_c_c.s:4147, 0x080e94b8, 561 ROM
 * instructions) --
 * NON-MATCHING, 296 of 584 encodings differ.
 *
 * SIZE IS EXACT (1288 bytes both sides).  COUNT IS 583 AGAINST 584 -- ONE SHORT
 * -- so the objcmp figure above is SATURATED and is not a distance.  Rank this
 * file with tools/aligncmp.py: 83.0% aligned-equal (485 of 584, 113 in 59
 * hunks).  That is an aligncmp figure and must never be quoted on the claim
 * line.
 *
 * THE RELOCATION SYMBOL SEQUENCE IS EXACT: all 49 relocations, same symbols in
 * the same order.  Only the OFFSETS differ, and they differ for one reason --
 * the ROM dumps a THIRD constant pool early (a gcc-generated `b .Le954c` over
 * words at 0x78..0x94) and this candidate dumps only two.  A relocation FORM /
 * offset difference is not a residue; the sequence is the confirmation that the
 * program shape, the argument order and every call target are right.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Ragnarok.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func Anim_Ragnarok
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT.  *** THE EXPORTS LANDED IN BATCH 312 AND THE
 * REFUSAL BELOW IS STALE. ***  All twelve `.global` lines named by the tool are
 * now in asm/rom_c9000/rom_e7320_c_c.s, each immediately before its label, in
 * one build+compare-gated commit, and `split_s.py --dry-run` now SUCCEEDS on
 * this file (confirmed on BaseAnim_Meteor in batch 313: it prints the three-way
 * 131 / 1750 / 3765 shape).  So EVERY split of this eight-function file is now
 * free of asm work -- for all eight members, not just this one.
 *
 * *** SUFFIX COLLISION, AND IT IS A REAL BLOCKER FOR WHICHEVER GOES SECOND. ***
 * Anim_Annihilation, Anim_Ragnarok and BaseAnim_Meteor each have a park naming
 * `rom_e7320_c_c_b` as their own output stem, and ONLY ONE CAN HAVE IT.  The
 * stem a cut produces depends on the member's POSITION, so the three claims are
 * mutually exclusive rather than merely duplicated.  Whoever converts first
 * takes `_b`; every later conversion of this file must RE-DERIVE its stems from
 * a fresh `--dry-run` against the then-current file, not from the shape written
 * in its own park.  Check this before writing linker rows.
 *
 * The historical reading, kept because the export list is still the right list:
 * tools/datacheck.py says the stem has a `.rodata` section and that
 * Anim_Ragnarok reads `.Leef06` and `.Leef0c` -- and nothing else.
 * `split_s.py --dry-run` USED TO REFUSE until they were exported, naming ten
 * labels across the whole file:
 *
 *     .global .Leee76  .global .Leeea0  .global .Leeebc  .global .Leeeca
 *     .global .Leeed8  .global .Leeee1  .global .Leeeea  .global .Leeef8
 *     .global .Leef06  .global .Leef0c
 *
 * Only the last two are needed BY THIS FUNCTION; the first eight are
 * BaseAnim_Meteor's and Anim_Ramses's and are refused only because the tool cuts
 * the file three ways at once.  rom_e7320_c_c.s holds EIGHT functions --
 * Func_80e7338, Func_80e73a0, BaseAnim_Meteor, Anim_Ramses, Anim_DragonCloud,
 * Anim_Annihilation, Anim_Ragnarok, Anim_TitanBlade -- and Anim_Ragnarok is the
 * SEVENTH, so the cut is
 *
 *     asm/rom_c9000/rom_e7320_c_c_a.s   the first six  (unchanged asm)
 *     src/rom_c9000/rom_e7320_c_c_b.c   THIS FILE
 *     asm/rom_c9000/rom_e7320_c_c_c.s   Anim_TitanBlade + the .rodata section
 *
 * Every data label lives in the LAST piece, which is why the eight
 * non-Ragnarok references become cross-file.  A `.global` emits no bytes:
 * export all ten, run `make compare` BEFORE the split so the two changes stay
 * separable, then split.  Anim_Annihilation (same file, reads no data label) is
 * ALREADY PARKED at src/non_matching/rom_c9000/Anim_Annihilation.c and claims
 * the `_b` stem for a cut one function earlier -- COORDINATE THE SUFFIXES if
 * both land.
 *
 * SHIMS: `python3 tools/shimcount.py` prints the filename and nothing else --
 * ZERO register pins, zero `.equ` shims, zero `"+r"` barriers.  PIN-FREE.  No
 * fakematch.txt row, no per-file Makefile flag override.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN ORDER, WITH FIGURES (aligncmp % of ref)
 * ================================================================
 *
 * First candidate straight off the disassembly: 62.2%, size -12, count -6, and
 * THE RELOCATION SEQUENCE ALREADY EXACT.  Then, each measured alone:
 *
 *   1. 62.2 -> 63.2  ONE POINTER FOR iwram_3001eec, NOT THE SYMBOL TWICE.
 *      The ROM reads `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r0} /
 *      ldr r3,[r3] / ldr r2,[r2,#8]` -- tbl[2] comes off the register the
 *      symbol was loaded into.  Writing `iwram_3001eec[2]` reloads the symbol.
 *      `tbl = iwram_3001eec; pp = tbl; base = *pp++; ctx = *pp; gfx = tbl[2];`
 *      is the shape (the Anim_Annihilation idiom), worth one pool load.
 *      Bundled with it: `int k = i * 2;` NAMED BEFORE the Random() call, because
 *      the ROM computes `lsl r6, r0, #1` BEFORE `bl Random` -- a value computed
 *      earlier than its use means the source named it (Anim_Ice's ax/ay lever).
 *      Unnamed it recomputed `lsl` twice, once per trig product.
 *
 *   2. 63.2 -> 68.8 AND THE FRAME FROM 0x38 TO THE ROM'S 0x34.  THE INIT LOOP'S
 *      TWO OUTER QUANTITIES ARE `j *` EXPRESSIONS, NOT HAND-WRITTEN
 *      ACCUMULATORS.  Written `bp += 0x1c0` / `idx += 0x154` at the latch, gcc
 *      makes `bp` a biv and then STRENGTH-REDUCES `&gBuffer[idx]` into a scaled
 *      pointer accumulator that needs a TENTH spill word -- which is the whole
 *      4-byte frame gap, and every `[sp,#N]` in the function is wrong while it
 *      is there.  Written `base + j * 0x1c0 + (0xe1 << 7)`, loop.c builds the
 *      ROM's giv instead: `mov r8, r11` in the preheader, `add r8, r0` at the
 *      latch, and `mov r7,#0xe1 / lsl r7,#7 / add r7,r8` INSIDE the loop --
 *      byte for byte the ROM's, including the fact that 0x7080 is NOT folded
 *      into the giv.  Nine spill words, frame 0x34, slot map exact.
 *
 *   3. 68.8 -> 69.5  MUL OPERAND ORDER: `sin(t) * k`, NOT `k * sin(t)`.
 *      Thumb `mul` is destructive, so the first operand is the one that lands in
 *      the destination.  The ROM has `mov r3, r6 / mul r3, r0` (the named
 *      multiplier into the destination, the call result as the source); ours had
 *      the two swapped.  Four sites, and the SAME flip paid again at step 8.
 *
 *   4. 69.5 -> 70.4  `h` CARRIES ITS OWN 0x80 -- `h = h - y - 0x18`, NOT
 *      `h = 0x80 - y - 0x18`.  fold folds `0x80 - 0x18` to 0x68 and emits
 *      `mov r3,#0x68 / sub r1,r3,r5`; the ROM emits `sub r3, r1, r5 / sub r1,
 *      #0x18` off the register that already holds h's value.  The same reading
 *      applies to the other 0x80 in the block: `x = h - frame * 10` in the
 *      f4 != 1 arm, where the ROM's `sub r2, r1, r3` reuses the same register.
 *      This is the LITERAL-IS-NOT-THE-VARIABLE rule used constructively: h is a
 *      real variable initialised to 0x80 before the if-chain, and both
 *      subtractions read it rather than respelling the constant.
 *
 *   5. 69.5 -> 72.4  DECLARATION ORDER IS THE SPILL-SLOT MAP, AND a/b/c/d ARE
 *      DECLARED INSIDE THE FRAME LOOP.  reload assigns spill slots in ascending
 *      pseudo number and the frame grows down, so the ROM's
 *      ctx 0x28 / j 0x24 / gfx 0x20 / actor 0x1c / &fns 0x18 / a 0x14 / b 0x10 /
 *      c 0x0c / d 0x08 reads off as a creation order -- and the `&fns` COMPILER
 *      TEMP, created at the BuildDraw2DFuncs call, sits BETWEEN actor and a.
 *      A temp cannot outrank a function-level declaration, so a/b/c/d are not
 *      function-level: they are block-scoped in the frame-loop body.  Declaring
 *      ctx, j, gfx, actor in that order at the top and a/b/c/d in the loop puts
 *      all nine slots on the ROM's offsets.
 *
 *   6. 73.3 -> 76.9  NO int CARRIER FOR REG_BLDALPHA.  The recorded lever --
 *      "an `int` carrier for a vu16 register write whose constant the ROM pools
 *      as a WORD" -- is WRONG HERE and is worth 3.6 points backwards.  The ROM's
 *      `ldr r2,=REG_BLDALPHA / ldr r3,.Le9534 @ 0x1010 / strh r3,[r2]` comes out
 *      of a PLAIN `REG_BLDALPHA = 0x1010;`: gcc has no PC-relative `ldrh`, so it
 *      pools the constant in SImode by itself.  The explicit `int alpha = 0x1010`
 *      carrier assigns the value first and so loads the VALUE into the lower
 *      register and the ADDRESS into the higher, inverting the ROM's pair.
 *      Check the precondition before reaching for the carrier: it is for a
 *      constant gcc would otherwise BUILD, not one it already pools.
 *      NOTE A CROSS-FUNCTION CONTRADICTION, deliberately left standing: batch
 *      307's BaseAnim_Attack brief measured the carrier HELPING on the same
 *      constant 0x1010, and narrowed the lever to "works where the constant is
 *      not thumb_shiftable_const".  0x1010 is not shiftable (0x101 is nine
 *      bits) on either side, so shiftability does not separate the two results.
 *      The carrier is therefore conditional on something further: here gcc
 *      pools 0x1010 WITHOUT help and the carrier only inverts which of the
 *      address/value pair gets the lower register.  Measure it, do not assume
 *      it, and do not take either result as the family default.
 *
 *   7. 77.9% AND SIZE EXACT  THE FRAME LOOP'S a, c AND d ARE `j *` EXPRESSIONS.
 *      Same mechanism as step 2, one level out: `c = j * 8 + 0x10` becomes the
 *      giv at sp+0x0c with `add #8` at the latch and its init hoisted to the
 *      preheader, and -- the tell that decided it -- `j * 8` is then computed
 *      AT THE TOP OF THE LOOP BODY for the `frame < j * 8 + 0x12` test, which is
 *      what the ROM's otherwise-unmotivated `lsl r1, r2, #3` at .Le9742 is.
 *      Accumulators cannot produce that: they never re-derive j * 8.
 *      This is the step that closed the size gap: 1288 against 1288.
 *
 *   8. 77.9 -> 81.5  AND `b` IS THE ONE THAT IS NOT.  Written `b = j * 5`, fold
 *      associates the region-3 index `b * 68` into `j * 0x154` and loop.c then
 *      reduces the whole `&gBuffer[b * 68 + i]` address to an inner-loop pointer
 *      walk -- the `lsl #4 / add / lsl #2 / add i / lsl #3 / sub / lsl #2` that
 *      the ROM recomputes EVERY ITERATION disappears entirely (ref 10 encodings
 *      against our 4).  Written as a plain accumulator `b += 5`, neither
 *      reduction fires and the ROM's per-iteration form comes back exactly,
 *      including `ldr r7,[sp,#0x10]` hoisted once before the loop.
 *      SO THE MIX IS THE FINDING, NOT EITHER FORM: a, c, d as `j *` expressions
 *      and b as an accumulator, IN THE SAME LOOP.  Both uniform choices are
 *      worse -- all four as `j *` reads 77.9%, and making a an accumulator too
 *      costs 1.5 points back (80.8%).
 *
 *   9. 81.5 -> 82.4  `int r = 0x1ff; r &= Random();` RATHER THAN
 *      `int r = Random() & 0x1ff;`.  Thumb `and` is destructive and commutative,
 *      so which operand reaches the destination is a source-order question: the
 *      ROM's `ldr r5,=0x1ff / and r5, r0` puts the CONSTANT in the destination,
 *      which is what a leading `r = 0x1ff` gives.  Note the ROM does NOT do this
 *      for the `0xffff` mask three instructions later (`mov r6,r0 / and r6,r3`),
 *      and that asymmetry is real, not noise -- `r` is later modified in place
 *      (`add r5,#0x20`) and the 0xffff value is not.  Spelling both the same way
 *      is worse.  Also measured: applied on top of the pre-step-7 baseline this
 *      lever was 0.2 points NEGATIVE, so it only pays once the givs are right.
 *
 *  10. 82.4 -> 83.0  The step-3 mul flip again, on the second inner loop's
 *      `p->vx = (sin(t) * r) >> 6` and `p->vy = -((cos(t) * r) * 2) >> 6`.
 *
 * MEASURED AND REJECTED (all at the baseline named):
 *   - `int idx = j * 0x154; p = &gBuffer[idx];` -- the named local defeats
 *     fold's associate as intended, and that is exactly what LETS loop.c reduce
 *     the address: 68.8 -> 63.2 and the tenth spill word returns.  The fold is
 *     load-bearing here.
 *   - the same as a block-scoped accumulator: 76.9 -> 65.6.
 *   - re-tried once more at the 81.5% baseline: 71.6% and size +8.
 *   - `p = gBuffer + j * 0x154` (pointer arithmetic instead of the array
 *     subscript): byte-identical to `&gBuffer[...]`.  INERT, so UNTESTED as a
 *     hypothesis, not disproved.
 *   - `int n = b * 68 + i; Part *g = &gBuffer[n];` -- INERT (77.9 both ways).
 *     The named index does not give the address pseudo a second set and so does
 *     not block `consec_sets_giv`.
 *   - naming the `.Leef0c` table pointer before the region-2 loop: 83.0 -> 82.7.
 *   - swapping the c/d declaration order in the j loop: 83.0 -> 82.0.
 *
 * ================================================================
 * NAMED BLOCKER: loop.c's strength_reduce DECLINES THE `j * 0x154` GIV IN THE
 * ROM AND TAKES IT HERE -- AND IT IS THE ONLY STRUCTURAL RESIDUE LEFT
 * ================================================================
 *
 * Four hunks, one cause.  The ROM keeps the init loop's gBuffer ELEMENT INDEX
 * unscaled in r9 -- `mov r9, r2` (r2 = 0) in the preheader, `add r9, r3` at the
 * latch reusing the register that already holds the inner bound 0x154 -- and
 * scales it AT THE USE with `mov r1,r9 / lsl r3,r1,#3 / sub r3,r3,r1 /
 * lsl r3,#2` plus `add r7, r3, gBuffer`.  Eight encodings.
 *
 * This candidate instead lets fold associate `0x154 * 28` into the single
 * constant 0x2530 and emits `ldr r3,=0x2530 / ldr r0,[sp,#0x24] / mov r2,r0 /
 * mul r2,r3 / ldr r3,=gBuffer / add r7,r2,r3` -- ten encodings, a real `mul`,
 * and no induction variable at all.  Net over the four hunks we are ONE
 * instruction short, which is the whole count gap.
 *
 * WHAT RULES OUT THE ALTERNATIVES.  It is not scheduling: sched1 does not run in
 * this build and the residue is a different set of OPCODES, not an order.  It is
 * not allocation: the surviving register in the ROM is a high one (r9) that this
 * candidate has free and uses for other things in the same range, and the frame
 * and all nine spill slots already agree.  It is not the fold itself in
 * isolation: defeating the fold with a named local (measured, above) produces
 * the giv AND the address reduction together, which is worse, so the ROM's
 * compiler must have taken the fold and then DECLINED the address giv.  Three
 * of the four source shapes for one quantity are therefore eliminated, and what
 * is left is the threshold inside `strength_reduce` -- `v->lifetime * threshold
 * * benefit < insn_count`, whose `threshold` term depends on `n_non_fixed_regs`
 * and on whether the loop contains a call.  That is the same suspicion
 * REG_ALLOC_ORDER carries for the allocation parks: a compiler-configuration
 * difference this tree cannot test without rebuilding gcc-2.96.
 *
 * The step-8 finding is the evidence that makes this the right reading rather
 * than a guess: in region 3 the SAME address giv is declined by gcc here as
 * soon as the outer quantity is an accumulator, and the ROM's unreduced form
 * comes back instruction for instruction.  So the pass is reachable from source
 * -- it just is not reachable for THIS quantity, whose only use is the address.
 *
 * SECOND RESIDUE, cosmetic and downstream of nothing: the ROM's third constant
 * pool.  gcc's own minipool logic inserts `b .Le954c` over six words at 0x78 in
 * the ROM and does not here; every remaining `ldr rN,[pc,#imm]` hunk in this
 * file is that one pool's displacement and nothing else.  It should close on its
 * own when the count does -- do not chase it separately.
 *
 * THIRD, one register pair: region 3 holds `g` in r6 and `gx` in r5 where this
 * candidate has them the other way round, and reloads `b` from its slot twice
 * where the ROM loads it once into r7.  Worth ~6 encodings.  The r0 donor lever
 * is the thing to try: `Func_80e3908(g, ...)`'s first argument dies at that
 * call, so `g` is a donor candidate for the value the ROM keeps in r6.
 *
 * WHAT IS ALREADY RIGHT AND READ OFF THE REFERENCE, so do not re-derive it:
 *   - THE PROLOGUE NAMES ITS ARGUMENT.  Unlike every sibling in this bank, this
 *     entry point keeps the State * in r5, reads `st->f8` and WRITES
 *     `st->f18 = 1` THROUGH IT, and only then stores `*(base + 0x7828) = st`.
 *     The bank's "re-derive base+0x7828, never name it" rule does not apply to
 *     the prologue -- but it DOES apply everywhere inside the frame loop, where
 *     the ROM re-derives the slot at five separate sites, and writing it out at
 *     each is what matches.
 *   - `*(int *)(base + 0x77a8) = frame;` at frame == 8 stores the VARIABLE, not
 *     the literal 8 -- the ROM's `str r0,[r3]` reuses the register the compare
 *     left holding frame.  Writing `= 8` invites a cross-jump the ROM does not
 *     have.
 *   - `q->t = t > 0 ? t - 1 : -1;` as a TERNARY: the ROM sets r3 in both arms
 *     and has ONE `str r3,[r5,#0x18]`.  Two `if` bodies give two stores.
 *   - `(unsigned int)t <= 0x11` for `cmp r0,#0x11 / bhi`, with a SIGNED
 *     `__divsi3` for `t / 3` immediately after -- the cast is on the guard only.
 *   - the two `ldrsh` reads at OFFSET 2 and OFFSET 6 of the Part are the high
 *     halves of the 16.16 x and y: `*(short *)((char *)q + 2)` /
 *     `+ 6`.  Thumb has no immediate-offset `ldrsh`, so the `mov r3,#6` beside
 *     it is forced, not a named-offset lever.
 *   - `region 4` is the one place the offset IS a lever: the ROM holds 0x7828 in
 *     a register and uses it three times (`ldr r3,[r0,r2]` twice, `add r5,r3,r2`
 *     once), which is the reg+reg addressing mode.  It comes out of the plain
 *     `*(State **)(base + 0x7828)` spelling here because the expression already
 *     has three uses in that region -- the derived-address-needs-more-than-one-
 *     use precondition is satisfied without naming anything.
 *   - the region-4 loop is a real `while` (guard on `(*slot)->f14` plus a bottom
 *     test), not a do-while: that guard IS jump.c's duplicate_loop_exit_test
 *     copy and a do-while can never reach it.
 *   - `0xd8 << 15`, `0x80 << 5`, `0x80 << 6`, `0xe1 << 7`, `0xef << 7`,
 *     `0x90 << 3`, `0xb0 << 15` all left UNFOLDED: each is gcc's own
 *     `mov #k / lsl #n` const synth and folding them pools a word instead.
 *   - `n = t / 5 + 1` is computed from the ALREADY-DECREMENTED t, and `g->y` is
 *     read ONCE into a local before `g->t` is stored -- the ROM's load order is
 *     t, y, sub, store, so the y read cannot be re-derived after the store.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned char Leef06[] __asm__(".Leef06");
extern unsigned char Leef0c[] __asm__(".Leef0c");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void WaitFrames(unsigned int n);
extern void **_GetBattleActor(int id);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _Func_80bd7dc(int a);
extern void _PlaySound(int id);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void _Actor_SetAnim(void *actor, int anim);
extern void _Actor_SetAnimSpeed(void *actor, int speed);
extern void Func_80e46f0(int id);
extern void Func_80e3908(Part *g, int a, int b);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);

void Anim_Ragnarok(State *st)
{
    DrawFn fns[2];
    void *ctx;
    int j;
    unsigned char *gfx;
    void *actor;
    void **tbl;
    void **pp;
    unsigned char *base;
    State **slot;
    int i;
    int frame;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    gfx = (unsigned char *)tbl[2];
    slot = (State **)(base + 0x7828);
    actor = *_GetBattleActor(st->f8);
    st->f18 = 1;
    *slot = st;
    AnimStart(1);
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncs((*slot)->f4, (void **)fns);
    _Actor_SetAnim(actor, 2);
    _Actor_SetAnimSpeed(actor, 0x30);
    LoadVFXFile(FILE_55, base, 1, 1);
    LoadVFXFile(FILE_7d, base + (0x80 << 6), 1, 0);
    LoadVFXFile(FILE_73, gfx, 0, 0);

    j = 0;
    do {
        Part *q = (Part *)(base + j * 0x1c0 + (0xe1 << 7));
        Part *p;
        i = 0;
        do {
            int k = i * 2;
            int t = Random() & 0xffff;
            q->x = sin(t) * k;
            q->y = -(cos(t) * k);
            q->t = i / 2 + 0x19;
            i++;
            q++;
        } while (i != 0x10);
        p = &gBuffer[j * 0x154];
        i = 0;
        do {
            int r = 0x1ff;
            int t;
            r &= Random();
            t = Random() & 0xffff;
            p->x = Leef06[j + (*(State **)(base + 0x7828))->f4 * 3] << 16;
            p->y = 0xb0 << 15;
            r += 0x20;
            p->vx = (sin(t) * r) >> 6;
            p->vy = -((cos(t) * r) * 2) >> 6;
            p->t = (Random() & 7) + 0x20;
            i++;
            p++;
        } while (i != 0x154);
        j++;
    } while (j != 3);

    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);

    frame = 0;
    do {
        int b;
        if (frame == 4) {
            _PlaySound(0xd4);
        }
        if (frame == 8) {
            *(int *)(base + 0x77a8) = frame;
        }
        if (frame == 0x12) {
            _PlaySound(0x91);
        }
        if (frame == 0x28) {
            _Func_80bd7dc(0x86);
        }
        if (frame <= 0x27) {
            int x, y;
            int h = 0x80;
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                if (frame <= 9) {
                    x = frame * 10 - 8;
                    y = frame * 16 - 0x80;
                } else if (frame > 0x14) {
                    x = frame + 0x3e;
                    y = frame * 2 - 0x18;
                } else {
                    x = 0x52;
                    y = 0x10;
                }
            } else {
                if (frame <= 9) {
                    x = h - frame * 10;
                    y = frame * 16 - 0x80;
                } else if (frame > 0x14) {
                    x = 0x3a - frame;
                    y = frame * 2 - 0x18;
                } else {
                    x = 0x26;
                    y = 0x10;
                }
            }
            if (y + 0x80 > 0x68) {
                h = h - y - 0x18;
            }
            if (h > 0) {
                fns[0](ctx, base, x - 0x20, y, 0x40, h);
            }
        }
        if (frame > 0x10) {
            Func_80e46f0(FILE_c0);
        }
        b = 0;
        j = 0;
        do {
            int a = j * 8 + 0x16;
            int c = j * 8 + 0x10;
            unsigned char *d = base + j * 0x1c0;
            if (frame == c) {
                *(int *)(base + 0x77a8) = 0xc;
            }
            if (frame >= c) {
                Part *q;
                if (frame < j * 8 + 0x12) {
                    fns[0](ctx, base + (0x80 << 6),
                           Leef06[j + (*(State **)(base + 0x7828))->f4 * 3] - 0x10,
                           0x38, 0x20, 0x40);
                }
                q = (Part *)(d + (0xe1 << 7));
                i = 0;
                do {
                    int qy = *(short *)((char *)q + 6);
                    int xx = *(short *)((char *)q + 2)
                             + Leef06[j + (*(State **)(base + 0x7828))->f4 * 3];
                    int t = q->t;
                    if ((unsigned int)t <= 0x11) {
                        fns[0](ctx,
                               base + (Leef0c[t / 3] << 11) + (0x80 << 6),
                               xx - 0x10, qy + 0x38, 0x20, 0x40);
                        t = q->t;
                    }
                    q->t = t > 0 ? t - 1 : -1;
                    i++;
                    q++;
                } while (i != 0xc);
            }
            if (frame > c + 5) {
                i = 0;
                do {
                    Part *g = &gBuffer[b * 68 + i];
                    if (g->t > 0) {
                        int t;
                        int gy;
                        Func_80e3908(g, 0x40, 0x80 << 5);
                        t = g->t - 1;
                        gy = g->y;
                        g->t = t;
                        if (gy > (0xd8 << 15)) {
                            g->vy = -g->vy / 2;
                        } else if ((unsigned int)g->x <= 0x7effff && gy >= 0) {
                            int yy = gy >> 16;
                            int gx = g->x >> 16;
                            int n = t / 5 + 1;
                            int n2 = n * 2;
                            fns[i & 1](ctx, gfx + Data_ede48[n - 1],
                                       gx - n / 2, yy - n, n, n2);
                        }
                    }
                    i++;
                } while (i != 0x100);
            }
            i = 0;
            if ((*(State **)(base + 0x7828))->f14 != 0) {
                do {
                    if (frame == a) {
                        State **sl = (State **)(base + 0x7828);
                        Func_80d6888((*sl)->ids[i], 7, 5, i, 0xa);
                        _SetBattleActorKnockback((*sl)->ids[i], 4);
                    }
                    i++;
                } while (i != (*(State **)(base + 0x7828))->f14);
            }
            b += 5;
            j++;
        } while (j != 2);
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x50);
    _Actor_SetAnimSpeed(actor, 0x10);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
