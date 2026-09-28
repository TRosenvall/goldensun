/* Debug_PaletteEditor  --  asm/rom_8a000/rom_8ba38_a_c_a_c.s  (0x0808d0c8)
 * NON-MATCHING, 315 encodings of 337.  NOT a distance (ref 716 bytes / 337 encodings
 * against ours 720 / 339, two instructions over).  READ `--align` INSTEAD: 124 of 352,
 * DOWN FROM 380, and now converging rather than misaligned -- all ten relocations present,
 * same symbols and order, offsets differing by a constant +4 after the first WaitFrames.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808d0c8.c \
 *     asm/rom_8a000/rom_8ba38_a_c_a_c.s --func Debug_PaletteEditor
 * Distance while iterating (the number that ranks variants here):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808d0c8.c \
 *     --ref asm/rom_8a000/rom_8ba38_a_c_a_c.s --align
 *
 * NON-MATCHING: 315 encodings of 337 differ (objcmp), ours 339.  SIZE ref 716, ours 720.
 * Working distance: 124 instructions in disagreeing regions of 352 (tryc --align).
 * NOT a true distance, but it IS now converging: the previous park read 329 of 337 with
 * --align 380 of 352 (longer than the ROM itself).  380 -> 124, length 334 -> 339 against
 * the ROM's 337, and ALL TEN RELOCATIONS are present in the right order with the same
 * symbols; only their offsets differ, by a constant +4 after the first WaitFrames.
 * Batch 294 brief G, single-target.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808d0c8.c asm/rom_8a000/rom_8ba38_a_c_a_c.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808d0c8.c --ref asm/rom_8a000/rom_8ba38_a_c_a_c.s --align
 *
 * Whole-file conversion: one function, no data (datacheck silent).  No pins, no flags.
 * ZERO SHIMS in both classes: no `register ... __asm__` declaration and no
 * `__asm__(".equ ...")` line.  The one asm-name alias, `extern const void *L9e4ce
 * __asm__(".L9e4ce")`, is the tree's existing convention for the `.incrom` blob exported
 * by src/rom_8a000/rom_8ba38_c_c_b.c and is used in landed files (src/rom_c9000/rom_d82b0_b.c).
 *
 * The `.s` comment block names it UpdateFollowerPositions and says "~180-instruction body";
 * both are wrong for this file -- it is 321 instructions and it is a palette editor.
 *
 * ============================================================================
 * THE PRIMARY QUESTION OF THE BRIEF, ANSWERED OUT OF THE DUMP, AND THE PREVIOUS PARK'S
 * REASONING WAS INVERTED
 * ============================================================================
 * The previous park said: "The bound is a CONST_INT so check_dbra_loop's vanilla path is
 * open, and what normally shuts it is loop.c:7896's `giv_count == 0` test -- this loop has
 * three givs (the tile number, the palette pointer and the map pointer), so the ROM's
 * non-reversal is expected and OURS is the anomaly."
 *
 * BOTH HALVES ARE WRONG, and the `.08.loop` dump says so in one screen:
 *
 *   Loop from 79 to 173: 23 real insns.
 *   Insn 156: possible biv, reg 42, const =1     <- j
 *   Insn 159: possible biv, reg 41, const =1     <- t
 *   Insn 162: possible biv, reg 34, const =2     <- pal
 *   Insn 165: possible biv, reg 32, const =2     <- p
 *   Reg 32/34/41/42: biv verified
 *   Insn 86: dest address src reg 32 benefit 0 lifetime 1 replaceable ncav mult 1 add 0
 *   Insn 89: dest address src reg 34 benefit 0 lifetime 1 replaceable ncav mult 1 add 0
 *   Can reverse loop
 *   Reversed loop and added reg_nonneg
 *   ...
 *   giv of insn 86 not worth while, -68 vs 23.
 *   giv of insn 89 not worth while, -68 vs 23.
 *
 *  (1) There are FOUR BIVS AND ZERO GIVS.  The tile number, the palette pointer and the map
 *      pointer are each a plain `+= const`, so strength_reduce verifies them as BIVS, not
 *      givs.  The only two giv candidates are the two DEST_ADDR uses, and both are refused
 *      ("not worth while, -68 vs 23").  `bl->giv_count` is 0.
 *  (2) `giv_count == 0` at loop.c:7896 is NOT a protection, it is the ENABLING CONDITION.
 *      Read the source (/opt/camelot-gcc/gcc-2.96/gcc/loop.c):
 *         if (bl->giv_count == 0 && ! loop->exit_count)
 *           { ... no_use_except_counting = 1; for (...) <scan for non-counting uses> }
 *      and the gate it feeds is
 *         if ((num_nonfixed_reads <= 1 && !has_call && ... ) || no_use_except_counting)
 *      so having no givs is exactly what lets gcc conclude the counter is used only to
 *      count, which is what OPENS reversal.  The park had the polarity backwards.
 *  (3) What actually blocks it is the SIGNEDNESS OF THE COMPARISON:
 *         if (comparison && (GET_CODE (comparison) == LT
 *                            || (GET_CODE (comparison) == LE && no_use_except_counting)))
 *      A signed `int j` with `j <= 0xf` is LE, and with no_use_except_counting == 1 it
 *      passes; gcc then normalises initial_value to 0, takes the vanilla path, and emits
 *      `mov #0xe / sub #1 / cmp #0 / bge`.  An UNSIGNED counter with `<= N` produces LEU,
 *      which matches NEITHER arm, so check_dbra_loop returns 0 and the loop stays ascending
 *      and spells `bls` -- which is precisely what the ROM has (`cmp r2,#0xf / bls`).
 *      This is already recorded in docs/elevation.md ("An `unsigned` counter with `<= N`
 *      blocks check_dbra_loop and still spells `bls`"); the park simply did not apply it.
 *      With `unsigned int j` the dump still prints "Can reverse loop" and no longer prints
 *      "Reversed loop and added reg_nonneg".  Worth 380 -> 337 alone (see the drop table).
 *  (4) THE BRIEF'S SUGGESTED DIAGNOSTIC IS A CLEAN NEGATIVE, and for a reason worth
 *      recording: `-fno-rerun-loop-opt` on the old candidate leaves it at exactly 380.
 *      The reversal happens in loop pass 1 (the dump's first "Loop from 79 to 173" block),
 *      so no second-pass flag can touch it, and `no_use_except_counting` did not need the
 *      second pass here -- the three pointers were bivs from the start.  loop->vtop is
 *      irrelevant too: the vanilla path was taken, not the NE path that needs vtop.
 *
 * ============================================================================
 * THE BIGGEST FINDING, AND IT INVALIDATES ITEM 3 OF THE PREVIOUS PARK
 * ============================================================================
 * THE ROM'S FOUR REDRAW ENTRY POINTS ARE gcse's OWN PRE HOIST OF `row << 12` AND
 * `row << 5`.  THEY ARE NOT SOURCE VARIABLES.  Naming them destroyed them.
 *
 * The old candidate carried `rowhi = row << 12; rowlo = row << 5;` as source variables
 * assigned in the two row branches.  That made `rowhi` AVAILABLE at every edge into the
 * redraw block, and gcc's PRE then took one step further and hoisted `rowhi + 0xd1` as well:
 *
 *   PRE: redundant insn 69 (expression 2) in bb 1, reaching reg is 180
 *   PRE/HOIST: edge (0,1), copy expression 2
 *   PRE/HOIST: edge (19,1), copy expression 2
 *   PRE/HOIST: edge (23,1), copy expression 2
 *
 * -- expression 2 being `(plus:SI (reg/v:SI 39) (const_int 209))`.  It fires because the
 * A/B-key branches also jump back to redraw WITHOUT touching rowhi, so the sum is partially
 * redundant on that path and LCM pays three insertions to save one computation.  The result
 * is that ours carried `(row<<12)+0xd1` and the ROM carries `row<<12`.
 *
 * Delete rowhi/rowlo and compute `t = (row << 12) + 0xd1` and
 * `pal = (u16 *)(0x5000002 + (row << 5))` from `row` in the redraw block, and gcc does the
 * carry itself.  Now PRE's operand is `row`, which IS killed on the two row branches and is
 * NOT killed on the A/B path, so it hoists the two SHIFTS onto exactly the five edges that
 * change row and leaves `+ 0xd1` in the block:
 *
 *   PRE: redundant insn 74 (expression 6) in bb 1, reaching reg is 191
 *   PRE: redundant insn 69 (expression 4) in bb 1, reaching reg is 192
 *   PRE/HOIST: edge (0,1) / (17,1) / (18,1) / (20,1) / (21,1), copy expressions 4 and 6
 *
 * Five edges: the entry, row-- no-wrap, row-- wrap, row++ no-wrap, row++ wrap.  jump2 then
 * cross-jumps their tails into the ROM's .L8d0f2 / .L8d108 / .L8d10e / .L8d112 chain, and
 * `add r0, #0xd1` appears in the redraw block exactly where the ROM has it.
 *   WORTH 273 -> 124.  The single largest lever on this function by a factor of five.
 *
 * > The general form: a value the ROM appears to CARRY across a join may be gcc's own PRE
 * > insertion on the incoming edges.  Naming it in the source makes it available at those
 * > edges and lets PRE hoist the NEXT expression out too -- so the hand-written hoist both
 * > reproduces nothing and costs you the thing above it.  This is the join-point twin of the
 * > recorded "naming a value gcc already carries destroys the carry", and the tell is a
 > cross-jumped fan of two-or-three-instruction blocks feeding one label.
 * > This is NOT the PRE already in docs/elevation.md.  The recorded note (the `mov`+`lsl`
 * > constant hoisted to a DOMINATING use) says the per-use-site naming lever cannot touch
 * > it.  This is the join-point case, it runs the other way -- the source must NOT name the
 * > value -- and it is actionable, because the dump prints the edge list to compare against
 * > the ROM's entry points.
 * > `-fno-gcse` is NOT the diagnostic: it reads 374 against 281, because the same pass is
 * > carrying the rest of the function.  Read the `PRE/HOIST: edge (a,b)` lines instead and
 * > count them against the ROM's entry points.
 *
 * ============================================================================
 * EVERY LOAD-BEARING CONSTRUCT, EACH MEASURED BY DROPPING IT ALONE FROM THIS FILE
 * ============================================================================
 *   construct                                             this file   dropped
 *   the PRE shape above (no rowhi/rowlo)                     124        273
 *   `bp` + running `d` in the redraw header, `p = bp + 1`    124        319
 *   `unsigned int c` (ROM has `lsr`, an int gives `asr`)     124        172
 *   `int q` carrying `row + 0xfffff0e0` (see below)          124        166
 *   `unsigned int j` (the reversal, above)                   124        131
 *   `unsigned int r/g/b` (ROM has `bhi`, an int gives `bgt`) 124        127
 *   `unsigned int f` (ROM has `bls` on `f > 0x27`)           124        125
 * The last one is the brief's warning about --align made concrete: `unsigned f` moves the
 * aligned count by ONE but changes `ble` to `bls`, taking the function's `bls` count from
 * 1 to the ROM's 2.  It is correct and --align barely sees it.
 *
 * THE LITERALS: the park's reading was right, and there is one exception it missed.
 *   The ROM's `ldr r3, .L8d144 @ 0xf052` / `.L8d148 @ 0xf047` / `.L8d14c @ 0xf042` and
 *   `ldr r5, .L8d150 @ 0x1f` / `ldr r4, .L8d154 @ 0xf0e0` are gcc's own `ldrh rD, <label>`
 *   over a pool word (batch-292 correction; Thumb-1 has no PC-relative ldrh so gas
 *   assembles both as `ldr rD,[pc,#imm]`), so the bare HImode literals are right here.
 *   THE EXCEPTION IS THE FIRST STORE.  `*(u16 *)0x600205a = row + 0xfffff0e0` is narrowed
 *   AT EXPAND, not by combine: the .00.rtl dump shows
 *       (set (reg:HI 55) (const_int 61664))            <- 0xfffff0e0 truncated to 0xf0e0
 *       (set (reg:HI 56) (subreg:HI (reg/v:SI 36)))
 *       (set (reg:SI 57) (plus:SI (subreg:SI (reg:HI 55)) (subreg:SI (reg:HI 56))))
 *   -- i.e. convert.c distributes the implicit truncation-to-u16 over the `+`, and the pool
 *   word becomes 0xf0e0 where the ROM's is 0xfffff0e0.  THIS CONFIRMS AN EXISTING NOTE:
 *   docs/elevation.md's "Blocker 1b also protects an ADDEND" says exactly this for a written
 *   `+ 0xffff`, and it extends unchanged to an addend whose OTHER operand is a variable.
 *   An `int` carrier assigned in its own
 *   statement (`q = row + 0xfffff0e0; *bp = q;`) leaves the addition in SImode, keeps the
 *   pool word at 0xfffff0e0, and gives the ROM's destructive `add r3, r9`.  Worth 284 -> 281,
 *   and it also drops the sixth HImode pool word so the minipool holds exactly the ROM's
 *   five (0xf052, 0xf047, 0xf042, 0x1f, 0xf0e0) before the SImode ones.
 *   So: the HImode-literal rule has no direction WITHIN ONE FUNCTION either.  Four of the
 *   five halfword stores here want the bare literal and the fifth wants an int carrier,
 *   because only the fifth has a non-constant operand for convert.c to distribute over.
 *
 * STATEMENT ORDER IN THE REDRAW HEADER IS A CLIFF, NOT A GRADIENT.
 *   `p = bp + 1;` must be written BEFORE `pal = ...`.  Moving it after reads 317 against
 *   124, and the PRE log is IDENTICAL in both -- the cause is later: `bp` ends up in `ip`,
 *   costing `mov ip,r2 / mov r0,ip`, and the pool rotates (0x600205a ahead of 0xfffff0e0),
 *   which moves every relocation.  Four orderings of the four header assignments were
 *   measured: (p,t,pal,j) 124, (t,p,pal,j) 124, (t,pal,p,j) 317, (pal,t,p,j) 320,
 *   (j,t,pal,p) 317, (j,p,t,pal) 125.  Only `pal` after `p` is safe.
 *
 * THE PARK'S RUNNING-POINTER LEVER RE-VALIDATES AT THE NEW STRUCTURE, AND EXTENDS.
 *   Three independent offsets off `p` in the draw loop: 124 -> 313.  Same mechanism the park
 *   found (gcc spends both `ip` and `lr` as address temporaries), still worth 189 here.
 *   NEW: the redraw HEADER wants the same shape.  The ROM writes the first map row through
 *   a base it keeps (`ldr r1,=0x600205a / strh r3,[r1]`), walks the other three with
 *   `mov r2,r1 / add r2,#0x40` x3, and then derives the loop pointer with `add r1,#2`.
 *   Four literal addresses instead (0x600205a/0x600209a/0x60020da/0x600211a plus
 *   `p = (u16 *)0x600205c`) reads 319: gcc uses ONE running register for all four stores and
 *   then derives `p` backwards from the LAST address (`sub r2,#0xbe`) where the ROM derives
 *   it forwards from the FIRST (`add r1,#2`).  A base pointer plus a separate running
 *   pointer plus `p = bp + 1` is the only shape that reproduces both.
 *
 * ============================================================================
 * INERT OR WORSE -- AS A DELTA TO THE PARK'S LIST, NOT A RESTATEMENT
 * ============================================================================
 * NEWLY INERT, and two of these RETIRE things the park asked for:
 *  - PARK ITEM 2 IS RETIRED.  The park wanted `0x1f` named once and used at all six masking
 *    sites, and wanted the gKeyRepeat pool address named as a pointer, to buy the ROM's
 *    `sub sp,#0xc` and its `str r1,[sp,#4] / str r4,[sp]` spill pair.  Under the OLD
 *    structure both paid (named mask 330 -> 285, named pointer 285 -> 284, and the pointer
 *    is what took the frame from 8 to the ROM's 0xc).  Under the PRE structure both are
 *    BYTE-IDENTICAL to writing `0x1f` and `gKeyRepeat` directly, and the frame is 0xc
 *    either way.  Four variants -- {named mask, bare} x {named pointer, bare} -- all
 *    produce the same 124 and, after label renumbering, the same assembly file.  The
 *    invented variables are gone from this draft; they were a proxy for pressure that the
 *    correct structure supplies on its own.
 *  - THE KEYS LOOP'S SPELLING IS INERT.  `keys: ... goto keys;` and
 *    `while (1) { ... if (gKeyRepeat & 4) break; ... }` are byte-identical, so the brief's
 *    break-vs-goto lever (stmt.c:2257) does not reach this function: the loop is `while (1)`
 *    with no entry test, so there is no first exit test for expand_end_loop to roll.  This
 *    draft uses the `while (1)` form because it is the more natural C, not because it pays.
 *  - `for (j = 1; j <= 0xf; j++)` is byte-identical to the do/while with `j++` in the body.
 *  - Moving `j++` to after `p++` in the loop body: inert.
 *  - `d = p;` as its own statement before the first `+ 0x40`: inert.
 *  - `} while (j != 0x10)` instead of `<= 0xf`: 125, one worse.
 *  - Init order `ch,col,row` 128, `col,ch,row` 136, `row,col,ch` 132 against `row,ch,col` 124.
 *  - DECLARATION ORDER IS INERT HERE, three orderings tested (counter first, `p` last,
 *    `bp` first): all 124.  Worth recording because it was the cheapest candidate lever for
 *    the p-vs-j high-register split below -- gcc-2.96 allocates DECL_RTL for an
 *    address-free uninitialised local at first USE, not at declaration, so the pseudo
 *    numbers and hence the allocno tie-break do not follow the declaration list.
 * WORSE, WITH THE MECHANISM:
 *  - `-fno-gcse`: 374.  Not a diagnostic for the PRE question (see above).
 *  - `-fno-rerun-loop-opt`: exactly 380, i.e. no effect at all (see (4) above).
 * STILL TRUE FROM THE PARK: the running pointer in the draw loop (now 189, was 34) and the
 * bare HImode literals at four of the five sites.
 *
 * ============================================================================
 * WHAT IS LEFT: 124, AND IT IS ALMOST ALL REGISTER NAMING PLUS TWO CROSS-JUMPS
 * ============================================================================
 * The hunk-by-hunk length tally (59 hunks) nets to exactly +2 instructions, and only six
 * hunks have a nonzero length delta worth naming:
 *  1. +5 across the two row-WRAP blocks.  The ROM's row-- wrap block ends
 *     `mov r3,r9 / mov r1,r9 / lsl r3,#12 / b .L8d10e`, sharing `lsl r1,#5 / str r3,[sp,#8]
 *     / mov r11,r1` with the row++ no-wrap block; ours picks r2/r3 where the ROM picks r3/r1,
 *     so the instructions are not textually equal and jump.c's cross_jump cannot merge them,
 *     and the block is emitted in full.
 *  2. -2 at the entry block.  The ROM's `mov r3,#0 / str r3,[sp,#8] / mov r11,r3 / b` stands
 *     alone because its row++ wrap twin uses r1 (`mov r1,#0 / mov r9,r1 / str r1,[sp,#8]`);
 *     ours uses r3 in both, so cross_jump merges them and ours is two shorter.
 *     (1) and (2) are the SAME cause read from both ends: the ROM's local-alloc put the
 *     zero-and-wrap constants in r1/r3 where ours puts them in r3/r0.  Not steerable from
 *     source that I could find; four init orderings and two counter placements all failed.
 *  3. Net 0 but ~10 instructions misnamed in the draw loop: the ROM gives `p` r1 and the
 *     counter `j` r12, ours gives `p` ip and `j` r7.  The same eight low registers and one
 *     high register are used on both sides -- only which of p/j takes the high one differs.
 *     ROM: `add r1,#2` for p and `mov r3,#1 / add r12,r3 / mov r2,r12` for j; ours the
 *     mirror.  In our preheader `p` is set first and so has the longer live range and the
 *     lower global.c priority; the ROM sets `mov r2,#1` (j's value) early and `mov r12,r2`
 *     last.  Moving `j = 1` earlier in the source reads 125 or 317, never better.
 *  4. Net 0, the move2add pairing.  `-1` is derived from the mask by `sub r3,#0x41` in the
 *     ROM's ch-- block and by `sub r3,#0x21` in OUR col-- block, with `mov/neg` in the
 *     other one each -- reload_cse_move2add is per hard register, so this follows (3).
 *  5. +1: ours emits `b .Lnn` immediately before `.Lnn:` where the ROM has a `.pool_aligned`
 *     between them.  A pool-placement consequence, not an extra computation.
 *  6. The prologue's two `ldr`s are swapped and `row`'s temp is r0 where the ROM's is r1;
 *     the pool order is nevertheless already correct (`.L9e4ce` relocates at 0x38 in both).
 * Everything else in the 124 is low-scratch naming in the key and A/B blocks, with one
 * consistent shift: ours uses r0 where the ROM uses r2, r1 where the ROM uses r5, r2 where
 * the ROM uses r3.  The ROM never touches r0 in the whole key-handling section and ours
 * does; I did not find the pressure that forbids it.
 *
 * HONEST DISTANCE.  This is no longer a shape problem: the control-flow graph, the frame,
 * the five PRE edges, the pool contents and order, the relocation list and the instruction
 * count (+2) all agree with the ROM.  What remains is one local-alloc decision in the draw
 * loop (p vs j for the high register) which then propagates to the wrap-block cross-jumps
 * and the whole low-register naming.  I could not move it from source in about a dozen
 * attempts.  It is the "right instructions, wrong registers" class, i.e. the class
 * docs/elevation.md says a pin or a pressure change answers -- and the brief forbids pins
 * here, correctly, since a landing needs none.
 */
