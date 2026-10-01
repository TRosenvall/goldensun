/* Anim_TitanBlade -- 0x080e99c0, asm/rom_c9000/rom_e7320_c_c.s line 4764,
 * 779 ROM instructions.
 * NON-MATCHING, 731 of 816 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 731 is NOT a distance: 1796 bytes against
 * the ROM's 1816 (-20) and 807 encodings against 816 (-9) -- we are SHORT, not
 * long, and only just.  tools/aligncmp.py reads 514 aligned-equal of 816
 * (63.0%), 333 differing/ins/del in 144 hunks, and 63.0% / 144 hunks is the
 * figure to beat.  *** -20 BYTES AND -9 ENCODINGS IS THE CLOSEST FIRST PASS ANY
 * FUNCTION IN THIS FAMILY HAS OPENED WITH, and 63.0% is the highest aligned
 * figure in the family. ***
 *
 * THE FRAME IS TWO SLOTS OVER: `sub sp, #0x3c` against the ROM's `sub sp, #0x34`.
 * The accounting is exact on the ROM's side and names every byte: 8 bytes of
 * outgoing-argument area, EIGHT spilled scalars at 0x24 down to 0x08, and ONE
 * 12-byte address-taken aggregate at 0x28 -- 8 + 32 + 12 = 52 = 0x34.  We spill
 * TEN, two pass-created pseudos more than the ROM, and the aggregate then lands
 * at 0x30 instead of 0x28.  By the descending-offset rule the ROM's declaration
 * order is base (0x24), ctx (0x20), f2 (0x1c), f1 (0x18), g2 (0x14), cx (0x10)
 * -- SIX declared locals, with 0x0c (frame - 0x20) and 0x08 (cx << 16) BELOW them
 * and therefore pass-created.  This candidate declares exactly those six in that
 * order, plus `cx16`, which is lever (3) below and is the one place the rule is
 * knowingly broken because the measurement says to.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/e99c0_TitanBlade.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func Anim_TitanBlade
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/e99c0_TitanBlade.c
 * (it includes the family header src/non_matching/rom_c9000/decls.h by plain
 * name and compiles verbatim from that directory -- no edit on install.)
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and here the precondition is CHECKED rather than assumed: the two
 * Func_80008d8 fill sites are the `frame == 9` arm and the `frame == 0x3c` arm,
 * which are DIFFERENT basic blocks, so the Blob three-site pin has nothing to
 * defeat.  Same reading as BaseAnim_Attack's, third confirmation of the
 * basic-block precondition.
 *
 * ================================================================
 * THE SPLIT SHAPE AND THE EXACT `.global` LIST -- BOTH TOOLS RUN, --dry-run ONLY
 * ================================================================
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e7320_c_c.s` reports a .rodata
 * section and EIGHT functions, so converting any one of them needs a TEXT/DATA
 * split and the data must keep its own object:
 *     Func_80e7338, Func_80e73a0, BaseAnim_Meteor, Anim_Ramses, Anim_DragonCloud,
 *     Anim_Annihilation, Anim_Ragnarok, Anim_TitanBlade
 * Anim_TitanBlade is the LAST of the eight, so the cut is THREE ways:
 *     asm/rom_c9000/rom_e7320_c_c_a.s   the other seven functions
 *     src/rom_c9000/rom_e7320_c_c_b.c   Anim_TitanBlade
 *     asm/rom_c9000/rom_e7320_c_c_c.s   the .rodata tail
 *
 * EXPORTS LANDED IN BATCH 311.  All twelve `.global` lines are now in
 * asm/rom_c9000/rom_e7320_c_c.s, each immediately before its label (the house
 * convention, from asm/rom_b5000/rom_b5368_c.s:7), in one build+compare-gated
 * commit.  The dry-run now SUCCEEDS and prints exactly the three-way shape
 * predicted below -- 7 functions / 1 function / 39-line data tail -- so that
 * shape is verified rather than inferred, and EVERY LATER SPLIT OF THIS
 * EIGHT-FUNCTION FILE IS NOW FREE OF ASM WORK.  The dry-run was confirmed to
 * have written nothing (no split files, stage1.ld untouched), which matters
 * because this is the tool whose --dry-run was silently ignored until batch
 * 302 gave it a real flag.
 *
 * The refusal recorded below is the PRE-311 state, kept for the record:
 * `python3 tools/split_s.py asm/rom_c9000/rom_e7320_c_c.s Anim_TitanBlade
 * --dry-run` REFUSED until the exports existed -- "12 local label(s) would cross
 * files" -- and datacheck attributes every one of the twelve.  THE EXACT
 * `.global` LIST, in the order the tool prints it:
 *
 *     .global .Leee76      BaseAnim_Meteor
 *     .global .Leeea0      BaseAnim_Meteor
 *     .global .Leeebc      BaseAnim_Meteor
 *     .global .Leeeca      BaseAnim_Meteor
 *     .global .Leeed8      Anim_Ramses
 *     .global .Leeee1      Anim_Ramses
 *     .global .Leeeea      Anim_Ramses
 *     .global .Leeef8      Anim_Ramses
 *     .global .Leef06      Anim_Ragnarok
 *     .global .Leef0c      Anim_Ragnarok
 *     .global .Leef12      Anim_TitanBlade   (this function's own)
 *     .global .Leef18      Anim_TitanBlade   (this function's own)
 *
 * Ten of the twelve cross on the _a piece and only two on the _b piece, but the
 * tool demands all twelve and that is the CHEAP outcome: a `.global` emits no
 * bytes, so ONE `make compare`-gated commit adding all twelve is byte-neutral and
 * makes EVERY LATER split of this 8-function file free of asm work.  Four of the
 * eight are already parked in src/non_matching/rom_c9000 (Anim_DragonCloud,
 * Anim_Annihilation, Anim_Ragnarok, Anim_Ramses' neighbours), so the file will be
 * cut more than once and the twelve-export commit pays for itself immediately.
 * Func_80e7338, Func_80e73a0, Anim_DragonCloud and Anim_Annihilation read no data
 * label and need nothing.
 *
 * LINKER SCRIPT: stage1.ld names this object TWICE -- line 1932 `(.text)` and
 * line 1993 `(.rodata)`.  The .rodata line must end up on the piece that KEEPS
 * the data, i.e. the _c piece.  The batch-300 hazard is live here.
 * ORDER: exports FIRST, verify `make compare`, and only THEN split.  Nothing in
 * the tree was touched by this batch; `make compare` is owed for both steps.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline t1 (the family template, per-loop block-scoped counters, per-loop
 * walkers): size 1780 (-36), count 799 (-17), 426 aligned (52.2%), 468
 * differing, 173 hunks, objcmp 782.  The relocation sequence was already in the
 * ROM's order on that first candidate.
 *
 * (1) *** UNIFY EVERY INNER-LOOP COUNTER INTO ONE VARIABLE.  52.2% -> 60.8%
 *     aligned, 468 differing -> 352, AND BOTH GATING FIGURES MOVED TOWARD THE
 *     REFERENCE AT ONCE: size 1780 -> 1796 (-36 -> -20), count 799 -> 807
 *     (-17 -> -9), objcmp 782 -> 731.  LARGEST STEP ON THE FUNCTION AND THE ONLY
 *     ONE THIS BATCH THAT MOVED SIZE, COUNT AND ALIGNED TOGETHER. ***
 *     This function has NINE disjoint counted loops -- three particle seeds, the
 *     `frame > 0x37` trail loop, the `frame == 0x1c` reseed, the
 *     `(unsigned)(frame - 0x20) <= 0x1f` reseed, the `frame <= 0x47` physics
 *     loop, the `frame > 0x3b` ewram_2014ad0 loop and the `frame == 0x44`
 *     knockback loop -- and the ROM makes 38 references to r8 across all nine.
 *     One `int i` reproduces 37 of them.  The second counter, loop C's seeded
 *     count, stays its own variable because the ROM gives it r9.
 *     *** THIS IS THE EXACT CONVERSE OF WHAT PAID ON BaseAnim_RapidSlash IN THE
 *     SAME BATCH, where splitting one counter into three was worth 41.0% ->
 *     47.3%.  Two functions, the same r8-across-disjoint-loops evidence, opposite
 *     answers -- so the shared register is NOT the discriminator and the
 *     partition has to be measured per function.  The useful half: try BOTH
 *     directions, they are one edit apart and the payoff is large either way. ***
 *
 * (2) READ THE LAST TWO ARMS OF THE `frame <= 0x5f` y-DISPATCH OFF THE BRANCH
 *     SENSE.  60.8% -> 61.0%, 352 -> 351, hunks 150 -> 149.  The ROM has
 *     `cmp r3,#9 / bgt .Le9f24` with the shift arm FALLING THROUGH and the
 *     constant arm at the branch target, which is `else if (frame <= 9)
 *     y = (frame << 4) - 0x80; else y = 0x10;` -- writing the test the other way
 *     round (`if (frame > 9) y = 0x10; else ...`) inverts the layout and loses
 *     the `b` plus the `movs r4,#0x10` that aligncmp showed as ref-only.  Small,
 *     but it is program shape rather than allocation, so it is not negotiable.
 *
 * (3) *** NAME `cx << 16` ONCE.  61.0% -> 63.0%, 351 -> 333, hunks 149 -> 144.
 *     THIS IS THE SECOND-LARGEST STEP AND IT CONTRADICTS THE SLOT-ORDER RULE. ***
 *     The value appears three times in the source -- the third particle seed's
 *     `q->x`, and the x seed in each of the two gBuffer reseed loops -- and the
 *     ROM holds it in ONE pseudo at slot 0x08, computed in the third seed loop's
 *     preheader (`ldr r0,[sp,#0x10] / lsl r0,#16 / str r0,[sp,#8]`).  Left as
 *     three textual occurrences gcc makes more than one pseudo; hoisting it into
 *     `int cx16 = cx << 16;` just before that loop collapses them.
 *     Slot 0x08 is the LOWEST slot, so by the declaration-order rule the ROM's
 *     copy is pass-created and `cx16` should NOT be declared -- and declaring it
 *     is still worth 18 exact encodings.  Recorded as a measured exception, the
 *     same kind of conflict RapidSlash's park records from the other side.
 *
 * ================================================================
 * MEASURED INERT (untested, not disproved)
 * ================================================================
 *   - Naming the 0x3f Random mask as a local in the two reseed bodies (the ROM
 *     keeps it in r10 across each loop): BYTE-IDENTICAL, 498/351/149.
 *   - Spelling the second `(unsigned)(frame - 0x20) <= 0x1f` guard off `frame`
 *     instead (`frame >= 0x20 && frame <= 0x3f`), to try to stop jump.c merging
 *     it with the first: BYTE-IDENTICAL.  See blocker (B).
 * MEASURED WORSE:
 *   - The `frame == 0x44` knockback loop guarded by a NESTED `if` instead of
 *     `if (frame == 0x44 && (*slot)->f14 != 0)`: 487 aligned (59.7%), 363
 *     differing, 152 hunks against 498/351/149.  The asm superficially reads as
 *     nested -- `cmp r3,#0 / beq` after the slot load -- but that is
 *     duplicate_loop_exit_test on the `while`, not a source `if`, and the `&&`
 *     form is what reaches it.  A reminder that an entry test against zero is
 *     the loop's, not the guard's.
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) *** `frame` TAKES r10 FOR US AND r11 IN THE ROM -- global.c's find_reg,
 *     and it is the two extra spill slots and most of the residue. ***
 *     The register census states the gap exactly:
 *         ROM   r8=38 (the unified counter)  r9=6 (loop C's count)
 *               r10=15 (the short-lived block quantities)  r11=32 (frame)
 *         ours  r8=37                        r9=12
 *               r10=31 (frame)               r11=5
 *     r8 is now within one reference of the ROM, which is lever (1) landing; but
 *     `frame` -- 0x66 iterations, referenced in nineteen separate tests inside the
 *     body -- goes to r10 for us and r11 for the ROM, and the quantities the ROM
 *     puts in r10 (the `.Leef12` table base, the 0x3f mask, `cx - 0x11`, the 0x22
 *     draw width) are in r9 and low registers for us.  One high-register
 *     quantity is mis-ranked and everything downstream of it shifts, including
 *     two pseudos that then have to spill.  This is the same class of defect as
 *     BaseAnim_RapidSlash's blocker (A) -- one allocno on the wrong side of the
 *     priority sort -- and on THIS function it is much closer to closing,
 *     because size is only -20 and count only -9.
 *
 * (B) jump.c MERGES THE TWO `(unsigned)(frame - 0x20) <= 0x1f` TESTS.  The ROM
 *     tests it TWICE -- once to guard the gBuffer reseed loop and again to guard
 *     the two `__modsi3` draw calls -- and the second test is a real four-insn
 *     block (`ldr r1,[sp,#0xc] / cmp r1,#0x1f / bhi / mov r2,fp`) that aligncmp
 *     reports as ref-only.  The label between them has two predecessors (the
 *     loop's normal exit and its `cnt == 0x10` break), which is what stops gcc
 *     folding them in the ROM; our stream folds them anyway.  Re-spelling the
 *     second condition off `frame` was byte-identical, so the merge is not
 *     driven by the expression -- it is the block structure.  Worth 4 encodings
 *     plus the register assignment around them.
 *
 * (C) THE CONSTANT `7` IS SHARED WHERE THE ROM REMATERIALIZES IT.  In the
 *     `frame == 0x44` loop the ROM emits `mov r1,#7` for Func_80d6888's second
 *     argument AND AGAIN for _SetBattleActorKnockback's, and reloads `*slot` and
 *     `(*slot)->f14` for the loop test on every iteration; we common the 7.  This
 *     is the access-count rule applied to a CONSTANT rather than a field, and I
 *     found no source spelling that reaches it -- a local carrier per call site
 *     was not tried and is the obvious next probe.
 *
 * ================================================================
 * FACTS FOR WHOEVER REOPENS THIS, none of them guessed
 * ================================================================
 *  - *** ITS REG_BLDALPHA IS ADDRESS-FIRST, SO THERE IS NO `int` CARRIER HERE.
 *    `ldr r2,=REG_BLDALPHA / ldr r3,.Le9a2c @0x1010 / strh r3,[r2]` -- address
 *    load first, so `REG_BLDALPHA = 0x1010;` is written DIRECTLY, and it is in
 *    this candidate that way.  This is the SIXTH row for batch 308A's load-order
 *    table and it lands on the address-first side with Breath, Ragnarok and Nova;
 *    Blob and RapidSlash are the value-first pair that need the carrier. ***
 *  - IT TAKES ONE PARAMETER, `(void *context)`, not the two-parameter
 *    Blob/Breath/RapidSlash form, and `context` is never spilled -- it goes
 *    straight into `*slot` and dies.  There is no parameter slot in the frame.
 *  - *** IT CALLS AnimStart(1), NOT AnimStart(0). ***  Every other function read
 *    in this family so far passes 0.  `mov r0,#1 / bl AnimStart`, unmistakable,
 *    and getting it wrong is a free encoding lost in the first twenty.
 *  - THE DRAW FUNCTIONS COME OUT OF iwram_3001eec ITSELF, NOT gPtrs.  `f1` is
 *    `g[7]` and `f2` is `g[8]` (`ldr r0,[r6,#0x1c]`, `ldr r6,[r6,#0x20]` off the
 *    one pool word already loaded for the prologue), so there is no second pool
 *    word and no gPtrs reference anywhere in the function.  Contrast
 *    BaseAnim_RapidSlash, which uses gPtrs[0x2e] and iwram_3001f0c.
 *  - THE MAIN FRAME LOOP IS `do`-`while`, NOT `while`.  `mov r11,#0` then the
 *    body, and the ONLY test is `cmp r1,#0x66 / beq / b` at the bottom -- no
 *    duplicate_loop_exit_test entry test, so jump.c:1137 never saw a `while`.
 *    Contrast all three of BaseAnim_RapidSlash's `while` loops, which do carry
 *    the entry test.  The `frame == 0x44` knockback loop IS a `while` and its
 *    entry test proves it; every other loop here is `do`-`while`.
 *  - IT DIVIDES BY 3 AND BY 5 AND MODS BY 3 AND BY 0x68 THROUGH THE LIBRARY.
 *    `bl __divsi3` with r1=3 and r1=5, `bl __modsi3` with r1=3 and r1=0x68.
 *    Thumb-1 has no `umull`, so gcc-2.96 cannot use the reciprocal-multiply
 *    expansion and calls the helper for every non-power-of-two divisor.  Write
 *    the divisions plainly (`t / 3`, `i % 3`, `t / 5`, `x % 0x68`) and they land;
 *    do NOT hand-expand them.  The `* 62 / 64` in the physics loop, by contrast,
 *    IS inline -- `lsl r3,r2,#5 / sub r3,r2 / lsl r3,#1` then the
 *    `cmp / add #0x3f / asr #6` round-toward-zero sequence, which is synth_mult
 *    plus a power-of-two divide.
 *  - .Leef12 IS A BYTE TABLE indexed by `p->t / 3` (`bl __divsi3` r1=3, then
 *    `ldrb r1,[r10,r0]`, then `<< 11` into a base offset), and .Leef18 IS A WORD
 *    TABLE indexed by `i & 3` (`and r2,r4 / lsl r2,#2 / ldr r3,[r3,r2]`).  That
 *    settles what the two tables the split must export actually are.
 *  - THE PARTICLE X/Y ARE READ AS SIGNED HALFWORDS AT OFFSETS 2 AND 6, i.e. the
 *    HIGH half of each 16.16 field: `mov r4,#2 / ldrsh r3,[r5,r4]` and
 *    `mov r2,#6 / ldrsh r7,[r5,r2]`.  Spelled `*(short *)((char *)p + 2)` and
 *    `+ 6` in this candidate.  The register+register form is forced -- Thumb-1
 *    `ldrsh` has no immediate-offset encoding at all -- so the `mov rX,#2` beside
 *    each one is not a missed constant fold.
 *  - Data_ede48 IS INDEXED THE SAME WAY AS IN BaseAnim_Attack AND
 *    BaseAnim_RapidSlash: `*(unsigned short *)((char *)Data_ede48 + (sz - 2))`
 *    with `sz = k * 2`.  Third function in the family on the same spelling, twice
 *    within this one function.
 *  - THREE UNSIGNED COMPARES ARE REAL AND MUST BE SPELLED UNSIGNED:
 *    `(unsigned)(frame - 0x19) <= 0x16`, `(unsigned)(frame - 0x20) <= 0x1f`,
 *    `(unsigned)p->t <= 0x11` and `(unsigned)q->x <= 0x7effff` all come out as
 *    `bhi`, and a signed pair of tests would cost two branches each.
 *  - ORACLES: Anim_Ragnarok is the function IMMEDIATELY BEFORE this one in the
 *    same .s, parked at 296 of 584 with size exact, all 49 relocations exact and
 *    the frame exact, so its header is already reconciled against this exact
 *    .rodata run and this exact linker entry.  Anim_Annihilation.c and
 *    Anim_DragonCloud.c are two more parks from the same object.
  *
 * *** SUFFIX COLLISION, FLAGGED IN BATCH 313 -- DO NOT TRUST THE STEM BELOW. ***
 * FIVE parks in this directory plus one recon all name `rom_e7320_c_c_b` as
 * their own output stem: Anim_Ragnarok, Anim_Annihilation, Anim_DragonCloud,
 * Anim_Ramses, e99c0_TitanBlade and RECON_Anim_TitanBlade.txt.  ONLY ONE CAN
 * HAVE IT.  The stem a cut produces depends on the member's POSITION in the
 * file, so these claims are mutually exclusive rather than merely duplicated --
 * and `split_s.py` cuts out ONE NAMED TARGET, so the shape changes for everyone
 * else each time a member converts.
 *
 * Whoever converts FIRST takes `_b`.  Every later conversion of this
 * eight-function file must RE-DERIVE its stems from a FRESH `--dry-run` against
 * the then-current file, never from the shape recorded here.  Check this before
 * writing any linker row.  (The twelve `.global` exports this file needed landed
 * in batch 312, so the cut itself is no longer blocked -- only the naming is.)
*/
#include "decls.h"

