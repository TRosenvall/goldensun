/* BaseAnim_ParticleCloud -- 0x080d52c8, asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s
 * line 12, 755 ROM instructions.
 * NON-MATCHING, 739 of 791 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 740 is NOT a distance: 1760 bytes against
 * the ROM's 1768 (-8) and 787 encodings against 791 (-4) -- we are SHORT, not
 * long, and only just.  tools/aligncmp.py reads 645 aligned-equal of 791
 * (81.5%), 187 differing/ins/del in 76 hunks.  81.5% is the figure to beat, and
 * it is the highest first-pass residue any function in this family has reached
 * (BaseAnim_Attack landed at 56.2%).
 *
 * *** THE FRAME IS EXACT.  `sub sp, #0x74` on both sides, and the FOUR
 * AGGREGATES land on the ROM's offsets exactly -- fns 0x48, pos 0x50, tv 0x5c,
 * mv 0x68.  Eleven of the sixteen scalar spill slots also carry the ROM's own
 * reference count (0x04, 0x08, 0x10, 0x14, 0x18, 0x24, 0x2c, 0x30, 0x34, 0x3c,
 * 0x44); five are permuted (see residue (C)). ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/d52c8_ParticleCloud.c \
 *     asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s --func BaseAnim_ParticleCloud
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/d52c8_ParticleCloud.c
 * It includes the family header src/non_matching/rom_c9000/decls.h, as
 * e47b8_SpecialAttack.c already does, and declares only the seven externs that
 * header lacks.
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and MEASURED not assumed: the function has exactly TWO
 * `_call_via_r3` sites and they call DIFFERENT functions (Func_8001af8 for the
 * palette DMA, Func_8000948 for the vector length), so there is no shared
 * `0xa0 << 19` for cse2 to unify and nothing for Blob's r0/r2 pins to defeat.
 * That is the basic-block precondition BaseAnim_Attack's park states, confirmed
 * a second time.  ELEVENTH instance of not needing a pin.
 *
 * ================================================================
 * THE SPLIT SHAPE -- confirmed with the tools, and it needs ONE EXPORT
 * ================================================================
 * tools/datacheck.py: the .s HAS a `.rodata` section and holds FOUR functions
 * (BaseAnim_ParticleCloud, Anim_Sleep, Anim_Curse, Anim_Unused_Fizz), so this
 * is a TEXT/DATA SPLIT and the data must keep its own object.
 *     *** THE SPLIT MUST ADD EXACTLY ONE EXPORT:   .global .Lee2ae ***
 * datacheck attributes .Lee2ae to BaseAnim_ParticleCloud and to NO other
 * function in the file (the other three read no data label at all), and
 * tools/split_s.py --dry-run REFUSES the split until the export exists:
 *     "1 local label(s) would cross files ... _b.s references .Lee2ae,
 *      defined in ... _c.s"
 * A `.global` emits no bytes: add it, verify `make compare` is still green, and
 * only THEN split, so the two changes stay separable.  The target is the FIRST
 * function in the file.  .Lee2ae is read with `ldrb`, so it comes into C as
 *     extern unsigned char Lee2ae[] __asm__(".Lee2ae");
 * which is what this file uses -- the recon left the width unconfirmed and the
 * asm settles it (`ldrb r2,[r3,r0]` at .Ld5666-4).
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline v1 (the BaseAnim_Attack template transplanted, first candidate):
 * size 1752 (-16), count 782 (-9), 531 aligned (67.1%), 136 hunks, objcmp 709.
 * THE RELOCATION SEQUENCE WAS ALREADY ALMOST EXACT ON THAT FIRST CANDIDATE --
 * every call and every pool symbol in the ROM's order bar one -- which is the
 * cheapest possible confirmation that the program shape read off the asm is
 * right.  Everything below is register allocation, addressing and expression
 * shape, not program shape.
 *
 * (1) *** THE GetFile ARGUMENT MUST NOT CROSS A CALL: SPLIT `f` INTO `f` AND A
 *     BLOCK-SCOPED `id`. ***  The file dispatch is an eight-arm if-chain ending
 *     in `s = GetFile(f)`.  The ROM loads the constant straight into **r0** in
 *     the arms for variants 0,1,2,3,4 and 7, and keeps it in **r5** across the
 *     extra `LoadVFXFile` in the arms for variant 5 and the default, finishing
 *     with `mov r0,r5`.  ONE PSEUDO CANNOT BE BOTH: passing `f` itself to that
 *     LoadVFXFile makes `f` live across a call, so global-alloc gives it a
 *     callee-saved register for the WHOLE chain and every arm becomes
 *     `ldr r5,=FILE_x`.  Declaring a separate block-scoped `int id` in each of
 *     the two arms that call LoadVFXFile, and assigning `f = id` after the call,
 *     puts `f` in r0 and `id` in r5 -- and it also LETS jump.c CROSS-JOIN THE
 *     TWO LoadVFXFile CALLS INTO ONE, which is what the ROM has (three
 *     LoadVFXFile relocations before GetFile, not four).  It further fixes the
 *     pool-word ORDER: `_FILE_bb` moves to 0x234, the ROM's slot.
 *     Alone: size 1752 -> 1744, count 782 -> 779, aligned 67.1% -> 67.5%.
 *     Note the two gating figures went the WRONG way here while aligned rose;
 *     lever (2) is what turned that around, and both are needed.
 *
 * (2) *** ONE COUNTER FOR ALL FOUR LOOPS, AND IT LANDS IN r11 LIKE THE ROM'S.
 *     ***  The ROM uses `fp` (r11) as the induction variable in the gBuffer seed
 *     loop, the 0xa0-iteration sin loop, the two-iteration particle-spawn loop
 *     inside the variant==3 arm, AND the inner physics loop -- four disjoint
 *     loops, one high register, and it PAYS FOR IT (`movs r4,#0 / mov fp,r4`
 *     where a low register would need one insn).  With four block-scoped
 *     counters we got r7 in every loop and the ROM's loop-invariant `0x80 << 11`
 *     was SPILLED to sp+8 and reloaded every iteration instead.  This is
 *     lever 7 of the brief ("counters unify across disjoint loops") in its
 *     strongest measured form yet: a single function-level `int i` shared by all
 *     four loops raises that one allocno's priority enough to push it into r11
 *     and hand r7 back to the constant.
 *     Together with the aggregate reorder in (3): size 1744 -> 1752,
 *     count 779 -> 783, aligned 67.5% -> 74.0%, hunks 139 -> 107.
 *
 * (3) AGGREGATE DECLARATION ORDER IS REVERSED FROM THE FRAME.  gcc-2.96 gives
 *     the FIRST-declared addressable local the HIGHEST sp offset, so to land
 *     fns 0x48 / pos 0x50 / tv 0x5c / mv 0x68 the declarations must read
 *     `mv, tv, pos, fns`.  Worth recording against BaseAnim_Attack's park,
 *     which measured aggregate reordering INERT -- it is inert there because
 *     that function's three vec3_t slots are interchangeable, and it is NOT
 *     inert here, because `pos` is read sp-relative (`ldr r6,[sp,#0x50]`) and
 *     `mv` is written through a pointer, so the two are distinguishable.
 *
 * (4) *** THE FRAME-CLOSING CHANGE: THE j-LOOP NEEDS **TWO** QUANTITIES EQUAL TO
 *     j*8, NOT ONE. ***  The ROM carries a strength-reduced giv at sp+0xc
 *     (init 0 in the preheader, `add r3,#8` at the loop bottom) serving three
 *     uses -- the variant==3 window test, its `+ 0x20` bound, and the
 *     `frame == lo + 0x10` gate -- AND a SECOND pseudo at sp+0x14 computed as
 *     `lsl r3,#3` whose only use is the inner loop's `frame > lo + i` test.
 *     Writing one `lo = j * 8` lets loop.c reduce it completely: 15 spill slots,
 *     frame 0x70, four bytes short, and EVERY aggregate offset wrong.  Writing
 *     the three outer uses against a hand-carried accumulator (`t = 0` before
 *     the frame loop, `t += 8` at the j-loop bottom) and keeping `lo = j * 8`
 *     for the inner test gives SIXTEEN slots and `sub sp, #0x74` -- the ROM's
 *     frame exactly, and every aggregate offset snaps onto the ROM's.
 *     size 1752 -> 1756, count 783 -> 785, and that is the 16th slot arriving.
 *     A block-scoped second `int lo2 = j * 8;` DOES NOT WORK: cse commons it
 *     with the first and the frame falls back to 0x70 (measured, 1752/783/74.5%).
 *     The accumulator is the only spelling found that keeps the two apart.
 *
 * (5) DECLARATION ORDER FOR THE SPILLED SCALARS, plus a named `pt` for gPtrs.
 *     Reordering the function-level declarations to
 *     `ctx, frame, total, j, len, cam, rec, pt, look, fp, tp, ...` and hoisting
 *     `cam`/`look` out of the frame-loop body to function level: 74.0% -> 75.0%,
 *     hunks 114 -> 105, size and count unchanged.  `pt = gPtrs; fns[0] = pt[0x2e]`
 *     is what produces the ROM's `ldr r0,=gPtrs / str r0,[sp,#0x28]` in the
 *     preheader plus `ldr r3,[sp,#0x28] / add r3,#0xb8 / ldr r3,[r3]` inside --
 *     writing `gPtrs[0x2e]` directly folds the 0xb8 into the pool word instead
 *     and there is then nothing to hoist.  NOTE THE ASYMMETRY, which is real:
 *     gPtrs[0x2f] is the SAME address as `iwram_3001f0c` (gPtrs = 0x03001E50,
 *     +0xbc = 0x03001F0C) and the ROM spells THAT one as its own pool symbol, so
 *     this file uses `iwram_3001f0c` for the 0x2f slot and `pt[0x2e]` for the
 *     other.  Two spellings of one array, both required.
 *
 * (6) *** THE BIGGEST SINGLE JUMP: READ `mp->y` INSIDE EACH DRAW ARM, NOT ONCE
 *     ABOVE THE DISPATCH. ***  The four draw arms each need the y component of
 *     the vec Func_80e3944 filled in, and the ROM emits `ldr r3,[r5,#4]` FOUR
 *     TIMES, once per arm.  Reading it once into a local `y` above the dispatch
 *     keeps it in a register and costs four instructions AND the whole
 *     register assignment around them.  Per-arm reads, together with a named
 *     `int sel = (i & 3) != 0;` for the `fp[...]` index in the default arm:
 *     75.0% -> 80.8%, hunks 105 -> 72, size and count UNCHANGED at 1756/785.
 *     This is the exact converse of BaseAnim_Attack's lever 4 and it belongs
 *     beside it: THERE the destination vec had to be written THROUGH ITS
 *     POINTER; HERE the source vec has to be RE-READ THROUGH IT, once per arm.
 *     The rule that covers both: reproduce the ROM's number of accesses, not a
 *     tidy single read.
 *     The `sel` half matters separately -- `fp[(i & 3) != 0]` written inline
 *     compiles to a BRANCH (`cmp r3,#0 / beq / adds r4,#4`) while the ROM has
 *     the arithmetic boolean `negs r4,r3 / orrs r4,r3 / lsrs r4,#31 / lsls r4,#2
 *     / ldr r4,[r4,r0]`.  Naming the boolean forces the value context.
 *
 * (7) RE-READ `*slot` PER USE, AND FLIP TWO COMMUTATIVE OPERANDS.
 *     The ROM keeps the ADDRESS `base + 0x7828` in r5 across the j-loop body and
 *     re-loads `*r5` for the GetBattleActorPos2 call (`ldr r3,[r5]` a second
 *     time), so the source keeps a `State **sl` and writes `(*sl)->ids[j]`
 *     twice, not a `State *st` once.  And gcc canonicalises a commutative
 *     operand pair the opposite way from the ROM in two places: `r * sin(a0)`
 *     gives `mov r3,r0 / mul r3,r5` where the ROM has `mov r3,r5 / mul r3,r0`,
 *     so write `sin(a0) * r`; `Random() & 3` gives `movs r3,#3 / ands r3,r0`
 *     where the ROM has `movs r1,#3 / ands r0,r1`, so write `3 & Random()`.
 *     Both flips plus the re-read: 80.8% -> 81.4%, size 1756 -> 1760,
 *     count 785 -> 787.  ALL THREE FIGURES IMPROVED TOGETHER.
 *
 * (8) THE PARTICLE ARRAY IS INDEXED THROUGH AN `int *`, NOT A `Part *`.
 *     The ROM computes the per-actor base as
 *     `lsl r2,j,#6 / lsl r3,j,#9 / sub r3,r3,r2 / lsl r3,#2 / add r7,r3,gBuffer`
 *     -- that is synth_mult(448) = (j<<9) - (j<<6), and THEN a `<<2`.  A
 *     `Part *` subscript asks synth_mult for 1792 instead and gets
 *     `((j<<3) - j) << 8`: the same algorithm with the shifts distributed the
 *     other way, which is the batch-298 tell.  `(Part *)((int *)gBuffer +
 *     j * 448)` reproduces the ROM's distribution.  With the squares written
 *     inline rather than through three named temps (which the ROM interleaves
 *     one-temp-at-a-time): 81.4% -> 81.5%, 187 differing, size and count
 *     unchanged.  The two ranking views split by ONE aligned encoding here
 *     (645 against 644) and five hunks (76 against 71); this file keeps the
 *     inline-squares form because aligned-equal is the ranking metric when size
 *     and count are tied, but whoever reopens this should re-rank the moment
 *     size and count go exact.
 *
 * ================================================================
 * A REFERENCE-FIDELITY DEFECT, NOT A DECOMPILATION DEFECT
 * ================================================================
 * *** objcmp WILL ALWAYS REPORT "RELOCATIONS differ" ON THIS FUNCTION UNTIL THE
 * .s IS CORRECTED, AND THE CORRECTION IS ONE CHARACTER RANGE. ***
 * asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s:.Ld535a writes
 *     ldr  r5, =0xcd
 * where every other file id in the same chain is spelled as its symbol
 * (`=_FILE_a0`, `=_FILE_bb`, ...).  file_table.sym:125 has `_FILE_cd = 0xcd`,
 * so `=0xcd` and `=_FILE_cd` assemble to the same four bytes but the first
 * carries NO R_ARM_ABS32.  Our pool word at 0x248 is relocated against
 * _FILE_cd; the reference's is a bare literal.  Same linked bytes, one spurious
 * relocation difference.  gcc cannot be made to emit the bare literal: 0xcd is
 * 8-bit-movable, so a CONST_INT 0xcd becomes `movs r5,#0xcd` and never reaches
 * the pool at all -- only a SYMBOL_REF pools, which is the proof that the ROM's
 * word IS `_FILE_cd`.  RECOMMENDED: change that one `=0xcd` to `=_FILE_cd` in
 * the .s and re-run `make compare`; it is byte-neutral.  Until then read
 * objcmp's relocation verdict on this function as "identical apart from
 * _FILE_cd".  The pool word COUNT and POSITION already match exactly.
 *
 * ================================================================
 * MEASURED WORSE / INERT
 * ================================================================
 *   - Block-scoped `int lo2 = j * 8;` as the second j*8 quantity: cse commons
 *     it, frame falls to 0x70 (1752 / 783 / 74.5%).  See lever (4).
 *   - Keeping `lo = t` as an explicit copy inside the variant==3 arm (which is
 *     what the ROM's `ldr r0,[sp,#0xc] / str r0,[sp,#0x14]` looks like):
 *     BYTE-IDENTICAL to omitting it -- copy propagation removes it either way,
 *     and it costs one extra spill slot when the arm's compares are written
 *     against `lo` instead of `t` (17 slots, frame 0x78, 73.5%).
 *   - Four separate block-scoped loop counters: see lever (2), 6.5 aligned
 *     points worse and it spills the sin loop's constant.
 *
 * ================================================================
 * THE RESIDUE, BY PASS
 * ================================================================
 *
 * (A) *** jump.c CROSS-JUMPS THE variant==1 AND variant==4 ARMS AND THE ROM
 *     DOES NOT.  This is the whole of the -4 instruction shortfall's largest
 *     identifiable piece and NO SPELLING WAS FOUND THAT BLOCKS IT. ***
 *     Both arms are `f = FILE_bb;` and both tails are therefore the identical
 *     two insns `ldr r0,=_FILE_bb / b <GetFile>`.  gcc redirects the variant==1
 *     conditional straight into the variant==4 arm (`beq` instead of
 *     `bne`+body), losing 2 instructions; the ROM emits both copies, each
 *     referencing the SAME pool word at 0x234 (so the assembler's `.pool_aligned`
 *     dedup is not the difference either).  aligncmp: `hunk replace ref[33:36]
 *     ours[33:34]`, the only asymmetric hunk in the first 200 encodings.
 *     What was RULED OUT: it is not a switch (there is no `.word` table and no
 *     duplicated `cmp`, and a switch over the dense set 0..7 would have got a
 *     table); it is not the pool (same word, same slot); it is not lever (1)'s
 *     r0/r5 question (the merge survives that fix unchanged).  Anyone
 *     reopening this should look for what makes gcc-2.96's find_cross_jump
 *     decline a two-insn tail, because that is the remaining question.
 *
 * (B) THE `0x96 << 6` ADDEND IS HOISTED IN OURS AND REMATERIALISED IN THE ROM.
 *     The ROM builds the spawn-loop source address as
 *     `ldr r2,=Data_edebe / lsl r3,r7,#1 / ldrh r1,[r2,r3] / ... /
 *      movs r2,#0x96 / lsls r2,#6 / add r1,r9 / adds r1,r1,r2`
 *     -- the index `m << 1` and the constant 0x2580 both built INSIDE the
 *     two-iteration loop.  We hoist `m << 1` to sp+0x24 and pool 0x2580, which
 *     is loop.c doing its job: in the ROM the shift lives inside the load
 *     insn's MEM and is only materialised by reload, so scan_loop never sees a
 *     `(set reg (ashift ...))` to move.  Net -4 over hunks ref[411:425].  This
 *     is the one place where being MORE optimal than the ROM costs us, and the
 *     handle is whatever keeps the index inside the MEM.
 *
 * (C) FIVE OF SIXTEEN SPILL SLOTS ARE PERMUTED -- local-alloc/reload ordering.
 *     Ours against the ROM's, by reference count: 0x0c 5/6, 0x1c 6/3, 0x20 2/6,
 *     0x28 6/2, 0x38 5/6, 0x40 13/12.  The SET is right and the frame is right;
 *     the ROM's descending-offset order is
 *     ctx, frame, total, j, len, cam, rec, gPtrs, look, fp, tp, lim, lo, off,
 *     t(giv), a0 and ours transposes the {gPtrs, look, fp, tp} group.  Slot
 *     order is NOT plain declaration order -- lever (5) moved it several slots
 *     with a pure reorder but could not close it, and the hand-carried `t` of
 *     lever (4) lands at 0x28 where the ROM's loop.c-created giv is at 0x0c,
 *     which is the cost of that lever: a declared variable cannot get a
 *     loop.c pseudo's regno.  If someone finds the spelling that makes the ROM's
 *     sp+0x14 quantity fall out of loop.c instead of out of a declaration, this
 *     group should close with it.
 *
 * (D) `iwram_3001e80` IS RE-READ PER FRAME AND WE MATCH THAT, so lever 1 of the
 *     brief ("a pointer read from a global and used across a call must be a
 *     source local, ONE PER LOOP") is already satisfied here: `cam` is assigned
 *     at the top of the frame-loop body and used at MatrixSetLook across three
 *     calls, and `look` is `(char *)cam + 0xc` hoisted into the j-loop
 *     preheader by scan_loop.  Recorded as a confirmation, not a residue.
 
 *
 * *** CLAIM UPDATED 740 -> 739 ON INSTALL, AND THE CAUSE IS A REFERENCE FIX. ***
 * This park was measured at 740 against a reference that spelled `ldr r5, =0xcd` where
 * file_table.sym:125 defines `_FILE_cd = 0xcd`.  That literal made objcmp report
 * `RELOCATIONS differ` on every candidate for this function permanently, because our
 * compiled .c emits the symbol and so carries a relocation the reference lacked.
 * asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s:90 now spells `=_FILE_cd`; the change is
 * byte-neutral (gated on make compare) and it removed exactly one differing encoding.
 * The proof that the ROM's word IS the symbol is a codegen argument: 0xcd is
 * 8-bit-movable, so gcc would emit `mov r5,#0xcd` and the value would never reach the
 * pool at all -- a pool word holding 0xcd can only come from a symbol.  The sibling call
 * two lines above already used the symbol form.  So 739 is measured against a CORRECTED
 * reference and is not comparable to figures taken before this commit.
 */
