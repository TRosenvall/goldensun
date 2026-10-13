/* ======================================================================
 * BATCH 348, BRIEF B.  STILL 16 OF 140 -- NO IMPROVEMENT, BUT THE PARK'S
 * PLATEAU IS NOT WHAT IT LOOKS LIKE: ***TWENTY-TWO OF ITS RECORDED ROWS ARE
 * ONE PROGRAM REACHED TWENTY-TWO WAYS.***  THREE MECHANISM ROUTES REFUTED.
 * ======================================================================
 *
 * RE-DERIVED, all four length dimensions: **16 differing STREAM encodings of
 * 140**, 121/121 16-bit instructions, 17/17 32-bit entries, **316/316 bytes**,
 * first differing entry at index 29.  Relocations agree (`_divsi3_RAM` /
 * `__divsi3` is ONE symbol and objcmp says so).  A TRUE DISTANCE, and the
 * park's figure reproduced exactly before any edit.
 *
 * *** THE BATCH-348 SELECTION CRITERION SAID THIS PARK HAD "NO RECORDED BATCH
 * WORK AT ALL" AND THAT "THE CHEAP ANSWERS MAY STILL BE HERE".  IT CARRIES A
 * FULL BATCH-341 BRIEF B WRITE-UP.  The criterion's query is not reading park
 * headers; see scratch_elev/b348/B/FINDINGS.md. ***
 *
 * ---- 1.  THE SPELLING PLATEAU IS BIT-IDENTICAL, MEASURED ----
 *
 * I crossed the SIX island-A spellings with the SIX island-B spellings (36
 * variants -- one-at-a-time is the measured top obstacle, so the crossing was
 * the point).  All 36 read **16 of 140, instructions 121/121, first at index
 * 29, relocations clean** -- identical in every column.  That is the
 * signature the brief warns about, so I checked it: compiling
 * `a0_b0` / `a2_b4` / `b_uns` and diffing the `.s` files gives
 *
 *     2c2
 *     < 	.file	"a0_b0.c"
 *     > 	.file	"a2_b4.c"
 *
 * and nothing else.  **The assembly is byte-identical apart from the `.file`
 * directive.**  gcc canonicalises both the lerp expression and the four-term
 * OR chain, so every re-ordering, re-grouping and re-parenthesisation of
 * either island -- the park's "every order of the four OR terms", batch 341's
 * five extra re-groupings, `0x80 << 23` spelled out, the constant first, the
 * two halves parenthesised, `-L38 + L21`, `t *` on the left, `u16` vs `short`
 * on the store -- **is the same program**.  Twenty-two rows, one measurement.
 * DO NOT ADD ANOTHER SPELLING TO THAT LIST; it cannot measure anything.
 *
 * ---- 2.  THE ANCHOR AXIS IS CLOSED, AND IT IS WIDER THAN THE PARK SAID ----
 *
 * The park says "All 18 anchor placements inert or worse (best 17)".  There
 * are **24** statement boundaries, not 18 (the three nested blocks contribute
 * 17 of them).  I swept all 24 singles AND all 276 PAIRS -- 300 variants, zero
 * compile failures.  Singles: best **17**, 14 of them at 17.  **PAIRS: best
 * 18.**  Nothing ties 16, let alone beats it.  The anchor axis is closed at
 * depth 2 here, which matters because the shared brief's standing note is that
 * the axis is NOT separable and two anchors can pay where neither does alone
 * -- that is true elsewhere and FALSE here, measured.
 *
 * ---- 3.  BOTH PRIORITY ROUTES ON THE .L38 ADDRESS ARE REFUTED ----
 *
 * Batch 341 left the question as "does the .L38 address die at the
 * sign-extend", a local-alloc tie, with the way in being "give the address a
 * reference the source does not currently place there".  local-alloc ranks by
 * QTY_CMP_PRI (local-alloc.c:1496-1498):
 *
 *     ((double) (floor_log2 (qty[q].n_refs) * qty[q].n_refs * qty[q].size)
 *      / (qty[q].death - qty[q].birth)) * 10000
 *
 * -- shorter-lived wins, so lengthening the .L38 address's range lowers its
 * priority and should stop it taking r2 ahead of the .L21 VALUE.  There are
 * exactly two ways to lengthen a range and I measured both:
 *
 *   EARLIER BIRTH -- `s38 = (short *)L38;` hoisted OUT of the `if (n != 0)`
 *   block, at four positions in the entry block, crossed with three ways of
 *   using it inside (addend only / subtrahend only / both), 12 variants:
 *   **26, 35 and 42**, never better.  Instruction count stays 121 in all 12,
 *   so this is not a length effect -- hoisting the pool load out of the block
 *   simply moves the whole entry block.  Same for the .L21 address (36, 51)
 *   and for both together (**57**).
 *
 *   LATER DEATH -- the addend read as its OWN statement after the
 *   subtraction, which is the one position the park never generated (its
 *   "addend last" row puts the `ldrh` after the `__divsi3` CALL, which ends
 *   the basic block).  Eight shapes, `int` and `short`, naming the
 *   difference / the product / the quotient, with and without a named addend:
 *   **26, 29, 41, 75, 104, 105, 106, 109**.  Four of them also change the
 *   instruction count (119, 120, 122, 123), so they are not even comparable.
 *
 * >> CONCLUSION, AND IT IS A BOUND WITH EVIDENCE ATTACHED: no range-length
 * >> edit reaches island A.  The park's circularity is real -- the only RTL
 * >> reference that would lengthen the address is the addend read itself, and
 * >> every source position for it is either before the extend (where we
 * >> already are) or after the call (which the park measured at 19).  Island A
 * >> is not reachable from the lerp statement.
 *
 * ---- 4.  WHAT I ADD TO THE RESIDUE READING ----
 *
 * ISLAND A, idx 29 and 32-41, TEN encodings.  The ref/ours register map, which
 * is the whole of it:
 *      .L21 address   r3  |  r3      <- THE SAME
 *      .L21 value     r2  |  r3      <- ours writes IN PLACE
 *      .L38 address   r1  |  r2
 *      .L38 value     r3  |  r2      <- ours writes IN PLACE
 * Both in-place writes are the same tie.  With REG_ALLOC_ORDER giving
 * r3, r2, r1, r0 (arm.h:989, {3,2,1,0,12,14,4,...}; r12/r14 are not LO_REGS),
 * the ROM's order of allocation was .L21addr, .L21val, .L38addr -- i.e. the
 * .L21 VALUE took r2 BEFORE the .L38 address asked for it.  Ours allocates
 * .L38addr second.  So the deciding comparison is between the .L38 ADDRESS and
 * the .L21 VALUE, not between the two addresses, and both have n_refs small
 * enough that floor_log2 is 1 -- the ratio is doing all the work.
 *
 * ISLAND B, idx 96-101, SIX encodings.  Side by side (ref | ours):
 *      movs r4,#0x80  | movs r1,#0
 *      movs r1,#0     | ldrsh r3,[r3,r1]
 *      ldrsh r3,[r3,r1]| lsls r3,r3,#28
 *      lsls r4,r4,#23 | orrs r2,r3
 *      lsls r3,r3,#28 | movs r3,#0x80
 *      orrs r2,r4     | lsls r3,r3,#23
 *      orrs r2,r3     | orrs r2,r3
 * ONE constant, 0x40000000, synthesised as mov+lsl either way.  The ROM holds
 * it in **r4** -- the FIFTH low register in REG_ALLOC_ORDER -- so it is live
 * at the same time as the .L19 value in r3; ours builds it AFTER the .L19
 * value dies and reuses r3.  All four high registers are already committed
 * across this block (r8 = .L45/.L29 address, r9 = .L32 address, r10 = tile,
 * r11 = q) and r5/r6/r7 hold t / the addend / n, so r4 is the only register a
 * fifth simultaneously-live value could take.  **This is the same shape as
 * island A** -- ref materialises into a fresh register, ours reuses one about
 * to die -- which is why no spelling separates them.
 *
 * I also measured the .L19 term's own register: `(unsigned)*(short *)L19 << 28`
 * and `*(short *)L19 * 0x10000000` are INERT AT 16 (bit-identical), while
 * `(*(short *)L19 & 0xf) << 28` and `*(unsigned short *)L19 << 28` are **64
 * and ONE INSTRUCTION SHORT (120)** -- they delete the `ldrsh`, and the ROM
 * has one, so the signed halfword read is confirmed correct.
 *
 * >> NEXT AGENT: the only untried route left is the park's own -- make a FIFTH
 * >> value simultaneously live in each island so r4 is handed out.  That needs
 * >> a value whose range SPANS the island, and the function has no natural
 * >> candidate (`w19 = *(short *)L19;` read early is 61; the x/y read order
 * >> reversed is 58).  Nothing in the lerp statement or the OAM word reaches
 * >> it, which is measured above; the lever, if it exists, is in the ENTRY
 * >> BLOCK's four high-register commitments.
 *
 * MEASURED THIS ROUND, FULL LIST (all at 121/121 instructions unless noted):
 *   36 island-A x island-B crossings .................. all 16, BIT-IDENTICAL
 *   24 anchor singles ................................. best 17
 *   276 anchor pairs .................................. best 18
 *   .L38 address hoisted out of the block, 12 shapes .. 26 / 35 / 42
 *   .L21 address hoisted, 2 shapes .................... 36 / 51
 *   both hoisted, 2 declaration orders ................ 57
 *   .L38 + .L45 both hoisted .......................... 36
 *   addend/difference/product/quotient named, 8 shapes  26 / 29 / 41 / 75 /
 *                                                       104 / 105 / 106 / 109
 *   (unsigned) cast and `* 0x10000000` on the .L19 term  16, bit-identical
 *   `& 0xf` and u16 read on the .L19 term ............. 64, insn 120
 *   `w19` named and read before the `if` .............. 61
 *   x/y read order reversed ........................... 58
 */
