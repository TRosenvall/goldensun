/* Anim_Ramses -- NON-MATCHING, 756 of 875.
 *
 * SIZE  NOT exact: ref 1968 bytes, ours 1964  (-4)
 * COUNT NOT exact: ref 875 encodings, ours 874  (-1)
 * Both figures inexact, so objcmp's 756 is SATURATED and carries no distance.
 * aligncmp, reported separately and with its own masking rules: 517 of 875
 * aligned-equal = 59.1 percent of the reference, 449 differing in 160 hunks.
 *
 * RELOCATION SYMBOL SEQUENCE MATCHES THE REFERENCE ENTRY FOR ENTRY -- all 71
 * entries, same symbols in the same order; the only disagreement is the byte
 * OFFSET of each, because the literal pools sit two bytes earlier throughout.
 * That is the cheapest confirmation available that the program shape is right:
 * every call, every actor slot, every data label and both _call_via_r4
 * indirect sites are correct. Both sides carry exactly two _call_via_r4
 * veneers, so no veneer-count defect.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Ramses.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func Anim_Ramses
 *
 * and for the aligned figure, which is the one that moves on this function:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Ramses.c \
 *     asm/rom_c9000/rom_e7320_c_c.s Anim_Ramses
 *
 * SHIMS: NONE. `python3 tools/shimcount.py <this path>` reports zero -- the
 * candidate is PIN-FREE, no register pin, no asm shim, no fakematch.txt row.
 * No per-file Makefile flag override applies and none was screened as needed:
 * the residue below is a reload decision, not a pass gcc can be asked to skip.
 *
 * ================================================================
 * SPLIT SHAPE -- and the recon doc UNDERSTATED THE EXPORTS
 * ================================================================
 *
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e7320_c_c.s`:
 *
 *     data sections : .rodata
 *     functions     : Func_80e7338, Func_80e73a0, BaseAnim_Meteor, Anim_Ramses,
 *                     Anim_DragonCloud, Anim_Annihilation, Anim_Ragnarok,
 *                     Anim_TitanBlade
 *     -> converting a function here needs a TEXT/DATA SPLIT; the data must keep
 *        its own object.
 *     Anim_Ramses reads .Leeed8, .Leeee1, .Leeeea, .Leeef8
 *
 * `python3 tools/split_s.py asm/rom_c9000/rom_e7320_c_c.s Anim_Ramses --dry-run`
 * REFUSES, and it names EIGHT labels, not the four in
 * docs/recon-959_200a7b0-Anim_Ramses.md:
 *
 *     rom_e7320_c_c_a.s references .Leee76, .Leeea0, .Leeebc, .Leeeca
 *     rom_e7320_c_c_b.s references .Leeed8, .Leeee1, .Leeeea, .Leeef8
 *     all eight defined in rom_e7320_c_c_c.s
 *
 * The extra four are BaseAnim_Meteor's, which lands in the _a piece -- a
 * three-way split puts the data in _c, so the _a piece crosses the file
 * boundary too even though BaseAnim_Meteor is not being converted. So the
 * export list to add to asm/rom_c9000/rom_e7320_c_c.s is EIGHT lines:
 *
 *     .global .Leee76   .global .Leeea0   .global .Leeebc   .global .Leeeca
 *     .global .Leeed8   .global .Leeee1   .global .Leeeea   .global .Leeef8
 *
 * A .global emits no bytes. Per the tree's discipline, add them and verify
 * `make compare` is still GREEN BEFORE running the split, so the export and the
 * split stay separable.
 *
 * Resulting shape (Anim_Ramses is the FOURTH of eight functions, so the target
 * piece is _b and everything after it stays asm):
 *
 *     asm/rom_c9000/rom_e7320_c_c_a.s   Func_80e7338 + Func_80e73a0
 *                                       + BaseAnim_Meteor
 *     src/rom_c9000/rom_e7320_c_c_b.c   THIS FILE, once it matches
 *     asm/rom_c9000/rom_e7320_c_c_c.s   Anim_DragonCloud, Anim_Annihilation,
 *                                       Anim_Ragnarok, Anim_TitanBlade and the
 *                                       whole .rodata block
 *
 * and stage1.ld's `asm/rom_c9000/rom_e7320_c_c.o` lines become three, in that
 * order. ALWAYS run split_s.py with --dry-run first: it DELETES a tracked .s
 * and REWRITES a linker script.
 *
 * THE ASM-LABEL CAPTURE HAZARD IS SCREENED, NOT ASSUMED. The screen is
 * "contains a hex letter", because gcc's own label counter is DECIMAL. All
 * eight names carry letters -- eee76, eeea0, eeebc, eeeca, eeed8, eeee1,
 * eeeea, eeef8 -- so gcc cannot mint a colliding .LNNN. The five-digit length
 * is not what makes them safe; the letters are.
 *
 * ================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * The band doc's prediction for the branch-dense population is CONFIRMED: the
 * ordinary lever set applies essentially unchanged, and none of
 * docs/band-800plus.md section 2 (cse1 cross-call constant commoning) is
 * visible here. 54 branches over 818 instructions means ~15-instruction
 * blocks, and cse1's window never gets long enough to park anything.
 *
 * Pass 1 (first compile, no probing) read 851 of 872 with SIZE -8 and COUNT -3,
 * and with the relocation symbol sequence ALREADY exact. That is the landed
 * sibling oracle doing the work, and it is worth stating how cheaply:
 *   - src/rom_c9000/rom_e0564_a_b.c (Anim_Hail, MATCHING, same bank) gave the
 *     `tbl = iwram_3001e..`/`base`/`ctx` prologue, `*(State **)(base + 0x7828)`,
 *     `*(int *)(base + (0xef << 7))`, the `arg = 0x90; arg <<= 3; StartTask`
 *     idiom, `DrawFn fns[2]` with `gPtrs[0x2e]`/`gPtrs[0x2f]`, and the
 *     Func_80e3908 particle-integrator signature.
 *   - src/rom_c9000/rom_dd2ac_c_c_b.c (Anim_Vine, MATCHING) gave
 *     `while (i != (*(State **)(base + 0x7828))->f14)` for the Func_80d6888
 *     knockback loop and BuildDraw2DFuncEx's five-argument form.
 *   - src/rom_c9000/rom_d82b0_b.c (Anim_Break, MATCHING) gave
 *     `int *q = &ewram_2010018; do { i++; *q = -1; q = (int *)((char *)q + 0x1c); }`
 *     which is the EXACT shape of both of this function's two stride-0x1c
 *     clear loops, i++ before the store included.
 * A landed sibling in the same bank really is the strongest oracle in this
 * project, and three of them cover almost every idiom in an 818-instruction
 * animation.
 *
 * LEVER THAT PAID, 1 of 1: THE TWO-STEP COMPUTED FORM FOR A HELD CONSTANT.
 * Worth SIZE -8 -> -4, COUNT -3 -> -1, aligned 53.8 -> 59.1 percent, in one
 * edit group. Four quantities are live across the frame loop:
 *
 *     ax = 0xa0; ax <<= 16;     anchor x, 160.0
 *     ay = 0xb8; ay <<= 15;     anchor y, 92.0
 *
 * Written as `ax = 0xa0 << 16;` gcc parks a REG_EQUIV constant and
 * rematerialises `mov`+`lsl` at every use inside the loop; written as the
 * two-step computed form gcc keeps the value alive and reload gives it a
 * stack slot, which is what the reference has (sp+0x14 and sp+0x18). This is
 * docs/elevation.md's "gcc will rematerialise a constant but will keep a
 * COMPUTED value alive" rule, used in the direction the doc does not spell
 * out -- the doc uses it to AVOID holding; here the ROM holds, so the
 * two-step form is the WANTED one.
 *
 * ALSO PAID, same edit group and separable in principle:
 *   - `void **p = iwram_3001ef0; ctx = p[0]; base = p[-1]; scratch = p[1];`
 *     The three globals are read off ONE pool word at iwram_3001ef0 with
 *     offsets -4, 0, +4. Reading `iwram_3001ef0[-1]` DIRECTLY off the symbol
 *     gives `mov r2,#4 / neg r2,r2 / ldr r2,[r3,r2]` -- three instructions,
 *     because the address is a CONSTANT `(symbol_ref + -4)` and thumb has to
 *     build the negative offset in a register. Going through a declared
 *     pointer local makes the address `(plus (reg) (const_int -4))`, which
 *     memory_address force_reg's into the reference's two-instruction
 *     `sub r2, r3, #4 / ldr r2, [r2]`. Worth 1 encoding and 2 bytes.
 *   - The OBJ-priority read-modify-write in the six-sprite creation loop:
 *     `int b = q[9]; q[9] = (b & mask) | 4;` with `mask = 0xd; mask = -mask;`
 *     declared before the loop. Written the obvious way,
 *     `q[9] = (q[9] & ~0xc) | 4;`, fold() narrows the AND through the
 *     unsigned-char conversion and gcc emits `mov r3,#0xf3 / and r3,r2`
 *     INSIDE the loop -- 0xf3 is a valid 8-bit thumb immediate, so it scores
 *     while being the wrong program shape. The reference does the AND in
 *     SImode against -13 held in r7 across the whole loop
 *     (`mov r1,#0xd / neg r1,r1 / add r7,r1,#0`), which only an int-typed
 *     intermediate and a named mask reproduce. Note this is the SAME two-step
 *     computed-value rule as above, applied to a negated constant: the doc's
 *     "name a negated constant as -1, not as x = 1; x = -x;" is the opposite
 *     direction and does NOT apply when the ROM holds the value.
 *
 * LEVERS MEASURED AND REJECTED, with figures, so nobody repeats them:
 *   - The 0x77a8 range store as `if (A) store; else if (B) store;` instead of
 *     `if (A || B) store;`. The reference's shape -- each arm computing
 *     `base + 0x77a8` separately, the first arm branching to a shared
 *     `mov r3,#1 / str r3,[r2]` -- LOOKS like cross-jumping over two
 *     duplicated stores (lever 7, two sets prevent a hoist). It is not:
 *     the two-arm form measures SIZE +4 / COUNT +3 against the `||` form's
 *     -4 / -1, i.e. 8 bytes and 4 encodings WORSE, and 158 hunks against 160.
 *     The `||` form is correct and the duplicated address add is jump.c's
 *     thread_jumps, not source duplication.
 *   - The register-offset operand ORDER. The reference stores the sprite with
 *     `str r5, [r6, r2]` where r6 is the walking byte offset 0x77fc+4i and r2
 *     is the state base; ours emits `str r5, [r1, r6]`, the same two
 *     registers transposed, at both the store and the read-back. THREE source
 *     spellings are BYTE-IDENTICAL to each other: `base + off`,
 *     `off + base`, and `(int)base + off`. fold() canonicalises PLUS before
 *     any RTL exists, so the tree order never reaches
 *     print_operand_address. Note this also RETIRES a stale assumption:
 *     docs/agbcc-thumb-regoffset-bug.md says the Thumb backend NEVER emits
 *     register-offset addressing -- that is true of the checked-in
 *     tools/agbcc, but the gcc296 this project actually builds with emits it
 *     freely. The bug report's scope line should be qualified.
 *
 * ================================================================
 * THE BLOCKER, ATTRIBUTED, AND WHAT RULES OUT THE ALTERNATIVES
 * ================================================================
 *
 * ONE DEFECT ACCOUNTS FOR THE WHOLE 41 PERCENT THAT DOES NOT ALIGN, AND IT IS
 * FOUR BYTES OF FRAME.
 *
 *     reference  sub sp, #0x54        ours  sub sp, #0x50
 *
 * The reference's frame is, bottom up: outgoing args sp+0x00 and sp+0x04;
 * THREE gcc-made address pseudos spilled at sp+0x08 (&v), sp+0x0c (&sc),
 * sp+0x10 (&fns); EIGHT spilled scalars at sp+0x14 through sp+0x30; then the
 * expand-time aggregates sc at sp+0x34, fns at sp+0x3c, v at sp+0x44.
 * 8 + 12 + 32 + 32 = 84 = 0x54, and it closes exactly.
 *
 * Sorting the spilled scalars DESCENDING gives the declaration order directly,
 * and this candidate uses it:
 *
 *     sp+0x30 ctx     sp+0x2c base    sp+0x28 frame   sp+0x24 scratch
 *     sp+0x20 oy      sp+0x1c ox      sp+0x18 ay      sp+0x14 ax
 *
 * The aggregates are REVERSED as the rule says -- v declared first takes the
 * HIGHEST offset -- and this candidate declares them v, fns, sc.
 *
 * OURS IS MISSING TWO OF THOSE EIGHT SCALARS AND HAS ONE EXTRA, netting -4.
 * ox (sp+0x1c, 0xbc0000 = 188.0) and oy (sp+0x20, 0x5c0000 = 92.0) are the
 * seven-piece figure's origin. Each is SET EXACTLY ONCE before the frame loop
 * and USED EXACTLY ONCE, inside the seven-piece loop -- verified by grepping
 * every access to those two slots across the whole reference. gcc gives a
 * one-set constant pseudo a REG_EQUIV note in
 * local-alloc.c:update_equiv_regs, and reload1.c then NEVER ALLOCATES A SLOT
 * for such a pseudo -- it rematerialises `mov r2,#188 / lsl r2,#16` at the
 * use instead. Ours therefore spends 4 bytes and 2 encodings rebuilding them
 * per iteration and saves 8 bytes of frame, and because the frame is 4 short
 * EVERY `ldr`/`str [sp, #N]` in 818 instructions carries the wrong offset.
 * That is the entire alignment gap: the program is right and the frame
 * arithmetic is wrong by one word.
 *
 * The missing third slot in ours is a CONSEQUENCE, not an independent defect:
 * with two fewer live quantities ours has a spare high register, so it hoists
 * `0x7f` and `0xff` out of the trail loops into r10 where the reference
 * rebuilds them per iteration, and in exchange ours SPILLS the group-base
 * counter `fb` to sp+0x08 where the reference keeps it in r6. Fix ox and oy
 * and the pressure that spills `fb` goes with them: 3 + 8 = 11 words is the
 * reference's count.
 *
 * THE GATE IS REG_N_SETS, NOT REG_N_REFS. That is measured, not assumed, and
 * it is the useful new fact here, because docs/elevation.md's existing
 * treatment of rematerialisation is entirely about the REG_N_REFS == 2 clause:
 *
 *   | probe                                   | size | count | aligned | frame |
 *   |-----------------------------------------|------|-------|---------|-------|
 *   | `ox = 0xbc << 16;` (plain literal)      |  -8  |  -3   | 53.8%   | 0x50  |
 *   | `ox = 0xbc; ox <<= 16;` (THIS FILE)     |  -4  |  -1   | 59.1%   | 0x50  |
 *   | `volatile int ox, oy;`                  | +20  | +11   | 57.7%   | 0x50  |
 *   | second, unreachable set in a later block| +16  |  +9   | 57.5%   | 0x50  |
 *   | ox and oy each USED TWICE per iteration |   0  |  +1   | 57.8%   | 0x50  |
 *
 * Read the last row carefully, because it is a trap this park is recording so
 * the next session does not fall into it: doubling the USE count makes SIZE
 * EXACT and COUNT +1, which is the best size-and-count pair in the table --
 * AND THE FRAME IS STILL 0x50. The slots did not appear; the size agreed by
 * coincidence while the aligned figure went DOWN 1.3 points. A duplicated
 * store is also a different program. This is the brief's "a figure can improve
 * for the WRONG REASON" and "a closer figure can be a WRONG PROGRAM" in one
 * row, and the frame-size column is what caught it. Carry the frame size as a
 * column on every probe of a long function.
 *
 * So REG_N_REFS is ruled out by measurement, and the remaining gate is
 * REG_N_SETS == 1. What that means for the source is uncomfortable and should
 * be said plainly rather than guessed at:
 *
 *   A CONSTANT-VALUED LOCAL CANNOT BE FORCED INTO A RELOAD SPILL SLOT BY ANY
 *   SOURCE SPELLING, because every all-constant data flow collapses to one
 *   set. `ox = 0xbc; ox <<= 16;` has two sets in the parse tree, cse1 folds
 *   the shift inside the one basic block, and flow.c then deletes the now-dead
 *   first set. Splitting the two halves across a real basic-block boundary
 *   would defeat cse1 -- cse1's window IS the basic block -- but the only
 *   boundaries available before this point are the sprite-creation loop's,
 *   which would hold the value across eight calls and grow the prologue.
 *   `volatile` does force a slot, but an expand-time one, so it lands in the
 *   AGGREGATE region above the scalars and the layout comes out reversed; the
 *   +20/+11 row above is that.
 *
 * The honest conclusion is therefore that ox and oy ARE NOT CONSTANT-VALUED
 * LOCALS IN THE ORIGINAL SOURCE, and the next session's whole job on this
 * function is to find what non-constant quantity produces 188.0 and 92.0 with
 * a `mov`+`lsl` build. Three readings were considered and none survives:
 *   - a local aggregate holding all four anchor values. Ruled out by
 *     ARITHMETIC: expand-time slots are allocated from the frame top downward
 *     and reload spills continue below them, so an aggregate can never sit at
 *     sp+0x14 while scalars sit at sp+0x24. Every arrangement that puts a
 *     4-int or 2-int aggregate in the anchor region reverses the ctx/base/
 *     frame/scratch order against the reference.
 *   - derivation from ax or ay (`ox = ax + (0x1c << 16)`, `oy = ay`). Still
 *     all-constant, so it folds; and `oy = ay` is already in the -4/-1 row's
 *     ancestor and measured byte-identical to two independent builds, which
 *     is itself worth recording: cse commons the two identical `0xb8 << 15`
 *     computations into ONE register that the reference stores to BOTH
 *     sp+0x20 and sp+0x18, so the source cannot be distinguished there.
 *   - a file-scope `const int` or `static const int`. Ruled out by the
 *     EMITTED FORM: a .rodata read reaches the value with a pool word and an
 *     `ldr`, and the reference reaches it with `mov`+`lsl`.
 *
 * NOT THE SCHEDULER, and this is worth stating because two residues look like
 * it. sched1 DOES NOT RUN in this build. The two places where ours and the
 * reference differ only in the order of two independent instructions -- the
 * CreateSummonSprite argument fill (`ldr r1,=0x17b` before `mov r0,#9` in the
 * reference, after in ours) and the _UpdateSprite fill in the seven-piece loop
 * (the reference loads `&sc` from sp+0x0c before the `ldmia r6!, {r0}`) -- are
 * sched2 tie-breaks on dependent count, the one term that can reorder past
 * EXPAND order. Neither is reachable from the source and neither costs a byte.
 *
 * ================================================================
 * ONE OPEN ITEM WORTH A SEPARATE LOOK, BECAUSE IT IS TREE-WIDE
 * ================================================================
 *
 * `_AnimTransitionIn(1, 0x3c, 0)` -- the reference reaches the SECOND
 * ARGUMENT WITH A LITERAL-POOL LOAD, `ldr r1, =0x3c`, and the pool word
 * carries NO relocation. gcc emits `mov r1, #60`, because 0x3c is a valid
 * 8-bit thumb immediate. Ours is therefore 4 bytes and 1 encoding SHORT there
 * (the pool word), which is most of the remaining -4.
 *
 * THIS IS NOT LOCAL TO THIS FUNCTION. Every _AnimTransitionIn site in the
 * tree does the same thing: `ldr r1, =0x3b` in rom_dbbdc_c_c_c_c_c_c_c.s and
 * rom_ea0d8.s and rom_d6970.s, `=0x3a` in rom_eb754_a.s and rom_ea0d8.s,
 * `=0x3e` and `=0x36` in rom_ea0d8.s, `=0x3d` in rom_eb754_c_c.s and
 * rom_e0564_c.s, `=0x3c` here. NINE sites, nine small ints, every one of them
 * pool-loaded and none of them a relocation in the project's own disassembly.
 * A value in 0..255 that gcc pools cannot be a CONST_INT in the final RTL, so
 * either the original source names a SYMBOL whose link-time value is a small
 * integer -- in which case the project's disassembler is resolving it to a
 * bare `.word` at these sites while naming `_FILE_73` and `_FILE_c0` two
 * instructions away in this very function -- or these arguments are reached
 * some third way. src/non_matching/rom_c9000/80d67dc.c and Anim_Condemn.c both
 * pass a `*(u16 *)(base + (0xc9 << 3))` LOAD as this same argument, which is
 * consistent with the second argument being an id that normally comes from
 * memory. Resolving it is worth a tool pass over all nine sites rather than
 * one more spelling probe here, and it would pay out across at least six
 * functions in rom_c9000 alone.
 *
 * ================================================================
 * FACTS READ OFF THE REFERENCE THAT THE MATCH ALREADY CONFIRMS
 * ================================================================
 *
 *  - EVERY HALFWORD STORE OF A CONSTANT IS POOLED, AND THAT IS CORRECT HERE.
 *    REG_WIN0H = 0xf0 comes out `ldr r3, =0xf0 / strh`, not `mov r3,#0xf0`,
 *    and the two zeroed palette entries at 0x5000000 share ONE pooled zero.
 *    `*thumb_movhi_insn` has no immediate form, so a plain literal store is
 *    right and the band doc's batch-306 pooled-zero DEFECT is not a defect in
 *    this direction -- the int-carrier lever would BREAK all six of this
 *    function's register writes. Check which way the ROM goes before applying
 *    lever 9.
 *  - The frame loop is a `while ((gKeyRepeat & 3) == 0)` with `frame++` and
 *    an `if (frame == (0xa0 << 1)) break;` at the bottom, NOT a `for`. The
 *    reference tests gKeyRepeat BEFORE the loop (jumping straight to
 *    teardown) and tests `frame` first then gKeyRepeat at the bottom, which
 *    is jump.c:1137 duplicate_loop_exit_test copying the `while` condition
 *    below the break test. A `do`-`while` can never reach that pass.
 *  - Both stride-0x1c clear loops increment the counter BEFORE the store, and
 *    both walk a plain `int *` rather than indexing a struct, which is why
 *    the 58-entry loop's base is the pooled 0x7140 (= 0x7128 + 0x18, the `t`
 *    field) and not 0x7128 with an `str [r3, #0x18]`.
 *  - The two seeded-spark inner loops take a SOURCE-LEVEL pointer init
 *    (`s = (Part *)(base + 0x7128) + i * 8`), which is why the pooled 0x7128
 *    is born FIRST in the preheader and the hoisted 0xff second. This is
 *    Anim_Hail's preheader-transposition signature read in the other
 *    direction, and it comes out right first time.
 *  - `.Leeed8` and `.Leeee1` are UNSIGNED char (`ldrb`); `.Leeeea` and
 *    `.Leeef8` are UNSIGNED short (`ldrh`), and the size is halved with `lsr`,
 *    so it must stay unsigned through the subtraction.
 *  - The 0x104 payoff is TWO consecutive `if (frame == (0x82 << 1))` blocks,
 *    not one. The reference's duplicated test at .Le8854 is jump.c's
 *    thread_jumps redirecting the first block's false-exit to the second
 *    block's false-exit; writing one block merges them and loses the test.
 *  - 0x18 at base+0x77b4, 0x32 at base+0x7784 and the 1024-entry free at
 *    ewram_2010018 against a 512-entry launch and 512-entry draw are all the
 *    reference's own asymmetries, not transcription errors.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int h, int w);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

typedef struct {
    int a, b;
} Scale;

extern void *iwram_3001ef0[];
extern int ewram_2010018;
extern Part gBuffer[];
extern void *gPtrs[];
extern int gKeyRepeat;
extern Scale Data_edac8;
extern unsigned short Data_ede48[];
extern unsigned char Leeed8[] __asm__(".Leeed8");
extern unsigned char Leeee1[] __asm__(".Leeee1");
extern unsigned short Leeeea[] __asm__(".Leeeea");
extern unsigned short Leeef8[] __asm__(".Leeef8");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Func_80c9048(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void AnimTransitionOut(int a, int b);
extern void _AnimTransitionIn(int a, int b, int c);
extern void Func_80d6750(State *s);
extern void CreateSummonSprite(int count, int res, int prio);
extern void *_CreateSprite(int res);
extern void _Sprite_SetAnim(void *spr, int anim);
extern void _DeleteSprite(void *spr);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void _UpdateSprite(void *spr, int *pos, void *scale, int mode);
extern void _PlaySound(int id);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _Func_80bd7dc(int a);
extern void Func_80d67dc(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Ramses(void *context)
{
    int v[4];
    DrawFn fns[2];
    Scale sc;
    void *ctx;
    u8 *base;
    int frame;
    u8 *scratch;
    int oy;
    int ox;
    int ay;
    int ax;
    int i;
    int j;
    int off;
    int arg;
    int k;
    int y;
    int mask;
    void **p;
    void **list;
    Part *tr;
    Part *g;
    Part *s;

    p = iwram_3001ef0;
    ctx = p[0];
    base = (u8 *)p[-1];
    scratch = (u8 *)p[1];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    Func_80c9048();
    *(vu16 *)0x5000000 = 0;
    *(vu16 *)0x5000002 = 0;
    *(int *)(base + (0xef << 7)) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    AnimTransitionOut(1, 0);
    Func_80d6750(*(State **)(base + 0x7828));
    CreateSummonSprite(9, 0x17b, 2);
    mask = 0xd;
    mask = -mask;
    off = 0x77fc;
    i = 0;
    do {
        void *spr = _CreateSprite(0xc3 << 1);
        *(void **)(off + base) = spr;
        if (spr != 0) {
            *((u8 *)spr + 0x26) = 0;
            _Sprite_SetAnim(spr, i % 3);
            {
                u8 *q = *(u8 **)(off + base);
                int b = q[9];
                q[9] = (b & mask) | 4;
            }
        }
        i++;
        off += 4;
    } while (i != 6);
    k = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, k);
    fns[0] = (DrawFn)gPtrs[0x2e];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
    fns[1] = (DrawFn)gPtrs[0x2f];
    REG_WININ = 0x2737;
    REG_WIN0H = 0xf0;
    REG_WIN1V = 0x1088;
    WaitFrames(1);
    _AnimTransitionIn(1, 0x3c, 0);
    AnimTransitionOut(1, 1);
    LoadVFXFile(FILE_73, scratch, 0, 0);
    LoadVFXFile(FILE_c0, base, 1, 1);
    REG_DISPCNT = 0x7741;
    REG_BG2PA = 0x80;
    REG_BLDALPHA = 0x1010;
    REG_BLDCNT = 0x3f44;
    *(int *)(base + (0xef << 7)) = k;
    *(int *)(base + 0x7784) = 0x32;
    ox = 0xbc;
    ox <<= 16;
    oy = 0xb8;
    oy <<= 15;
    ay = oy;
    ax = 0xa0;
    ax <<= 16;
    tr = (Part *)(base + (0xe1 << 7));
    y = 0;
    i = 0;
    do {
        tr->x = (Random() & 0x7f) << 16;
        tr->y = y;
        tr->vx = 0;
        tr->vy = 0;
        tr->t = 0;
        y -= 0x10 << 16;
        tr++;
        i++;
    } while (i != 6);
    {
        int *q = (int *)(base + 0x7140);
        i = 0;
        do {
            i++;
            *q = 0x18;
            q = (int *)((char *)q + 0x1c);
        } while (i != 0x3a);
    }
    {
        int *q = &ewram_2010018;
        i = 0;
        do {
            i++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (i != (0x80 << 3));
    }
    *(int *)(base + 0x77b4) = 0x18;
    *(int *)(base + 0x77b8) = 0;
    frame = 0;
    while ((gKeyRepeat & 3) == 0) {
        if (frame == 0x5e) {
            _PlaySound(0x9c);
        }
        if (frame == 0x88) {
            _PlaySound(0x9c);
        }
        if (frame == 0xb2) {
            _PlaySound(0x9c);
        }
        if (frame == (0x82 << 1)) {
            _PlaySound(0x91);
        }
        sc = Data_edac8;
        if ((frame >= 0x60 && frame <= 0xfb) || (frame >= 0x104 && frame <= 0x107)) {
            *(int *)(base + 0x77a8) = 1;
        }
        v[3] = 0;
        v[1] = 0;
        list = (void **)(base + 0x77d8);
        i = 0;
        do {
            v[0] = (Leeed8[i] << 16) + ox - (0x20 << 16);
            v[2] = (Leeee1[i] << 16) + oy - (0x20 << 16);
            _UpdateSprite(*list++, v, &sc, 0);
            i++;
        } while (i != 7);
        if (frame <= 0x5a) {
            int a = frame << 9;
            ax = (sin(a) << 4) + (0x9c << 16);
            ay = (cos(a) << 4) + (0x5c << 16);
        }
        if (frame <= 0xc4) {
            int fb = 0x5b;
            i = 0;
            do {
                if (frame >= fb && frame < fb + 4) {
                    ay += 0x80 << 12;
                }
                if (frame == fb + 3) {
                    s = (Part *)(base + 0x7128) + i * 8;
                    j = 0;
                    do {
                        s->x = 0x80 << 15;
                        s->y = 0xc0 << 15;
                        s->vx = ((Random() & 0xff) - 0x7f) << 10;
                        s->vy = ((Random() & 0xff) - 0x7f) << 10;
                        s->t = Random() & 0xf;
                        s++;
                        j++;
                    } while (j != 4);
                }
                if (frame >= fb + 0x14 && frame < fb + 0x24) {
                    ay -= 0x2 << 16;
                }
                i++;
                fb += 0x28;
            } while (i != 3);
        }
        if (frame >= 0xf4 && frame <= 0xfb) {
            ax -= 0x1 << 16;
        }
        if (frame >= 0xfc && frame <= 0x113) {
            ax -= (frame - 0xfa) << 16;
        }
        if (frame <= 0x103) {
            v[1] = 0xff << 24;
            v[2] = ay + (0xff << 24);
            v[0] = ax;
            _UpdateSprite(*(void **)(base + 0x77f4), v, &sc, 0);
            v[0] = ax + (0x80 << 14);
            _UpdateSprite(*(void **)(base + 0x77f8), v, &sc, 0);
        }
        v[1] = 0;
        tr = (Part *)(base + (0xe1 << 7));
        i = 0;
        do {
            if (tr->t != 2) {
                v[0] = tr->x;
                v[2] = tr->y;
                _UpdateSprite(*(void **)(base + 0x77fc + i * 4), v, &sc, 0);
                tr->x += tr->vx;
                tr->y += tr->vy;
                if (frame > 0x60) {
                    tr->vy += 0x80 << 7;
                }
                if (tr->y > (0xf0 << 15)) {
                    tr->t++;
                    if (tr->t == 1) {
                        tr->vy = -tr->vy / 2;
                        s = (Part *)(base + 0x73c8) + i * 2;
                        j = 0;
                        do {
                            s->x = tr->x / 2;
                            s->y = tr->y - (0x20 << 16);
                            s->vx = ((Random() & 0xff) - 0x7f) << 10;
                            s->vy = ((Random() & 0xff) - 0x7f) << 10;
                            s->t = Random() & 0xf;
                            s++;
                            j++;
                        } while (j != 2);
                    } else if (frame <= 0xc7) {
                        tr->y = 0;
                        tr->vy = 0;
                        tr->t = 0;
                    }
                }
            }
            i++;
            tr++;
        } while (i != 6);
        s = (Part *)(base + 0x7128);
        i = 0;
        do {
            if (s->t >= 0) {
                if (s->t <= 0x17) {
                    int n = s->t / 6;
                    unsigned int w = Leeef8[n + 3];
                    fns[0](ctx, base + Leeeea[n + 3],
                           *(short *)((char *)s + 2) - (w >> 1),
                           *(short *)((char *)s + 6) - (w >> 1),
                           w, w);
                }
                Func_80e3908(s, 0x3c, -(0x1 << 14));
                s->t++;
            }
            i++;
            s++;
        } while (i != 0x38);
        if (frame == (0x82 << 1)) {
            int io = 0x24;
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                _SetBattleActorKnockback(*(short *)((char *)*(State **)(base + 0x7828) + io), 4);
                Func_80d6888(*(short *)((char *)*(State **)(base + 0x7828) + io), 7, -1, i, 8);
                i++;
                io += 2;
            }
            *(int *)(base + 0x77a8) = 8;
        }
        if (frame == (0x82 << 1)) {
            g = gBuffer;
            i = 0;
            do {
                int m = (Random() & 0x3ff) + 0x20;
                int ang = Random() & 0xffff;
                g->x = 0x80 << 14;
                g->y = 0xb8 << 15;
                g->vx = (m * sin(ang)) >> 7;
                g->vy = -(m * cos(ang) * 2) >> 7;
                g->t = (Random() & 0xf) + 0x20;
                g++;
                i++;
            } while (i != (0x80 << 2));
        }
        g = gBuffer;
        i = 0;
        do {
            if (g->t >= 0) {
                int n = (g->t >> 3) + 1;
                int w = n * 2;
                fns[i & 1](ctx, scratch + Data_ede48[n - 1],
                           *(short *)((char *)g + 2) - n / 2,
                           *(short *)((char *)g + 6) - n,
                           n, w);
                Func_80e3908(g, 0x3e, 0x80 << 5);
                g->t--;
            }
            i++;
            g++;
        } while (i != (0x80 << 2));
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
        if (frame == (0xa0 << 1)) {
            break;
        }
    }
    _Func_80bd7dc(0x86);
    Func_80d67dc();
    list = (void **)(base + 0x77d8);
    i = 0;
    do {
        i++;
        _DeleteSprite(*list++);
    } while (i != 0xf);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
