/* BaseAnim_RapidSlash -- 0x080e2974, asm/rom_c9000/rom_e28f4_c_c_a.s line 12,
 * 742 ROM instructions.
 * NON-MATCHING, 740 of 778 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 740 is NOT a distance: 1668 bytes against
 * the ROM's 1720 (-52) and 751 encodings against 778 (-27) -- we are SHORT, not
 * long.  tools/aligncmp.py reads 391 aligned-equal of 778 (50.3%), 530
 * differing/ins/del in 108 hunks, and 50.3% / 108 hunks is the figure to beat.
 *
 * THE FRAME IS ONE SLOT SHORT: `sub sp, #0x88` against the ROM's `sub sp, #0x8c`.
 * The ROM spills TWENTY scalars (0x54 down to 0x08); we spill NINETEEN, and the
 * missing one is `base` -- see blocker (A), which is the whole residue in one
 * sentence.  Everything else in the map is already right or right-shifted by that
 * one slot:
 *     EXACT, same offset as the ROM:  0x4c ctx, 0x48 rep, 0x44 g2, 0x40 cam,
 *                                     0x30 slotC, 0x0c + 0x08 reload scratch
 *     right order, shifted by 4:      variant (ours 0x50, ROM 0x54)
 *     and the six address-taken aggregates are in the ROM's OWN ORDER
 *     (mv, pos, tv, o1, o2, fns from the high end down), occupying 52 bytes on
 *     both sides -- so the aggregate half of the frame is already solved and only
 *     base's slot is missing.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/e2974_RapidSlash.c \
 *     asm/rom_c9000/rom_e28f4_c_c_a.s --func BaseAnim_RapidSlash
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/e2974_RapidSlash.c
 * (the file includes the family header src/non_matching/rom_c9000/decls.h by
 * plain name, so it compiles verbatim from that directory and needs no edit on
 * install.)
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and that is BY PRECONDITION rather than by measurement: the two
 * indirect-copy sites here (Func_8001af8 at 0x080e2aa6 and Func_80008d8 inside
 * the nest) are ONE site each, in different basic blocks, so the Blob three-site
 * pin has nothing to defeat.  That is the precondition BaseAnim_Attack's park
 * states and confirms, and no pin was bought.
 *
 * ================================================================
 * THE SPLIT SHAPE -- RE-VERIFIED THIS BATCH, AND THE RECON WAS RIGHT
 * ================================================================
 * tools/datacheck.py prints NOTHING for this .s: no data section, so this is a
 * TEXT-ONLY split.  tools/split_s.py --dry-run does NOT refuse and demands NO
 * export, because .Leed3e -- the one table the function reads -- already carries
 * `.global` in asm/rom_c9000/rom_e28f4_c_c_c.s.  THE EXACT `.global` LIST IS
 * EMPTY.  This was the one claim in the recon worth re-checking (both of batch
 * 308B's targets DID need exports) and it stands.
 *
 * The .s holds FOUR functions -- BaseAnim_RapidSlash (0x080e2974), Anim_Gaia
 * (0x080e302c), Func_80e38b8, Func_80e3908 -- and the target is the FIRST, so
 * --dry-run cuts TWO ways:
 *     asm/rom_c9000/rom_e28f4_c_c_a_b.s   BaseAnim_RapidSlash  (1 fn,  802 lines)
 *     asm/rom_c9000/rom_e28f4_c_c_a_c.s   the other three      (3 fn, 1143 lines)
 * and it would REMOVE asm/rom_c9000/rom_e28f4_c_c_a.s and rewrite stage1.ld.
 * Comes into C as src/rom_c9000/rom_e28f4_c_c_a_b.c.  --dry-run ONLY; nothing in
 * the tree was touched, and `make compare` after the split is still owed.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline v1 (the whole family template transplanted, one shared counter, one
 * shared gBuffer walker, fp/mp/look declared at function level): size 1668-ish
 * at 1676 (-44), count 755 (-23), 319 aligned (41.0%), 133 hunks, objcmp 739.
 * *** THE RELOCATION SEQUENCE WAS ALREADY ALMOST EXACT ON THAT FIRST CANDIDATE
 * -- every call in the ROM's order, all 5 LoadVFXFile, all 6 BuildDraw2DFuncEx,
 * all 4 Random, both _SetBattleActorKnockback, all 4 gfree -- which is the
 * cheapest possible confirmation that the program shape read off the asm is
 * right.  Everything below is register allocation, not program shape. ***
 *
 * (1) THE `ldmia` WALKING-POINTER PROLOGUE.  In from v1 and never questioned:
 *     `g = iwram_3001eec; w = g; base = *w++; ctx = *w; g2 = g[2];
 *      cam = *(void **)((char *)g - 0x6c);` reproduces
 *     `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r0} / ldr r3,[r3] /
 *      ldr r1,[r2,#8] / sub r2,#0x6c / ldr r2,[r2]` -- FIFTH function in this
 *     family to take it unchanged.
 *
 * (2) *** SPLIT THE ONE COUNTER INTO THREE.  41.0% -> 47.3% aligned, 626
 *     differing -> 553.  THIS IS THE LARGEST SINGLE STEP ON THE FUNCTION. ***
 *     The ROM uses r8 for the seed loop's index, for the nest's index and for
 *     the final PhysMove-style loop's index -- 23 references to r8, more than any
 *     other high register.  I read that as lever 3's "counters unify" and gave
 *     all three loops ONE `i`; that was WRONG HERE.  Three disjoint ranges in one
 *     variable make live_length the SUM, which is exactly lever 4's spill
 *     mechanism, and the measurement says so.
 *     *** SO A SHARED HARD REGISTER ACROSS DISJOINT LOOPS IS NOT EVIDENCE OF ONE
 *     VARIABLE.  Disjoint ranges can and do land in the same register precisely
 *     BECAUSE they do not conflict.  The tell for unification has to be
 *     something else -- an overlapping range, or a value carried between loops.
 *     Seventh converse pair in this family, and the one that matters most,
 *     because r8-across-three-loops is the shape lever 1 invites you to read
 *     backwards. ***
 *
 * (3) TWO WALKING POINTERS FOR ONE gBuffer ARRAY, not one.  47.3% -> 49.2%,
 *     553 differing -> 538, hunks 135 -> 125.  The ROM walks gBuffer with r5 in
 *     the seed loop and RE-LOADS the same `ldr r6,=gBuffer` into r6 for the final
 *     loop: two registers, therefore two variables.  This is BaseAnim_Attack's
 *     lever 3 transferring verbatim -- and note it lands in the SAME function as
 *     (2), in the OPPOSITE direction, again: pointers split, counters here split
 *     too, so on RapidSlash the "pointers split, counters unify" pair is
 *     "pointers split, counters ALSO split".
 *
 * (4) DO NOT DECLARE `fp`, `mp` OR `look` -- LET loop.c CREATE THEM.  49.2% ->
 *     50.3%, 538 -> 530, and hunks 125 -> 108 (the biggest hunk-count drop).
 *     Write `fns[j & 1](...)`, `fns[1] = ...`, `GetBattleActorPos2(..., &mv)`,
 *     `mv.x = mv.x / 2` and `MatrixSetLook(cam, (char *)cam + 0xc)` and gcc's own
 *     loop-invariant motion hoists the three quantities into the outer loop's
 *     preheader, which is where the ROM computes them
 *     (`str r0,[sp,#0x1c] / add r3,#0xc / str r3,[sp,#0x14]`).
 *     *** THE SLOT MAP IS WHAT SAYS SO, AND THIS IS THE READING THE BRIEF'S FREE
 *     LEVER BUYS YOU: the ROM's fp/mp/look sit at 0x1c/0x18/0x14, BELOW the
 *     gcse/loop pseudos at 0x2c-0x20.  A DECLARED local cannot be below a
 *     pass-created one, because expand_decl numbers every declared pseudo before
 *     any pass runs.  So their offsets PROVE they are not declared, and the
 *     measurement then confirms it. ***  Block-scoping them inside the loop body
 *     instead (the batch-306 complement, which would also make them late pseudos)
 *     measured WORSE: 49.7%, 534, 112 hunks.
 *
 * ================================================================
 * MEASURED INERT (untested, not disproved) -- and two of these RETIRE
 * STANDING ADVICE FOR THIS FUNCTION
 * ================================================================
 *   - *** `variant * 8 - variant` IS BYTE-IDENTICAL TO `variant * 7`. ***  Same
 *     size, same count, same objcmp 739, same 319 aligned, same 133 hunks on the
 *     v1 base.  synth_mult already emits `lsl rX,variant,#3 / sub rX,variant` for
 *     the plain product, so THE RECON'S HAND-DISTRIBUTION IS UNNECESSARY HERE and
 *     batch 308A's point 2 is confirmed on a second constant: distribute only
 *     against a measured MISMATCH, never on principle.
 *   - THE NEST'S GetBattleActorPos2 DESTINATION written `mp->x = mp->x / 2`
 *     through a loop-body-scoped `vec3_t *mp = &mv;` against the sp-relative
 *     `mv.x = mv.x / 2`: BYTE-IDENTICAL, 391 aligned / 530 differing / 108 hunks
 *     both ways, same size and same count.
 *     *** SO BaseAnim_Attack'S LEVER 4 -- THE VEC-STORE-THROUGH-POINTER THAT MADE
 *     ITS FRAME BYTE-EXACT -- IS INERT ON RapidSlash, AND THE SLOT MAP EXPLAINS
 *     WHY: Attack's `mv` pointer is a DECLARED local competing for a register,
 *     while RapidSlash's sits at 0x18, below the pass-created pseudos, so gcc is
 *     already making the pointer itself and the spelling cannot change which. ***
 *     The lever is conditional on the pointer being a declared quantity.
 *   - The tile-copy source as an `int *` subscript,
 *     `(unsigned char *)((int *)base + row * 10)`, against `base + row * 0x28`:
 *     BYTE-IDENTICAL.  The ROM's `add r3,r6,r5 / lsl r3,#3` with r6 a +4 giv comes
 *     out of the plain product already, so the ParticleCloud/Nova stride question
 *     does not arise here.
 *   - An inner-nest alias of `pp` (`vec3_t *tp = pp;` at the top of the
 *     `while (j != count)` body, to reproduce the ROM's `mov r11,r5`):
 *     BYTE-IDENTICAL.
 *   - Reassociating the tile-copy destination index constant-first,
 *     `base[(0xa2 << 7) + row * 0x14 + k / 2]`: BYTE-IDENTICAL.
 * MEASURED WORSE:
 *   - `int v8 = variant * 8;` declared between `cam` and `pp`, indexing
 *     `Leed3e[v8 - variant + k]`.  This is what the ROM's OWN SLOT MAP asks for --
 *     its 0x3c holds `variant << 3` and sits ABOVE pp (0x38) and count (0x34), so
 *     by the declaration-order rule it IS a declared local -- and it still LOSES:
 *     size 1644 (-76), count 739 (-39), 378 aligned (48.6%), 138 hunks on the best
 *     base; 1644/739/44.0%/146 on the v1 base.  Naming the index costs 12
 *     encodings because the subtraction is then commoned per region instead of
 *     recomputed at each of the nine sites, and the ROM recomputes it every time.
 *     *** THIS IS A REAL CONFLICT BETWEEN TWO RULES THIS TREE TRUSTS -- the
 *     slot-order rule says "declared", the access-count rule says "recomputed" --
 *     and on this function the access count wins by 12 encodings.  Recorded as a
 *     known unresolved residue, not as a disproof of either rule. ***
 *   - `int v7 = variant * 7;` declared, indexing `Leed3e[v7 + k]`: 1644 (-76),
 *     739 (-39), 42.3%, 140 hunks.  Worse the same way and for the same reason.
 *   - Naming `lo + 2` as `int hi`: 48.5%, 553, 112 hunks.
 *   - The tile-copy destination through a precomputed `d = base + (0xa2 << 7)`
 *     (tried to lower base's depth-2 reference count and provoke blocker (A)'s
 *     spill): 47.2%, 581, 122 hunks.  It did not spill base and it cost 51 exact
 *     encodings.
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) *** `base` KEEPS r11 FOR US AND THE ROM SPILLS IT -- global.c's
 *     find_reg/allocno priority, and it is the whole frame defect and most of
 *     the residue. ***  The ROM's second store is `str r0,[sp,#0x50]` and every
 *     later use is a reload (`ldr r0,[sp,#0x50]`, `ldr r4,[sp,#0x50]`,
 *     `ldr r2,[sp,#0x50]`, `ldr r5,[sp,#0x50]` ...), including one INSIDE the
 *     tile-copy inner loop, where `strb r2,[r4,r3]` is register+register on the
 *     reloaded base.  We emit `mov fp,r0` and then 20 references to fp, and our
 *     tile-copy store folds base into the index (`add r3,r3,fp / mov r4,ip /
 *     strb r2,[r3,r4]`) instead of keeping base as the array base.
 *     The register census is the clearest statement of the gap:
 *         ROM   r8=23 (the three counters)  r9=5 (lo+2)  r10=9 (lo)  r11=9 (pp)
 *               r12=2  r14=2 (the tile-copy invariants)   base SPILLED
 *         ours  r8=13  r9=13 (counters)  r10=5  r11=20 (base)  r12=2  lr=3
 *     gcc-2.96 ranks allocnos by roughly log2(n_refs)*freq/live_length and hands
 *     out hard registers in that order; base's depth-2 reference in the tile loop
 *     gives it a high freq, so it wins r11 before the nest's `pp` can take it.
 *     In the ROM the four high-register quantities are counters/lo+2/lo/pp and
 *     base loses.  Four separate attempts to provoke the spill -- the `d` pointer
 *     above (lowering base's freq), the `tp` alias (raising pp's), naming `hi`,
 *     and the `int *` stride -- were inert or worse.  It needs one more competing
 *     allocno with a range that overlaps base's whole-function range, and
 *     identifying it is where the next round should go.  Do NOT transplant a
 *     high-register base pin from BaseAnim_Attack: the thing to reproduce here is
 *     the ABSENCE of base's register.
 *
 * (B) CONSTANT-POOL PLACEMENT -- arm_reorg / dump_table, downstream of (A).
 *     1720 bytes is far past Thumb's 1020-byte `ldr rd,[pc]` reach so the pool
 *     MUST split, and the ROM splits it THREE ways while we split it TWO:
 *         ROM   0x74..0x84 (5 words, behind a `b .Le29fc` gcc inserted itself,
 *                           and it is where `.word 0x1010` lives),
 *               0x458..0x494 (16 words, behind the `.pool_aligned` in the .s),
 *               0x694..0x6b4 (9 words)
 *         ours  0x384..0x3d0, 0x650..0x688
 *     Every one of our ~30 `ldr rX,[pc,#imm]` encodings therefore carries a
 *     different immediate from the ROM's, which is a large part of the 530.  A
 *     missing or extra pool word is also a size-and-count defect, so part of the
 *     -52/-27 is this and not a missing instruction.  Do not chase it directly;
 *     it follows the block layout, which follows (A).
 *
 * (C) CROSS-JUMPING THE FOUR BuildDraw2DFuncEx SETUPS.  The ROM expands the four
 *     calls in the `(*slotC)->f4` dispatch as four INDEPENDENT sequences, each
 *     carrying the fifth argument's `2` in a DIFFERENT scratch register and in a
 *     different order (`mov r1,#2 / str r1,[sp]` then `mov r2,#2 / str r2,[sp]`
 *     then `mov r3,#2 / str r3,[sp]` then `mov r4,#2 / ... / str r4,[sp]`), and
 *     a fifth and sixth after the nest the same way.  We share setup between
 *     arms; aligncmp shows one arm's six-instruction prologue and two later
 *     `str rX,[sp]` with no counterpart.  All six calls ARE present and in the
 *     ROM's order (the relocation list matches), so this is expand-order and
 *     tail-merging, i.e. lever 8's territory, worth about 10 encodings.
 *
 * (D) TWO READS OF `pp` THROUGH A HIGH REGISTER.  The ROM reads pp->x and pp->y
 *     in the nest's draw arms as `mov r3,r11 / ldr r2,[r3]` and
 *     `mov r5,r11 / ldr r3,[r5,#4]` -- two `mov` out of r11 because Thumb-1
 *     cannot `ldr` off a high register.  We emit one.  This is a CONSEQUENCE of
 *     pp not being in r11 (see (A)) and not a separate source question.
 *
 * ================================================================
 * FACTS FOR WHOEVER REOPENS THIS, none of them guessed
 * ================================================================
 *  - .Leed3e IS a 7-byte-per-variant parameter record, exactly as the recon says,
 *    and all seven k values are in use: +0 picks FILE_b5 vs FILE_b6 and also
 *    gates the nest's arm choice, +1 is a 4-way palette switch over
 *    FILE_8d/FILE_a0/FILE_b6/FILE_b4 (emit_case_nodes over three case nodes plus
 *    a default -- `cmp r3,#1 / beq / cmp r3,#1 / bgt / cmp r3,#0 / beq / b` then
 *    `cmp r3,#2`, the duplicated `cmp #1` being gcc's own), +2 is the nest's
 *    inner count, +3 is the Random-spray count, +4 is the per-index stride
 *    multiplied by the inner index, +5 is the outer repeat count AND the gate on
 *    the whole animation body, +6 selects the Func_80008d8 fill.
 *  - ALL THREE LOOPS THAT COULD HAVE BEEN `do`-`while` ARE `while`, and the ROM
 *    proves it: the outer repeat loop, the `j != count` nest and the Random-spray
 *    loop each carry a duplicate_loop_exit_test entry test against 0
 *    (`cmp r3,#0 / bne enter / b exit`) that a `do`-`while` can never reach
 *    (jump.c:1137).  The tile-copy double loop and the two fixed-length gBuffer
 *    loops are `do`-`while`, and their `cmp rX,#imm / bne` bottom tests with no
 *    entry test prove that too.  Five loop forms, all read off the asm, none
 *    guessed -- and getting one wrong is worth more than any register lever here.
 *  - THE Random-SPRAY LOOP'S BOUND IS RE-READ EVERY ITERATION.  The ROM keeps
 *    .Leed3e's base in r1 and the index in r7 ACROSS the `bl Random`, spilling
 *    both to sp+0x0c and sp+0x08, and reloads `ldrb r3,[r1,r7]` for the test.
 *    That is because Random() can write memory, so the bound cannot be hoisted --
 *    write `while (k != Leed3e[variant * 7 + 3])` literally and do NOT name it.
 *    Those two spill slots are the 0x0c/0x08 pair and they land EXACTLY.
 *  - THE `int` CARRIER IS RIGHT HERE AND IT IS IN.  `{ int bld = 0x1010;
 *    REG_BLDALPHA = bld; }` puts the VALUE pool load before the address load,
 *    which is the ROM's order three lines after `bl AnimStart`, per batch 308A's
 *    load-order discriminator.  0x1010 is not thumb_shiftable_const (0x101 > 255)
 *    so the carrier reaches the pool, unlike BaseAnim_Attack's 0x1f80.
 *  - THE TWO TILE-COPY INVARIANTS NEED NO HELP.  `0xa2 << 7` and `0x90 << 1` are
 *    both shiftable, gcc emits `mov/lsl` for each and loop.c hoists both out of
 *    the double loop into r12 and lr on our side too (ip=2, lr=3 references), so
 *    the recon's warning to keep "lr" and "r12" out of a clobber list never had
 *    to be acted on -- no inline asm was written and none is needed.
 *  - SIX AGGREGATES, AND THEIR DECLARATION ORDER IS THE ROM'S.  mv (the
 *    GetBattleActorPos2 destination in the nest, ROM sp+0x80), pos (the one
 *    filled before the seed loop, 0x74), tv (the Func_80e3944 destination in the
 *    final loop, 0x68), then the two `int` out-parameters Anim_Djinni takes by
 *    address (0x64 and 0x60, in that argument order), then `DrawFn fns[2]`
 *    (0x58).  First-declared takes the HIGHEST offset.  Reversing was not tried
 *    because the order already lands; if base's spill (A) moves anything, re-try
 *    both directions.
 */
