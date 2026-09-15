/*
 * ### BATCH 265 -- BOTH RESIDUES ARE MOOT: THIS .s IS NOT gcc OUTPUT.
 *
 * The two open items below -- the extra `cmp` and the `push {lr}` prologue --
 * are not solvable, and neither is the register rotation recorded in 2df0.c
 * next door. gfree and free were HAND-ASSEMBLED. Three facts, the first
 * decisive:
 *
 * 1. ONE LITERAL POOL WORD SERVES BOTH FUNCTIONS. The pair occupies
 *    0x08002dd8..0x08002e00 exactly (0x08002e00 begins the next TU,
 *    rom_2e00_b.o, with `push {r5, lr}`). There is exactly one pool word in
 *    that span, at 0x08002dfc, holding 0x03001e50:
 *
 *        0x08002dd8  4c08   ldr r4,[pc,#32]  -> (0x2dd8+4 & ~3) + 32 = 0x2dfc
 *        0x08002df0  4c02   ldr r4,[pc,#8]   -> (0x2df0+4 & ~3) +  8 = 0x2dfc
 *
 *    gcc-2.96 builds its minipool PER FUNCTION in arm_reorg and emits it with
 *    an explicit local label inside that function -- `ldr r1, .L4` with
 *    `.L4: .word gPtrs` before .Lfe1, which is what both candidates below
 *    produce, measured. It never emits the assembler's `ldr rX, =sym` form, so
 *    GAS never gets the chance to merge the two literals. Two functions each
 *    loading gPtrs give TWO pool words and two relocations. The ROM has one.
 *    No source text can change that.
 *
 * 2. A PUSHLESS CONDITIONAL BRANCH DOES NOT OCCUR IN THIS TREE. Of 4,339
 *    functions in generated .s, 581 are pushless and SIX of those contain any
 *    local branch -- all six an unconditional `b` hopping a literal pool (e.g.
 *    Func_8003b70 in rom_3adc_b.s). Zero contain a conditional branch. The
 *    park's own observation that `free` is pushless and gfree is not was
 *    already this rule; it is general, not a quirk of this pair.
 *
 * 3. THE NEIGHBOURHOOD IS HAND-WRITTEN RUNTIME LIBRARY. rom_1b70.s in the same
 *    region carries `.thumb_stub` divide routines, ARM `add pc, r12, lsl #3`
 *    unrolled jump tables, and a `cos` that FALLS THROUGH into `sin` -- a
 *    second entry point into one body, which no compiler emits.
 *
 * THE DETECTOR GENERALISES AND IS CHEAP. Scan a thumb function's ROM bytes for
 * 0x4800..0x4FFF (ldr Rd,[pc,#imm8*4]), resolve to ((a+4)&~3)+imm8*4, and flag
 * any target at or past the next .thumb_func_start. Run over all 293
 * hand-disassembled thumb functions with a ROM address it returns FOUR: gfree
 * and three in rom_f9000 (MPlayJumpTableCopy, m4aSoundVSync, MP2KPlayerMain),
 * which are the known hand-written MP2K driver. No other candidate in the pool
 * is affected.
 *
 * ONE TRAP IN WRITING IT: the scan must exclude pool words before reading
 * halfwords as code. The low halfword of the word 0x02004c00 is 0x4c00, a
 * perfectly valid `ldr r4,[pc,#0]`, and reading it as code produced three
 * false positives -- Func_80f7e34, GetVenusDjinni and Field_Whirlwind, all of
 * which are ordinary compiler output. Collect every pool target in a first
 * pass, then rescan skipping those four-byte spans.
 *
 * DO NOT RE-TRY EITHER FUNCTION. The C below is a correct reading and should
 * stay as documentation of what they do.
 */