#include "gba/types.h"

extern const void *L9e4ce __asm__(".L9e4ce");
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int iwram_3001e40;

extern void DecompressLZ16(const void *src, void *dst);
extern void WaitFrames(int n);
extern void Func_800479c(void);
extern void ClearVRAM(void);

void Debug_PaletteEditor(void);

void Debug_PaletteEditor(void)
{
    u16 *p;
    u16 *d;
    u16 *bp;
    u16 *pal;
    u16 *sel;
    int row;
    int ch;
    int col;
    int t;
    int q;
    unsigned int j;
    int i;
    unsigned int c;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int f;

    row = 0;
    ch = 1;
    col = 1;
    DecompressLZ16(&L9e4ce, (void *)0x6001a00);
redraw:
    bp = (u16 *)0x600205a;
    q = row + 0xfffff0e0;
    *bp = q;
    d = bp;
    d = (u16 *)((char *)d + 0x40);
    *d = 0xf052;
    d = (u16 *)((char *)d + 0x40);
    *d = 0xf047;
    d = (u16 *)((char *)d + 0x40);
    *d = 0xf042;
    p = bp + 1;
    t = (row << 12) + 0xd1;
    pal = (u16 *)(0x5000002 + (row << 5));
    j = 1;
    do {
        *p = t;
        c = *pal;
        d = (u16 *)((char *)p + 0x40);
        *d = (c & 0x1f) + 0xf0e0;
        d = (u16 *)((char *)d + 0x40);
        *d = ((c >> 5) & 0x1f) + 0xf0e0;
        d = (u16 *)((char *)d + 0x40);
        *d = ((c >> 10) & 0x1f) + 0xf0e0;
        j++;
        t++;
        pal++;
        p++;
    } while (j <= 0xf);
    WaitFrames(1);
    while (1) {
        if (gKeyRepeat & 0x40) {
            ch--;
            if (ch <= 0)
                ch = 3;
        }
        if (gKeyRepeat & 0x80) {
            ch++;
            if (ch > 3)
                ch = 1;
        }
        if (gKeyRepeat & 0x20) {
            col--;
            if (col <= 0)
                col = 0xf;
        }
        if (gKeyRepeat & 0x10) {
            col++;
            if (col > 0xf)
                col = 1;
        }
        if (gKeyRepeat & 0x200) {
            row--;
            if (row < 0)
                row = 0xd;
            goto redraw;
        }
        if (gKeyRepeat & 0x100) {
            row++;
            if (row > 0xd)
                row = 0;
            goto redraw;
        }
        if (gKeyRepeat & 1) {
            sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
            c = *sel;
            r = c & 0x1f;
            g = (c >> 5) & 0x1f;
            b = (c >> 10) & 0x1f;
            if (ch == 1 && r <= 0x1e)
                r++;
            if (ch == 2 && g <= 0x1e)
                g++;
            if (ch == 3 && b <= 0x1e)
                b++;
            *sel = (b << 10) | (g << 5) | r;
            goto redraw;
        }
        if (gKeyRepeat & 2) {
            sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
            c = *sel;
            r = c & 0x1f;
            g = (c >> 5) & 0x1f;
            b = (c >> 10) & 0x1f;
            if (ch == 1 && r != 0)
                r--;
            if (ch == 2 && g != 0)
                g--;
            if (ch == 3 && b != 0)
                b--;
            *sel = (b << 10) | (g << 5) | r;
            goto redraw;
        }
        if (gKeyRepeat & 8) {
            sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
            c = *sel;
            f = 0;
            goto flashwait;
flash:
            if (f == 0)
                *sel = 0x7fff;
            if (f == 0xa)
                *sel = c;
            if (f == 0x14)
                *sel = 0;
            if (f == 0x1e)
                *sel = c;
            f++;
            if (f > 0x27)
                f = 0;
flashwait:
            WaitFrames(1);
            if (gKeyHeld & 8)
                goto flash;
            *sel = c;
        }
        if (gKeyRepeat & 4)
            break;
        i = iwram_3001e40;
        WaitFrames(1);
    }
    Func_800479c();
    ClearVRAM();
}
