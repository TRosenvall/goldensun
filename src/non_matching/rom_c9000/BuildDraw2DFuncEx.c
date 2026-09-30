/* BuildDraw2DFuncEx -- 0x080ed408, asm/rom_c9000/rom_ed408.s, 737 instructions.
 *
 * NON-MATCHING, 718 of 781 encodings differ
 *
 * SIZE IS NOT EXACT and COUNT IS NOT EXACT: ref 1648 bytes / 781 encodings,
 * ours 1640 / 777 (-8 bytes, -4 encodings; -3 INSTRUCTIONS plus one pool/pad
 * word).  The objcmp figure above is therefore SATURATED and cannot rank
 * candidates -- rank by size-and-count first, then by the aligned view, which
 * reads
 *     aligned-equal 700 (89.6% of ref), 106 differing/ins/del in 91 hunks
 * and that is the number that moved this batch (v1 first draft 72.7%).
 *
 * THE RELOCATION SEQUENCE IS EXACT.  All 27 relocations -- one R_ARM_THM_CALL
 * to galloc_iwram and 26 R_ARM_ABS32 to the template symbols -- appear in the
 * ROM's order, symbol for symbol, only at different offsets.  That is the
 * cheapest confirmation that the program shape is right, and it was already
 * true of the second draft.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/BuildDraw2DFuncEx.c \
 *     asm/rom_c9000/rom_ed408.s --func BuildDraw2DFuncEx
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/BuildDraw2DFuncEx.c \
 *     asm/rom_c9000/rom_ed408.s BuildDraw2DFuncEx
 *
 * Everything above is plain production -O2; no flag row is needed and none was
 * found to help.
 *
 * SHIMS.  tools/shimcount.py reports `register pins : 4` and flags a
 * fakematch-class shim.  ALL FOUR ARE THE `register ... __asm__("rN")`
 * DECLARATIONS INSIDE DMA3_COPY_RW BELOW, which is a local copy of the
 * dma.h idiom -- the same four every dma.h helper has.  shimcount does not
 * count them when the helper lives in include/dma.h: the control,
 * src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_c_b.c, uses DMA3_CLEAR and
 * reports NOTHING.  So THIS CANDIDATE IS PIN-FREE ONCE DMA3_COPY_RW IS
 * PROMOTED TO include/dma.h, which is the tree change landing it requires --
 * exactly what batch 299 did for DMA3_COPY16_RW, on the standing instruction
 * "promote it if a second function needs it".  No other file needs it yet, so
 * a reviewer may prefer to leave it here until one does; the park is measured
 * with it here and the figures do not change on promotion (the text is
 * identical either way).
 *
 * SPLIT SHAPE.  asm/rom_c9000/rom_ed408.s holds ONE function and a trailing
 * .rodata block, so landing it is a CODE/DATA split, not a function split.
 * stage1.ld names the object TWICE:
 *     line 1943   asm/rom_c9000/rom_ed408.o(.text)     -> this .c's object
 *     line 1997   asm/rom_c9000/rom_ed408.o(.rodata)   -> a data-only object
 * The rodata block is asm/rom_c9000/rom_ed408.s:862-869 and THREE labels need
 * `.global` on the data half:
 *     .Leefa4   .Leefdc   .Lef034
 * A grep over the whole tree finds those three names in NO other file, so
 * three is the complete list.  They come into C as
 *     extern u32 Leefa4[] __asm__(".Leefa4");
 * per the tree's existing idiom (src/rom_c9000/rom_e0564_a_b.c:102-104).
 * Step 1 of the split is verifiable on its own with `make compare` and should
 * be landed before this .c is installed.  ALWAYS pass tools/split_s.py
 * --dry-run first.
 * The Data_ed* template symbols are NOT part of the split: they already live
 * in a sibling object, asm/rom_c9000/rom_eda78.s (`.incdata Data_edcc4,
 * 0xedcc4, 0xede48`).
 *
 * WHAT THE FUNCTION IS.  A runtime code generator, as the .s banner says.  It
 * (1) walks the feature flags and the pixel mode adding up how many WORDS of
 * ARM code it is about to emit, (2) `<< 2` and asks galloc_iwram for that many
 * bytes, (3) appends code fragments out of the template blob at Data_edcc4 --
 * by DMA3 for runs, by `*d++ = *s++` for single words -- skipping the ones the
 * flags do not want, patching two pixel-width literals as it goes, and
 * (4) back-patches the six branch placeholders it left behind, `(target - site
 * - 8) >> 2 & 0xffffff` being the ARM B/BL offset encoding.  It returns 1 if
 * the source cursor walked exactly to the end of the template (Data_ede48),
 * 0 otherwise -- a self-check that the emitter and the template agree.
 * `a`, `b` and `g` below are placeholder SITES; `c`, `e` and `f` are branch
 * TARGETS.  The size walk in the first half and the emit walk in the second
 * MUST stay in lockstep -- every `n +=` has exactly one emit counterpart, and
 * that is the invariant to check if anyone edits this.
 *
 * FOUR LEVERS PAID, IN ORDER, WITH FIGURES.  First draft 763 encodings /
 * 1604 bytes / 72.7% aligned; now 777 / 1640 / 89.6%.
 *
 *  1. EVERY SWITCH HAS A `case 0` THAT SHARES THE `default` ARM, AND THE
 *     SHARED ARM COMES FIRST.  Worth 763 -> 775 together with (2) and (3),
 *     and it is the largest item.  All six `switch (mode)` in the ROM dispatch
 *     as
 *         cmp #1 / beq case1 / cmp #1 / bcc arm0 / cmp #2 / beq / cmp #3 / beq
 *     and a three-case switch CANNOT produce that.  stmt.c's
 *     balance_case_nodes has an explicit "if there are just three nodes, split
 *     at the middle one", so cases {1,2,3} give root 2 and the balanced shape
 *         cmp #2 / beq / cmp #2 / bhi / cmp #1 / beq / b default / cmp #3 / beq
 *     which is what the first draft emitted, four instructions longer per
 *     switch.  FOUR nodes {0,1,2,3} take the bisect branch instead,
 *     i = (4+0+1)/2 = 2, which moves the head pointer ONCE, so root = node 1
 *     with left = {0} and right = {2,3}.  emit_case_nodes then emits the EQ
 *     for node 1, sees node_is_bounded(left) -- node 0 IS the minimum of an
 *     unsigned index, so its low bound is free -- and emits `LT -> case 0`,
 *     which is the `bcc`, and the ROM's `bcc` target is CASE 0's body, not the
 *     default's.  Here case 0 and default share one body, so the fallthrough
 *     after `cmp #3 / beq` lands on it too.  The shape was not guessed: a scan
 *     of the 3,914 compiler-generated .s files for `cmp rX,#N / beq /
 *     cmp rX,#N / bcc` found 15 sites, and the nearest has its .c beside it --
 *     src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_b.c, whose switch has
 *     cases 0,1,2,3 and an unsigned selector.  READ THAT FILE FIRST next time a
 *     small switch comes out four instructions short.
 *     The selector is `unsigned`: `bcc`, not `bgt`.  Its own header records
 *     that a signed selector costs four more instructions, and that is a
 *     SECOND, independent reason to keep it unsigned here.
 *
 *  2. CASES 1 AND 2 NEED SEPARATE ARMS EVEN WHERE THE BODY IS IDENTICAL.
 *     Two of the size switches add 4 for both.  Written `case 1: case 2:`,
 *     add_case_node merges the adjacent same-label pair into ONE RANGE node,
 *     which drops the count to three nodes (0, 1-2, 3), re-enters the
 *     three-node middle split, and emits a `bls` range test.  Written as two
 *     arms with the same text, the case tree keeps them distinct -- two `beq`
 *     to the same label, the ROM's -- and jump.c cross-jumps the bodies for
 *     free.  This is the complement of the usual advice: here the DUPLICATION
 *     in the source is what produces the ROM's dispatch.
 *
 *  3. A DMA SOURCE CHOSEN BY A FLAG IS AN if/else OVER TWO CALLS, NOT A
 *     TERNARY ARGUMENT.  Eight sites.  The ROM loads `&REG_DMA3SAD` in BOTH
 *     arms:
 *         beq L / ldr r3,=REG_DMA3SAD / ldr r0,=Data_edaf0 / b M
 *       L: ldr r3,=REG_DMA3SAD / ldr r0,=Data_edaf8
 *       M: mov r1,r5 / ldr r2,=cnt / stmia r3!,{r0,r1,r2}
 *     A ternary argument evaluates the choice first and expands the helper
 *     ONCE after the join, so the base load lands after M and the arms are one
 *     instruction shorter each.  Two whole calls expand the helper twice, and
 *     cross-jumping merges only the TAIL -- it stops at the `ldr r0,=Data*`
 *     that differs, which leaves the base load stranded above it in both arms.
 *     That the helper's register variables are emitted in DECLARATION order
 *     (base, src, dst, cnt) is what puts the base above the src and so out of
 *     cross-jumping's reach.
 *     It also explains the eight-step loop in the second switch's arm 0: the
 *     ROM hoists `flags & 4` and both template symbols out of that loop but
 *     NOT `&REG_DMA3SAD`, because the if/else gives that pseudo TWO SETS and
 *     scan_loop refuses a two-set pseudo as invariant.  With a ternary it has
 *     one set and gets hoisted, and the loop comes out short.  This is the
 *     batch-306 "two sets prevent a hoist" lever observed a second time, and
 *     the source shape that reaches it is the if/else, not an expression
 *     written twice.
 *
 *  4. THE `==` MUST BE SPELLED AS A ZERO TEST ON AN EXPLICIT XOR.  Worth 2.
 *     The ROM's return is branchless:
 *         eor r3,r6 / neg r2,r3 / orr r2,r3 / lsr r2,#31 / mov r0,#1 / sub r0,r2
 *     `return s == Data_ede48;` gives the branch form
 *     `mov r0,#0 / cmp / bne / mov r0,#1` instead, and so do `? 1 : 0`,
 *     `!(s != end)`, an int cast on both sides, and assigning to a local
 *     first -- five spellings, all measured, all the branch form.  expand's
 *     do_store_flag reaches the branchless sequence only when the comparison
 *     is already against ZERO, because that is the only form the Thumb
 *     `neg/orr/lsr #31` trick covers; a compare against a pooled symbol is
 *     not.  Doing the XOR in the source, into its own unsigned local, and
 *     then testing THAT against 0 is what hands do_store_flag the shape it
 *     can take.  Generalises to any ROM `x == y` that comes out branchless.
 *
 * THE BLOCKER: combine.c FOLDS CONSECUTIVE CONSTANT ADDITIONS TO ONE PSEUDO,
 * AND THE ROM HAS THREE THAT IT DID NOT FOLD.  Worth exactly 3 instructions
 * and it is the WHOLE remaining count gap.  The ROM's size walk contains
 *         cmp r3,#0 / beq L / add r1,#2 / add r1,#2 / add r1,#5
 * where every candidate emits `b L / add r1,#9`: the three adds fold to one
 * and the second test of `flags & 1` is threaded away into an if/else.
 *
 *   Why the adds fold.  gcc-2.96 gives ONE PSEUDO PER DECLARED VARIABLE, so
 *   the three insns are (set n (plus n 2)), (set n (plus n 2)),
 *   (set n (plus n 5)).  try_combine substitutes i2's source into i3 and asks
 *   whether it must KEEP i2: `added_sets_2 = ! dead_or_set_p (i3, i2dest)`,
 *   and i3 SETS n, so n is dead at i3, added_sets_2 is 0, i2 is deleted and
 *   the fold is legal.  Nothing about the spelling changes that -- the
 *   variable already has many sets, which is the usual escape and does not
 *   apply because the blocking condition is on the INTERMEDIATE, and here the
 *   intermediate IS the accumulator.
 *
 *   The corpus says it is unreachable.  A scan of every compiler-generated .s
 *   in asm/ (3,914 files, gcc's own banner) for two consecutive
 *   `add rX, rX, #imm` on the SAME register found ZERO occurrences.  Not rare:
 *   none.  So no C spelling in this configuration has ever produced the ROM's
 *   shape, and the three adds are not a spelling I failed to find.
 *
 *   What was ruled out, with figures.  Four pin-free spellings measured at
 *   `add r1,#9` with one test: the three adds under one `if` (this file),
 *   three separate `if (flags & 1) n += K;`, the same through a named
 *   `int t = flags & 1` with `== 0` / `!= 0` tests, and an if/else.  The two
 *   tests and the three adds are ONE residue with two causes: at jump1 the two
 *   `flags & 1` are distinct pseudos so thread_jumps cannot fire, cse1 then
 *   commons them, combine folds the adds, and jump2 -- which runs after
 *   combine -- threads the now-identical tests.  Both halves therefore need
 *   something between the adds, and nothing source-level is.
 *
 *   The pinned route, measured.  Two `__asm__ volatile ("" : "+r" (n))`
 *   between the adds read 779 of 781 / 1644 bytes / 90.0% aligned -- CLOSER
 *   ON BOTH AXES than this pin-free file, leaving one instruction (the
 *   threaded test) and the allocation cluster below.  A THIRD barrier before
 *   the `if`, to put a non-jump insn at the thread target, went BACKWARDS to
 *   777 / 1640 / and is not the answer, and so did an operand-free
 *   `__asm__ volatile ("")` in the same place, also 777 -- so the threaded
 *   test costs 2 to attack and buys 1, whichever barrier form is used.  shimcount classes the two as a
 *   fakematch, so they would need a fakematch.txt row; this file is parked
 *   pin-free deliberately, and the 2-pin variant is the thing to reach for
 *   only if someone closes the allocation cluster first.
 *
 * WHAT IS LEFT AFTER THAT IS ONE ALLOCATION CLUSTER, NOT A SECOND BLOCKER.
 * With the three adds excluded, every other hunk is a register choice or an
 * adjacent transposition and the instruction MULTISET matches:
 *   - `dstw` and `b`.  The ROM spills the third parameter (`str r2,[sp,#0xc]`,
 *     `sub sp,#0x10`, four slots) and keeps the placeholder site `b` in r11.
 *     We do the reverse: `mov r11,r2` for dstw, `str r5,[sp,#4]` for b, three
 *     slots, `sub sp,#0xc`.  It is 1-for-1 in instructions at every one of the
 *     four sites (`ldr r3,[sp,#0xc]` against `mov r3,r11` and so on), which is
 *     why the count survives it.  Both are two-reference values with
 *     whole-function ranges, so by allocno_compare's three inputs (n_refs,
 *     live_length, declaration order) only declaration order separates them,
 *     and dstw is a parameter so it is declared first and wins.  THE REUSE
 *     LEVER IS THE THING TO TRY: `b` wants r11, and a donor whose earlier
 *     range already lands there would hand it over.
 *   - `k` and the eight-step counter take r4 where the ROM takes r3.  Note
 *     r4 is CALLER-saved here (-fcall-used-r4), so this is find_reg's
 *     accept_call_clobbered retry firing where the ROM's did not; the
 *     batch-306 complement (shorten a competitor's live range) is the lever
 *     to try, not reordering.
 *   - about a dozen adjacent transpositions, all of the same two shapes:
 *     a `mov rlo, r8` reload copy for the Thumb `and` moving one slot, and a
 *     `stmia r5!,{r3}` trading places with the instruction after it.  These
 *     are sched2, whose tie-break is priority -> dependent count -> INSN_LUID;
 *     DO NOT reach for -fno-schedule-insns, which controls sched1 and sched1
 *     DOES NOT RUN in this build.
 * Close the allocation cluster and this is a candidate for the reuse lever,
 * with only the 3-instruction combine residue between it and a match.
 */
