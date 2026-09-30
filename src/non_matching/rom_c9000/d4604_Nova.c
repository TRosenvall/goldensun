/* BaseAnim_Nova -- 0x080d4604, asm/rom_c9000/rom_d45ec_c_c.s line 12,
 * 776 ROM instructions.
 * NON-MATCHING, 674 of 809 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 674 is NOT a distance: 1724 bytes against
 * the ROM's 1764 (-40) and 789 encodings against 809 (-20) -- we are SHORT.
 * tools/aligncmp.py reads 435 aligned-equal of 809 (53.8%), 474
 * differing/ins/del in 134 hunks.  53.8% is the figure to beat.
 *
 * *** THE FRAME IS EXACT ON THE FIRST CANDIDATE.  `sub sp, #0x50` on both
 * sides, and the SLOT COUNT is exact too -- fifteen scalar words at
 * 0x04..0x3c, no more and no fewer.  Seven of them already carry the ROM's own
 * reference count.  See residue (C) for the five that are permuted. ***
 *
 * *** THE RELOCATION SEQUENCE IS EXACT: 58 relocations against the ROM's 58, in
 * the ROM's order, on the FIRST candidate. ***  Diffing the two symbol
 * sequences leaves only pool-word PLACEMENT (iwram_3001eec and gPtrs sit in a
 * different pool block, and three calls move relative to a pool block) -- no
 * call added, none missing, none transposed.  Per BaseAnim_Attack's park that is
 * the cheapest possible confirmation that the program shape read off the asm is
 * right, and everything below is allocation, addressing and pool layout.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/d4604_Nova.c \
 *     asm/rom_c9000/rom_d45ec_c_c.s --func BaseAnim_Nova
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/d4604_Nova.c
 * It includes the family header src/non_matching/rom_c9000/decls.h -- which
 * already declares BaseAnim_Nova with this exact signature -- and adds only the
 * five externs that header lacks.
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and MEASURED: the three `_call_via_r3` sites are the two
 * Func_8001af8 palette DMAs in the mutually exclusive `variant == 1` and
 * `variant == 2` arms -- DIFFERENT BASIC BLOCKS -- plus one Func_80008d8 fill,
 * a different callee entirely.  Nothing shares a block, so per
 * BaseAnim_Attack's confirmed precondition Blob's r0/r2 pins have nothing to
 * defeat.  TWELFTH instance of not needing a pin.
 *
 * ================================================================
 * THE SPLIT SHAPE -- confirmed with the tools, TWO EXPORTS
 * ================================================================
 * tools/datacheck.py: the .s HAS a `.rodata` section and holds TWO functions
 * (BaseAnim_Nova, Anim_Volcano), so this is a TEXT/DATA SPLIT and the data must
 * keep its own object.
 *     *** THE SPLIT MUST ADD EXACTLY TWO EXPORTS:
 *           .global .Lee262
 *           .global .Lee294                                              ***
 * tools/split_s.py --dry-run REFUSES until both exist:
 *     "_b.s references .Lee262, defined in ..._c.s"
 *     "_b.s references .Lee294, defined in ..._c.s"
 * datacheck attributes .Lee29a, .Lee29d and .Lee2a9 to Anim_Volcano -- LEAVE
 * THOSE ALONE, they are not this function's and exporting them is unnecessary
 * churn.  Add the two, verify `make compare`, then split.  The target is the
 * FIRST function in the file.
 * The two widths are settled off the asm, not guessed:
 *     extern unsigned short Lee262[] __asm__(".Lee262");   -- `ldrh r3,[r1,r3]`
 *     extern unsigned char  Lee294[] __asm__(".Lee294");   -- `ldrb r1,[r2,r0]`
 *
 * ================================================================
 * *** THE BATCH RESULT: WHAT SEPARATES THE `int` CARRIER CASES ***
 * ================================================================
 * The brief carries this as AN UNRESOLVED CONTRADICTION -- the `int` carrier for
 * a pooled `vu16` register write is required on some siblings and measures 3.6
 * aligned points NEGATIVE on Anim_Ragnarok, for the SAME constant 0x1010, with
 * shiftability unable to separate them and the printed mnemonic unable either
 * (Thumb-1 has no PC-relative LDRH, so `ldr` and `ldrh` assemble identically for
 * a pool load).  THIS FUNCTION SETTLES THE DIRECTION, AND THE DISCRIMINATOR IS
 * ONE LINE OF ASM YOU CAN READ BEFORE WRITING ANYTHING.
 *
 * WHAT THE CARRIER ACTUALLY CHANGES is not the pool (0x1010 is unshiftable in
 * BOTH modes -- 0x101 > 255 -- so it reaches the pool either way) and not the
 * mnemonic.  It changes WHICH OF THE TWO POOL LOADS IS EMITTED FIRST, and
 * therefore which register ends up holding the destination address and the
 * operand order of the `strh`:
 *     `REG_BLDALPHA = 0x1010;`        -> ADDRESS pool load first, value second,
 *                                        `strh value,[addr]`
 *     `{int b=0x1010; REG_BLDALPHA=b;}`-> VALUE pool load first, address second,
 *                                        `strh addr_reg_swapped,[...]`
 * A plain store expands the destination address before the source; an `int`
 * carrier makes the value a named SImode quantity that is expanded, and
 * allocated, first.
 *
 * NOW READ THE ROMs.  Five siblings, and the order is explicit in every one:
 *     BaseAnim_Blob     `ldr r3,.Lcf954 @0x1010` THEN `ldr r2,=REG_BLDALPHA` -> VALUE FIRST
 *     BaseAnim_RapidSlash `ldr r3,.Le29e4 @0x1010` THEN `ldr r2,=REG_BLDALPHA` -> VALUE FIRST
 *     BaseAnim_Breath   `ldr r2,=REG_BLDALPHA` THEN `ldr r3,.Ldbc6c @0x1010` -> ADDRESS FIRST
 *     Anim_Ragnarok     `ldr r2,=REG_BLDALPHA` THEN `ldr r3,.Le9534 @0x1010` -> ADDRESS FIRST
 *     BaseAnim_Nova     `ldr r2,=REG_BLDALPHA` THEN `ldr r3,.Ld46c0 @0x1010` -> ADDRESS FIRST
 * *** THE RULE: VALUE-FIRST IN THE ROM MEANS USE THE CARRIER.  ADDRESS-FIRST
 * MEANS WRITE THE STORE DIRECTLY.  Read the two pool loads in the ROM and the
 * question is answered before you compile anything. ***
 *
 * THE MEASUREMENTS BEHIND IT:
 *   - HERE (address-first): dropping the carrier moved ALL FOUR FIGURES the
 *     right way at once -- objcmp 774 -> 674 differing, size 1716 -> 1724
 *     (-48 -> -40), count 786 -> 789 (-23 -> -20), aligned 51.3% -> 53.8%.
 *     That is the strongest single change on this function and it is the ONLY
 *     one that improved every view simultaneously.
 *   - Anim_Ragnarok (address-first): the carrier is 3.6 aligned points negative,
 *     per the brief.  Consistent.
 *   - BaseAnim_Breath (address-first): PREDICTED AND THEN MEASURED, by taking
 *     the installed park src/non_matching/rom_c9000/dbc30_Breath.c and changing
 *     ONLY its `{ int b0 = 0x1010; REG_BLDALPHA = b0; }` to a direct store:
 *         carrier (as installed): objcmp 647 of 660, 1476 (+8), 669 (+9), 61.7%
 *         direct store          : objcmp 580 of 660, 1484 (+16), 672 (+12), 62.9%
 *     SIXTY-SEVEN more exact encodings and 1.2 aligned points, at the cost of 8
 *     bytes and 3 encodings.  Breath's two ranking views therefore SPLIT, so
 *     this is a prediction confirmed on the encoding axis and NOT a
 *     recommendation to change that park yet -- but Breath should be re-ranked
 *     the moment its size goes exact, and its park's stated reason for the
 *     carrier ("gcc pools 0x1010 as a HALFWORD and emits `ldrh`... the ROM has a
 *     WORD pool") is VOID, because those two assemble to the same bytes.
 *     BaseAnim_Blob's park states the same void reason for a case where the
 *     carrier happens to be right for the OTHER reason.
 *   - BaseAnim_RapidSlash is VALUE-FIRST, so its recon's advice to use the
 *     carrier there stands, and now it stands on a measured mechanism.
 *
 * ================================================================
 * THE REST OF THE PROGRAM, AS READ OFF THE ASM
 * ================================================================
 * (1) THE `ldmia` WALKING-POINTER PROLOGUE IN THE WITH-`g`-COPY FORM.
 *     `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r1} / ldr r3,[r3]` then
 *     `ldr r2,[r2,#8]`, so `g` must stay alive for the g[2] read and the extra
 *     copy is needed: `g = iwram_3001eec; pp = g; base = *pp++; ctx = *pp;
 *     g2 = g[2];`.  Present and exact on the first candidate.
 *
 * (2) THE THREE-ARM OPENING DISPATCH SETS THE SAME TWO SLOTS AND MUST NOT BE
 *     TIDIED.  variant==0 gives AnimStart(1), cx=0x3c, cy=0x30; variant==1
 *     gives AnimStart(0), cx=0x3c, cy=0x40; otherwise AnimStart(0),
 *     GetBattleActorPos2(slot->f8,&vec), cx = vec.x / 2 (the signed
 *     `lsr #31 / add / asr #1` idiom), cy = vec.y + 0x30.  gcc cross-joins the
 *     `cy` store on its own (.Ld4680 is shared by all three arms while the `cx`
 *     store is duplicated per arm), so WRITE BOTH STORES IN EVERY ARM and hoist
 *     neither -- the BaseAnim_Breath warning working in the documented
 *     direction, and it reproduces on the first try.
 *
 * (3) THE LOOP BOUNDS ARE A 5-HALFWORD RECORD INDEXED BY slot->f18, AND EVERY
 *     SITE RECOMPUTES THE WHOLE INDEX.  `Lee262[f18 * 5 + 1]` is the outer gate
 *     and the frame count, `Lee262[f18 * 5]` the particle count, and
 *     `Lee262[f18 * 5 + row + 2]` the per-row x offset.  Plain `f18 * 5` is
 *     right: synth_mult(5) gives `lsl r3,r2,#2 / add r3,r2`, which is the ROM's
 *     shape, and the halfword scale supplies the `lsl r3,#1`.  The recon's
 *     advice to hand-distribute it as `f18 * 4 + f18` is UNNECESSARY here -- and
 *     more importantly, DO NOT hoist it into a named local: the ROM recomputes
 *     the full index at all NINE sites, which is what a fresh inline expression
 *     per site gives.
 *
 * (4) THE FRAME-LOOP GUARD IS gcc'S OWN ARITHMETIC AND IT COMES OUT FREE.
 *     The loop is `for (frame = 0; frame != (Lee262[f18*5+1] << 3) + 0x38;
 *     frame++)` and the ROM's entry test is the baffling
 *     `ldr r2,=0x1ffffff9 / cmp r3,r2`.  It is not a disassembly defect and
 *     there is nothing to spell: gcc solved `0 == (n << 3) + 0x38` for n,
 *     and 0x1ffffff9 * 8 == 0xffffffc8 == -0x38.  Write the natural loop and
 *     the pool word appears by itself.  RECORD THIS: a pool word that looks like
 *     garbage can be a folded loop-entry test, so solve it before calling the
 *     reference wrong.
 *
 * (5) THE PARTICLE ARRAY IS A `Part *` SUBSCRIPT HERE, NOT ParticleCloud'S
 *     `int *` ONE.  `&gBuffer[Lee262[f18*5] * row + i]` gives synth_mult(0x1c) =
 *     `lsl r3,r2,#3 / sub r3,r2 / lsl r3,#2`, which is the ROM's exact
 *     distribution.  Two functions in one family, two different spellings of the
 *     same array -- read the shift distribution off the asm every time.
 *
 * (6) THE 12-PARTICLE DRAW LOOP READS THE FIXED-POINT INTEGER PARTS AS SIGNED
 *     HALFWORDS.  `ldrsh r1,[r5,r0]` with r0 = 2 and `ldrsh r3,[r5,r1]` with
 *     r1 = 6 -- byte offsets 2 and 6, i.e. the HIGH halves of Part.x and Part.y,
 *     which the seed loops wrote as `value << 16`.  Spell them `((short *)p)[1]`
 *     and `((short *)p)[3]`.  The register+register form is free: Thumb-1 LDRSH
 *     has no immediate form at all, so there is nothing to pin (the same note
 *     BaseAnim_Attack's residue (C) makes about its four 0x24 offsets).
 *
 * (7) ONE COUNTER FOR ALL SIX LOOPS.  A single function-level `int i` serves the
 *     16-particle seed loop, the Lee262-bounded spawn loop, the 12-particle draw
 *     loop, the physics loop and the knockback loop, which is what the ROM does
 *     with r10 -- and the `movs r0,#1 / add sl,r0` it pays to increment a high
 *     register is part of the target, not a defect.  `row` likewise is ONE
 *     variable across the seed section and the frame loop (sp+0x30, 17 refs in
 *     the ROM, 16 in ours).  Lever 7 of the brief, and it is already right.
 *
 * (8) THE SINGLE-USE / TWO-USE RULE FOR `*(State **)(base + 0x7828)`, which is
 *     read NINE times in this function.  Where the ROM has the reg+reg form
 *     `ldr r0,=0x7828 / mov r2,r9 / ldr r3,[r2,r0]` the slot pointer has ONE use
 *     and the source must write the whole thing inline; where it has
 *     `add r5,r1,r0` followed by two `ldr [r5]` the address has TWO uses and the
 *     source needs a named `State **`.  This is the same discriminator
 *     BaseAnim_ParticleCloud's j-loop needs, and inlining the physics loop's
 *     pointer (one use) was worth one aligned encoding: 51.2% -> 51.3%.
 *
 * ================================================================
 * MEASURED INERT / WORSE
 * ================================================================
 *   - Block-scoping the `State *st` of the first frame-loop block so that the
 *     later `(*sl)->f18` read has to reload: BYTE-IDENTICAL.  The ROM does
 *     reload there (`ldr r1,[sp,#0x1c] / ldr r3,[r1] / ldr r3,[r3,#0x18]`
 *     against our `ldr r3,[r1,#0x18]`) and scoping is NOT the handle; the
 *     intervening `strh r3,[r1,#0x36]` store does not stop gcc reusing the
 *     pseudo it already has.  See residue (B).
 *   - Unifying `cy << 16` (seed loop) and `row * 8` (frame loop) into ONE
 *     function-level variable, on the theory that the ROM's r11 holds both:
 *     size 1716 -> 1732 and count 786 -> 794, BOTH toward the ROM, but aligned
 *     51.2% -> 49.9%, hunks 133 -> 157, and the generated frame layout breaks
 *     (our `fns` array leaves sp+0x3c and one slot picks up 19 references).
 *     THE MECHANISM IS THE OPPOSITE OF WHAT WAS INTENDED: the unified pseudo's
 *     live_length is the SUM of the two ranges, so instead of winning r11 it
 *     SPILLS, and the extra loads and stores are what moved size and count.
 *     That is lever 3 of the brief, not lever 2 -- the ROM's r11 holds TWO
 *     pseudos with disjoint ranges, which global-alloc gives the same hard
 *     register for free, and forcing them into one variable is strictly worse.
 *     REJECTED: a size-and-count gain bought by a spill is not progress.
 *
 * ================================================================
 * THE RESIDUE, BY PASS
 * ================================================================
 *
 * (A) *** THE HIGH-REGISTER ROTATION, AND IT IS ONE MISPLACED ALLOCNO.  The ROM
 *     spills `frame` to sp+0x2c (14 references) and keeps r9 = base,
 *     r10 = i, r11 = the two disjoint cy/row quantities, r8 = three more.  We
 *     keep `frame` in r10, put `base` in r11 and leave r9 almost unused. ***
 *     Every downstream slot is displaced by exactly one word because of it:
 *     the ROM's descending order is ctx 0x34, row 0x30, frame 0x2c, g2 0x28,
 *     cx 0x24, cy 0x20, sl 0x1c, fp 0x18, vm1 0x14, and ours simply omits
 *     `frame` and shifts g2/cx/cy/sl/fp up into its place.  The frame SIZE and
 *     the slot COUNT are already exact, so this is not a missing quantity --
 *     it is a priority inversion, and it is the single highest-value thing left.
 *     What is RULED OUT: it is not the counter (lever 7 is already applied and
 *     `i` does get a high register); it is not a missing variable (15 slots on
 *     both sides); and it is NOT fixable by forcing two quantities to share a
 *     variable, which spills instead (see MEASURED WORSE).  The remaining
 *     hypothesis is that the ROM's extra 20 instructions -- residues (B) and
 *     (D) -- are themselves the pressure that pushes `frame` out, in which case
 *     this closes only after those do.  Do not attack it directly first.
 *
 * (B) CSE KEEPS A PSEUDO THE ROM RELOADS, in the frame loop's opening block.
 *     The ROM reads `*(base + 0x7828)` for the `f18 == 2` test, stores to
 *     `cam[0x36]`, and then RELOADS the slot for the `f18 == 3` test; we keep
 *     the first load live across the store.  Block-scoping the variable is
 *     BYTE-IDENTICAL (above), so the handle is not lifetime -- gcc's memory
 *     invalidation at the `strh` does not force a reload when a pseudo already
 *     holds the value.  Two instructions per frame iteration, and it recurs at
 *     three more sites (ref[173:178], ref[266:269], ref[683:687] in the
 *     aligncmp -v listing), all of the same shape: the ROM rematerialises the
 *     0x7828 pool word and re-derives the address, we hold one pointer.  This is
 *     the bulk of the -20.
 *
 * (C) FIVE OF FIFTEEN SPILL SLOTS ARE PERMUTED, and they are permuted as a
 *     BLOCK by exactly one word -- entirely downstream of (A).  Ours against the
 *     ROM's, by reference count: 0x08 4/2, 0x10 2/4, 0x1c 5/6, 0x20 7/6,
 *     0x2c 17/14, 0x30 6/17.  Nothing to spell here; fix (A) and this follows.
 *
 * (D) CONSTANT-POOL PLACEMENT -- arm_reorg / dump_table, and it is worse here
 *     than on any sibling because the ROM dumps THREE blocks INSIDE the opening
 *     dispatch.  The reference has `.pool_aligned` behind the `b` gcc inserted
 *     after the variant==1 arm (four words plus a `.short` alignment filler at
 *     ~0x54), a second at ~0xbc carrying `.word 0x1010` and `.word 0x04000052`,
 *     and a third past 0x3cc; we dump one block at 0x30c.  1764 bytes is far
 *     past Thumb's 1020-byte `ldr rd,[pc]` reach so the pool MUST split, and
 *     because ours splits differently EVERY pc-relative offset in the function
 *     differs -- which is why aligncmp reads only 53.8% while the relocation
 *     sequence is exact.  The `.short` fillers also count as encodings in
 *     `objdump -dz`, so part of the -20 is pool structure and not missing code.
 *     Do not chase it directly; it is downstream of (A) and (B).
 */
