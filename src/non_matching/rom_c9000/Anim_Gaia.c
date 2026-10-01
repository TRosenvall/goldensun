/* Anim_Gaia (asm/rom_c9000/rom_e28f4_c_c_a.s:814, 0x080e302c, 936 ROM
 * instructions / 990 encodings) --
 * NON-MATCHING, 921 of 990 encodings differ.
 *
 * READ THE FIGURE CORRECTLY.  921 is objcmps PRODUCTION-FLAG number and it is
 * SATURATED: the instruction COUNT is 992 against 990, TWO OVER, so the figure
 * is not a distance and must never be ranked against another function.
 *
 *   SIZE IS EXACT -- 2188 bytes both sides, objcmp prints no SIZE line.
 *   COUNT IS 992 against 990 (two over).
 *   tools/aligncmp.py, reported SEPARATELY and never on the claim line:
 *       aligned-equal 579 of 990 = 58.5%,  534 differing/ins/del in 177 hunks
 *       (first candidate: 471 of 990 = 47.6%, 648 in 184 hunks)
 *   RELOCATIONS: 77 rows ours against 78 ref.  The SYMBOL SEQUENCE is right
 *   entry for entry apart from the two items in BLOCKERS below; every callee,
 *   every pooled data label, both `__divsi3` sites, all three `__umodsi3`, both
 *   `__modsi3` and every `_call_via` veneer is present, once, in the ROM
 *   order.
 *
 * SIZE-EXACT-WITH-COUNT-OVER IS AN ARITHMETIC COINCIDENCE HERE, AND THE
 * COINCIDENCE IS THE DIAGNOSIS.  We carry TWO instructions too many (+4 bytes)
 * and ONE literal-pool word too few (-4 bytes), and they cancel.  Do not read
 * the exact size as "one axis closed" -- it is two open defects that happen to
 * sum to zero.  The missing pool word is the ROM SECOND `gBuffer` entry (see
 * BLOCKERS 1).
 *
 * Verify with (the delivered park body, runnable as written):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Gaia.c \
 *     asm/rom_c9000/rom_e28f4_c_c_a.s --func Anim_Gaia
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Gaia.c \
 *     asm/rom_c9000/rom_e28f4_c_c_a.s Anim_Gaia
 * RECIPE PATH CORRECTED ON INSTALL: the delivered recipe named the agent's own
 * scratch_elev/b311b/ workspace, which is GITIGNORED and would have made this
 * park unverifiable the moment the workspace was cleaned.  Both lines now name
 * the installed .c and the tracked asm/ reference.  FOURTH instance of this
 * defect; see the install note in src/non_matching/ovl_7892c8/200888c.c.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py <this file>` reports the filename
 * and no rows -- PIN-FREE, no register pin, no barrier, no per-file flag
 * override, no fakematch.txt row.
 *
 * SPLIT SHAPE: TEXT-ONLY, THREE PARTS, AND NO NEW `.global` AT ALL.
 * `tools/split_s.py --dry-run asm/rom_c9000/rom_e28f4_c_c_a.s Anim_Gaia` writes
 *
 *     rom_e28f4_c_c_a_a.s   BaseAnim_RapidSlash                 802 lines
 *     rom_e28f4_c_c_a_b.s   Anim_Gaia                          1055 lines
 *     rom_e28f4_c_c_a_c.s   Func_80e38b8, Func_80e3908           88 lines
 *
 * and rewrites stage1.ld.  `tools/datacheck.py` reports NO data section for
 * this stem, so there is no TEXT/DATA split to do.  THE EXPORT LIST IS EMPTY,
 * and both halves of that were checked rather than assumed:
 *   - the seven data labels Anim_Gaia reads -- `.Leed7e`, `.Leed90`, `.Leed9a`,
 *     `.Leeda0`, `.Leeda3`, `.Leeda6`, `.Leedac` -- all live in
 *     asm/rom_c9000/rom_e28f4_c_c_c.s and are ALREADY `.global` there, in that
 *     files own `.global` block.  Plus `Data_edab0` (rom_eda78.s, `.incdata`,
 *     so global) and `Data_ede48`.
 *   - the only cross-reference inside this stem is `bl Func_80e38b8` at line
 *     741, which is in BaseAnim_RapidSlash, not in Anim_Gaia -- and
 *     `.thumb_func_start` in include/macros.inc expands to `.global \sym`, so
 *     every one of the four functions is already exported.
 * Verify `make compare` is green AFTER the split and BEFORE writing the .c,
 * as the recipe printed by split_s.py says.
 *
 * ================================================================
 * WHAT THIS FUNCTION IS
 * ================================================================
 *
 * The Venus-summon "Gaia" battle animation, 0xc0 frames, synchronous in the
 * shape docs/battle-animations.md describes: grab the three allocation-table
 * pointers, set up BG2 affine and blending, unpack two sprite sheets into the
 * state block, build two draw-2D blitters, seed two particle pools, then one
 * frame loop ending in `*(int *)(base + 0x7824) = 1; WaitFrames(1);`.
 * `FILE_VFX_GAIA` is `FILE_7b` in include/file_table.h, which is what names it.
 *
 * `iwram_3001eec` is read exactly as docs/battle-animations.md and
 * Anim_Ragnarok spell it: [0] the 0x782C state block (`base`), [1] the
 * destination render buffer (`ctx`, argument 0 of every blit), [2] the 0x302
 * sprite scratch (`gfx`, which is the buffer `Data_ede48` indexes -- that
 * pairing is the proof the three reads are assigned correctly).  The blitters
 * come from `gPtrs[0xb8/4]` and `gPtrs[0xbc/4]`, the third of the three idioms
 * that doc lists.
 *
 * ================================================================
 * THE FRAME IS THREE GREPS AND IT WAS THE FIRST THING DONE
 * ================================================================
 *
 *     sub sp, #0x48                     total 72 bytes
 *     add r7, sp, #0x30 / add r6, sp, #0x38     TWO aggregates
 *     (no `mov rX, sp`; both aggregates surface in the `add rX, sp` grep)
 *
 * The spill-slot map, scalars sorted DESCENDING, is the declaration list, and
 * ours now reproduces it ENTRY FOR ENTRY -- this was the single most valuable
 * structural check in the whole reconstruction:
 *
 *     slot   quantity                      ref accesses   ours
 *     0x2c   base   = iwram_3001eec[0]          27          28
 *     0x28   ctx    = iwram_3001eec[1]          12          12
 *     0x24   blitB  = gPtrs[0xbc/4]              5           6
 *     0x20   blitA  = gPtrs[0xb8/4]              6           7
 *     0x1c   gfx    = iwram_3001eec[2]           3           3
 *     0x18   xofs                               10          10
 *     0x14   yofs                                5           5
 *     0x10   loop.c hoisted `xofs + 0x40`      4           4
 *     0x0c   the k-loop `y`, and the -1          8           4
 *     0x08   loop.c hoisted `s * 2`            6           6
 *     0x30   scale (2 words, aggregate)        4 + 1       4 + 1
 *     0x38   pos   (4 words, aggregate)          --          --
 *     0x00   outgoing arguments 5 and 6     14 + 11     14 + 11
 *
 * AGGREGATE ORDER IS REVERSED, confirmed again: `scale` is declared BEFORE
 * `pos` and lands at the LOWER offset (0x30 against 0x38), because expand
 * assigns declared locals last-first and the frame grows downward.
 *
 * ================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (0) THE STARTING POINT, for scale.  The first candidate -- written straight
 *     off the disassembly with the bank vocabulary from Anim_Ragnarok and
 *     Anim_Unused_Fizz -- read SIZE 2172/2188, COUNT 985/990, aligned 471 of
 *     990 (47.6%) and a relocation symbol sequence already correct except for
 *     pool placement and four veneer registers.  The brief claim that a
 *     landed/parked sibling is the strongest evidence in this project held
 *     completely: every extern declaration, the `State`/`Part` struct shapes,
 *     the `(Type *)(base + 0xNNNN)` idiom and the `CopyFn copy = Func_8001af8`
 *     indirect-call idiom were all read off siblings, not guessed.
 *
 * (1) `base + 0x7828` IS NOT A NAMED LOCAL -- 4 BYTES OF FRAME, and it was the
 *     single biggest defect.  The first candidate hoisted `slot = (State **)
 *     (base + 0x7828)` into a local, exactly as Anim_Ragnarok does.  That buys
 *     an ELEVENTH spill slot and the frame goes to 0x4c; every slot reference
 *     in the function then reads 4 too high.  Writing
 *     `(*(State **)(base + 0x7828))->fNN` out at all nine sites puts the frame
 *     at 0x48 and took 471 -> 505 of 990 (47.6% -> 51.0%).
 *     THIS IS THE BANK RULE Anim_Ragnarok park states in words -- "the bank
 *     re-derive base+0x7828, never name it rule" -- and Ragnarok is the
 *     EXCEPTION to it, not the model.  Read that park region-4 note before
 *     copying its `slot` local into a new function.
 *     cse still keeps `base + 0x7828` in a register across `AnimStart` with no
 *     help: the value crossing the call is a pseudo-based ADDRESS, not a
 *     memory load, and the brief own discriminator says cse commons that
 *     free.
 *
 * (2) EVERY LOOP IN THIS FUNCTION IS `!=`, NOT `<` -- and it is uniform.
 *     Thirteen loops.  The ROM ends every one of them with `bne`, including
 *     the two whose bound is a register (`cmp r7, r1`, `cmp r8, r0`) and the
 *     frame loop (`cmp r2, #0xc0 / beq`).  `i < N` gives gcc 2.96 `cmp #N-1 /
 *     ble` or a COUNTDOWN (`sub r4, #1 / cmp r4, #0 / bge`), and the countdown
 *     form also changes which quantity loop.c picks as the giv -- in the two
 *     four-iteration blit loops `i < 4` produced a countdown AND lost the
 *     ROM separate `y` accumulator, while `i != 4` produced both the
 *     ascending counter and the accumulator.  Converting all thirteen was part
 *     of the change that took 505 -> 528 of 990.
 *     GENERALISABLE TELL, cheap: `grep -c ble\|blt\|bge\|bgt ` against
 *     `grep -c bne` on the references loop-closing branches BEFORE writing any
 *     loop.  A `bne` on a counter with a constant bound cannot come from `<`.
 *
 * (3) THE SPRITE ARRAY WANTS AN EXPLICIT BYTE-OFFSET ACCUMULATOR, NOT A
 *     SUBSCRIPT -- this is the brief "two quantities, opposite spellings"
 *     lever and here it decides WHICH quantity loop.c makes the giv.  The ROM
 *     keeps `0x77d8 + i*4` in r5 and RELOADS `base` from its slot at both
 *     uses, storing with the register-offset form `str r0, [r5, r1]`:
 *
 *         o = 0x77d8;
 *         for (i = 0; i != 0xb; i++) { ... *(void **)(base + o) = spr; ...
 *                                      o += 4; }
 *
 *     `((void **)(base + 0x77d8))[i]` and `*(void **)(base + (0x77d8 + i * 4))`
 *     BOTH fail the same way -- fold associates the add, loop.c eats the whole
 *     address, and you get one hoisted pointer giv and no base reload.  The
 *     teardown loop over the SAME array wants the opposite spelling: there the
 *     ROM has `ldmia r5!, {r0}` plus a countdown, the walking-pointer tell, so
 *     `pp = (void **)(base + 0x77d8); _DeleteSprite(*pp++);` is right.  Same
 *     array, same function, opposite spellings.
 *
 * (4) THE `& ~0xc` MASK MUST STAY INT-WIDE, AND THE PROOF IS THAT THE ROM
 *     HOISTS IT.  The ROM has `mov r4, #0xd / neg r4, r4 / mov r6, r4` in the
 *     sprite loops PREHEADER and `and r3, r6` inside.  Written as
 *     `q[9] = (q[9] & ~0xc) | 4;` fold distributes the narrowing convert over
 *     the AND, the mask becomes 0xf3, and 0xf3 is a ONE-INSTRUCTION constant --
 *     so loop.c does not hoist it and you get `mov r3, #0xf3 / and r3, r2`
 *     inside the loop with the operands the wrong way round as well.  Breaking
 *     the expression so the AND is not under the convert:
 *
 *         m9 = ~0xc;            -- once, before the loop
 *         v = q[9];  v &= m9;  q[9] = v | 4;
 *
 *     keeps -13, which costs `mov`+`neg`, which is cost 2, which is what makes
 *     loop.c move it.  THE MECHANISM IS THE PASS ORDER: loop runs BEFORE
 *     combine, so once the constant is a hoisted register the AND is reg-reg
 *     and combine can no longer narrow it.  Lose the hoist and you lose the
 *     width permanently.
 *
 * (5) `scale` IS A STRUCT ASSIGNMENT FROM `Data_edab0`, NOT TWO SUBSCRIPT
 *     STORES -- 557 -> 579 of 990 (56.3% -> 58.5%), the largest single move
 *     after the frame.  The ROM reads `Data_edab0[1]` BEFORE `[0]` and stores
 *     [0] then [1], both with DIRECT sp-relative stores, and only takes
 *     `&scale` later, at the `scale.b = scale.a` that follows the branch merge:
 *
 *         ldr r4, [r3, #4] / ldr r3, [r3] / str r3, [sp,#0x30] / str r4, [sp,#0x34]
 *
 *     Loads-both-then-stores-both IS `emit_block_move`.  Two subscript
 *     statements instead materialise `add r7, sp, #0x30` EARLY and route the
 *     second store through it, which also loses the `[sp, #0x34]` slot access
 *     entirely -- the slot map is what caught this, not the hunk list.
 *     `Data_edab0` is 8 bytes (rom_eda78.s `.incdata ... 0xedab0, 0xedab8`),
 *     which is the independent confirmation that the aggregate is 2 words.
 *
 * (6) BOTH `p->t` GUARDS FALL THROUGH INTO THE BODY, NOT INTO THE DECREMENT --
 *     528 -> 552 of 990 (53.3% -> 55.8%), together with (7).  `if (p->t != 0)
 *     { p->t--; } else { body }` puts the decrement inline and branches to the
 *     body; the ROM does the reverse at BOTH particle loops.  Write
 *     `if (p->t == 0) { body } else { p->t--; }`.  In the second loop the ROM's
 *     `beq .Le3690 / b .Le37c4` pair is the same source shape with the else-arm
 *     out of conditional-branch range.
 *
 * (7) `UpdateScreenShake`'s TWO ARMS WERE SWAPPED.  The ROM's fall-through arm
 *     is (2,2):  `if ((unsigned)(frame - 0x5a) > 0x46) UpdateScreenShake(2, 2);
 *     else UpdateScreenShake(8, 8);`.  Written with the `<= 0x46` test first
 *     you get `bhi` and the arms in the other order -- 6 encodings.
 *
 * (8) DECLARATION ORDER DOES SET THE RELOAD SPILL-SLOT ORDER, AND THIS IS A
 *     NARROWING OF THE RECORDED "DECLARATION ORDER -- INERT" FINDING.
 *     docs/band-800plus.md section 3 records declaration order as inert; that
 *     was measured on constants placed by LOCAL-alloc, and it does NOT cover
 *     reload spill slots.  Here the first candidate put `gfx` at 0x24 and
 *     `blitB` at 0x1c where the ROM has them the other way round -- 7
 *     encodings' worth of wrong `[sp, #imm]` fields.  Declaring
 *     `base, ctx, blitB, blitA, gfx` in that order -- the ROM's slot map read
 *     downward -- swaps them to the ROM's assignment.
 *     THE TRAP, AND IT COST A WASTED CONCLUSION: the aligned figure moved by
 *     TWO encodings (557 -> 559) and the hunk count by one, so on the figures
 *     alone the change reads as noise and the first pass wrote it up as inert.
 *     The SLOT MAP is what shows it worked.  This is the brief's "figures that
 *     lie" rung in a new form -- carry the slot map as a column, not just size,
 *     count and aligned.  (A separate earlier attempt at the same reorder was
 *     a no-op because the edit's anchor string did not match; the lesson there
 *     is to re-read the declaration block after any scripted edit.)
 *
 * (9) THE MASK CONSTANT IN THE PALETTE LOOP IS TWO QUANTITIES, NOT ONE, AND
 *     THE OPERAND ORDER IS THE TELL.  The ROM has a loop-invariant 0x1f
 *     (`and r0, r6`, `and r1, r6`, value in rd) and a SECOND 0x1f materialised
 *     inside the body (`mov r4, #0x1f` then `and r4, r2`, CONSTANT in rd).
 *     Thumb's AND is two-address, so `x & 0x1f` puts x in rd and `0x1f & x`
 *     puts the constant in rd -- a constant in rd cannot be the preserved
 *     loop-invariant copy, which is exactly why the ROM needs two.  Writing the
 *     red channel as `(0x1f & c) - 8` and naming the other two's mask
 *     `int mask = 0x1f;` reproduces both quantities.
 *     MEASURED, AND HONESTLY: 558 against 557 aligned, 190 hunks against 186 --
 *     a WASH.  The structure is the ROM's; the figures do not pay for it, and
 *     the named `mask` costs a call-saved register for the whole function,
 *     which is why the delivered body uses the plain literal.  Recorded
 *     because the operand-order tell is reusable even though this instance is
 *     not worth taking.
 *
 * ================================================================
 * SPELLINGS MEASURED AND INERT -- UNTESTED, NOT DISPROVED, BUT DO NOT RESPEND
 * ================================================================
 *
 * - `Leeda0[i] * Leeda3[i]` against `Leeda3[i] * Leeda0[i]`: BYTE-IDENTICAL
 *   objects.  Both give `mul r3, r2` with the same register assignment, so the
 *   brief's "Thumb mul puts the SECOND source operand in the destination" rule
 *   cannot be used to READ an operand order back out of this site -- fold
 *   normalises the commutative multiply before any of it matters.  Consistent
 *   with docs/band-800plus.md section 8's "do not re-sweep constant spellings".
 * - A PER-REGION blitter local in the 0x58..0x9f arm (`DrawFn fa = blitA; ...`,
 *   the Anim_Unused_Fizz one-per-loop lever): BYTE-IDENTICAL.  Copy
 *   propagation erases the distinction when the outer variable is itself a
 *   plain local with no call between the read and the use.  THE BOUND ON THAT
 *   LEVER IS NOW KNOWN: it pays when the value otherwise comes from a GLOBAL
 *   across a call (Fizz, 630 -> 139), not when it is already a pseudo.
 * - Dropping the redundant `if ((*slot)->f14 != 0)` wrapper around the
 *   knockback loop: 557 against 557.  It does move `mov r7, #0` to before the
 *   `cmp r3, #0`, which is the ROM's order, so it is kept on structure.
 * - Declaration order of the three table pointers: see lever (8) -- inert on
 *   the figures, NOT inert on the object.
 *
 * ================================================================
 * BLOCKERS, EACH ATTRIBUTED TO A PASS
 * ================================================================
 *
 * 1. THE MISSING LITERAL-POOL DUMP, and it owns the relocation-row difference
 *    AND the -4 bytes that mask the +2 instructions.
 *    `final.c`'s pool-dump heuristic.  The ROM dumps a pool IN THE MIDDLE of
 *    the palette loop -- the `b .Le3154` at 0x080e312e jumps over
 *    `.Le3130: .word 0x1f` and five relocated words (`.Leeda6`, `gBuffer`,
 *    `_FILE_7b`, `_FILE_7c`, `Func_8001af8`) -- and `gBuffer` is therefore
 *    pooled TWICE, once in that dump and again in the dump at `.Leed90`.  That
 *    is why the ROM has 78 relocation rows and we have 77, and why its byte
 *    count is 4 higher than our instruction count alone would predict.
 *    gcc emits that dump when the distance from a `ldr rX, [pc, #imm]` to its
 *    word would otherwise exceed the 1020-byte Thumb range, so it is a
 *    CONSEQUENCE of the two surplus instructions, not an independent defect.
 *    DO NOT CHASE IT DIRECTLY.  Close item 2 and this should close with it --
 *    and the relocation OFFSETS will all move at the same time, which is why
 *    the offsets are not counted as residue here.
 *
 * 2. THE TWO SURPLUS INSTRUCTIONS, both in register shuffling, both the same
 *    mechanism: `global.c`/`reload1.c` give us ONE MORE hard register than the
 *    ROM had, in two regions, so we keep a value the ROM rematerialised.
 *    (a) The nine-way copy loop's preheader.  The ROM:
 *        `mov r14, r2 / ... / mov r10, r14 / mov r3, r14 / ldrb r3, [r3] /
 *         mov r12, r3 / ... / mov r1, r12` -- the `.Leed90` base in r2 with a
 *        high-register backup in r10, restored with `mov r2, r10` after the
 *        inner loop clobbers r2.  Ours keeps the base in r12 and walks r14,
 *        which needs one extra `mov r12, r14`.
 *    (b) The 0x58..0x9f blit arm.  The ROM loads `blitA` ONCE into r10 and
 *        `blitB` ONCE into r8 and calls `_call_via_r10` / `_call_via_r8` twice
 *        each; we load r4 at all four call sites (`_call_via_r4` x4).  That is
 *        the +1 on slot 0x20 and the +1 on slot 0x24 in the table above, and it
 *        is the whole remaining veneer-register difference together with
 *        `_call_via_r6` against `_call_via_r5` for the other six blits.
 *        A per-region local does NOT reach it (measured byte-identical, above);
 *        what differs is which allocno wins r8/r10, and the brief's
 *        initialise-at-the-declaration lever is the untried move here -- lower
 *        a competing allocno rather than try to raise these two.
 *    NOT sched1: sched1 does not run, so none of this is scheduling.
 *
 * 3. `copy = Func_8001af8` IS ONE INSTRUCTION EARLY, AND THIS ONE IS A KNOWN
 *    DEAD END.  The ROM has
 *        mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80
 *    and we have the `ldr` after the `lsl`.  This is EXACTLY the residue
 *    src/rom_c9000/rom_cc5d8_a_a_b.c (Anim_UnleashIntro) records, in mirror
 *    image, at 2 of 80 twice; that park needed `register ... __asm__("r0")`
 *    pins to reach it and says seven spellings had already failed because every
 *    one of them left the choice to gcc.  A pin has measured worse ten times
 *    and is not a landing route, so this is left open deliberately.  2
 *    encodings.
 *    What DID pay: assigning the GetFile result to a local first, so that
 *    `copy = Func_8001af8` falls after `bl GetFile` instead of before it.  That
 *    alone moved the veneer from `_call_via_r5` to the ROM's `_call_via_r3`,
 *    because the pointer no longer has to survive a call.
 *
 * 4. THE POOLED 0xcc / 0xaa / 0x1f -- *** NOT A BLOCKER AND NOT 3 ENCODINGS.
 *    RETRACTED IN BATCH 313: IT COSTS ZERO. ***
 *
 *    `ldrh rX, <pool>` against `ldr rX, <pool>` ON THE SAME POOL WORD is a
 *    MNEMONIC-TEXT ARTEFACT WITH IDENTICAL ENCODINGS.  Thumb-1 has no
 *    PC-relative halfword load at all, so GAS must encode a `ldrh rX,<label>`
 *    pool reference as the same word load the reference listing prints as `ldr`.
 *    The two listings differ; the two objects do not.
 *
 *    Measured independently by two agents in batch 313.  One proved it through a
 *    clean aligncmp band (reference indices 42-67 aligned-equal across the
 *    disagreeing mnemonics) and separately confirmed `ldrsh` 12/12 and `lsl`
 *    60/60 exact, so carrier width cannot explain any delta.  The other found
 *    `ldrh` 20 in its candidate against 6 in its reference, where the six DATA
 *    loads match one-for-one and the other fourteen are pool loads -- and closed
 *    it arithmetically: that candidate is +7 encodings in total, so fourteen
 *    extra instructions is impossible.
 *
 *    So the 3 encodings this section claimed belong somewhere else, and the
 *    section's own caution -- "do not write this up as a new mechanism without
 *    reproducing it in a minimal probe" -- was right for a better reason than it
 *    knew: there is no mechanism here to reproduce.
 *
 *    *** AND THE GENERAL CONSEQUENCE, which is the valuable half: A RUNG-8
 *    PER-OPCODE HISTOGRAM MUST BE TAKEN OVER ENCODINGS (objdump -dz), NEVER OVER
 *    LISTING TEXT. ***  This is the second listing-level aliasing found, after
 *    `.call_via` (whose macro line expands to `mov r12,pc` + `bx`, so raw text
 *    under-counts both by one per site).  A histogram over text will invent
 *    opcode deltas that no object carries.
 *
 *    The original reading is kept below for the record, since its machine-
 *    description reasoning is sound and only its conclusion was wrong.
 *
 *    THE POOLED 0xcc / 0xaa / 0x1f, 3 encodings, UNEXPLAINED AND FLAGGED AS
 *    SUCH.  `REG_BG2PA = 0xcc` gives us `ldrh r3, <pool>` where the ROM has
 *    `ldr r3, <pool>` -- same pool word (`.word 0xcc`), different load width.
 *    The halfword load is `*thumb_movhi_insn` having no immediate form and
 *    calling `force_const_mem (HImode)`; the ROM's word load means its value
 *    reached the `strh` as SImode.  An `int` carrier does not produce it (an
 *    SImode `const_int 204` is `mov r3, #0xcc`, not a pool load), so the
 *    `int`-carrier table in docs/elevation.md does not cover this case and this
 *    is NOT a counterexample to it -- the carrier question there is about which
 *    pool load comes FIRST, and here the ADDRESS comes first on both sides.
 *    The same unexplained word-load-of-a-mov-able-constant appears at 0x1f.
 *    DO NOT write this up as a new mechanism without reproducing it in a
 *    minimal probe; three constants in one function is a hypothesis, not a
 *    finding.
 *
 * ================================================================
 * NOTES ON THE READING, so the next pass does not re-derive them
 * ================================================================
 *
 * - `0x74 - Leeda3[s]` and `xofs - Leeda0[s] + 0x40` are written out at the
 *   blitA site while blitB takes loop.c's hoisted `xofs + 0x40` from slot 0x10.
 *   There is NO source local for `xofs + 0x40`: it is loop-invariant in the
 *   frame loop, so loop.c hoists one copy to the frame-loop preheader (slot
 *   0x10) and a SECOND copy into each four-iteration inner loop's own
 *   preheader (r8), because inner loops are processed first.  One spelling,
 *   three registers.  Do not name it.
 * - `(frame / 4)` then `if (s > 2) s = (s & 1) + 1` is the ROM's own clamp, not
 *   a mis-read: `asr` with the `+3` bias is signed division, and the `(s & 1)
 *   + 1` arm folds everything above 2 onto {1, 2}, which is exactly the three
 *   entries `.Leed9a` / `.Leeda0` / `.Leeda3` have (6 bytes = 3 halfwords, and
 *   3 bytes each).  Every table's length confirms its index range: `.Leed7e` 18
 *   bytes = 9 halfwords for s in 0..7, `.Leed90` 10 bytes for i in 0..8,
 *   `.Leedac` 6 bytes for 6 iterations, `.Leeda6` 6 bytes = [2][3] indexed
 *   `f4 * 3 + f18` as a SIGNED char (`ldrsb`).
 * - The `ldrsh r2, [r5, r3]` with `r3 = 2` and `r6 = 6` are the HIGH halfwords
 *   of `Part.f0` and `Part.f4`, i.e. the 16.16 integer parts, spelled
 *   `((short *)&p->f0)[1]`.  Thumb-1 has no immediate-offset LDRSH, which is
 *   why the offset is in a register -- that is forced, not a lever.
 *   The physics then reads the SAME word as an int (`p->f4 -= p->f10`), so the
 *   struct is `int` fields with a cast, not halfword fields.
 * - `(frame / 2 + j) % 0xb` is compared against -1 and the call is skipped when
 *   equal.  `frame` is never negative so that arm is unreachable; it is in the
 *   ROM and is reproduced as written.  The -1 is shared with `p->t = -1`,
 *   which is why cse parks it and reload spills it at slot 0x0c.
 * - `Random() % 0x60` and friends call `__umodsi3`, so the operand reaches the
 *   divide unsigned; `(u32)Random() % 0x60` is how that is spelled here.  The
 *   two `/ 3` and the two `% 0xc` / `% 0xb` are signed (`__divsi3`,
 *   `__modsi3`).
 * - The nine-way copy loop reads `Leed90[i]` TWICE in the ROM, once through
 *   loop.c's walking pointer (for the zero guard and `src +=`) and once as
 *   base+index (for the inner bound).  ONE source expression produces both:
 *   the inner use sits inside the zero guard, so it is not executed
 *   unconditionally and loop.c will not hoist it.  This is the brief's
 *   "a loop bound sometimes must be re-derived" shape arising for free -- no
 *   local, no second spelling.
 *
 * ================================================================
 * NEXT MOVES, RANKED
 * ================================================================
 *
 * 1. Blocker 2(b): get `blitA`/`blitB` into r10/r8 across the 0x58..0x9f arm.
 *    Try the initialise-at-the-declaration lever on a COMPETING quantity --
 *    `w30` and `xofs` are the candidates -- rather than on the blitters
 *    themselves.  Carry the slot-0x20 and slot-0x24 access counts as columns;
 *    6 and 5 is the target and they are a cleaner signal than aligned.
 * 2. Blocker 2(a): the `.Leed90` base / backup pair in the copy loop.
 * 3. Blocker 1 should fall out of 1 and 2; re-check the relocation ROW COUNT
 *    (78) rather than the offsets.
 * 4. Blocker 4 only as a minimal `tools/tryc.py` probe, never inline here.
 *
 * Avoid rewriting the loop bounds, the `base + 0x7828` spelling, the `scale`
 * struct assignment or the sprite-array offset accumulator: each is measured
 * and each is load-bearing.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int f0, f4, f8, fc, f10, f14, t;
} Part;

typedef struct {
    int a, b;
} Scale;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern Scale Data_edab0;
extern unsigned short Leed7e[] __asm__(".Leed7e");
extern unsigned char  Leed90[] __asm__(".Leed90");
extern unsigned short Leed9a[] __asm__(".Leed9a");
extern unsigned char  Leeda0[] __asm__(".Leeda0");
extern unsigned char  Leeda3[] __asm__(".Leeda3");
extern signed char    Leeda6[] __asm__(".Leeda6");
extern unsigned char  Leedac[] __asm__(".Leedac");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int Random(void);
extern void *_CreateSprite(int id);
extern void _Sprite_SetAnim(void *spr, int anim);
extern void _DeleteSprite(void *spr);
extern void _UpdateSprite(void *spr, int *pos, Scale *scale, int style);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int flags, int e);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Gaia(State *d)
{
    Scale scale;
    int pos[4];
    void **tbl;
    void **pp;
    unsigned char *base;
    void *ctx;
    DrawFn blitB;
    DrawFn blitA;
    unsigned char *gfx;
    CopyFn copy;
    unsigned char *src;
    volatile u16 *pal;
    void *gfx2;
    unsigned char *q;
    int v, m9, o;
    int w30 = 0x30;
    Part *p;
    int off;
    int xofs, yofs;
    int frame;
    int i, j, k, n, nm, s;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    gfx = (unsigned char *)tbl[2];
    *(State **)(base + 0x7828) = d;
    AnimStart(0);
    if ((*(State **)(base + 0x7828))->f18 == 0) {
        REG_BG2PA = 0xcc;
    } else if ((*(State **)(base + 0x7828))->f18 == 1) {
        REG_BG2PA = 0xaa;
    }
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        xofs = 8;
        yofs = 0x28;
        if ((*(State **)(base + 0x7828))->f18 != 0) {
            yofs = 0x24;
            if ((*(State **)(base + 0x7828))->f18 != 1) {
                yofs = 0x28;
            }
        }
    } else {
        xofs = -0x10;
        yofs = -0xc;
    }
    REG_BG2X = Leeda6[(*(State **)(base + 0x7828))->f4 * 3
                      + (*(State **)(base + 0x7828))->f18] << 8;
    LoadVFXFile(FILE_7b, gBuffer, 1, 0);
    gfx2 = GetFile(FILE_7c);
    copy = Func_8001af8;
    copy((volatile u16 *)0x5000000, gfx2, 0x80);

    pal = (volatile u16 *)0x5000002;
    for (i = 0; i != 0x3f; i++) {
        int c = *pal;
        int t = c << 16;
        int b = (((u32)t >> 26) & 0x1f) - 8;
        int g = (((u32)t >> 21) & 0x1f) - 8;
        int r = (0x1f & c) - 8;
        if (b < 0) b = 0;
        if (g < 0) g = 0;
        if (r < 0) r = 0;
        *pal = (b << 10) | (g << 5) | r;
        pal++;
    }

    off = 0;
    src = (unsigned char *)gBuffer;
    for (i = 0; i != 9; i++) {
        for (j = 0; j != 0x20; j++) {
            for (k = 0; k != Leed90[i]; k++) {
                base[off++] = src[k];
            }
        }
        src += Leed90[i];
    }
    for (i = 0; i != 0x20; i++) {
        for (j = 0; j != 3; j++) {
            for (k = 0; k != 0x30; k++) {
                base[off++] = src[k];
            }
        }
        src += 0x30;
    }
    for (k = 0; k != 0x3f0; k++) {
        base[off++] = *src++;
    }
    for (i = 0; i != 3; i++) {
        nm = Leeda3[i] * Leeda0[i];
        for (k = 0; k != nm; k++) {
            base[off++] = *src++;
        }
    }
    LoadVFXFile(FILE_73, gfx, 0, 0);

    m9 = ~0xc;
    o = 0x77d8;
    for (i = 0; i != 0xb; i++) {
        void *spr;
        spr = _CreateSprite(0x186);
        *(void **)(base + o) = spr;
        if (spr != 0) {
            *((unsigned char *)spr + 0x26) = 0;
            _Sprite_SetAnim(spr, i / 4);
            q = (unsigned char *)*(void **)(base + o);
            v = q[9];
            v &= m9;
            q[9] = v | 4;
        }
        o += 4;
    }

    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    blitA = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    REG_BLDCNT = 0x3f46;
    REG_BLDALPHA = 0x1010;
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    blitB = (DrawFn)gPtrs[0xbc / 4];
    StartTask(Task_BlitAnim, 0x90 << 3);

    p = (Part *)(base + 0x7198);
    for (i = 0; i != 0xb; i++) {
        p->f0 = (Random() & 0xf) + 0x58;
        p->f4 = 0x80;
        p->f8 = (Random() & 0xf) + 2;
        p->fc = i;
        p->f10 = 1;
        p->f14 = 0x80 << 8;
        p->t = 0x2c - i * 4;
        p++;
    }
    p = (Part *)(base + 0x7240);
    for (i = 0; i != 6; i++) {
        p->f0 = Leedac[i];
        p++;
    }
    p = gBuffer;
    for (i = 0; i != (0x80 << 1); i++) {
        p->f0 = ((Random() & 0x3f) + xofs + 0x20) << 16;
        p->f4 = ((Random() & 7) + 0x60) << 16;
        p->f10 = ((Random() & 0x3f) + 0x20) << 13;
        p->t = Random() & 0x1f;
        p++;
    }
    REG_BG2CNT = 0x785;
    *(int *)(base + 0x77a8) = 0xfa;

    for (frame = 0; frame != 0xc0; frame++) {
        if (frame == 0) _PlaySound(0xd4);
        if (frame == 0x28) _PlaySound(0x8d);
        if (frame == 0x60) _PlaySound(0x91);
        if (frame == 0x78) _Func_80bd7dc(0x86);

        if (frame <= 0x51) {
            s = frame / 4;
            if (s > 2) s = (s & 1) + 1;
            blitA(ctx, base + Leed9a[s], xofs - Leeda0[s] + 0x40,
                  0x74 - Leeda3[s], Leeda0[s], Leeda3[s]);
            blitB(ctx, base + Leed9a[s], xofs + 0x40,
                  0x74 - Leeda3[s], Leeda0[s], Leeda3[s]);
        }
        if ((unsigned)(frame - 0xc) <= 0x4b) {
            s = (frame - 0x40) / 3;
            if (s < 0) s = 0;
            if (s > 7) s = 7;
            for (k = 0; k != 4; k++) {
                blitA(ctx, base + Leed7e[s], xofs - Leed90[s] + 0x40,
                      k * 0x20 - 0xc, Leed90[s], 0x20);
                blitB(ctx, base + Leed7e[s], xofs + 0x40,
                      k * 0x20 - 0xc, Leed90[s], 0x20);
            }
        }
        if ((unsigned)(frame - 0xa0) <= 0x17) {
            s = 7 - (frame - 0xa0) / 3;
            if (s < 0) s = 0;
            if (s > 7) s = 7;
            for (k = 0; k != 4; k++) {
                blitA(ctx, base + Leed7e[s], xofs - Leed90[s] + 0x40,
                      k * 0x20 - 0xc, Leed90[s], 0x20);
                blitB(ctx, base + Leed7e[s], xofs + 0x40,
                      k * 0x20 - 0xc, Leed90[s], 0x20);
            }
        }
        if ((unsigned)(frame - 0x58) <= 0x47) {
            blitA(ctx, base + (0x9e << 5), xofs + 0x10, 0, w30, 0x60);
            blitB(ctx, base + (0x9e << 5), xofs + 0x40, 0, w30, 0x60);
            blitA(ctx, base + (0x97 << 6), xofs + 0x10, 0x60, w30, 0x15);
            blitB(ctx, base + (0x97 << 6), xofs + 0x40, 0x60, w30, 0x15);
        }
        if (frame > 0x57) {
            p = gBuffer;
            for (i = 0; i != 0x40; i++) {
                if (p->t == 0) {
                    int m = (i & 3) + 5;
                    blitA(ctx, gfx + Data_ede48[m - 1],
                          ((short *)&p->f0)[1] - m / 2,
                          ((short *)&p->f4)[1] - m, m, m * 2);
                    p->f4 -= p->f10;
                    if (p->f4 < 0 && frame <= 0x9f) {
                        p->f4 = 0xc0 << 15;
                    }
                } else {
                    p->t--;
                }
                p++;
            }
        }
        if (frame > 4) {
            n = 6;
            if (frame > 0x47) n = 0xb;
            p = (Part *)(base + 0x7198);
            for (j = 0; j != n; j++) {
                if (p->t == 0) {
                    scale = Data_edab0;
                    if (frame > 0x47) {
                        scale.a = (j << 12) + (0x80 << 8);
                        scale.a += (*(State **)(base + 0x7828))->f18 << 14;
                    } else {
                        scale.a = 0x80 << 8;
                    }
                    scale.b = scale.a;
                    pos[3] = 0;
                    pos[0] = (p->f0 + yofs * 2) << 16;
                    pos[2] = 0x80 << 18;
                    pos[1] = pos[2] - (p->f4 << 16);
                    k = (frame / 2 + j) % 0xb;
                    if (k != -1) {
                        _UpdateSprite(((void **)(base + 0x77d8))[k], pos, &scale, 0);
                    }
                    p->f4 -= p->f8;
                    p->fc += p->f10;
                    if (p->fc > 0xc) p->fc -= 0xc;
                    if (p->f4 < 0) {
                        if (frame > 0x9f) {
                            p->t = -1;
                        } else {
                            if (frame > 0x57) {
                                p->f8 = (Random() & 7) + 8;
                                if ((*(State **)(base + 0x7828))->f18 == 0) {
                                    p->f0 = (u32)Random() % 0x60 + 0x2a;
                                } else if ((*(State **)(base + 0x7828))->f18 == 1) {
                                    p->f0 = (u32)Random() % 0x70 + 0x22;
                                } else {
                                    p->f0 = (u32)Random() % 0xa0 + 0xa;
                                }
                            }
                            p->f4 = 0x80;
                            p->t = 8;
                        }
                    }
                } else {
                    p->t--;
                }
                p++;
            }
        }
        if (frame <= 0x9e) {
            for (i = 0; i != (*(State **)(base + 0x7828))->f14; i++) {
                if (frame > 0x55) {
                    if (frame % 0xc == 0) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 6);
                    }
                    if ((frame & 3) == 0) {
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 5);
                    }
                }
            }
        }
        if ((unsigned)(frame - 0x5a) > 0x46) {
            UpdateScreenShake(2, 2);
        } else {
            UpdateScreenShake(8, 8);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    pp = (void **)(base + 0x77d8);
    for (i = 0; i != 0xb; i++) {
        _DeleteSprite(*pp++);
    }
    AnimEnd();
}