#include "gba/types.h"
#include "gba/io.h"

extern void *galloc_iwram(int tag, int size);

extern u32 Data_edaf0[];
extern u32 Data_edaf8[];
extern u32 Data_edb00[];
extern u32 Data_edb10[];
extern u32 Data_edb20[];
extern u32 Data_edb84[];
extern u32 Data_edbe8[];
extern u32 Data_edbf8[];
extern u32 Data_edc08[];
extern u32 Data_edc48[];
extern u32 Data_edc88[];
extern u32 Data_edca0[];
extern u32 Data_edcb8[];
extern u32 Data_edcc4[];
extern u32 Data_ede48[];

extern u32 Leefa4[] __asm__(".Leefa4");
extern u32 Leefdc[] __asm__(".Leefdc");
extern u16 Lef034[] __asm__(".Lef034");

/* DMA3_COPY without the promise that the transfer preserves src and count --
 * the 32-bit analogue of dma.h's DMA3_COPY16_RW.  Every stmia in the ROM
 * reloads both r0 and r2, and the count is NOT hoisted out of the eight-step
 * loop even though it is invariant, which is only possible if it is an output.
 */
static inline void DMA3_COPY_RW(void *src, void *dst, u32 size)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register void *_src __asm__("r0") = src;
    register void *_dst __asm__("r1") = dst;
    register u32 _cnt __asm__("r2") = 0x84000000 | (size / 4);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        : "+l" (_src), "+l" (_cnt)
        : "l" (_base), "l" (_dst)
        : "memory"
    );
}