#include "decls.h"

extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void _Func_80b82c4(int a, int b, int c, int d);
extern void *GetFile(int id);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern Part ewram_2010018[];
extern unsigned char ewram_2015e00[];
extern unsigned char Leed3e[] __asm__(".Leed3e");

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

void BaseAnim_RapidSlash(void *context, int variant)
{
    vec3_t mv;
    vec3_t pos;
    vec3_t tv;
    int o1;
    int o2;
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int rep;
    void *g2;
    void *cam;
    vec3_t *pp;
    int count;
    State **slotC;
    void **g;
    void **w;
    State **slotA;
    State **slotB;
    State **slotD;
    int *rec;
    Part *p;
    Part *e;
    vec3_t *vp;
    int i;
    int j;
    int n;
    int mask;
    int row;
    int f;

    g = (void **)iwram_3001eec;
    w = g;
    base = (unsigned char *)*w++;
    ctx = *w;
    g2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    slotA = (State **)(base + 0x7828);
    *slotA = (State *)context;
    AnimStart(0);
    { int bld = 0x1010; REG_BLDALPHA = bld; }
    if ((*slotA)->f1c == 1)
        Anim_Djinni(context, 7, (*slotA)->f4, 2, &o1, &o2);
    LoadVFXFile(FILE_73, g2, 0, 0);
    LoadVFXFile(FILE_99, base, 1, 0);
    row = 0;
    do {
        unsigned char *s = base + row * 0x28;
        int k = 0;
        do {
            base[k / 2 + row * 0x14 + (0xa2 << 7)] = *s;
            k++;
            s++;
        } while (k != 0x28);
        row++;
    } while (row != (0x90 << 1));
    if (Leed3e[variant * 7] == 0)
        LoadVFXFile(FILE_b5, base, 1, 1);
    else
        LoadVFXFile(FILE_b6, base, 1, 1);
    LoadVFXFile(FILE_6b, ewram_2015e00, 1, 0);
    switch (Leed3e[variant * 7 + 1]) {
    case 0:
        f = FILE_8d;
        break;
    case 1:
        f = FILE_a0;
        break;
    case 2:
        f = FILE_b6;
        break;
    default:
        f = FILE_b4;
        break;
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
    WaitFrames(1);
    slotB = (State **)(base + 0x7828);
    pp = &pos;
    GetBattleActorPos2((*slotB)->ids[0], pp);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    {
        int arg = 0x90;
        arg <<= 3;
        StartTask(Task_BlitAnim, arg);
    }
    rec = (int *)*_GetBattleActor((*slotB)->ids[0]);
    p = gBuffer;
    i = 0;
    mask = 0xff;
    do {
        p->x = rec[2];
        p->y = rec[3] + (0xc8 << 13);
        p->z = rec[4];
        p->vx = (Random() & mask) << 12;
        p->vy = ((Random() & mask) - 0x7f) << 12;
        p->vz = ((Random() & mask) - 0x7f) << 12;
        if (p->x > 0)
            p->vx = -p->vx;
        p->t = -1;
        i++;
        p++;
    } while (i != (0xc0 << 2));
    _Func_80b82c4((*(State **)(base + 0x7828))->f8,
                  (*(State **)(base + 0x7828))->ids[0], 4, 0);
    rep = 0;
    slotC = (State **)(base + 0x7828);
    while (rep != Leed3e[variant * 7 + 5]) {
        count = Leed3e[variant * 7 + 2];
        GetBattleActorPos2((*slotC)->f8, &mv);
        mv.x = mv.x / 2;
        if ((*slotC)->f4 == 0) {
            BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
            BuildDraw2DFuncEx(0x2f, 7, 7, 0xb, 2);
        } else {
            BuildDraw2DFuncEx(0x2e, 7, 7, 7, 2);
            BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, 2);
        }
        fns[0] = (DrawFn)gPtrs[0x2e];
        fns[1] = (DrawFn)iwram_3001f0c;
        j = 0;
        while (j != count) {
            int lo = Leed3e[variant * 7 + 4] * j;
            if (rep >= lo && rep < lo + 6) {
                int d = rep - lo;
                if ((j & 3) <= 1 || Leed3e[variant * 7] == 1) {
                    int q = (d * 8 - d) * 4 - d;
                    if ((*slotC)->f4 == 0)
                        fns[j & 1](ctx, base + (q << 7),
                                  pp->x / 2 - 0x10, pp->y - 0x28, 0x30, 0x48);
                    else
                        fns[j & 1](ctx, base + (q << 7),
                                  pp->x / 2 - 0x20, pp->y - 0x28, 0x30, 0x48);
                } else {
                    if ((*slotC)->f4 == 0)
                        fns[j & 1](ctx, ewram_2015e00 + ((d * 2 + d) << 8),
                                  pp->x / 2 - 0x10, mv.y - 8, 0x30, 0x10);
                    else
                        fns[j & 1](ctx, ewram_2015e00 + ((d * 2 + d) << 8),
                                  pp->x / 2 - 0x20, mv.y - 8, 0x30, 0x10);
                }
            }
            if (rep == lo + 2) {
                if (Leed3e[variant * 7 + 6] == 1) {
                    FillFn fill;
                    int n = 0x80;
                    fill = Func_80008d8;
                    n <<= 7;
                    fill(ctx, n, 0x2f2f2f2f);
                }
                slotD = (State **)(base + 0x7828);
                Func_80d6888((*slotD)->ids[0], 7, 5, 0, 4);
                if (j == count - 1) {
                    _SetBattleActorKnockback((*slotD)->ids[0], 4);
                    *(int *)(base + 0x77a8) = 8;
                    _Func_80bd7dc(0x86);
                } else {
                    if (j & 1)
                        _SetBattleActorKnockback((*slotD)->ids[0], 7);
                    *(int *)(base + 0x77a8) = 4;
                    _PlaySound(0x86);
                }
                {
                    Part *q = &ewram_2010018[j * 32];
                    int k = 0;
                    while (k != Leed3e[variant * 7 + 3]) {
                        q->x = (Random() & 7) + 0xf;
                        k++;
                        q++;
                    }
                }
            }
            if (rep >= lo + 2 && rep < lo + 14) {
                int h = (rep - lo - 2) / 2;
                fns[0](ctx, base + (((h << 4) - h) << 6) + (0xa2 << 7),
                       pp->x / 2 - 0xa, pp->y - 0x18, 0x14, 0x30);
            }
            j++;
        }
        gfree(0x2f);
        gfree(0x2e);
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
        fns[0] = (DrawFn)gPtrs[0x2e];
        fns[1] = (DrawFn)iwram_3001f0c;
        e = gBuffer;
        n = 0;
        vp = &tv;
        do {
            int t = e->t;
            if (t > 0) {
                int tq;
                int sz;
                Func_80e3944((vec3_t *)e, vp);
                vp->x = vp->x / 2;
                tq = (t >> 3) + 1;
                sz = tq * 2;
                fns[(n / 2) & 1](ctx,
                    (char *)g2 + *(unsigned short *)((char *)Data_ede48 + (sz - 2)),
                    vp->x - tq / 2, vp->y - tq, tq, sz);
                Func_80e38b8(e, 0x3c, -0x400);
                if (e->y <= 0x7ffff)
                    e->vy = -e->vy / 2;
                e->t -= 1;
            }
            n++;
            e++;
        } while (n != (0x80 << 2));
        gfree(0x2f);
        gfree(0x2e);
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        rep++;
    }
    StopTask(Task_BlitAnim);
    AnimEnd();
}