extern void GetBattleActorPos3(int id, vec3_t *out);
extern void Func_80e3908(Part *p, int a, int b);
extern Part ewram_2010018[];
extern Part ewram_2014ad0[];
extern unsigned char Leef12[] __asm__(".Leef12");
extern int Leef18[] __asm__(".Leef18");

void Anim_TitanBlade(void *context)
{
    vec3_t pos;
    unsigned char *base;
    void *ctx;
    DrawFn f2;
    DrawFn f1;
    void *g2;
    int cx;
    int cx16;
    void **g;
    void **w;
    State **slot;
    Part *p;
    Part *q;
    int frame;
    int i;

    g = (void **)iwram_3001eec;
    w = g;
    base = (unsigned char *)*w++;
    ctx = *w;
    g2 = g[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    REG_BLDALPHA = 0x1010;
    GetBattleActorPos3((*slot)->ids[0], &pos);
    cx = pos.x / 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    f1 = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 1);
    f2 = (DrawFn)g[8];
    LoadVFXFile(FILE_56, base + 0x4e20, 1, 1);
    LoadVFXFile(FILE_85, base, 1, 0);
    LoadVFXFile(FILE_7d, base + (0xdd << 4), 1, 0);
    LoadVFXFile(FILE_73, g2, 0, 0);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    {
        int arg = 0x90;
        arg <<= 3;
        StartTask(Task_BlitAnim, arg);
    }
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    do {
        int a;
        int wv = i * 2;
        a = Random() & 0xffff;
        p->x = sin(a) * wv;
        p->y = -(cos(a) * wv);
        p->t = i / 2 + 0x19;
        i++;
        p++;
    } while (i != 0x20);
    q = ewram_2010018;
    i = 0;
    do {
        q->x = -1;
        i++;
        q++;
    } while (i != (0xab << 2));
    cx16 = cx << 16;
    q = ewram_2014ad0;
    i = 0;
    do {
        int a;
        int r = (Random() & 0x1ff) + 0x20;
        a = Random() & 0xffff;
        q->x = cx16;
        q->y = 0xb0 << 15;
        q->vx = (sin(a) * r) >> 5;
        q->vy = -(cos(a) * r) >> 6;
        q->t = (Random() & 7) + 0x20;
        i++;
        q++;
    } while (i != (0xaa << 1));
    frame = 0;
    do {
        int fr;
        if ((unsigned)(frame - 0x19) <= 0x16)
            Func_80e46f0(FILE_c0);
        if (frame > 0x38)
            Func_80e46f0(FILE_c4);
        if (frame == 8)
            *(int *)(base + 0x77a8) = 8;
        if (frame == 0x30)
            *(int *)(base + 0x77a8) = 8;
        if (frame == 0x3c)
            *(int *)(base + 0x77a8) = 0x10;
        if (frame == 4)
            _PlaySound(0xd4);
        if (frame == 0x20)
            _PlaySound(0xa4);
        if (frame == 0x3c) {
            _PlaySound(0x91);
            _Func_80bd7dc(0x86);
        }
        if (frame > 0x37) {
            Part *b = (Part *)(base + (0xe1 << 7));
            i = 0;
            do {
                int x = *(short *)((char *)b + 2) + cx;
                int t = b->t;
                int y = *(short *)((char *)b + 6);
                if ((unsigned)t <= 0x11) {
                    f1(ctx, base + (Leef12[t / 3] << 11) + (0xdd << 4),
                       x - 0x10, y + 0x30, 0x20, 0x40);
                    t = b->t;
                }
                if (t > 0)
                    b->t = t - 1;
                else
                    b->t = -1;
                i++;
                b++;
            } while (i != 0x10);
        }
        if (frame == 0x1c) {
            Part *b = gBuffer;
            i = 0;
            do {
                if (b->t == -1) {
                    int a;
                    int r = Random() & 0x3f;
                    a = Random() & 0xffff;
                    b->x = ((sin(a) * r) >> 3) + cx16;
                    b->y = ((cos(a) * r) >> 2) + (0xc0 << 15);
                    b->vx = ((Random() & 0x3f) - 0x20) << 14;
                    b->vy = (-(Random() & 0x3f) - 8) << 13;
                    b->t = 0;
                }
                i++;
                b++;
            } while (i != (0x80 << 1));
        }
        fr = frame - 0x20;
        if ((unsigned)fr <= 0x1f) {
            Part *b = gBuffer;
            int cnt = 0;
            i = 0;
            do {
                if (b->t == -1) {
                    int a;
                    int r = Random() & 0x3f;
                    a = Random() & 0xffff;
                    b->x = ((sin(a) * r) >> 3) + cx16;
                    b->y = ((cos(a) * r) >> 2) + (0xc0 << 15);
                    b->vx = ((Random() & 0x3f) - 0x20) << 14;
                    b->vy = (-(Random() & 0x3f) - 8) << 13;
                    b->t = 0;
                    cnt++;
                    if (cnt == 0x10)
                        break;
                }
                i++;
                b++;
            } while (i != (0xab << 2));
        }
        if ((unsigned)fr <= 0x1f) {
            int m = ((frame << 4) - 0x100) % 0x68;
            f1(ctx, base, cx - 0x11, 4 - m, 0x22, 0x68);
            f1(ctx, base, cx - 0x11, 0x6c - m, 0x22, m);
        }
        if (frame <= 0x47) {
            Part *b = gBuffer;
            i = 0;
            do {
                if (b->t >= 0) {
                    int k = i % 3 + 2;
                    int sz;
                    if (b->vy > 0)
                        k += 2;
                    if (frame > 0x44 && k <= 5)
                        k = 6;
                    if (frame > 0x46 && k <= 6)
                        k = 7;
                    if (frame > 0x48 && k <= 7)
                        k = 8;
                    if (frame > 0x4a && k <= 8)
                        k = 9;
                    if (frame > 0x4c)
                        k = 0xa;
                    sz = k * 2;
                    f1(ctx,
                       (char *)g2 + *(unsigned short *)((char *)Data_ede48 + (sz - 2)),
                       *(short *)((char *)b + 2) - k / 2,
                       *(short *)((char *)b + 6) - k, k, sz);
                    b->x += b->vx;
                    b->y += b->vy;
                    if (frame > 0x50)
                        b->vy = b->vy - 0x8000;
                    else
                        b->vy = b->vy + Leef18[i & 3];
                    b->vx = b->vx * 62 / 64;
                    b->vy = b->vy * 62 / 64;
                    b->t += 1;
                    if (b->vy > 0 && *(short *)((char *)b + 6) > 0x6c)
                        b->t = -1;
                }
                i++;
                b++;
            } while (i != (0xaa << 1));
        }
        if (frame <= 0x5f) {
            int y;
            int h = 0x78;
            if (frame > 0x3c)
                y = (frame << 3) - 0x1c2;
            else if (frame > 0x20)
                y = fr / 2 + 0x10;
            else if (frame <= 9)
                y = (frame << 4) - 0x80;
            else
                y = 0x10;
            if (y + 0x78 > 0x6c)
                h = 0x78 - y - 0xc;
            if (h > 0)
                f2(ctx, base + 0x4e20, cx - 0x12, y, 0x24, h);
        }
        if (frame > 0x3b) {
            Part *b = ewram_2014ad0;
            i = 0;
            do {
                if (b->t > 0) {
                    int t;
                    int by;
                    Func_80e3908(b, 0x40, 0x80 << 6);
                    t = b->t - 1;
                    by = b->y;
                    b->t = t;
                    if (by > (0xd8 << 15)) {
                        b->vy = -b->vy / 2;
                    } else if ((unsigned)b->x <= 0x7effff && by >= 0) {
                        int k = t / 5 + 1;
                        int sz = k * 2;
                        f1(ctx,
                           (char *)g2 + *(unsigned short *)((char *)Data_ede48 + (sz - 2)),
                           (b->x >> 16) - k / 2, (by >> 16) - k, k, sz);
                    }
                }
                i++;
                b++;
            } while (i != (0xaa << 1));
        }
        if (frame == 0x44 && (*slot)->f14 != 0) {
            i = 0;
            while (i != (*slot)->f14) {
                Func_80d6888((*slot)->ids[i], 7, 5, i, 0x10);
                _SetBattleActorKnockback((*slot)->ids[i], 7);
                i++;
            }
        }
        if (frame == 9) {
            FillFn fill;
            int n = 0x80;
            fill = Func_80008d8;
            n <<= 7;
            fill(ctx, n, 0x3f3f3f3f);
        }
        if (frame == 0x3c) {
            FillFn fill;
            int n = 0x80;
            fill = Func_80008d8;
            n <<= 7;
            fill(ctx, n, 0x3f3f3f3f);
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x66);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