#define BOFF(site, target) \
    (((((u32)(target)) - ((u32)(site)) - 8) >> 2) & 0xffffff)

int BuildDraw2DFuncEx(int tag, int srcw, int dstw, int flags, u32 mode)
{
    u32 *d;
    u32 *s;
    u32 *a;
    u32 *b;
    u32 *c;
    u32 *e;
    u32 *f;
    u32 *g;
    u32 *t;
    u32 u;
    int n;
    int k;
    int i;

    n = 3;
    if (mode == 3)
        n = 6;
    k = flags & 0xc;
    if (k == 4)
        n += 3;
    if (k == 8)
        n += 4;
    if (k == 0xc)
        n += 3;
    if (k == 0)
        n += 1;
    if (flags & 2)
        n += 7;
    n += 2;
    if (!(flags & 1))
        n += 1;
    if (flags & 1) {
        n += 2;
        n += 2;
        n += 5;
    }
    n += 9;
    if (!(flags & 1))
        n += 1;
    n += 9;
    if (!(flags & 1))
        n += 1;
    n += 3;
    switch (mode) {
    default:
    case 0:
        n += 2;
        break;
    case 1:
        n += 4;
        break;
    case 2:
        n += 4;
        break;
    case 3:
        n += 6;
        break;
    }
    n += 3;
    if (!(flags & 1))
        n += 2;
    n += 2;
    switch (mode) {
    default:
    case 0:
        for (i = 0; i < 8; i++)
            n += 2;
        break;
    case 1:
        n += 0x19;
        break;
    case 2:
        n += 0x20;
        break;
    case 3:
        n += 0x1c;
        break;
    }
    n += 1;
    if (!(flags & 1))
        n += 2;
    n += 5;
    switch (mode) {
    default:
    case 0:
        n += 2;
        break;
    case 1:
        n += 4;
        break;
    case 2:
        n += 4;
        break;
    case 3:
        n += 6;
        break;
    }
    n += 8;

    d = galloc_iwram(tag, n << 2);
    s = Data_edcc4;
    DMA3_COPY_RW(s, d, 12);
    d += 3;
    s += 3;
    if (mode == 3) {
        DMA3_COPY_RW(Data_edcb8, d, 12);
        d += 3;
    }
    k = flags & 0xc;
    if (k == 4) {
        DMA3_COPY_RW(s, d, 12);
        d += 3;
    }
    s += 3;
    if (k == 8) {
        DMA3_COPY_RW(s, d, 16);
        d += 4;
    }
    s += 4;
    if (k == 0xc) {
        DMA3_COPY_RW(s, d, 12);
        d += 3;
    }
    s += 3;
    if (k == 0)
        *d++ = s[0];
    s += 1;
    if (flags & 2) {
        *d++ = s[0];
        *d++ = s[1];
        if (flags & 8)
            *d++ = s[2];
        else
            *d++ = s[3];
        *d++ = s[4];
        *d++ = s[5];
        *d++ = s[6] + (1 << dstw);
        *d++ = s[7] + (1 << dstw);
    }
    s += 8;
    *d++ = *s++;
    a = d;
    *d++ = *s++;
    if (!(flags & 1))
        *d++ = s[0] + (1 << srcw) - 1;
    s += 1;
    if (flags & 1) {
        *d++ = s[0];
        *d++ = s[1];
        if (flags & 4) {
            *d++ = s[2];
            *d++ = s[3];
        } else {
            *d++ = s[4];
            *d++ = s[5];
        }
        *d++ = s[6];
        *d++ = s[7];
        *d++ = s[8] + Lef034[srcw];
        if (flags & 4)
            *d++ = s[9];
        else
            *d++ = s[10];
        *d++ = s[11] + Lef034[srcw];
    }
    s += 12;
    *d++ = *s++;
    b = d;
    *d++ = *s++;
    DMA3_COPY_RW(s, d, 24);
    d += 6;
    s += 6;
    *d++ = *s++;
    if (!(flags & 1))
        *d++ = s[0];
    s += 1;
    DMA3_COPY_RW(s, d, 20);
    d += 5;
    s += 5;
    c = d;
    *d++ = s[0];
    *d++ = s[1] + (1 << (dstw - 3)) - 1;
    *d++ = s[2] + ((srcw - 3) << 7);
    *d++ = s[3];
    s += 4;
    if (!(flags & 1))
        *d++ = s[0];
    s += 1;
    *d++ = *s++;
    g = d;
    *d++ = *s++;
    if (flags & 4)
        *d++ = s[0];
    else
        *d++ = s[1];
    s += 2;
    e = d;
    switch (mode) {
    default:
    case 0:
        if (flags & 4)
            DMA3_COPY_RW(Data_edaf0, d, 8);
        else
            DMA3_COPY_RW(Data_edaf8, d, 8);
        d += 2;
        break;
    case 1:
        if (flags & 4)
            DMA3_COPY_RW(Data_edb10, d, 16);
        else
            DMA3_COPY_RW(Data_edb00, d, 16);
        d += 4;
        break;
    case 2:
        if (flags & 4)
            DMA3_COPY_RW(Data_edbf8, d, 16);
        else
            DMA3_COPY_RW(Data_edbe8, d, 16);
        d += 4;
        break;
    case 3:
        if (flags & 4)
            DMA3_COPY_RW(Data_edca0, d, 24);
        else
            DMA3_COPY_RW(Data_edc88, d, 24);
        d += 6;
        break;
    }
    s += 4;
    *d++ = *s++;
    *d++ = *s++ + BOFF(d, e);
    *d++ = *s++;
    if (!(flags & 1)) {
        *d++ = s[0];
        *d++ = s[1];
    }
    s += 2;
    *g |= BOFF(g, d);
    *d++ = *s++;
    g = d;
    *d++ = *s++;
    f = d;
    switch (mode) {
    default:
    case 0:
        for (i = 0; i < 8; i++) {
            if (flags & 4)
                DMA3_COPY_RW(Data_edaf0, d, 8);
            else
                DMA3_COPY_RW(Data_edaf8, d, 8);
            d += 2;
        }
        break;
    case 1:
        if (flags & 4)
            DMA3_COPY_RW(Data_edb84, d, 100);
        else
            DMA3_COPY_RW(Data_edb20, d, 100);
        d += 0x19;
        break;
    case 2:
        t = (flags & 4) ? Data_edc48 : Data_edc08;
        DMA3_COPY_RW(t, d, 64);
        d += 0x10;
        DMA3_COPY_RW(t, d, 64);
        d += 0x10;
        break;
    case 3:
        t = (flags & 4) ? Leefdc : Leefa4;
        DMA3_COPY_RW(t, d, 56);
        d += 0xe;
        DMA3_COPY_RW(t, d, 56);
        d += 0xe;
        break;
    }
    s += 4;
    *d++ = *s++;
    if (!(flags & 1)) {
        *d++ = s[0];
        *d++ = s[1];
    }
    s += 2;
    *d++ = *s++;
    *d++ = *s++ + BOFF(d, f);
    *g |= BOFF(g, d);
    *d++ = *s++;
    g = d;
    *d++ = *s++;
    if (flags & 4)
        *d++ = s[0];
    else
        *d++ = s[1];
    s += 2;
    e = d;
    switch (mode) {
    default:
    case 0:
        if (flags & 4)
            DMA3_COPY_RW(Data_edaf0, d, 8);
        else
            DMA3_COPY_RW(Data_edaf8, d, 8);
        d += 2;
        break;
    case 1:
        if (flags & 4)
            DMA3_COPY_RW(Data_edb10, d, 16);
        else
            DMA3_COPY_RW(Data_edb00, d, 16);
        d += 4;
        break;
    case 2:
        if (flags & 4)
            DMA3_COPY_RW(Data_edbf8, d, 16);
        else
            DMA3_COPY_RW(Data_edbe8, d, 16);
        d += 4;
        break;
    case 3:
        if (flags & 4)
            DMA3_COPY_RW(Data_edca0, d, 24);
        else
            DMA3_COPY_RW(Data_edc88, d, 24);
        d += 6;
        break;
    }
    s += 4;
    *d++ = *s++;
    *d++ = *s++ + BOFF(d, e);
    *g |= BOFF(g, d);
    DMA3_COPY_RW(s, d, 12);
    d += 3;
    s += 3;
    *d++ = *s++ + BOFF(d, c);
    *a |= BOFF(a, d);
    *b |= BOFF(b, d);
    *d++ = *s++;
    *d = *s++;
    u = (u32)s ^ (u32)Data_ede48;
    return u == 0;
}