#include "decls.h"

extern void *iwram_3001e80;
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  Func_8000948(int n);
extern void _Func_80b82c4(int a, int b, int c, int d);
extern void MatrixTranslatev(vec3_t *v);
extern unsigned char Lee2ae[] __asm__(".Lee2ae");

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*SqrtFn)(int n);

void BaseAnim_ParticleCloud(void *context, int variant)
{
    vec3_t mv;
    vec3_t tv;
    vec3_t pos;
    DrawFn fns[2];
    void *ctx;
    int frame;
    int total;
    int j;
    int len;
    void *cam;
    int *rec;
    void **pt;
    void *look;
    DrawFn *fp;
    vec3_t *tp;
    int **w;
    unsigned char *base;
    State **slot;
    int i;
    int t;
    int f;

    w = iwram_3001eec;
    base = (unsigned char *)*w++;
    ctx = *w;
    len = 0x10;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_9e, base, 1, 1);
    if (variant == 0) {
        f = FILE_a0;
    } else if (variant == 1) {
        f = FILE_bb;
    } else if (variant == 2) {
        f = FILE_a3;
    } else if (variant == 3) {
        f = FILE_c0;
    } else if (variant == 4) {
        f = FILE_bb;
    } else if (variant == 5) {
        int id;
        id = FILE_b7;
        LoadVFXFile(id, base, 1, 0);
        f = id;
    } else if (variant == 7) {
        len = 0x18;
        LoadVFXFile(FILE_b7, base, 1, 0);
        f = FILE_8d;
    } else {
        int id;
        id = FILE_cd;
        len = 0x20;
        LoadVFXFile(id, base, 1, 0);
        f = id;
    }
    {
        void *s;
        int d0;
        CopyFn copy;
        s = GetFile(f);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    }
    if (variant == 4)
        LoadVFXFile(FILE_aa, base, 1, 1);
    if (variant == 3)
        LoadVFXFile(FILE_ce, base + (0x96 << 6), 1, 0);
    {
        Part *p;
        int mask;
        p = gBuffer;
        i = 0;
        mask = 0xff;
        do {
            if ((unsigned)variant <= 1 || variant == 4 || variant == 5 ||
                variant == 6 || variant == 7) {
                p->x = ((Random() & mask) - 0x7f) << 15;
                p->y = ((Random() & mask) - 0x7f) << 14;
                p->z = ((Random() & mask) - 0x7f) << 15;
            } else {
                p->x = ((Random() & mask) - 0x7f) << 13;
                p->y = ((Random() & mask) - 0xff) << 13;
                p->z = ((Random() & mask) - 0x7f) << 13;
            }
            p->t = 0;
            i++;
            p++;
        } while (i != (0x80 << 2));
    }
    if ((unsigned)variant <= 1 || variant == 4 || variant == 5 ||
        variant == 6 || variant == 7)
        total = ((*(State **)(base + 0x7828))->f14 << 3) + 0x40;
    else
        total = ((*(State **)(base + 0x7828))->f14 << 3) + 0x20;
    if ((unsigned)variant > 1 && variant != 3)
        StartTask(Func_80dbb9c, 0x90 << 3);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    _PlaySound(0x8e);
    t = 0;
    for (frame = 0; frame != total; frame++) {
        cam = iwram_3001e80;
        if (variant == 7) {
            if (frame == total - 0x2e) {
                State *st = *(State **)(base + 0x7828);
                _Func_80b82c4(st->f8, st->ids[0], 0x10, 0);
            }
            if (frame == total - 0x20) {
                State *st;
                _Func_80bd7dc(0x86);
                st = *(State **)(base + 0x7828);
                _SetBattleActorKnockback(st->ids[0], 4);
                *(int *)(base + 0x77a8) = 8;
            }
        } else {
            if (frame == total - 0x20)
                _Func_80bd7dc(0x85);
        }
        {
            int *q;
            int a;
            q = (int *)(base + (0xd3 << 7));
            a = frame << 12;
            i = 0;
            do {
                *q++ = ((0x80 << 11) - (sin(a) << 2)) >> 10;
                a += 0x80 << 4;
                i++;
            } while (i != 0xa0);
        }
        for (j = 0; j != (*(State **)(base + 0x7828))->f14; j++) {
            State **sl;
            int lo;
            sl = (State **)(base + 0x7828);
            rec = (int *)*_GetBattleActor((*sl)->ids[j]);
            lo = j * 8;
            if (variant == 3) {
                if (frame > t && frame < t + 0x20) {
                    int m;
                    GetBattleActorPos2((*sl)->ids[j], &pos);
                    m = frame & variant;
                    i = 0;
                    do {
                        int a0;
                        int r;
                        int x;
                        int y;
                        a0 = Random() & 0xffff;
                        r = (Random() & 0x1f) + 4;
                        x = pos.x / 2 + ((sin(a0) * r) >> 17) - (Data_edeca[m] >> 1);
                        y = pos.y - ((cos(a0) * r) >> 16) - (Data_eded0[m] >> 1);
                        BuildDraw2DFuncEx(0x2f, 7, 7, 3 | Lee2ae[3 & Random()], 2);
                        ((DrawFn)iwram_3001f0c)(ctx,
                            base + Data_edebe[m] + (0x96 << 6),
                            x, y + 0x10, Data_edeca[m], Data_eded0[m]);
                        gfree(0x2f);
                        i++;
                    } while (i != 2);
                }
            }
            BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
            pt = gPtrs;
            fns[0] = (DrawFn)pt[0x2e];
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
            fp = fns;
            fp[1] = (DrawFn)iwram_3001f0c;
            InitMatrixStack();
            look = (char *)cam + 0xc;
            MatrixSetLook(cam, look);
            tp = &tv;
            tp->x = rec[2];
            tp->y = 0xa0 << 13;
            tp->z = rec[4];
            MatrixTranslatev(tp);
            if (frame > t) {
                int yaw;
                yaw = frame << 9;
                MatrixYaw(yaw);
                if ((unsigned)variant <= 1 || variant == 4)
                    MatrixPitch(yaw);
                {
                    Part *pw;
                    i = 0;
                    if (len != 0) {
                        pw = (Part *)((int *)gBuffer + j * 448);
                        do {
                            if (frame > lo + i) {
                                int d;
                                SqrtFn sq;
                                sq = Func_8000948;
                                d = sq((pw->x >> 8) * (pw->x >> 8) +
                                       (pw->y >> 8) * (pw->y >> 8) +
                                       (pw->z >> 8) * (pw->z >> 8)) >> 9;
                                if (d != 0 && pw->t <= 0x17) {
                                    int q;
                                    int x;
                                    vec3_t *mp;
                                    q = pw->t / 4;
                                    mp = &mv;
                                    Func_80e3944((vec3_t *)pw, mp);
                                    mp->x = mp->x >> 1;
                                    x = mp->x;
                                    if (variant == 5 || variant == 7) {
                                        int sz = 0x28;
                                        fp[1](ctx, base + q * 1600,
                                              x - 0x14, mp->y - 0x14, sz, sz);
                                    } else if (variant == 6) {
                                        fp[1](ctx, base + (0xc0 << 4),
                                              x - 6, mp->y - 0xc, 0xc, 0x18);
                                    } else if (variant == 4) {
                                        fp[1](ctx, base,
                                              x - 0xb, mp->y - 0x15, 0x16, 0x2a);
                                    } else {
                                        int sel;
                                        sel = (i & 3) != 0;
                                        fp[sel](ctx, base + q * 1152,
                                              x - 0xc, mp->y - 0x18, 0x18, 0x30);
                                    }
                                    if ((unsigned)variant <= 1 || variant == 4 ||
                                        variant == 5 || variant == 6) {
                                        pw->x -= pw->x / d;
                                        pw->y -= pw->y / d;
                                        pw->z -= pw->z / d;
                                    } else {
                                        pw->y += 0x80 << 9;
                                    }
                                    pw->t++;
                                    if (pw->t == 0x18) {
                                        if ((unsigned)variant <= 1 || variant == 4 ||
                                            variant == 5 || variant == 6) {
                                            pw->t = 0;
                                        } else {
                                            pw->x = ((Random() & 0xff) - 0x7f) << 13;
                                            pw->y = ((Random() & 0xff) - 0xff) << 12;
                                            pw->z = ((Random() & 0xff) - 0x7f) << 13;
                                        }
                                    }
                                }
                            }
                            i++;
                            pw++;
                        } while (i != len);
                    }
                }
            }
            gfree(0x2f);
            gfree(0x2e);
            if (frame == t + 0x10) {
                int n;
                n = total - frame;
                if (n > 0x1f)
                    n = 0x1f;
                Func_80d6888((*(State **)(base + 0x7828))->ids[j], 7, 5, j, n);
            }
            t += 8;
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
    }
    StopTask(Task_BlitAnim);
    if ((unsigned)variant > 1 && variant != 3)
        StopTask(Func_80dbb9c);
    AnimEnd();
}