#include "decls.h"

extern void *iwram_3001e80;
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void Func_80e3908(Part *p, int a, int b);
extern unsigned short Lee262[] __asm__(".Lee262");
extern unsigned char  Lee294[] __asm__(".Lee294");

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

void BaseAnim_Nova(void *context, int variant)
{
    vec3_t vec;
    DrawFn fns[2];
    void *ctx;
    int row;
    int frame;
    void *g2;
    int cx;
    int cy;
    State **sl;
    DrawFn *fp;
    int vm1;
    unsigned char *w2;
    unsigned char *w1;
    int **g;
    int **pp;
    unsigned char *base;
    State **slot;
    int i;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    g2 = g[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    if (variant == 0) {
        AnimStart(1);
        cx = 0x3c;
        cy = 0x30;
    } else if (variant == 1) {
        AnimStart(0);
        cx = 0x3c;
        cy = 0x40;
    } else {
        AnimStart(0);
        GetBattleActorPos2((*slot)->f8, &vec);
        cx = vec.x / 2;
        cy = vec.y + 0x30;
    }
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    fns[0] = (DrawFn)gPtrs[0x2e];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
    fp = fns;
    fp[1] = (DrawFn)gPtrs[0x2f];
    LoadVFXFile(FILE_7d, base, 1, 1);
    LoadVFXFile(FILE_73, g2, 0, 0);
    if (variant == 1) {
        void *s;
        int d0;
        CopyFn copy;
        s = GetFile(FILE_87);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    } else if (variant == 2) {
        void *s;
        int d0;
        CopyFn copy;
        s = GetFile(FILE_c4);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    }
    row = 0;
    if (Lee262[(*(State **)(base + 0x7828))->f18 * 5 + 1] != 0) {
        do {
            Part *p;
            p = (Part *)(base + row * 448 + (0xe1 << 7));
            i = 0;
            do {
                int a;
                int d2;
                d2 = i * 2;
                a = Random() & 0xffff;
                p->x = sin(a) * d2;
                p->y = -(cos(a) * d2);
                p->t = i / 2 + 0x19;
                i++;
                p++;
            } while (i != 0x10);
            i = 0;
            if (Lee262[(*(State **)(base + 0x7828))->f18 * 5] != 0) {
                int cyy;
                cyy = cy << 16;
                do {
                    State **s5;
                    State *st;
                    Part *e;
                    int rad;
                    int ang;
                    s5 = (State **)(base + 0x7828);
                    e = &gBuffer[Lee262[(*s5)->f18 * 5] * row + i];
                    rad = (0x3ff & Random()) + 0x20;
                    ang = Random() & 0xffff;
                    st = *s5;
                    if (st->f4 == 1)
                        e->x = (cx - Lee262[st->f18 * 5 + row + 2] + 0x1c) << 16;
                    else
                        e->x = (cx + Lee262[st->f18 * 5 + row + 2] - 0x1c) << 16;
                    e->y = cyy;
                    e->vx = (sin(ang) * rad) >> 6;
                    e->vy = (-(cos(ang) * rad * 2)) >> 6;
                    e->t = (7 & Random()) + 0x20;
                    i++;
                } while (i != Lee262[(*(State **)(base + 0x7828))->f18 * 5]);
            }
            row++;
        } while (row != Lee262[(*(State **)(base + 0x7828))->f18 * 5 + 1]);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    for (frame = 0;
         frame != (Lee262[(*(State **)(base + 0x7828))->f18 * 5 + 1] << 3) + 0x38;
         frame++) {
        void *cam;
        State *st;
        vm1 = variant - 1;
        sl = (State **)(base + 0x7828);
        cam = iwram_3001e80;
        st = *(State **)(base + 0x7828);
        if (st->f18 == 2 && frame <= 0x33) {
            if (st->f4 == 0)
                *(unsigned short *)((char *)cam + 0x36) += 0x80 << 1;
            else
                *(unsigned short *)((char *)cam + 0x36) -= 0x100;
        }
        if ((*sl)->f18 == 3 && frame == 4) {
            FillFn fill;
            fill = Func_80008d8;
            fill(ctx, 0x80 << 7, 0x3f3f3f3f);
        }
        if ((unsigned)vm1 <= 1) {
            if (frame == 2)
                _Func_80bd7dc(0x91);
        } else {
            if (frame == 2)
                _PlaySound(0x91);
            if (frame == 0x18)
                _Func_80bd7dc(0x86);
        }
        row = 0;
        if (Lee262[(*sl)->f18 * 5 + 1] != 0) {
            do {
                int r8x;
                r8x = row * 8;
                if (frame == r8x)
                    *(int *)(base + 0x77a8) = 0xc;
                if (frame >= r8x && frame < r8x + 2) {
                    State *st2;
                    st2 = *sl;
                    if (st2->f4 == 1)
                        fns[0](ctx, base,
                               cx - Lee262[st2->f18 * 5 + row + 2] + 0xc,
                               cy - 0x20, 0x20, 0x40);
                    else
                        fns[0](ctx, base,
                               cx + Lee262[st2->f18 * 5 + row + 2] - 0x2c,
                               cy - 0x20, 0x20, 0x40);
                }
                if (frame >= r8x) {
                    Part *p;
                    p = (Part *)(base + row * 448 + (0xe1 << 7));
                    i = 0;
                    do {
                        int xx;
                        int yy;
                        int t;
                        State *st3;
                        yy = ((short *)p)[3] + cy;
                        st3 = *sl;
                        if (st3->f4 == 1)
                            xx = ((short *)p)[1] + cx
                                 - Lee262[st3->f18 * 5 + row + 2] + 0x1c;
                        else
                            xx = ((short *)p)[1] + cx
                                 + Lee262[st3->f18 * 5 + row + 2] - 0x1c;
                        t = p->t;
                        if ((unsigned)t <= 0x11) {
                            fns[0](ctx, base + (Lee294[t / 3] << 11),
                                   xx - 0x10, yy - 0x20, 0x20, 0x40);
                            t = p->t;
                        }
                        if (t > 0)
                            p->t = t - 1;
                        else
                            p->t = -1;
                        i++;
                        p++;
                    } while (i != 0xc);
                }
                if (frame > r8x + 5) {
                    int gv;
                    gv = -0x1000;
                    if (variant != 2)
                        gv = 0x1000;
                    i = 0;
                    if (Lee262[(*(State **)(base + 0x7828))->f18 * 5] != 0) {
                        do {
                            Part *e;
                            e = &gBuffer[Lee262[
                                (*(State **)(base + 0x7828))->f18 * 5]
                                * row + i];
                            if (e->t > 0) {
                                int tt;
                                Func_80e3908(e, 0x3c, gv);
                                tt = e->t - 1;
                                e->t = tt;
                                if (e->y > (0xd8 << 15)) {
                                    e->vy = -e->vy / 2;
                                } else if ((unsigned)e->x <= 0x7effff &&
                                           e->y >= 0) {
                                    int n1;
                                    int sz;
                                    int xx;
                                    int yy;
                                    yy = e->y >> 16;
                                    xx = e->x >> 16;
                                    n1 = tt / 5 + 1;
                                    sz = n1 * 2;
                                    fp[i & 1](ctx,
                                        (char *)g2 + *(unsigned short *)
                                            ((char *)Data_ede48 + (sz - 2)),
                                        xx - n1 / 2, yy - n1, n1, sz);
                                }
                            }
                            i++;
                        } while (i != Lee262[(*(State **)(base + 0x7828))->f18 * 5]);
                    }
                }
                i = 0;
                if ((*(State **)(base + 0x7828))->f14 != 0) {
                    int fr;
                    fr = r8x + 6;
                    do {
                        if (frame == fr) {
                            State **s7;
                            s7 = (State **)(base + 0x7828);
                            Func_80d6888((*s7)->ids[i], 7, 5, i, 0xa);
                            _SetBattleActorKnockback((*s7)->ids[i], 4);
                        }
                        i++;
                    } while (i != (*(State **)(base + 0x7828))->f14);
                }
                row++;
            } while (row != Lee262[(*sl)->f18 * 5 + 1]);
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