/* OvlFunc_common1_1354 -- asm/overlays/common/common1_a_a_c_a.s (this function
 * alone, no data). Fresh in batch 288, written from scratch.
 *
 * 16 encodings of 140 differ (objcmp --func; sizes equal, 140/140).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_1354.c \
 *       asm/overlays/common/common1_a_a_c_a.s --func OvlFunc_common1_1354
 *
 * A sprite task: if a move is armed (.L32 = frame count) it lerps .L45/.L29
 * from .L38/.L42 toward .L21/.L48 by .L17/.L32 frames, then blinks one OAM
 * entry at .L41 for 14 of every 20 frames. `__divsi3` vs `_divsi3_RAM` is the
 * linker alias, already in all three overlay.ld files that link common.
 *
 * THE RESIDUE, two islands:
 *  1. The FIRST lerp only (the second is exact). ROM: `ldr r0,=L45 / ldr r1,=L38
 *     / ldr r3,=L21`, L21 read signed into r2, L38 signed into r3 with r0 as the
 *     ldrsh zero both times, and the plain `ldrh r6,[r1]` of L38 LAST. Ours
 *     reads L38 via r2 and does the ldrh first -- UNVERIFIED guess: the L38 address pseudo is
 *     shorter-lived in ours, so local-alloc gives it r2, not r1. Most of the 16.
 *  2. The OAM word: the ROM builds 0x40000000 in r4 (`mov r4,#0x80 / lsl r4,#23`)
 *     BEFORE the L19 ldrsh and ORs it in before L19<<28; ours ORs L19 first and
 *     builds the constant in r3 after. The rest.
 *
 * WHAT MOVED IT (56 -> 16):
 *  - q = .L41 as a local, assigned FIRST (the ROM keeps it in r11 and uses it
 *    both as the stmia base and as the call argument).
 *  - x and y (the raw .L45/.L29 reads) as locals read before `*p++ = 0`; the
 *    ROM loads both before the first stmia.
 *  - `(y - 8) | ((x - 8) << 16) | ...` -- y FIRST (51 -> 25 with the rest).
 *  - `(L21 - L38) * t` operand order (t in r0 for the mul).
 *
 * INERT: t as `short` / `++*(short *)L17` / `(*(short *)L17)++; t = ...` /
 * `+= 1`; x,y as short; every order of the four OR terms other than y-first;
 * an explicit int local for the difference (29, worse); the lerp as a static
 * inline function (56, worse); `(L21-L38)*t/n + L38` addend-last (19/21);
 * `-L38 + L21` (16, same); a `do {} while (0)` after lerp 1 (24, worse);
 * u16 vs short on the lerp store (same); q assigned after n or before the
 * if (21/25). A plain `int *p` gives the same stmia as `volatile int *`.
 *
 * ========================================================================
 * BATCH 341 BRIEF B -- FIGURE REPRODUCED, THE SIXTEEN ATTRIBUTED ENTRY BY
 * ENTRY, AND THE PARK'S "UNVERIFIED guess" REPLACED BY THE ACTUAL MECHANISM
 * (WHICH IS THE OPPOSITE WAY ROUND)
 * ========================================================================
 *
 * RE-DERIVED: **16 of 140**, ref 121 instructions against ours 121, 14 against
 * 14 thirty-two-bit entries, 316 bytes against 316, first differing entry at
 * index 29.  A TRUE DISTANCE.  Relocations agree except `_divsi3_RAM` /
 * `__divsi3`, which objcmp itself reports as ONE symbol.
 *
 * autosweep.py: both axes closed.  Flags tying at 16: -fno-gcse,
 * -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-loop-opt.
 * **-fno-rerun-cse-after-loop is 48 -- that cse is LOAD-BEARING here**; -O1 79,
 * -fno-schedule-insns2 70, -ffixed-r7 121.  All 18 anchor placements inert or
 * worse (best 17).
 *
 * THIS PIECE HOLDS ONLY THIS FUNCTION (tools/piece.py), so a landing installs
 * alone and **NO LINKER-SCRIPT WORK IS NEEDED** even though common is linked
 * into three overlays (overlays/rom_7db0c8, rom_7ddb88, rom_7e0928).  The piece
 * carries no `.lcomm` and no `.comm`, and six `.L` labels.
 *
 * ---- THE SIXTEEN, LISTED ----
 *
 * Two islands and nothing between them:
 *     idx 29, 32-39, 41   TEN entries -- the first lerp
 *     idx 110-115          SIX entries -- the OAM word
 * Both islands are the SAME SHAPE: **the reference materialises a value into a
 * FRESH register and ours reuses a register that is about to die.**
 *
 * ISLAND A, the two streams side by side (ref | ours), from index 29:
 *     ldr r1, =.L38        | ldr r2, =.L38
 *     ldr r3, =.L21        | ldr r3, =.L21
 *     mov r8, r0           | mov r8, r0
 *     mov r0, #0           | mov r1, #0
 *     ldrsh r2, [r3, r0]   | ldrsh r3, [r3, r1]
 *     mov r0, #0           | ldrh  r6, [r2]
 *     ldrsh r3, [r1, r0]   | mov r0, #0
 *     lsl r5, #16          | ldrsh r2, [r2, r0]
 *     sub r2, r3           | lsl r5, #16
 *     asr r5, #16          | sub r3, r3, r2
 *     ldrh r6, [r1]        | asr r5, #16
 *     mov r0, r5           | mov r0, r5
 *     mul r0, r2           | mul r0, r0, r3
 * Same thirteen instructions.  The reference puts the ADDEND's `ldrh` LAST,
 * after the subtraction; ours puts it between the two sign-extends.
 *
 * ---- THE PARK'S GUESS WAS BACKWARDS ----
 *
 * The park guessed "the L38 address pseudo is shorter-lived in ours, so
 * local-alloc gives it r2, not r1".  The dumps say the causation runs the other
 * way, and the deciding insn is the one the park did not look at:
 *
 *   In .19.flow2 -- i.e. BEFORE sched2, with hard registers already assigned --
 *   our stream is
 *       insn 68  r2 = .L38
 *       insn 70  r3 = .L21
 *       insn 74  r3 = sign_extend (mem:HI (r3))   [clobber r1]
 *       insn 79  r6 = mem:HI (r2)                 <- THE ADDEND, HERE
 *       insn 80  r2 = sign_extend (mem:HI (r2))   [clobber r0]
 *       insn 82  r3 = r3 - r2
 *   The addend's read (79) sits BEFORE the sign-extend (80).  That makes the
 *   .L38 ADDRESS PSEUDO DIE AT INSN 80, so local-alloc ties insn 80's
 *   destination to its dying source and the extend writes IN PLACE.  Writing in
 *   place then creates an ANTI-DEPENDENCE 79 -> 80 (80 clobbers 79's address
 *   register), **and that is what forbids sched2 from moving the addend read to
 *   where the reference has it.**  The reference's extend writes r3, not r1, so
 *   its address survives, there is no anti-dependence, and the `ldrh` is free to
 *   float down past the subtraction.
 *   The pattern permits it: `*thumb_extendhisi2_insn` (arm.md:3239-3243)
 *   constrains operand 0 as "=l" with no tie to operand 1; only the scratch is
 *   "=&l".  Nothing in the pattern forces the in-place form.
 *   Confirmed that scheduling is NOT the cause: .19.flow2 order is
 *   ... 68 70 74 79 80 82 83 ... and .23.sched2 is ... 68 70 66 74 79 80 62 82
 *   63 83 ..., so sched2 moved only insns 62/63 (the sign-extension of `t`) and
 *   66; it never touched the 74/79/80 order.
 *
 *   >> SO THE QUESTION IS "DOES THE .L38 ADDRESS DIE AT THE SIGN-EXTEND", AND
 *   >> IT IS A LOCAL-ALLOC TIE, NOT A LIVE-LENGTH RANKING.  To break it the
 *   >> RTL needs a use of that address AFTER the extend -- and the reference
 *   >> has exactly one: the addend read.  It is circular in our build and the
 *   >> way in is to give the address a reference the source does not currently
 *   >> place there.
 *
 * ISLAND B is the same shape with a constant: the reference builds 0x40000000
 * in **r4** (`mov r4, #0x80 / lsl r4, #23`) BEFORE the `.L19` sign-extend and
 * ORs it in; ours ORs `.L19` first and then builds the constant in r3, the
 * register `.L19` just vacated.  The spelling is already identical
 * (`0x80 << 23` either way); only the position differs.
 *
 * ONE MEASUREMENT THAT TIES BOTH ISLANDS TOGETHER AND IS WORTH THE NEXT
 * AGENT'S FIRST HOUR: **the reference uses r4 three times and our build uses it
 * ZERO times.**  REG_ALLOC_ORDER (arm.h, the `#define REG_ALLOC_ORDER` block)
 * is `{ 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ... }` and r12/r14 are not LO_REGS, so
 * in Thumb the effective order is r3, r2, r1, r0, **r4**, r5, r6, r7 -- r4 is
 * the FIFTH low register handed out.  The reference's build therefore had one
 * more value simultaneously live in these windows than ours does, in both
 * islands.  That is the single hypothesis to test: not the order of the OR
 * terms, and not the spelling of the store, but WHETHER A FIFTH LOW REGISTER IS
 * NEEDED.
 *
 * MEASURED EXACTLY INERT THIS BATCH (all 16 of 140, 121 instructions, no
 * relocation change) -- these are the rows the park's guess made look
 * promising, so they are recorded to stop them being re-run:
 *   the `.L38` address named in a `short *` local inside the block; the same
 *   local assigned before `t`; the destination `.L45` named in a local; the
 *   addend read as `*(unsigned short *)` in lerp 1 only and in both lerps;
 *   `++*L17` split into two statements; `n` re-read as `*(short *)L32` in the
 *   divisor; and FIVE re-orderings and RE-GROUPINGS of the OAM word that the
 *   park's "every order of the four OR terms" did not cover --
 *   `| ((L19 << 28) | 0x40000000)`, the constant first, the two halves
 *   parenthesised, and `0x80 << 23` spelled out.
 * MEASURED WORSE: both reads named in int locals (19); the addend last in
 * lerp 1 (19 -- this CONFIRMS the park's row, and the reason it cannot be the
 * reference's source is that it puts the `ldrh` after the `__divsi3` CALL,
 * which ends the basic block, so sched2 can never bring it back before the
 * multiply); the addend last in BOTH lerps (31); the `.L19` term first in the
 * OAM word (53); the destination pointer assigned before the `if` (52); both
 * address pointers named in both lerps (35); the OAM constant or the first
 * three OR terms named in a function-scope int (49 / 52, each adding a pool
 * word).
 *
 * NEXT: force a FIFTH simultaneously-live low register in each island, which is
 * the one thing both residues have in common and the one thing no spelling
 * tried so far does.  Do NOT vary the OR order or the pointer spellings again;
 * twenty-two rows across the park and this batch are a plateau.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

extern unsigned char L14[] __asm__(".L14");
extern unsigned char L17[] __asm__(".L17");
extern unsigned char L19[] __asm__(".L19");
extern unsigned char L21[] __asm__(".L21");
extern unsigned char L26[] __asm__(".L26");
extern unsigned char L29[] __asm__(".L29");
extern unsigned char L32[] __asm__(".L32");
extern unsigned char L38[] __asm__(".L38");
extern unsigned char L41[] __asm__(".L41");
extern unsigned char L42[] __asm__(".L42");
extern unsigned char L45[] __asm__(".L45");
extern unsigned char L48[] __asm__(".L48");
extern struct SpriteSlot gSpriteSlots[];
extern void __Func_8003dec(void *p, int n);

void OvlFunc_common1_1354(void)
{
    int tile;
    int n;
    int t;
    int c;
    volatile int *p;
    int x;
    int y;
    unsigned char *q;

    q = L41;
    tile = gSpriteSlots[*(short *)L14].vramOffset >> 5;
    n = *(short *)L32;
    if (n != 0) {
        t = (short)++*(unsigned short *)L17;
        *(short *)L45 = *(short *)L38
            + (*(short *)L21 - *(short *)L38) * t / n;
        *(short *)L29 = *(short *)L42
            + (*(short *)L48 - *(short *)L42) * t / n;
        if (t >= n)
            *(unsigned short *)L32 = 0;
        *(unsigned short *)L26 = 0;
    }
    c = (short)++*(unsigned short *)L26;
    if (c <= 13) {
        x = *(short *)L45;
        y = *(short *)L29;
        p = (volatile int *)q;
        *p++ = 0;
        *p++ = (y - 8) | ((x - 8) << 16) | 0x40000000 | (*(short *)L19 << 28);
        *p = tile | 0x400;
        __Func_8003dec(q, 0xff);
    } else if (c > 19) {
        *(unsigned short *)L26 = 0;
    }
}
