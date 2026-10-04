/* Func_801c154  @  0x0801c154  [rom_15000]   *** PARK -- 8 of 17 ***
 * NON-MATCHING, 8 of 17 encodings (measured batch 322).
 *
 * NOT A LANDING. Parked at 8, PIN-FREE. Same figure as the two parks this
 * replaces, but a DIFFERENT and sharper residue: this body already has the
 * ROM's register ASSIGNMENT, which neither park did.
 *
 * ===========================================================================
 * FIRST: THE TWO-PARK QUESTION. SETTLED BY MEASURING BOTH BODIES.
 * ===========================================================================
 *
 * Func_801c154 had TWO park files. Batch 317's lesson was that twice the park
 * claiming the better figure held NO FUNCTION DEFINITION AT ALL -- pure comment
 * compiling to a zero-byte TU that objcmp still prints a number against.
 *
 *   *** THAT FAILURE MODE IS NOT WHAT HAPPENED HERE. BOTH PARKS HAVE REAL,
 *       COMPILING FUNCTION DEFINITIONS, AND THE TWO BODIES ARE BYTE-IDENTICAL
 *       TO EACH OTHER. ***
 *
 *   md5 of everything from `#include` onward:
 *       src/non_matching/rom_15000/801c154.c   6dab0d707a826d4b1ba7379d2d2427de
 *       src/non_matching/rom_15000/rom_1c154.c 6dab0d707a826d4b1ba7379d2d2427de
 *   and both measure IDENTICALLY, --func and --whole:
 *       8 of 17 (ours 15, COUNT DIFFERS), size 40 v 36, relocations identical,
 *       MEM clean, idx=[1,2,3,4,11,12].
 *
 * So this is TRUE REDUNDANCY, not a race -- no work was lost, and no figure
 * was a phantom. The difference between the files is PROSE ONLY.
 *
 * WHICH TO KEEP: 801c154.c's prose is the strict superset. It is batch 317
 * (commit a2aeb2ac, 2026-10-01) and it already reproduces rom_1c154.c's whole
 * TRIED list, plus the corpus result, plus the refutation below. rom_1c154.c is
 * older prose (b67ea886, 2026-08-03) whose last touch was batch 319's recipe
 * backfill (a676b77b, 2026-10-03) -- a newer COMMIT DATE on OLDER CONTENT,
 * which is exactly how a duplicate park wins a headline it has not earned.
 * RETIRE rom_1c154.c.
 *
 * THREE DEFECTS TO FIX WHILE RETIRING IT, each of which bites someone later:
 *  1. 801c154.c's own `Verify with:` recipe names rom_1c154.c AS THE CANDIDATE.
 *     Retire rom_1c154.c without fixing that and the keeper's recipe breaks.
 *     This file's recipe names ITSELF. (parkcheck.py reads these.)
 *  2. rom_1c154.c says "NOT SPLIT. The .s still holds both of its functions and
 *     the linker script is untouched." STALE -- the split has happened:
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s holds exactly ONE
 *     .thumb_func_start and datacheck.py is clean.
 *  3. BOTH parks cite asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s, which no longer
 *     exists. The reference is the `_c` file above.
 * ONE THING WORTH MERGING FROM rom_1c154.c AND NOT IN 801c154.c: its three
 * TU-context tests, reproduced under "REFUTED" below.
 *
 * ===========================================================================
 * SECOND: WHICH READING OF THE POOL-PLACEMENT QUESTION SURVIVES
 * ===========================================================================
 *
 * The "*** CORRECTED, SAME BATCH ***" block is in NEITHER current file. It was
 * in the OLD 801c154.c, at commit c75a7c53 ("SETTLED: the branch-over-pool
 * shape is not a blocker, and my own park was wrong", 2026-08-30), and batch
 * 317 removed it, quoting and refuting it. It said: pool placement is not a
 * residue at all; the pool branch is a consequence of CODE LENGTH; the missing
 * `b` is "downstream of being two instructions short for some other reason";
 * screen the class normally.
 *
 *   *** THAT READING IS REFUTED HERE BY DIRECT COUNTING, WHICH IS A STRONGER
 *       ARGUMENT THAN THE ONE BATCH 317 USED. ***
 *
 * Per-index, with pool words and padding identified (the brief's trap: an
 * objcmp "encoding" can be a POOL WORD, and here two of them are):
 *
 *      0  b500  push {lr}            |  b500  push {lr}
 *   XX 1  4b06  ldr  r3,=0x1ff       |  4c06  ldr  r4,=0x1ff        real insn
 *   XX 2  88c4  ldrh r4,[r0,#6]      |  88c3  ldrh r3,[r0,#6]       real insn
 *   XX 3  4019  ands r1,r3           |  4021  ands r1,r4            real insn
 *   XX 4  4b05  ldr  r3,=0xfffffe00  |  4c05  ldr  r4,=0xfffffe00   real insn
 *      5  4023  ands r3,r4           |  4023  ands r3,r4   (same ENCODING,
 *      6  430b  orrs r3,r1           |  430b  orrs r3,r1    opposite meaning)
 *      7  21fc  movs r1,#0xfc        |  21fc  (same)
 *      8  80c3  strh r3,[r0,#6]      |  80c3  (same)
 *      9  7102  strb r2,[r0,#4]      |  7102  (same)
 *     10  bl Func_8003dec            |  bl    (same)
 *   XX11  e004  b .L1c178            |  bc01  pop {r0}              real insn
 *   XX12  0000  .short 0x0000        |  4700  bx r0            POOL PADDING
 *     13  .word 0x000001ff           |  .word 0x000001ff           POOL WORD
 *     14  .word 0xfffffe00           |  .word 0xfffffe00           POOL WORD
 *   XX15  bc01  pop {r0}             |  --
 *   XX16  4700  bx  r0               |  --
 *
 * ROM = 14 real instructions + 1 padding halfword + 2 pool words.
 * Ours = 13 real instructions + 2 pool words.
 *   *** WE ARE SHORT EXACTLY ONE REAL INSTRUCTION, AND IT IS THE `b`. ***
 * Every other real instruction is present. There is no "some other reason" to
 * be downstream of: the ONLY length difference IS the `b`, and the `b` exists
 * only because the pool is placed before the epilogue. "Length causes
 * placement" is therefore circular on this function, and self-refuting.
 *
 * SO 801c154.c's VERDICT SURVIVES -- but its ARITHMETIC DOES NOT, and it
 * contradicts itself. It says the placement "is 2 of the 8 encodings and all 4
 * bytes ... the rest is misalignment downstream of it", and then later says the
 * rename "is 2 encodings of the 8 and the placement is the other 6". MEASURED,
 * IT IS FOUR AND FOUR:
 *     rename    -> idx 1, 2, 3, 4                  = 4 of the 8, 0 bytes
 *     placement -> idx 11, 12 plus the 2-count gap = 4 of the 8, all 4 bytes
 * So "do not spend more passes on the register rename -- it is 2 encodings of
 * the 8" understates it by half. The rename is HALF THE FIGURE and it is the
 * half that moved this batch.
 *
 * THE CORPUS BOUND BEHIND THE PLACEMENT HALF: RE-MEASURED, IT SURVIVES, AND IT
 * IS SLIGHTLY STRONGER THAN RECORDED. Independent re-scan, denominator printed
 * (the brief's rule), 4,418 generated .s files carrying gcc's own banner,
 * 3,478 functions with a literal pool:
 *
 *                               pool BEFORE epilogue   pool AFTER
 *     has a narrow pool load        197 (min 12)        27 (min 4)
 *     only `ldr` pool loads         112 (min 23)      3142 (min 2)
 *
 *   A narrow pool load gives pool-before 88% of the time (197/224); without
 *   one, 3.4% (112/3254). The park recorded 90% / 3.8%, 189/110, floors 11/21.
 *   *** THE LOAD-BEARING FLOOR RE-MEASURES AT 23, NOT 21: there is no
 *       pool-before function in this tree below 23 instructions whose pool
 *       loads are all `ldr`. *** Func_801c154 is 14. The bound HOLDS.
 *
 *   The batch-322 correction that HImode pooling is ALTERNATIVE ORDER (alt 1's
 *   `mn` matches any const_int before alt 5's `I` is reached), not the absence
 *   of an immediate alternative, does NOT disturb this: the park's claim is
 *   that these masks can never BE HImode, because gcc-2.96 Thumb has no HImode
 *   AND, so every narrow spelling is promoted to SImode before the mask load.
 *   The correction changes WHY HImode pools, not whether these two constants
 *   can get there. Their pool words are already exact in both bodies.
 *
 * I ALSO HIT THE TRAP THE PARK WARNED ABOUT, INDEPENDENTLY. Two of my rows
 * (`e7`, `g4`) reach ref 17 encodings AND ref 40 bytes -- an apparent exact
 * size+count match -- by PUSHING r5 (`push {r5, lr}` / `pop {r5}`), two extra
 * instructions and four extra bytes, with the pool still after the epilogue and
 * still no `b`. They read 13 and 14. A size-only or count-only screen calls
 * those near-misses. They are not.
 *
 * REFUTED, and worth keeping from rom_1c154.c: the TU-CONTEXT theory. Three
 * tests, all negative -- Func_801c154 is the LAST function in its .s so "code
 * after it" cannot be the cause; adding a function after it in the .c does not
 * move the pool; adding a bulky literal-heavy function BEFORE it does not
 * either. Pool-before also happens in SINGLE-FUNCTION TUs (Func_8006358,
 * Func_80b09fc), so it is not a TU property at all.
 *
 * ===========================================================================
 * THIRD: THE RENAME HALF MOVED. TWO ORTHOGONAL LEVERS, ONE NEW RUNG.
 * ===========================================================================
 *
 * FIGURE of this body (measured):
 *   objcmp --func / --whole: 8 of 17 (ours 15), size 40 v 36, relocations
 *   identical, MEM clean, idx=[3,4,5,6,11,12].
 *   PIN COUNT: 0.
 *
 * Verify with (INSTALLED PATH):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801c154.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s --func Func_801c154
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s -> 1
 *   python3 tools/datacheck.py <that .s>            -> clean (no output)
 *   python3 tools/split_s.py   <that .s> --dry-run  -> nothing to split
 *   INSTALL PATH if it ever lands: src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.c
 *
 * *** THIS BODY'S idx LIST IS 3,4,5,6 AND THE PARKS' IS 1,2,3,4. SAME FIGURE,
 *     DIFFERENT RESIDUE. Indices 1 and 2 -- `ldr r3,=0x1ff` and
 *     `ldrh r4,[r0,#6]` -- NOW MATCH: the mask is in r3 and the loaded halfword
 *     is in r4, which IS the ROM's assignment and is the thing both parks
 *     called their residue. ***
 *
 * A 24-CELL CROSS over three dimensions found two orthogonal levers and a clean
 * 12-row inert dimension. Dimensions: v-form (`v &= M` / `v = v & M`) x t-form
 * (same) x OR order (`t | v` / `v | t`) x mask (two literals / ONE reassigned
 * named mask / TWO named masks). Results collapse into exactly three outcomes:
 *
 *   mask = ONE REASSIGNED NAMED MASK  -> 8, idx=[1,2,3,4,11,12]  (both parks)
 *       8 of 8 such rows, regardless of OR order or either and-form.
 *   mask = literals or TWO named  +  `v | t`  -> 8, idx=[3,4,5,6,11,12] (HERE)
 *       8 of 8 such rows.
 *   mask = literals or TWO named  +  `t | v`  -> 10, idx=[1,2,3,4,5,6,11,12]
 *       8 of 8 such rows -- both halves wrong.
 *
 *   LEVER 1: THE OR'S OPERAND ORDER. `p->f6 = v | t` fixes indices 1 and 2;
 *            `t | v` does not. Nobody had tried this.
 *   LEVER 2: A SINGLE REASSIGNED MASK PINS THE MASK TO r4 AND OVERRIDES LEVER 1
 *            COMPLETELY. This is the exact mechanism rom_1c154.c advertised as
 *            its headline win ("ONE VARIABLE REASSIGNED gives both the 32-bit
 *            loads AND the register reuse") -- and it is ALSO what blocked the
 *            rename. See below.
 *   INERT, 12 PAIRINGS: `v &= M` versus `v = v & M`, and the same for `t`.
 *            Exactly inert in every pairing. A free code-quality choice.
 *
 * WHY A REASSIGNED MASK CANNOT GET r3 -- the mechanism, from .17.lreg/.18.greg:
 *   Parks' body:  ;; 1 regs to allocate: 35
 *                 Register 35 (the mask `m`) ... set 2 times; DIES IN 2 PLACES
 *                 Register dispositions: ... 35 in 4   36 (`t`) in 3
 *   A reassigned mask dies twice, so LOCAL-ALLOC declines it and it is the one
 *   pseudo handed to GLOBAL-ALLOC -- which runs after local-alloc has already
 *   given r3 away. Under REG_ALLOC_ORDER (config/arm/arm.h:989 = 3,2,1,0,...)
 *   only r4 is left.
 *   AND THIS IS NOT A PRIORITY QUESTION. I flipped the body so the mask becomes
 *   the long-lived result carrier (`m &= t; p->f6 = m | v`), which moves it from
 *   n_refs 4 / span 10 to n_refs 8 / span 18 -- from QTY_CMP_PRI 8000 to 13333,
 *   far above `t`'s -- and `;; 1 regs to allocate: 35` and `35 in 4 / 36 in 3`
 *   DO NOT CHANGE. The dispositions are bit-identical. "Dies in 2 places" is
 *   the invariant; priority is irrelevant.
 *   So the 8 flip rows (all 11, one 12) are not an allocation regression at all:
 *   the flip only changes which operand of the two-operand `and` is the
 *   DESTINATION, costing 3 extra encodings at idx 5/6/8 with the allocation
 *   untouched. Both parks guessed this was "downstream of the same allocation
 *   question"; it is downstream of nothing -- it is a different pass.
 *
 * *** A CORRECTION TO BATCH 321'S CORRECTION 1, READ FROM THE SOURCE. ***
 * The brief states: "`global.c`'s allocno_compare INCLUDES floor_log2;
 * `local-alloc.c`'s qty_compare DOES NOT -- verified twice, both directions",
 * and warns that substituting one for the other turned a 1.3% margin into an
 * apparent factor of three. THE SECOND HALF IS WRONG IN THIS COMPILER.
 *   local-alloc.c:1496-1498 defines
 *       QTY_CMP_PRI(q) = (int)(((double)(floor_log2(qty[q].n_refs)
 *                        * qty[q].n_refs * qty[q].size)
 *                        / (qty[q].death - qty[q].birth)) * 10000)
 *   -- floor_log2 is THERE. There is exactly one definition (grep: 1496, used
 *   at 1504/1513/1544/1558, #undef at 1568), and the comment at 1485-1487 says
 *   the sameness is deliberate: the same algorithm in local- and global-alloc.
 *   THE REAL DIFFERENCE between the two formulae is the DENOMINATOR:
 *       local  (local-alloc.c:1498)  /  (qty[q].death - qty[q].birth)   a SPAN
 *       global (global.c:609/613)    /  allocno[v].live_length          a COUNT
 *   and where `size` multiplies in. For SImode (size 1) they are the same
 *   number. The `;; N regs to allocate` discriminator is still useful -- it
 *   tells you WHICH allocator decided -- but NOT because one lacks floor_log2.
 *
 * THE NEW RUNG FOR THIS BODY'S REMAINING RENAME (idx 3,4,5,6): REGMOVE.
 * The ROM has `and r1, r3` -- the AND's result lives in `v`'s register, the
 * mask's register dies, and mask2 reuses it. We emit `ands r3, r1`. That is not
 * reload, not sched2 and not the allocator. It is pass .15, regmove, rewriting
 * the two-address destination. Same insn, two consecutive dumps of THIS body:
 *
 *   .13.combine: (insn 21 (set (reg/v:SI 33) (and (reg/v:SI 33) (reg:SI 36))))
 *                 REG_DEAD (reg:SI 36)          dest = 33 = `v`  <- THE ROM
 *   .15.regmove: (insn 21 (set (reg:SI 36)    (and (reg/v:SI 33) (reg:SI 36))))
 *                 REG_DEAD (reg/v:SI 33)       dest = 36 = the mask pseudo
 *
 * regmove swapped the destination AND the REG_DEAD note -- which is precisely
 * why the mask's register stays live and the second mask has to take r1. The
 * second AND (insn 26) was left alone. Combine had it RIGHT; regmove undid it.
 * That is where the next pass on this function should aim, and it is a rung
 * this project has not previously named on any target.
 *
 * TRIED AND MEASURED WORSE on the rename, all 8 flavours of "make `v` the
 * destination again" (none reached idx 3):
 *   a third variable `w = v & 0x1ff`                            10
 *   `w =` with `t = p->f6` after                                10
 *   `w = 0x1ff & v` (operands written the other way)            10
 *   `v &= 0x1ff` with t's mask named, `v|t`                      8  (same shape)
 *   literal first mask + named second, `v|t`                     8  (same shape)
 *   `v &= 0x1ff` before `t = p->f6`                             11
 *   `t &= ...` before `v &= ...`                                11
 *   both masks named and both set up front                      14  (pushes r5)
 * Older rows from the parks, all reproduced: plain literals on the OLD
 * halfword-cast body 18; named pointer to +6 costs r5, 18; two named u32 masks
 * 14; one reassigned u32 mask 13; declaration order byte-identical either way;
 * flip + two separate masks 13 at dsize 0 by pushing r5 (the size coincidence).
 * The six mask/operand TYPE rows from 801c154.c (masks u16/s16 x t u16/u32 etc.)
 * were not re-run; its narrow-store argument for why they cannot produce a
 * narrow pool load is unaffected by anything above.
 *
 * STATUS: OPEN at 8. Placement half bounded by the re-measured corpus floor of
 * 23 instructions against this function's 14. Rename half now half-solved, with
 * the remaining decision localised to ONE regmove rewrite of ONE insn.
 *
 * NOTE ON THE BODY BELOW: it is NOT the parks' body. Both parks' (identical)
 * body also reads 8 and its residue is idx=[1,2,3,4,11,12]; it is preserved in
 * git at src/non_matching/rom_15000/801c154.c before this commit, and its
 * distinguishing feature -- one reassigned named mask -- is exactly LEVER 2
 * above, documented rather than lost.
 */
#include "gba/types.h"

struct S { u8 pad_00[4]; u8 f4; u8 pad_05; u16 f6; };

extern void Func_8003dec(struct S *p, s32 n);

void Func_801c154(struct S *p, u32 v, u32 b)
{
    u32 t;

    t = p->f6;
    v &= 0x1ff;
    t &= 0xfffffe00;
    p->f6 = v | t;
    p->f4 = b;
    Func_8003dec(p, 0xfc);
}
